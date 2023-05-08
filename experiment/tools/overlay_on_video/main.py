import util
import time
import cv2
import numpy as np
from collections import defaultdict
from icecream import ic
from tqdm import tqdm
import pandas as pd

upper_pad = 200
pad = 200

class FrameStat:
    STATS = {
            'ROI_TYPE' : ['OF', 'PD'],
            'ROI_ORIGIN' : ['BB', 'PD'],
            'ROI_ISPACKED' : [True, False],
            'BOX_ORIGIN' : ['FF', 'NF', 'BB', 'PD', 'NM', 'IP'],
            }

    def __init__(self):

        self.roi_id = []
        self.box_id = []
        self.roi_type = []
        self.roi_origin = []
        self.roi_ispacked = []

        self.box_origin = []

    def update(self, name, val):
        getattr(self, name).append(val)

    def print_ids(self, frame):
        '''
        self.roi_id = sorted(self.roi_id)
        pos = 0
        for i, _id in enumerate(self.roi_id):
            textlen = len(str(_id))
            util.write_text(frame, (pos, pad-20), str(_id), util.colorizer[_id])
            pos += textlen*25

        '''

        ids = list(set(self.roi_id) | set(self.box_id))


        util.write_text(frame, (0, upper_pad-20), f"ROI ID range: ({min(ids)} ~ {max(ids)}) |  total ID count: {len(ids)}")

    def print_stat(self, frame):

        cnt = 0
        linesize = 50
        for stat in FrameStat.STATS:
            rb, topio = stat.split("_")
            msg = f"[{rb}] {topio:<10}: "
            total = len(getattr(self, stat.lower()))
            for c in FrameStat.STATS[stat]:
                count = getattr(self, stat.lower()).count(c)
                ratio = f"{int(100*count/total)}%" if total != 0 else None
                msg += f" {c} ({count}, {ratio}) ||"
            msg = msg[:-2]
            if stat == "BOX_ORIGIN":
                if len(self.box_origin) != 0:
                    if (self.box_origin.count('FF') + self.box_origin.count('NF')) == len(self.box_origin):
                        msg += " => !!!FULL FRAME!!!"
            util.write_text(frame, (0, 1130 + linesize*cnt), msg)
            cnt += 1
        return msg

def update(fs, log, logtype):
    if logtype == 'roi':
        fs.update('roi_id', log.id)
        fs.update('roi_type', log.roi_type)
        fs.update('roi_origin', log.origin)
        fs.update('roi_ispacked', log.isPacked)
    else:
        fs.update('box_origin', log.origin)
        fs.update('box_id', log.id)
    return fs


def main(args):
    # Load videos, in {(key, keyCount) : [np.ndarray]}
    frames = util.load_video(args)
    global_start = time.time()
    frame_stats = {}

    # Pad the frames to show information at the padded region
    if args.stat:
        for frame_index in frames.keys():
            frames[frame_index] = np.pad(frames[frame_index], 
                    ((0,0),(0,pad),(0,0)), 'constant', constant_values=0)

    # Load attributes to write as text
    start = time.time()
    #print("Drawing rects and writing texts...", flush=True)
    text_attrs = {}
    if args.text:
        text_attrs['roi'] = [
                log_attr[1]
                for log_attr in args.text
                if log_attr[0] == 'roi'
                ]
        text_attrs['box'] = [
                log_attr[1]
                for log_attr in args.text
                if log_attr[0] == 'box'
                ]

    # Collect logs
    logs = []
    if args.roi_log or args.merged_roi:
        roi_logs = [('roi', log)
                    for log in util.load_roi_log(args.roi_log, args.frame_range)]
        logs += roi_logs
    if args.box_log:
        box_logs = [('box', log)
                    for log in util.load_box_log(args.box_log, args.frame_range)
                    if log.label == 'person']
        logs += box_logs

    # If gt exists, draw them and match boxes with them
    if args.ground_truth:
        box_correctness = {} # Dict[(frame_index, box_id), (b,g,r)]

        if box_logs:
            dt_boxes = defaultdict(list)
            for _, log in box_logs:
                dt_boxes[log.frameIndex].append(log)

        if 'mta' in args.video_config['path']:
            frame_boxes = defaultdict(list)
            anns = pd.read_csv(f'../ground_truths/mta/{args.video_config["path"].split("/")[-1]}.csv')
            anns = anns[(args.frame_range[0] <= anns['frame_no_cam'])
                        & (anns['frame_no_cam'] < args.frame_range[1])]
            for _, row in anns.iterrows():
                frame_boxes[int(row['frame_no_cam'])].append({
                    'xyxy': [
                        max(0, row['x_top_left_BB']),
                        max(0, row['y_top_left_BB']),
                        min(1920, row['x_bottom_right_BB']),
                        min(1080, row['y_bottom_right_BB']),
                    ],
                    'confidence': 1,
                    'label': 0,
                })
        elif 'virat' in args.video_config['path']:
            print('Not implemented for VIRAT dataset')
        else:
            raise ValueError(f'Unknown video: {args.video_config}')

        for frame_index in tqdm(range(*args.frame_range), desc='Drawing GT...'):
            gt_boxes = []
            frame = frames[frame_index]
            for box in frame_boxes[frame_index]:
                """
                box: Dict
                    xyxy : List[float, float, float, float]
                    confidence : float
                    label : int
                """
                if box['label'] == 0 and box['confidence'] >= 0.25: # if label == person
                    location = box['xyxy']
                    gt_boxes.append(location)
                    util.draw_rect(frame, location, (0,0,0))
                    util.write_text(frame, (location[2], location[3]), f"{box['confidence']:.2f}", color=(0, 0, 0))
            if box_logs:
                if dt_boxes[frame_index]:
                    pairs, ious = util.match_boxes(
                        np.array(gt_boxes),
                        np.array([log.location for log in dt_boxes[frame_index]])
                    )
                    correct_dt_box_ids = [j for i, j in pairs]
                    for j, box in enumerate(dt_boxes[frame_index]):
                        iou = [ious[_i, _j] for _i, _j in pairs if _j == j]
                        assert len(iou) <= 1, "iou len err"
                        box_correctness[(frame_index, box.id)] = ((0, 255, 0) if j in correct_dt_box_ids else (
                            0, 0, 255), iou[0] if len(iou) == 1 else None)

    if args.merged_roi:
        for logtype, log in tqdm(logs, desc="Drawing merged rois", bar_format='{desc:<15.15} {percentage:3.0f}%|{bar:20}{r_bar}'):
            if logtype == 'roi':
                frame = frames[log.frameIndex]
                location = log.mergedLoc
                util.draw_rect(frame, location, (255, 0, 0))

    # Draw them on frames
    # Profile statistics
    for logtype, log in tqdm(logs, desc="Drawing rects", bar_format='{desc:<15.15} {percentage:3.0f}%|{bar:20}{r_bar}'):
        frame = frames[log.frameIndex]
        location = log.paddedLoc if logtype == 'roi' else log.location
        if logtype == 'box' and args.ground_truth:
            bgr, iou = box_correctness[(log.frameIndex, log.id)]
            """
            if iou == None:
                util.write_text(frame, (location[0] if logtype == 'roi' else location[2], location[3]), "None")
            else:
                util.write_text(frame, (location[0] if logtype == 'roi' else location[2], location[3]), f"{iou:.2f}")
            """
        else:
            if args.color and logtype in args.color:
                bgr = util.colorizer[getattr(log, args.color[logtype])]
            else:
                bgr = (0, 0, 0)
        if logtype == 'roi':
            util.draw_rect(frame, location, bgr, style='dotted')
        else:
            util.draw_rect(frame, location, bgr)
        frame_id = (log.key, log.frameIndex)
        if frame_id not in frame_stats:
            fs = FrameStat()
            fs = update(fs, log, logtype)
            frame_stats[frame_id] = fs
        else:
            fs = update(frame_stats[frame_id], log, logtype)

    for logtype, log in tqdm(logs, desc="Writing texts", bar_format='{desc:<15.15} {percentage:3.0f}%|{bar:20}{r_bar}'):
        frame = frames[log.frameIndex]
        location = log.paddedLoc if logtype == 'roi' else log.location
        if logtype == 'box' and args.ground_truth:
            bgr, iou = box_correctness[(log.frameIndex, log.id)]
            """
            if iou == None:
                util.write_text(frame, (location[0] if logtype == 'roi' else location[2], location[3]), "None")
            else:
                util.write_text(frame, (location[0] if logtype == 'roi' else location[2], location[3]), f"{iou:.2f}")
            """
        else:
            if args.color and logtype in args.color:
                bgr = util.colorizer[getattr(log, args.color[logtype])]
            else:
                bgr = (0, 0, 0)
        if args.text and text_attrs[logtype]:
            text = ""
            for attr in text_attrs[logtype]:
                text += f"{getattr(log, attr)}_"
            text = text[:-1]
            util.write_text(frame, (location[0] if logtype == 'roi' else location[2], location[3]), text)


    end = time.time()
    #print(f"Done ({end-start:.2f}s)", flush=True)

    if args.stat:
        # Write stats and save as video & images
        start = time.time()
        bar_size = 60*5
        print("Writing stats...", end='', flush=True)
        for i in frames.keys():
            frames[i] = np.pad(frames[i], ((0,bar_size),(0,0),(0,0)), 'constant', constant_values=0)
            frame_id = (logs[0][1].key, i)
            if frame_id in frame_stats:
                frame_stats[frame_id].print_stat(frames[i])
            frames[i] = np.pad(frames[i], ((upper_pad,0),(0,0),(0,0)), 'constant', constant_values=0)
            if frame_id in frame_stats:
                frame_stats[frame_id].print_ids(frames[i])
            frames[i] = np.pad(frames[i], ((0,0),(pad,0),(0,0)), 'constant', constant_values=0)
        end = time.time()
        #print(f"Done ({end-start:.2f}s)")

    util.save_frames(frames, args)

    global_end = time.time()
    print(f"{global_end-global_start:.2f}s in total")


if __name__ == "__main__":
    args = util.get_args()
    main(args)
