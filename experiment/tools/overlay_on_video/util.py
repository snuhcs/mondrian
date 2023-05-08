import random
import json
import cv2
import csv
import argparse
import sys
from glob import glob
import os
from pathlib import Path
import re
import time
from typing import Dict, Tuple
from tqdm import tqdm

from scipy.optimize import linear_sum_assignment
import numpy as np

import log



class Num2Color:
    def __init__(self):
        self.n2c = {}

    def __getitem__(self, num):
        if num not in self.n2c:
            r = random.randrange(0, 256)
            g = random.randrange(0, 256)
            b = random.randrange(0, 256)
            self.n2c[num] = (r, g, b)

        return self.n2c[num]

def match_boxes(box1, box2, iou_thres=0.5):
    """
    Arguments:
        box1     : (N, 4) shaped np.ndarray
        box2     : (M, 4) shaped np.ndarray
        iou_thres: discard box pairs with iou < iou_thres

    return: np.ndarray of List[(i, j)] (N, 2)
    """
    ious = box_iou(box1, box2)
    ious[ious <= iou_thres] = 0
    N, M = ious.shape
    if N > M:
        padding = ((0, 0), (0, N-M))
    else:
        padding = ((0, M-N), (0, 0))
    ious = np.pad(ious, padding)

    row_ind, col_ind = linear_sum_assignment(ious, maximize=True)
    valid_mask = [
        ious[i, j] != 0
        for i, j in zip(row_ind, col_ind)
    ]
    row_ind, col_ind = row_ind[valid_mask], col_ind[valid_mask]
    assert np.all(row_ind < N) and np.all(col_ind < M), 'Sth wrong in masking'

    return np.array([row_ind, col_ind]).reshape(2, -1).T, ious


def box_iou(box1, box2):
    # https://github.com/pytorch/vision/blob/master/torchvision/ops/boxes.py
    """
    Return intersection-over-union (Jaccard index) of boxes.
    Both sets of boxes are expected to be in (x1, y1, x2, y2) format.
    Arguments:
        box1 (N, 4) shaped np.ndarray => (N, 1, 4) => (N, M, 4)
        box2 (M, 4) shaped np.ndarray => (1, M, 4) => (N, M, 4)
    Returns:
        iou (N, M): the N x M matrix containing the pairwise
            IoU values for every element in boxes1 and boxes2
    """
    box1, box2 = np.array(box1), np.array(box2)

    N, M = box1.shape[0], box2.shape[0]

    def box_area(box):
        # box = n x 4
        return (box[:, 2] - box[:, 0]) * (box[:, 3] - box[:, 1])

    area1 = box_area(box1)  # (N,)
    area2 = box_area(box2)  # (M,)

    # inter(N, M) = prod(rb(N,M,2) - lt(N,M,2), axis=2) => (N, M)
    rb = np.minimum(box1[:, 2:].reshape(N, 1, 2),
                    box2[:, 2:].reshape(1, M, 2))  # N, M, 2
    lt = np.maximum(box1[:, :2].reshape(N, 1, 2),
                    box2[:, :2].reshape(1, M, 2))  # N, M, 2
    wh = rb - lt  # N, M, 2
    wh[wh < 0] = 0
    inter = np.prod(wh, axis=2)  # N, M
    return inter / (area1.reshape(N, 1) + area2.reshape(1, M) - inter)




def save_frames(frames, args):
    start = time.time()
    video_path = f"videos/out/{args.hash}.mp4"
    #print(f"Writing video {video_path}...",flush=True)

    if args.save_video:
        shape = frames[args.video_config['frame_range'][0]].shape
        height = shape[0]
        width = shape[1]
        fourcc = cv2.VideoWriter_fourcc(*'mp4v')
        fps = 20
        out = cv2.VideoWriter(video_path, fourcc, fps, (width, height))

        for _, frame in tqdm(sorted(frames.items(), key=lambda x:x[0]), desc="video", bar_format='{desc:<5.5} {percentage:3.0f}%|{bar:20}{r_bar}'):
            out.write(frame)

        out.release()
    else:
        os.system("rm frames/*")
        for frame_index in tqdm(range(*args.frame_range), total=args.frame_range[1] - args.frame_range[0],
                                desc=f"Saving images", bar_format='{desc:<15.15} {percentage:3.0f}%|{bar:20}{r_bar}'):
            cv2.imwrite(f'frames/frame{frame_index}.jpg', frames[frame_index])
    end = time.time()
    #print(f"Done ({end-start:.2f}s)")


def load_video(args) -> Dict[Tuple[str, int], list]:
    start = time.time()
    print(f"Loading video {args.video_config['path']}.mp4...",
          end='', flush=True)

    video_dir = args.video_dir
    video_config = args.video_config

    video_path = video_dir + '/' + video_config['path'] + '.mp4'
    cap = cv2.VideoCapture(video_path)

    assert cap.isOpened(), f"Failed. Cannot open the video from {video_path}"

    cap.set(cv2.CAP_PROP_POS_FRAMES, args.frame_range[0])
    frames = {}
    for frame_index in range(*args.frame_range):
        ret, frame = cap.read()
        assert ret, f'Failed to parse frame {frame_index} from {video_path}'
        frames[frame_index] = frame
    cap.release()
    end = time.time()
    print(f"Done ({end-start:.2f}s)")
    return frames


def load_box_log(boxlog_file, frame_range):
    with boxlog_file.open('r') as f:
        lines = f.readlines()

    unit = 9
    boxlogs = []
    for line in lines:
        line = line[:-1].split(',')
        key = line[0]
        frameIndex = line[1]
        time = line[2]
        del line[0]
        del line[0]
        del line[0]

        if frame_range[0] <= int(frameIndex) < frame_range[1]:
            boxlogs += [
                log.BoxLog(key, frameIndex, line[unit*i:unit*(i+1)])
                for i in range(int(len(line)/unit))
            ]
    return boxlogs


def load_roi_log(roi_csv_path, frame_range):
    with roi_csv_path.open('r') as f:
        reader = csv.reader(f, delimiter='\t')
        roi_csv = list(reader)
    header = {k: v for v, k in enumerate(roi_csv[0])}
    del roi_csv[0]
    roi_logs = []
    for roi_info in roi_csv:
        roi_log = log.ROILog(header, roi_info)
        if frame_range[0] <= int(roi_log.frameIndex) < frame_range[1]:
            roi_logs.append(roi_log)
    return roi_logs


def draw_rect(frame, location, bgr, style=None):
    l, t, r, b = tuple(map(int, location))
    if style == 'dotted':
        drawline(frame, (l,t), (r,t), bgr)
        drawline(frame, (r,t), (r,b), bgr)
        drawline(frame, (r,b), (l,b), bgr)
        drawline(frame, (l,b), (l,t), bgr)
    else:
        cv2.rectangle(frame, (l, t), (r, b), bgr, 2)

def drawline(img,pt1,pt2,color,thickness=2,style='dotted',gap=10):
    dist =((pt1[0]-pt2[0])**2+(pt1[1]-pt2[1])**2)**.5
    pts= []
    for i in  np.arange(0,dist,gap):
        r=i/dist
        x=int((pt1[0]*(1-r)+pt2[0]*r)+.5)
        y=int((pt1[1]*(1-r)+pt2[1]*r)+.5)
        p = (x,y)
        pts.append(p)

    if style=='dotted':
        for p in pts:
            cv2.circle(img,p,thickness,color,-1)
    else:
        s=pts[0]
        e=pts[0]
        i=0
        for p in pts:
            s=e
            e=p
            if i%2==1:
                pass
                cv2.line(img,s,e,color,thickness)
            i+=1


def write_text(frame, position, text, color=(255,255,255)):
    text = str(text)
    x,y = tuple(map(int,position))
    cv2.putText(frame, text, (x, y), cv2.FONT_HERSHEY_SIMPLEX, 0.5, color, 1, cv2.LINE_AA)

def log_hasattr(logtype, attr):
    return getattr(log, logtype).hasattr(attr)


def check_logtype_attr(logtype_attr):
    logtype = logtype_attr[0]
    if len(logtype_attr) != 2 or (logtype != 'roi' and logtype != 'box'):
        print()
        print(f"Wrong format: {'.'.join(logtype_attr)}")
        print("Should be {roi|box}.{attr}")
        print()
        sys.exit(1)
    logtype = 'ROILog' if logtype == 'roi' else 'BoxLog'
    attr = logtype_attr[-1]
    if not log_hasattr(logtype, attr):
        print()
        print(f"No attribute named \"{logtype_attr[-1]}\" in {logtype} ")
        print("Should be one of")
        for attr in list(getattr(log, logtype).attrs):
            print('\t'+attr)
        print()
        sys.exit(1)


def get_args():
    parser = argparse.ArgumentParser()
    parser.add_argument('--hash',  required=True, default=None, help="hash of experiment")
    parser.add_argument('-r', '--roi-log', action='count', required=False, default=None, help="if specified, draw ROI with logs/{hash}/roi.csv")
    parser.add_argument('-b', '--box-log', action='count', required=False, default=None, help="if specified, draw BoundingBox with logs/{hash}/boxes.txt")
    parser.add_argument('-s', '--stat', action='count', required=False, default=None, help="if specified, write statistics")
    parser.add_argument('-g', '--ground-truth', action='count', required=False, default=None, help="if specified, draw groundtruth boxes")
    parser.add_argument('-m', '--merged-roi', action='store_true', required=False, default=None, help="if specified, draw merged rois")
    parser.add_argument('-c', '--color',  required=False, default=None, help="attribute(s) to colorize rect. ex) --color 'roi.id; box.id; gt'")
    parser.add_argument('-t', '--text', required=False, default=None, help="attribute(s) to write text. ex) --text 'roi.id; roi.avgErr; box.confidence'")
    parser.add_argument('-f', '--frame-range', nargs='*', default=[], type=int, required=False, help="range of frame index to save as image. If set as [0], draw all frames")
    parser.add_argument('--save-video', action='store_true', required=False, help="save frames in video format or images")
    parser.add_argument('-v', '--video-dir', required=False, default='videos', help="directory to load video")
    args = parser.parse_args()

    with open(f"logs/{args.hash}/config.json", "r") as f:
        config = json.load(f)

    args.key = 0

    args.video_config = {
        "path": ("/".join(config['video_configs'][args.key]['path'].split('/')[-2:]).split('.')[0]), 
        "frame_range": config['video_configs'][args.key]['frame_range']
    }

    if not os.path.exists(args.video_dir):
        print(f"\nNo directory exists: {args.video_dir}\n")

    def id_of(log_path: Path):
        m = re.search(r'_(\d+).[a-z]{3,4}$', str(log_path))
        return int(m.group(1)) if m is not None else 0

    if args.roi_log or args.merged_roi:
        args.roi_log = sorted(Path(f'logs/{args.hash}').glob('roi*.csv'),
                              key=id_of, reverse=True)[0]
    if args.box_log:
        args.box_log = sorted(Path(f'logs/{args.hash}').glob('boxes*.txt'),
                              key=id_of, reverse=True)[0]

    with open(f"logs/{args.hash}/config.json", "r") as f:
        config = json.load(f)

    if args.color:
        args.color = args.color.split(";")

        colors = {}
        for logtype_attr in args.color:
            la = tuple(map(str.strip, logtype_attr.split(".")))
            check_logtype_attr(la)
            logtype, attr = tuple(la)
            if logtype == 'roi':
                assert args.roi_log
            else:
                assert args.box_log
            assert logtype not in colors, "Two color for one log type"
            colors[logtype] = attr
        args.color = colors

    args.video_config = {
        "path": ("/".join(config['video_configs'][0]['path'].split('/')[-2:]).split('.')[0]), 
        "frame_range": config['video_configs'][0]['frame_range']
    }

    if args.frame_range:
        if args.frame_range == [0]:
            args.frame_range = args.video_config['frame_range']
        elif len(args.frame_range) == 1:
            args.frame_range = [0] + args.frame_range
        else:
            assert len(args.frame_range) == 2
    else:
        args.frame_range = args.video_config['frame_range']

    if args.text:
        args.text = args.text.split(";")
        args.text = list(map(lambda x: tuple(map(str.strip, x.split('.'))), args.text))
        for logtype_attr in args.text:
            check_logtype_attr(logtype_attr)
            if logtype_attr[0] == 'roi':
                assert args.roi_log
            else:
                assert args.box_log

    if args.roi_log or args.merged_roi:
        roi_logs = load_roi_log(args.roi_log, args.frame_range)
        videos = set([roi_log.key for roi_log in roi_logs])
    if args.box_log:
        box_logs = load_box_log(args.box_log, args.frame_range)
        videos2 = set([box_log.key for box_log in box_logs])
        if args.roi_log:
            #assert videos2 == videos, "ROI log & Box log contain logs for different videos"
            if videos2 != videos:
                assert True
        videos = videos2
    assert len(videos) == 1, "Log must be from single video"

    return args

colorizer = Num2Color()
