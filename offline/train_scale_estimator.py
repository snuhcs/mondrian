#!/usr/bin/env python3
"""
Step 4: Train the ROI scale estimator.

Input:  Scale canvases + YOLO model + original videos
Output: scaler.json (decision tree for runtime ROI scaling)

The scale estimator predicts how much each ROI can be downscaled at runtime
while still being detected. It uses optical flow features and ROI geometry
to make this prediction via a lightweight decision tree.
"""

import argparse
import json
from collections import defaultdict
from pathlib import Path

import cv2
import numpy as np
from scipy.optimize import linear_sum_assignment
from sklearn.tree import DecisionTreeClassifier
from tqdm import tqdm


# ─── Geometry ─────────────────────────────────────────────


class Rect:
    __slots__ = ('l', 't', 'r', 'b')

    def __init__(self, l, t, r, b):
        self.l, self.t, self.r, self.b = float(l), float(t), float(r), float(b)

    @property
    def width(self):
        return self.r - self.l

    @property
    def height(self):
        return self.b - self.t

    @property
    def max_length(self):
        return max(self.width, self.height)

    @property
    def area(self):
        return self.width * self.height

    @property
    def hw_ratio(self):
        return self.height / self.width if self.width > 0 else 0

    def iou(self, o):
        il, it = max(self.l, o.l), max(self.t, o.t)
        ir, ib = min(self.r, o.r), min(self.b, o.b)
        if ir <= il or ib <= it:
            return 0.0
        inter = (ir - il) * (ib - it)
        return inter / (self.area + o.area - inter)

    def contains(self, o, ratio=0.99):
        ol = max(self.l, o.l)
        ot = max(self.t, o.t)
        orr = min(self.r, o.r)
        ob = min(self.b, o.b)
        if orr <= ol or ob <= ot:
            return False
        overlap = (orr - ol) * (ob - ot)
        return overlap / o.area >= ratio if o.area > 0 else False


# ─── Optical Flow Features ────────────────────────────────


FEATURE_NAMES = [
    'maxEdgeLength', 'area', 'xyRatio',
    'shiftAvg', 'shiftStd', 'shiftNcc', 'avgErr',
]


def compute_of_features(prev_gray, curr_gray, rects):
    """Compute optical flow features for ROIs between two frames."""
    if not rects:
        return []

    points_per_roi = []
    for r in rects:
        l, t, ri, b = int(r.l), int(r.t), int(r.r), int(r.b)
        roi_gray = curr_gray[t:b, l:ri]
        if roi_gray.size == 0:
            pts = np.array([[(l + ri) / 2, (t + b) / 2]], dtype=np.float32)
        else:
            pts = cv2.goodFeaturesToTrack(
                roi_gray, maxCorners=50, qualityLevel=0.01,
                minDistance=5, blockSize=3)
            if pts is None or len(pts) == 0:
                pts = np.array([[(l + ri) / 2, (t + b) / 2]], dtype=np.float32)
            else:
                pts = pts.reshape(-1, 2)
                pts[:, 0] += l
                pts[:, 1] += t
        points_per_roi.append(pts)

    all_pts = np.vstack(points_per_roi).astype(np.float32)
    prev_pts, status, errs = cv2.calcOpticalFlowPyrLK(
        curr_gray, prev_gray, all_pts, None,
        winSize=(15, 15), maxLevel=2,
        criteria=(cv2.TERM_CRITERIA_COUNT | cv2.TERM_CRITERIA_EPS, 10, 0.03))
    status = status.reshape(-1)
    errs = errs.reshape(-1)

    features = []
    idx = 0
    for roi_pts in points_per_roi:
        n = len(roi_pts)
        shifts = roi_pts - prev_pts[idx:idx + n]
        st = status[idx:idx + n]
        er = errs[idx:idx + n]
        idx += n

        valid = st == 1
        if not np.any(valid):
            features.append([0, 0, 0, 100, 100])
            continue

        vs = shifts[valid]
        ve = er[valid]
        # Filter outliers
        sizes = vs[:, 0] ** 2 + vs[:, 1] ** 2
        q1 = np.quantile(sizes, 0.25) if len(sizes) > 1 else 0
        mask = sizes >= q1
        fs, fe = vs[mask], ve[mask]
        if len(fs) == 0:
            fs, fe = vs, ve

        shift_avg = np.mean(fs, axis=0)
        shift_std = np.std(fs, axis=0)
        # NCC
        if len(fs) > 1:
            ncc_sum, ncc_count = 0.0, 0
            for i in range(len(fs)):
                for j in range(i + 1, len(fs)):
                    ni = fs[i, 0] ** 2 + fs[i, 1] ** 2
                    nj = fs[j, 0] ** 2 + fs[j, 1] ** 2
                    if ni > 0 and nj > 0:
                        ncc_sum += (fs[i, 0] * fs[j, 0] + fs[i, 1] * fs[j, 1]) \
                                   / np.sqrt(ni * nj)
                    ncc_count += 1
            ncc = ncc_sum / ncc_count if ncc_count > 0 else 0
        else:
            ncc = 0

        features.append([
            shift_avg[0] ** 2 + shift_avg[1] ** 2,
            shift_std[0] ** 2 + shift_std[1] ** 2,
            ncc,
            np.mean(fe),
            0,  # placeholder
        ])

    return features


def feature_vector(rect, of_feat):
    """Build 7-element feature vector for a ROI."""
    return [
        rect.max_length,
        rect.area,
        rect.hw_ratio,
        of_feat[0],  # shiftAvg
        of_feat[1],  # shiftStd
        of_feat[2],  # shiftNcc
        of_feat[3],  # avgErr
    ]


# ─── Detection Matching ──────────────────────────────────


def match_detection(roi_region, pred_boxes, src_box, iou_thres, conf_thres):
    """Check if any prediction matches the source box within the ROI region."""
    for pb in pred_boxes:
        if roi_region.contains(pb['rect']) and \
           pb['rect'].iou(src_box) > iou_thres and \
           pb['conf'] > conf_thres:
            return True
    return False


def find_safe_scale(scores):
    """Find the smallest scale index where detection is stable."""
    min_idx, c0, c1 = None, 0, 0
    for i, s in enumerate(scores):
        if min_idx is None:
            if s == 1:
                min_idx, c0, c1 = i, 0, 1
        else:
            if s == 0:
                c0 += 1
            else:
                c1 += 1
            if c0 >= c1:
                min_idx = None
    return min_idx if min_idx is not None else len(scores) - 1


# ─── Tree Export ──────────────────────────────────────────


def tree_to_json(tree, feature_names):
    """Export sklearn DecisionTree to a JSON-serializable dict."""
    t = tree.tree_

    def recurse(node):
        if t.feature[node] == -2:  # leaf
            return {'value': int(np.argmax(t.value[node][0]))}
        return {
            'feature': feature_names[t.feature[node]],
            'threshold': round(float(t.threshold[node]), 5),
            'left': recurse(t.children_left[node]),
            'right': recurse(t.children_right[node]),
        }

    return recurse(0)


# ─── Main ─────────────────────────────────────────────────


def main():
    parser = argparse.ArgumentParser(
        description='Train ROI scale estimator',
        formatter_class=argparse.ArgumentDefaultsHelpFormatter)
    parser.add_argument('--scale-dir', required=True,
                        help='Scale canvases directory '
                             '(packed/scale/{canvas_size})')
    parser.add_argument('--videos', nargs='+', required=True,
                        help='Original video files (for optical flow)')
    parser.add_argument('--frame-map', required=True,
                        help='Path to frame_map.json')
    parser.add_argument('--model', default='yolov5mu.pt',
                        help='YOLO model (.pt path or model name)')
    parser.add_argument('--canvas-size', type=int, required=True)
    parser.add_argument('--output', required=True,
                        help='Output scaler.json path')
    parser.add_argument('--iou-thres', type=float, default=0.5,
                        help='IoU threshold for detection matching')
    parser.add_argument('--conf-thres', type=float, default=0.3,
                        help='Confidence threshold for detection matching')
    parser.add_argument('--num-levels', type=int, default=5,
                        help='Quantization levels for safe area')
    parser.add_argument('--max-depth', type=int, default=10,
                        help='Decision tree max depth')
    parser.add_argument('--border', type=int, default=2)
    args = parser.parse_args()

    from ultralytics import YOLO

    scale_dir = Path(args.scale_dir)
    info_dir = scale_dir / 'info'
    img_dir = scale_dir / 'images'
    assert info_dir.is_dir(), f'info/ not found in {scale_dir}'

    # ── Step 1: Run detection on scale canvases ──
    print(f'Running {args.model} on scale canvases...')
    model = YOLO(args.model)
    results = model.predict(
        source=str(img_dir),
        conf=0.1,  # low threshold — we filter later
        save=False, verbose=False, stream=True,
    )

    # Collect predictions per canvas
    canvas_preds = {}
    for result in tqdm(results, desc='Detecting'):
        name = Path(result.path).stem
        boxes = []
        for box in result.boxes:
            x1, y1, x2, y2 = box.xyxy[0].tolist()
            boxes.append({
                'rect': Rect(x1, y1, x2, y2),
                'conf': float(box.conf),
            })
        canvas_preds[name] = boxes

    # ── Step 2: Match detections to ROIs, compute safe scales ──
    print('Computing safe scales...')
    with open(args.frame_map) as f:
        frame_map = json.load(f)

    # Group ROIs: (image_name, roi_key) -> {scale: detected?}
    roi_scales = defaultdict(dict)  # (img_name, bbox_key) -> {scale: 0/1}
    roi_rects = {}  # (img_name, bbox_key) -> Rect (original roi_rect)
    roi_bboxes = {}  # (img_name, bbox_key) -> Rect (original bbox)

    info_files = sorted(info_dir.glob('*.json'))
    for info_path in tqdm(info_files, desc='Matching'):
        canvas_name = info_path.stem
        with open(info_path) as f:
            entries = json.load(f)

        preds = canvas_preds.get(canvas_name, [])

        for entry in entries:
            img_name = entry['image_name']
            scale = entry['scale']
            px, py = entry['pack_x'], entry['pack_y']
            rw, rh = entry['resized_w'], entry['resized_h']
            border = entry['border']
            roi_rect = entry['roi_rect']
            bboxes = entry['bboxes']

            if len(bboxes) != 1:
                continue  # Scale mode should have individual ROIs

            bbox = bboxes[0]
            bbox_key = f'{bbox[0]}_{bbox[1]}_{bbox[2]}_{bbox[3]}'
            key = (img_name, bbox_key)

            # ROI region in canvas (pixel coordinates)
            roi_region = Rect(
                px + border,
                py + border,
                px + border + rw - 2 * border,
                py + border + rh - 2 * border,
            )

            # Source box in canvas pixel coordinates
            or_l, or_t, or_r, or_b = roi_rect
            src_box = Rect(
                (bbox[0] - or_l) * scale + px + border,
                (bbox[1] - or_t) * scale + py + border,
                (bbox[2] - or_l) * scale + px + border,
                (bbox[3] - or_t) * scale + py + border,
            )

            # Predictions are already in pixel coordinates
            detected = match_detection(
                roi_region, preds, src_box,
                args.iou_thres, args.conf_thres)

            roi_scales[key][scale] = 1 if detected else 0
            roi_rects[key] = Rect(*roi_rect)
            roi_bboxes[key] = Rect(*bbox)

    # Compute safe scale for each ROI
    safe_data = {}  # key -> safe_scale
    for key, scale_dict in roi_scales.items():
        scales = sorted(scale_dict.keys())
        scores = [scale_dict[s] for s in scales]
        safe_idx = find_safe_scale(scores)
        safe_data[key] = {
            'safe_scale': scales[safe_idx],
            'safe_area': roi_rects[key].area * scales[safe_idx] ** 2,
        }

    print(f'{len(safe_data)} ROIs with safe scales computed')

    # ── Step 3: Extract optical flow features ──
    print('Extracting optical flow features...')

    # Group by video
    video_rois = defaultdict(list)  # video_path -> [(key, frame_index)]
    for key in safe_data:
        img_name = key[0]
        if img_name not in frame_map:
            continue
        vpath, fidx = frame_map[img_name]
        video_rois[vpath].append((key, fidx))

    of_features = {}  # key -> [5 floats]
    for vpath, roi_list in video_rois.items():
        frame_indices = sorted(set(fi for _, fi in roi_list))
        if not frame_indices:
            continue

        # Load needed frames (current + previous for OF)
        cap = cv2.VideoCapture(vpath)
        frames = {}
        needed = set()
        for fi in frame_indices:
            needed.add(fi)
            if fi > 0:
                needed.add(fi - 1)
        for fi in sorted(needed):
            cap.set(cv2.CAP_PROP_POS_FRAMES, fi)
            ret, frame = cap.read()
            if ret:
                frames[fi] = frame
        cap.release()

        # Compute OF per frame
        for fi in tqdm(frame_indices, desc=f'OF {Path(vpath).stem}'):
            prev_fi = fi - 1
            if fi not in frames or prev_fi not in frames:
                continue

            curr_gray = cv2.cvtColor(frames[fi], cv2.COLOR_BGR2GRAY)
            prev_gray = cv2.cvtColor(frames[prev_fi], cv2.COLOR_BGR2GRAY)

            # Get ROI rects for this frame
            frame_keys = [k for k, fid in roi_list if fid == fi]
            rects = [roi_rects[k] for k in frame_keys]

            feats = compute_of_features(prev_gray, curr_gray, rects)
            for k, feat in zip(frame_keys, feats):
                of_features[k] = feat

    # ── Step 4: Train decision tree ──
    print('Training decision tree...')
    X, Y, orig_areas = [], [], []
    for key in safe_data:
        if key not in of_features:
            continue
        rect = roi_rects[key]
        of_feat = of_features[key]
        X.append(feature_vector(rect, of_feat))
        Y.append(safe_data[key]['safe_area'])
        orig_areas.append(rect.area)

    X = np.array(X)
    Y = np.array(Y)
    orig_areas = np.array(orig_areas)

    if len(X) == 0:
        print('ERROR: No training data. Check scale canvases and model.')
        return

    # Train/test split (80/20 random)
    n = len(X)
    indices = np.random.permutation(n)
    split = int(n * 0.8)
    train_idx, test_idx = indices[:split], indices[split:]

    X_train, Y_train = X[train_idx], Y[train_idx]
    X_test, Y_test = X[test_idx], Y[test_idx]

    # Quantize Y
    thresholds = [float(np.quantile(Y_train, i / args.num_levels))
                  for i in range(1, args.num_levels)]
    Y_train_q = np.digitize(Y_train, thresholds, right=True)
    Y_test_q = np.digitize(Y_test, thresholds, right=True)

    clf = DecisionTreeClassifier(max_depth=args.max_depth)
    clf.fit(X_train, Y_train_q)

    # Evaluate
    Y_pred_q = clf.predict(X_test)
    accuracy = np.mean(Y_pred_q == Y_test_q)
    print(f'Accuracy: {accuracy:.3f} ({len(X_train)} train, {len(X_test)} test)')

    # Feature importances
    importances = list(zip(FEATURE_NAMES,
                           [round(v, 4) for v in clf.feature_importances_]))
    print(f'Feature importances: {importances}')

    # ── Step 5: Export ──
    output = Path(args.output)
    output.parent.mkdir(parents=True, exist_ok=True)

    scaler_json = {
        'feature_names': FEATURE_NAMES,
        'num_levels': args.num_levels,
        'thresholds': thresholds + [float('inf')],
        'tree': tree_to_json(clf, FEATURE_NAMES),
        'accuracy': round(accuracy, 4),
        'feature_importances': dict(importances),
        'num_train': len(X_train),
        'num_test': len(X_test),
    }

    with open(output, 'w') as f:
        json.dump(scaler_json, f, indent=2)

    print(f'Done: {output}')


if __name__ == '__main__':
    main()
