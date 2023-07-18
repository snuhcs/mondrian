import argparse
from collections import Counter, defaultdict
from functools import partial
from itertools import chain
import json
from multiprocessing import Pool
from pathlib import Path
import pickle
import re
from time import time
from typing import Dict, List, Tuple

import cv2
import matplotlib.pyplot as plt
import numpy as np
from scipy.optimize import linear_sum_assignment
from sklearn.ensemble import RandomForestClassifier
from sklearn.metrics import confusion_matrix
from sklearn.tree import DecisionTreeClassifier
from tqdm import tqdm

from datatype import Rect, BBox, OFFeatures, ROI
from util import NpEncoder, current_time, plot_confusion_matrix, plot_cdf, plot_error_cdf, tree_to_code_cpp


Vid = int
Fid = int


def default_to_regular(d):
    if isinstance(d, defaultdict):
        d = {k: default_to_regular(v) for k, v in d.items()}
    return d


def label_path_of(video_id, frame_index):
    return Path(f'/data1/mondrian/dataset/MTA/MTA_ext_short/train/labels/train_cam_{video_id}_{frame_index:05d}.txt')


def parse_roi(merged_roi_str: str):
    merged_roi_str = merged_roi_str.replace('train_cam_', '')
    merged_roi_str = merged_roi_str.replace('cam_', '')
    elems = merged_roi_str.split('_')
    video_id = int(elems[0])
    frame_index = int(elems[1])
    roi_rect_ao = Rect(l=int(elems[2]),
                       t=int(elems[3]),
                       r=int(elems[4]),
                       b=int(elems[5]))
    scale = float(elems[6])
    box_elems = elems[7:]
    assert len(box_elems) % 4 == 0
    src_rects_ao = [
        Rect(l=int(box_elems[i]),
             t=int(box_elems[i + 1]),
             r=int(box_elems[i + 2]),
             b=int(box_elems[i + 3]))
        for i in range(0, len(box_elems), 4)
    ]
    return video_id, frame_index, roi_rect_ao, scale, src_rects_ao


def load_rois(packing_info_path: Path,
              pred_label_dir: Path,
              input_size: int,
              border: int) -> List[ROI]:
    frame_name = packing_info_path.stem
    m = re.search(r'cam_(\d+)_(\d+)', frame_name)
    video_id, packed_frame_index = int(m.group(1)), int(m.group(2))

    with packing_info_path.open() as f:
        packing_info = json.load(f)

    label_path = pred_label_dir / f'{frame_name}.txt'
    if label_path.exists():
        pred_boxes_rp = [
            BBox(rect=Rect(l=cx - w / 2,
                           t=cy - h / 2,
                           r=cx + w / 2,
                           b=cy + h / 2),
                 conf=conf)
            for _, cx, cy, w, h, conf in np.genfromtxt(
                label_path, delimiter=' ', dtype=np.float32).reshape(-1, 6)
        ]
    else:
        print(label_path, 'not found')
        pred_boxes_rp = []

    def roi_of(pack_xy, merged_roi_str):
        pack_l, pack_t = [int(v) for v in pack_xy.split('_')]
        vid, frame_index, roi_rect_ao, scale, src_rects_ao = \
            parse_roi(merged_roi_str)
        assert video_id == vid and len(src_rects_ao) == 1
        src_rect_ao = src_rects_ao[0]
        roi_src_box = BBox(rect=Rect(
            l=((src_rect_ao.l - roi_rect_ao.l) * scale + pack_l + border) / input_size,  # noqa
            t=((src_rect_ao.t - roi_rect_ao.t) * scale + pack_t + border) / input_size,  # noqa
            r=((src_rect_ao.r - roi_rect_ao.l) * scale + pack_l + border) / input_size,  # noqa
            b=((src_rect_ao.b - roi_rect_ao.t) * scale + pack_t + border) / input_size,  # noqa
        ), conf=1.0)
        roi_rect_rp = Rect(
            l=(pack_l + border) / input_size,
            t=(pack_t + border) / input_size,
            r=(pack_l + border + max(1, int(roi_rect_ao.width * scale))) / input_size,
            b=(pack_t + border + max(1, int(roi_rect_ao.height * scale))) / input_size,
        )
        roi_pred_boxes = [box for box in pred_boxes_rp
                          if roi_rect_rp.contains(box.rect)]
        return ROI(video_id=video_id,
                   frame_index=frame_index,
                   rect_ao=roi_rect_ao,
                   src_rect_ao=src_rect_ao,
                   scale=scale,
                   input_size=input_size,
                   packed_frame_index=packed_frame_index,
                   pack_l=pack_l,
                   pack_t=pack_t,
                   src_box=roi_src_box,
                   pred_boxes=tuple(roi_pred_boxes))

    return [roi_of(pack_xy, packing_info[pack_xy])
            for pack_xy in sorted(packing_info.keys())]


def prepare_rois(
        dataset: str,
        model: str,
        int8: bool,
        input_size: int,
        border: int,
) -> Dict[Vid, Dict[Fid, Dict[Rect, Dict[float, ROI]]]]:
    info_dir = Path('packed_frames') / \
        f'{dataset}_scale' / str(input_size) / 'labels'
    pred_label_dirname = f'{dataset}_scale_pack_{model}_{input_size}' + ("_int8" if int8 else "")
    pred_label_dir = Path('runs/detect') / pred_label_dirname / 'labels'
    assert info_dir.is_dir() and pred_label_dir.is_dir()
    packing_info_paths = sorted(info_dir.glob('*.json'))
    print('packing_info_paths', len(packing_info_paths))

    runnable = partial(load_rois,
                       pred_label_dir=pred_label_dir,
                       input_size=input_size,
                       border=border)
    with Pool() as pool:
        rois = list(chain.from_iterable(tqdm(
            pool.imap_unordered(runnable, packing_info_paths),
            total=len(packing_info_paths),
            desc='Loading ROIs')))
    grouped_rois = defaultdict(lambda: defaultdict(lambda: defaultdict(dict)))
    for r in rois:
        grouped_rois[r.video_id][r.frame_index][r.rect_ao][r.scale] = r
    return default_to_regular(grouped_rois)


def prepare_safe_areas(
        grouped_rois: Dict[Vid, Dict[Fid, Dict[Rect, Dict[float, ROI]]]],
        iou_thres: float,
        conf_thres: float,
) -> Dict[Vid, Dict[Fid, Dict[Rect, float]]]:

    def predict_score(roi: ROI):
        for pred_box in roi.pred_boxes:
            if pred_box.rect.iou(roi.src_box.rect) > iou_thres \
                    and pred_box.conf > conf_thres:
                return 1
        return 0

    def safe_index_of(scores):
        min_index, count_0, count_1 = None, 0, 0
        for i, score in enumerate(scores):
            if min_index is None:  # New O
                if score == 1:
                    min_index = i
                    count_0 = 0
                    count_1 = 1
            else:
                if score == 0:
                    count_0 += 1
                elif score == 1:
                    count_1 += 1
                else:
                    raise ValueError(f'Unknown score: {score}')
                if count_0 >= count_1:
                    min_index = None
        if min_index is None:
            min_index = len(scores) - 1
        return min_index

    def safe_scale_of(scale_roi_dict: Dict[float, ROI]):
        scales = sorted(scale_roi_dict.keys())
        scores = np.array([predict_score(scale_roi_dict[scale])
                           for scale in scales])
        assert set(scores) <= {0, 1}
        safe_index = safe_index_of(scores)
        safe_scale = scales[safe_index]

        # if safe_index == len(scales) - 1:
        #     max_scale = max(scales)
        #     roi = scale_roi_dict[max_scale]
        #     ious, confs = [], []
        #     for pred_box in roi.pred_boxes:
        #         iou = pred_box.rect.iou(roi.src_box.rect)
        #         if iou <= iou_thres:
        #             ious.append(iou)
        #         if pred_box.conf <= conf_thres:
        #             confs.append(pred_box.conf)
        #     print(f'     |{"".join(["O" if s == 1 else "_" for s in scores]):>10}| {safe_index}     '
        #           f'Max: {max_scale} '
        #           f'Preds: {len(roi.pred_boxes)} '
        #           f'IoU: {len(ious)} {max(ious) if ious else 0.0:.2f} '
        #           f'Conf: {len(confs)} {max(confs) if confs else 0.0:.2f} '
        #           f'Size: {roi.rect_ao.max_length} '
        #           f'Video: {roi.video_id} '
        #           f'Frame: {roi.frame_index} '
        #           f'Packed: {roi.packed_frame_index} {(roi.pack_l, roi.pack_t)}')
        return safe_scale

    safe_areas = defaultdict(lambda: defaultdict(dict))
    for video_id in tqdm(sorted(grouped_rois.keys()),
                         total=len(grouped_rois),
                         desc='Preparing safe scales'):
        for frame_index in sorted(grouped_rois[video_id].keys()):
            for rect_ao in sorted(grouped_rois[video_id][frame_index].keys()):
                scale_roi_dict = grouped_rois[video_id][frame_index][rect_ao]
                safe_scale = safe_scale_of(scale_roi_dict)
                orig_area = scale_roi_dict[safe_scale].rect_ao.area
                assert orig_area == rect_ao.area
                safe_area = orig_area * safe_scale ** 2
                safe_areas[video_id][frame_index][rect_ao] = safe_area
    return default_to_regular(safe_areas)


def load_frames(dataset, video_id, start_index, end_index):
    if dataset == 'mta':
        video_path = f'/data1/mondrian/dataset/mta/MTA_ext_short/train/cam_{video_id}/cam_{video_id}.mp4'
    elif dataset == 'virat':
        video_path = f'/data1/mondrian/dataset/virat/video/train/train_cam_{video_id}.mp4'
    else:
        raise ValueError(f'Unknown dataset: {dataset}')
    cap = cv2.VideoCapture(video_path)
    cap.set(cv2.CAP_PROP_POS_FRAMES, start_index)
    frames = {}
    for frame_index in tqdm(range(start_index, end_index),
                            total=end_index - start_index,
                            desc=f'Loading frames of {video_id}'):
        ret, frame = cap.read()
        assert ret, f'Frame Index: {frame_index}'
        frames[frame_index] = frame
    return frames


def xywh2xyxy(xywhs):
    xyxys = xywhs.copy()
    xyxys[:, :2] = xywhs[:, :2] - xywhs[:, 2:] / 2
    xyxys[:, 2:] = xywhs[:, :2] + xywhs[:, 2:] / 2
    return xyxys


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
    if len(box1) == 0 or len(box2) == 0:
        return np.zeros((len(box1), len(box2)), dtype=np.float32)
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


def match_boxes(rects1: List[Rect], rects2: List[Rect], iou_thres=0.3):
    """
    Return match indices of box1 and box2 which results in largest total iou.

    Arguments:
        rects1   : List of Rect
        rects2   : List of Rect
        iou_thres: discard box pairs with iou < iou_thres

    return: np.ndarray of List[(i, j)] (N, 2)
    """
    rects1 = np.array([(rect.l, rect.t, rect.r, rect.b) for rect in rects1])
    rects2 = np.array([(rect.l, rect.t, rect.r, rect.b) for rect in rects2])
    ious = box_iou(rects1, rects2)
    if ious.size == 0:
        return np.zeros((0, 2), dtype=np.int32)
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
    return np.array([row_ind, col_ind]).reshape(2, -1).T


def extract_feature(prev_frame: np.ndarray,
                    curr_frame: np.ndarray,
                    curr_rects_ao: List[Rect]) -> OFFeatures:
    """
    Arguments:
        prev_frame
        curr_frame
        curr_rects_ao : List[box]
    
    Returns:
        List[FullFrame]
    """
    assert prev_frame.shape == curr_frame.shape == (1080, 1920, 3)
    if len(curr_rects_ao) == 0:
        return []

    gray_prev_frame = cv2.cvtColor(prev_frame, cv2.COLOR_BGR2GRAY)
    gray_curr_frame = cv2.cvtColor(curr_frame, cv2.COLOR_BGR2GRAY)

    curr_points_list = []
    for curr_rect_ao in curr_rects_ao:
        l, t, r, b = curr_rect_ao.l, curr_rect_ao.t, curr_rect_ao.r, curr_rect_ao.b
        points = cv2.goodFeaturesToTrack(
            gray_prev_frame[t:b, l:r],
            maxCorners=50,
            qualityLevel=0.01,
            minDistance=5,
            blockSize=3,
            useHarrisDetector=False,
            k=0.03)
        if points is None or len(points) == 0:
            points = np.array([(l + r) / 2, (t + b) / 2])
        points = points.reshape(-1, 2)
        points[:, 0] += l
        points[:, 1] += t
        curr_points_list.append(points)

    all_prev_points, all_status, all_errs = cv2.calcOpticalFlowPyrLK(
        gray_curr_frame, gray_prev_frame,
        prevPts=np.vstack(curr_points_list).astype(np.float32),
        nextPts=None,
        winSize=(15, 15),
        maxLevel=2,
        criteria=(cv2.TERM_CRITERIA_COUNT + cv2.TERM_CRITERIA_EPS, 10, 0.03))
    all_status, all_errs = all_status.reshape(-1), all_errs.reshape(-1)
    assert sum(len(v) for v in curr_points_list) == len(all_prev_points)
    assert len(all_prev_points) == len(all_status) == len(all_errs)
    assert set(all_status) <= {0, 1}

    prev_points_list, statuses_list, errs_list = [], [], []
    sidx, eidx = 0, 0
    for curr_points in curr_points_list:
        eidx = sidx + len(curr_points)
        prev_points_list.append(all_prev_points[sidx:eidx])
        statuses_list.append(all_status[sidx:eidx])
        errs_list.append(all_errs[sidx:eidx])
        sidx = eidx
    assert sidx == eidx == len(all_prev_points)

    # curr_frame_to_draw = curr_frame.copy()
    # for rect_ao in curr_rects_ao:
    #     cv2.rectangle(curr_frame_to_draw,
    #                   (rect_ao.l, rect_ao.t),
    #                   (rect_ao.r, rect_ao.b),
    #                   (0, 255, 0), 2)
    # for curr_points in curr_points_list:
    #     for x, y in curr_points:
    #         cv2.circle(curr_frame_to_draw, (int(x), int(y)), 2, (0, 0, 255), 2)
    # cv2.imwrite('curr_frame_to_draw.png', curr_frame_to_draw)
    # prev_frame_to_draw = prev_frame.copy()
    # for prev_points in prev_points_list:
    #     for x, y in prev_points:
    #         cv2.circle(prev_frame_to_draw, (int(x), int(y)), 2, (0, 0, 255), 2)
    # cv2.imwrite('prev_frame_to_draw.png', prev_frame_to_draw)
    # exit()

    offeatures_list = [
        OFFeatures(shifts=curr_points - prev_points,
                   errs=errs,
                   statuses=statuses)
        for curr_points, prev_points, statuses, errs in zip(
            curr_points_list, prev_points_list, statuses_list, errs_list)
    ]
    return offeatures_list


def prepare_of_features(
        dataset: str,
        grouped_rois: Dict[Vid, Dict[Fid, Dict[Rect, Dict[float, ROI]]]],
) -> Dict[Vid, Dict[Fid, Dict[Rect, np.ndarray]]]:
    features = defaultdict(lambda: defaultdict(dict))
    for video_id in sorted(grouped_rois.keys()):
        start_index = min(grouped_rois[video_id].keys())
        end_index = max(grouped_rois[video_id].keys()) + 1
        frames = load_frames(dataset, video_id, max(0, start_index - 1), end_index)
        for frame_index in tqdm(sorted(grouped_rois[video_id].keys()),
                                total=len(grouped_rois[video_id]),
                                desc=f'Prepare OF features for {video_id}'):
            curr_rects_ao = sorted(grouped_rois[video_id][frame_index].keys())
            prev_frame = frames[frame_index - 1]
            curr_frame = frames[frame_index]
            feature_list = extract_feature(
                prev_frame, curr_frame, curr_rects_ao)
            for rect_ao, feature in zip(curr_rects_ao, feature_list):
                features[video_id][frame_index][rect_ao] = feature
    return default_to_regular(features)


def cache(func, cache_path: Path, no_cache: bool):
    if not no_cache and cache_path.exists():
        print(f'Use cached {cache_path}')
        with cache_path.open('rb') as f:
            return pickle.load(f)
    else:
        result = func()
        cache_path.parent.mkdir(parents=True, exist_ok=True)
        with cache_path.open('wb') as f:
            pickle.dump(result, f)
        return result


def cache_path_of(name, *args):
    return Path(f'cache/{name}_{"_".join([str(v) for v in args])}.pkl')


feature_names = [
    'maxEdgeLength',
    'area',
    'xyRatio',
    'shiftAvg',
    'shiftStd',
    'shiftNcc',
    'avgErr',
]


def feature_vector_of(of_feature: OFFeatures, rect_ao: Rect):
    return [
        rect_ao.max_length,    # maxEdgeLength
        rect_ao.area,          # area
        rect_ao.hw_ratio,      # xyRatio
        of_feature.shift_avg[0]**2 + of_feature.shift_avg[1]**2,  # shiftAvg
        of_feature.shift_std[0]**2 + of_feature.shift_std[1]**2,  # shiftStd
        of_feature.shift_ncc,  # shiftNcc
        of_feature.avg_err,    # avgErr
    ]


def prepare_XY_vid_fid(
        dataset: str,
        model: str,
        int8: bool,
        input_size: int,
        border: int,
        iou_thres: float,
        conf_thres: float,
        no_cache: bool,
) -> Tuple[np.ndarray, np.ndarray]:
    grouped_rois = cache(lambda: prepare_rois(dataset, model, int8, input_size, border),
                         cache_path_of('grouped_rois', dataset, model, int8, input_size, border), no_cache)
    of_features = cache(lambda: prepare_of_features(dataset, grouped_rois),
                        cache_path_of('features', dataset, model, int8, input_size, border), no_cache)
    safe_areas = cache(lambda: prepare_safe_areas(grouped_rois, iou_thres, conf_thres),
                       cache_path_of('safe_areas', dataset, model, int8, input_size, border, iou_thres, conf_thres), no_cache)

    X, Y, orig_areas, video_ids, frame_indices = [], [], [], [], []
    for video_id in sorted(of_features.keys()):
        for frame_index in sorted(of_features[video_id].keys()):
            for rect_ao in sorted(of_features[video_id][frame_index].keys()):
                of_feature = of_features[video_id][frame_index][rect_ao]
                safe_area = safe_areas[video_id][frame_index][rect_ao]
                X.append(feature_vector_of(of_feature, rect_ao))
                Y.append(safe_area)
                orig_areas.append(rect_ao.area)
                video_ids.append(video_id)
                frame_indices.append(frame_index)
    return np.array(X), np.array(Y), np.array(orig_areas), np.array(video_ids), np.array(frame_indices)


def masks_of(dataset, data_split_type, video_ids, frame_indices):
    if data_split_type == 'video':
        if dataset == 'mta':
            train_video_ids = [0, 1, 2]
            test_video_ids = [3, 4, 5]
        elif dataset == 'virat':
            train_video_ids = [1, 2, 3]
            test_video_ids = [4, 6]
        else:
            raise ValueError(f'Unknown dataset {dataset}')
        assert set(video_ids) == set(train_video_ids + test_video_ids)
        train_mask = np.isin(video_ids, train_video_ids)
        test_mask = np.isin(video_ids, test_video_ids)
    elif data_split_type == 'frame':
        if dataset == 'mta':
            FPS = 41
            train_frame_range = (40 * FPS, 45 * FPS)
            test_frame_range = (45 * FPS, 50 * FPS)
            train_mask = (train_frame_range[0] <= frame_indices) \
                & (frame_indices < train_frame_range[1])
            test_mask = (test_frame_range[0] <= frame_indices) \
                & (frame_indices < test_frame_range[1])
        elif dataset == 'virat':
            train_ranges = {
                1: (4900, 5050),
                2: (1210, 1360),
                3: (1210, 1360),
                4: (1210, 1360),
                6: (1210, 1360),
            }
            test_ranges = {
                1: (5050, 5200),
                2: (1360, 1510),
                3: (1360, 1510),
                4: (1360, 1510),
                6: (1360, 1510),
            }
            train_masks, test_masks = [], []
            for vid, fid in zip(video_ids, frame_indices):
                if train_ranges[vid][0] <= fid < train_ranges[vid][1]:
                    train_masks.append(True)
                    test_masks.append(False)
                elif test_ranges[vid][0] <= fid < test_ranges[vid][1]:
                    train_masks.append(False)
                    test_masks.append(True)
                else:
                    raise ValueError(f'Unknown frame {fid} in video {vid}')
            train_mask = np.array(train_masks)
            test_mask = np.array(test_masks)
        else:
            raise ValueError(f'Unknown dataset {dataset}')
    return train_mask, test_mask


def quantize(Y_train, Y_test, num_levels):
    thresholds = [
        np.quantile(Y_train, i / num_levels)
        for i in range(1, num_levels)
    ]
    Y_train_q = np.digitize(Y_train, thresholds, right=True)
    Y_test_q = np.digitize(Y_test, thresholds, right=True)
    return Y_train_q, Y_test_q, thresholds + [np.inf]


def fit_and_predict(X_train, Y_train_q, X_test, clf_name):
    if clf_name == 'dt':
        clf = DecisionTreeClassifier(max_depth=10)
    elif clf_name == 'rf':
        clf = RandomForestClassifier()
    else:
        raise ValueError(f'Unknown classifier: {clf_name}')
    Y_pred_q = clf.fit(X_train, Y_train_q).predict(X_test)
    return clf, Y_pred_q


def main(
        args,
        dataset: str,
        model: str,
        data_split_type: str,
        area_shift: float,
        no_cache: bool,
        int8: bool,
        num_levels: int,
        border: int,
        iou_thres: float,
        conf_thres: float,
) -> None:
    input_size = {
        'yolov5x': 1024,
        'yolov5l': 1024,
        'yolov5m': 1280,
        'yolov5s': 1792,
        'yolov5n': 1536,
    }[model]
    X, Y, orig_areas, video_ids, frame_indices = cache(
        lambda: prepare_XY_vid_fid(dataset, model, int8, input_size, border, iou_thres, conf_thres, no_cache),
        cache_path_of('XY_vid_fid', dataset, model, int8, input_size, border, iou_thres, conf_thres), no_cache)
    assert np.all(Y <= orig_areas)

    train_mask, test_mask = masks_of(
        dataset, data_split_type, video_ids, frame_indices)
    assert not np.any(train_mask & test_mask)
    assert np.all(train_mask | test_mask)

    X_train, Y_train = X[train_mask], Y[train_mask]
    X_test, Y_test, orig_areas_test = X[test_mask], Y[test_mask], orig_areas[test_mask]
    print(f'Avg area change: {round(np.mean(orig_areas_test))} => {round(np.mean(Y_test))}')
    print(f'# Train Samples: {len(X_train)}, # Test Samples: {len(X_test)}')
    print(f'90% quantile: {round(np.quantile(Y_test, 0.9))}')

    exp_dir = Path('scaler') / current_time()
    exp_dir.mkdir()
    np.save(str(exp_dir / 'orig_areas_test.npy'), orig_areas_test)
    np.save(str(exp_dir / 'Y_train.npy'), Y_train)
    np.save(str(exp_dir / 'Y_test.npy'), Y_test)

    Y_train_q, Y_test_q, thresholds = quantize(Y_train, Y_test, num_levels)

    info = {
        'quantile_0.9': round(np.quantile(Y_test, 0.9)),
        'thresholds': thresholds,
    }
    for clf_name in ['dt', 'rf']:
        print(f'Start processing {clf_name}...', end='')
        start_time = time()
        clf, Y_pred_q = fit_and_predict(X_train, Y_train_q, X_test, clf_name)
        Y_pred = np.clip(np.array([thresholds[i] for i in Y_pred_q]), None, orig_areas_test)
        Y_pred_shift = np.clip(Y_pred + area_shift, None, orig_areas_test)
        Y_pred_shift_q = np.digitize(Y_pred_shift, thresholds, right=True)

        info[f'area_orig_{clf_name}'] = round(np.mean(orig_areas_test))
        info[f'area_resize_true_{clf_name}'] = round(np.mean(Y_test))
        info[f'area_resize_pred_{clf_name}'] = round(np.mean(Y_pred))
        info[f'area_resize_pred_shift_{clf_name}'] = round(np.mean(Y_pred_shift))
        info[f'feature_importances_{clf_name}'] = \
            list(zip(feature_names, clf.feature_importances_))
        info[f'under_estimation_error_{clf_name}'] = np.sum(Y_pred_shift < Y_test) / len(Y_pred)

        if clf_name == 'dt':
            with (exp_dir / f'scaler.cpp').open('w') as f:
                f.write(tree_to_code_cpp(clf, feature_names))
        np.save(str(exp_dir / f'Y_pred_shift_{clf_name}.npy'), Y_pred_shift)
        np.savetxt(str(exp_dir / f'cm_{clf_name}.txt'),
                   confusion_matrix(Y_test_q, Y_pred_q), delimiter=',', fmt='%d')
        plt.close(plot_cdf(Y_test / orig_areas_test, save_path=exp_dir / f'safe_scale_cdf_{clf_name}.png'))
        plt.close(plot_error_cdf(Y_test, Y_pred_shift,
                  save_path=exp_dir / f'error_cdf_{clf_name}.png'))
        plt.close(plot_confusion_matrix(Y_test_q, Y_pred_q, target_names=['L0', 'L1', 'L2', 'L3', 'L4'],
                  save_path=exp_dir / f'cm_{clf_name}.png'))
        plt.close(plot_confusion_matrix(Y_test_q, Y_pred_q, target_names=['L0', 'L1', 'L2', 'L3', 'L4'],
                  save_path=exp_dir / f'cm_norm_{clf_name}.png', normalize=True))
        print(f'Done ({time() - start_time:.2f})')
    with (exp_dir / 'info.json').open('w') as f:
        json.dump({**info, **vars(args)}, f, indent=4, cls=NpEncoder)


if __name__ == '__main__':
    parser = argparse.ArgumentParser()
    parser.add_argument('-d', '--dataset', required=True,
                        choices=['mta', 'virat'])
    parser.add_argument('-m', '--model', required=True,
                        choices=['yolov5x', 'yolov5l', 'yolov5m', 'yolov5s', 'yolov5n'])
    parser.add_argument('-t', '--data-split-type',
                        choices=['video', 'frame'], default='frame')
    parser.add_argument('--area-shift', type=float, default=0)
    parser.add_argument('--no-cache', action='store_true')
    parser.add_argument('--int8', action='store_true')
    parser.add_argument('-n', '--num-levels', type=int, default=5)
    parser.add_argument('-b', '--border', type=int, default=2)
    parser.add_argument('-i', '--iou-thres', type=float, default=0.5)
    parser.add_argument('-c', '--conf-thres', type=float, default=0.3)
    args = parser.parse_args()

    main(args, **vars(args))
