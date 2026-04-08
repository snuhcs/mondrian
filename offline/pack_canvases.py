#!/usr/bin/env python3
"""
Step 2: Generate packed canvases from labeled frames.

Input:  workspace/ with frames/*.jpg + labels/*.txt + frame_map.json
Output: workspace/packed/{mode}/{canvas_size}/images/*.jpg + labels/*.txt

Modes:
  train — ROIs resized to fixed target sizes, for detector fine-tuning
  val   — same as train, using held-out frames
  scale — each ROI at multiple relative scales, for scale estimator training
  all   — runs train + val + scale
"""

import argparse
import json
import random
from collections import defaultdict
from pathlib import Path

import cv2
import numpy as np
from tqdm import tqdm


# ─── Geometry ─────────────────────────────────────────────


class Rect:
    __slots__ = ('l', 't', 'r', 'b')

    def __init__(self, l, t, r, b):
        self.l, self.t, self.r, self.b = int(l), int(t), int(r), int(b)

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

    def clip(self, c):
        return Rect(max(self.l, c.l), max(self.t, c.t),
                     min(self.r, c.r), min(self.b, c.b))

    def merge(self, o):
        return Rect(min(self.l, o.l), min(self.t, o.t),
                     max(self.r, o.r), max(self.b, o.b))

    def pad(self, p):
        if isinstance(p, (int, float)):
            p = [p] * 4
        elif len(p) == 2:
            p = [p[0], p[1], p[0], p[1]]
        return Rect(self.l - p[0], self.t - p[1],
                     self.r + p[2], self.b + p[3])

    def iou(self, o):
        il, it = max(self.l, o.l), max(self.t, o.t)
        ir, ib = min(self.r, o.r), min(self.b, o.b)
        if ir <= il or ib <= it:
            return 0.0
        inter = (ir - il) * (ib - it)
        return inter / (self.area + o.area - inter)


# ─── ROI Classes ──────────────────────────────────────────


class ROI:
    def __init__(self, image_name, bbox, class_id, frame_w, frame_h,
                 avg_padding=20):
        self.image_name = image_name
        self.class_id = class_id
        clip = Rect(0, 0, frame_w, frame_h)
        pad_vals = [np.random.randint(max(1, avg_padding // 2),
                                      max(2, avg_padding * 3 // 2))
                    for _ in range(4)]
        self.bbox = bbox.clip(clip)
        self.rect = bbox.pad(pad_vals).clip(clip)


class MergedROI:
    def __init__(self, rois, border, target_size=None, target_scale=None):
        self.image_name = rois[0].image_name
        self.rois = rois
        self.rect = rois[0].rect
        for roi in rois[1:]:
            self.rect = self.rect.merge(roi.rect)
        self.border = border

        if target_scale is not None:
            min_ml = min(r.rect.max_length for r in rois)
            self._target_size = max(1, int(round(min_ml * target_scale)))
            self._scale_value = target_scale
        elif target_size is not None:
            self._target_size = target_size
            self._scale_value = None
        else:
            self._target_size = self.rect.max_length
            self._scale_value = None

    @property
    def target_size(self):
        return min(self._target_size, self.rect.max_length)

    @property
    def scale(self):
        ml = self.rect.max_length
        return min(1.0, self.target_size / ml) if ml > 0 else 1.0

    @property
    def resized_wh_inner(self):
        return (max(1, int(self.rect.width * self.scale)),
                max(1, int(self.rect.height * self.scale)))

    @property
    def resized_wh(self):
        w, h = self.resized_wh_inner
        return w + 2 * self.border, h + 2 * self.border

    @property
    def pack_area(self):
        w, h = self.resized_wh
        return w * h

    def resized_to(self, target_size):
        return MergedROI(self.rois, self.border, target_size=target_size)

    def rescaled_to(self, target_scale):
        return MergedROI(self.rois, self.border, target_scale=target_scale)

    def render(self, frame):
        r = self.rect
        roi_img = frame[r.t:r.b, r.l:r.r]
        if roi_img.size == 0:
            w, h = self.resized_wh
            return np.full((h, w, 3), 114, dtype=np.uint8)
        wi, hi = self.resized_wh_inner
        resized = cv2.resize(roi_img, (wi, hi))
        w, h = self.resized_wh
        patch = np.full((h, w, 3), 255, dtype=np.uint8)
        patch[self.border:self.border + hi,
              self.border:self.border + wi] = resized
        return patch


# ─── Packing ──────────────────────────────────────────────


def merge_overlapping(merged_list, iou_thres):
    for i in range(len(merged_list)):
        for j in range(i + 1, len(merged_list)):
            if merged_list[i].rect.iou(merged_list[j].rect) > iou_thres:
                combined = MergedROI(
                    merged_list[i].rois + merged_list[j].rois,
                    merged_list[i].border)
                merged_list[i] = combined
                merged_list.pop(j)
                return merge_overlapping(merged_list, iou_thres)
    return merged_list


def pack_rois(rois_to_pack, canvas_size, target_count=None):
    """Best-fit decreasing bin packing. Auto-repeats to reach target_count."""

    def _do_pack(rois, max_canvases):
        rois = sorted(rois, key=lambda m: m.pack_area, reverse=True)
        canvases, free = [{}], [Rect(0, 0, canvas_size, canvas_size)]
        for m in rois:
            w, h = m.resized_wh
            if w > canvas_size or h > canvas_size:
                continue
            best_idx, best_rem = -1, canvas_size ** 2
            while best_idx == -1:
                for idx, fr in enumerate(free):
                    if fr.width >= w and fr.height >= h:
                        rem = fr.area - w * h
                        if rem < best_rem:
                            best_rem, best_idx = rem, idx
                if best_idx == -1:
                    if max_canvases and len(canvases) >= max_canvases:
                        return canvases
                    canvases.append({})
                    free = [Rect(0, 0, canvas_size, canvas_size)]
            fr = free[best_idx]
            canvases[-1][(fr.l, fr.t)] = m
            free.pop(best_idx)
            # Split remaining space
            if fr.width >= fr.height:
                r0 = Rect(fr.l + w, fr.t, fr.r, fr.b)
                r1 = Rect(fr.l, fr.t + h, fr.l + w, fr.b)
            else:
                r0 = Rect(fr.l, fr.t + h, fr.r, fr.b)
                r1 = Rect(fr.l + w, fr.t, fr.r, fr.t + h)
            for r in (r0, r1):
                if r.area > 0:
                    free.append(r)
        return canvases

    if not target_count:
        return _do_pack(rois_to_pack, None)

    trial = _do_pack(rois_to_pack, None)
    if len(trial) >= target_count:
        return _do_pack(rois_to_pack, target_count)

    max_repeats = 5
    repeats = min(max_repeats, max(2, -(-target_count // len(trial))))
    print(f'  {len(trial)} canvases from 1x data, '
          f'repeating {repeats}x (max {max_repeats}x)')
    expanded = rois_to_pack * repeats
    random.shuffle(expanded)
    return _do_pack(expanded, target_count)


# ─── Load ROIs ────────────────────────────────────────────


def load_rois(image_names, frames_dir, labels_dir, border,
              avg_padding, filter_size, merge_iou):
    """Load and merge ROIs from labeled frames."""
    sample = cv2.imread(str(frames_dir / f'{image_names[0]}.jpg'))
    frame_h, frame_w = sample.shape[:2]

    all_rois = []
    for name in image_names:
        lp = labels_dir / f'{name}.txt'
        if not lp.exists() or lp.stat().st_size == 0:
            continue
        try:
            labels = np.loadtxt(str(lp)).reshape(-1, 5)
        except Exception:
            continue
        for cls, cx, cy, w, h in labels:
            l = int((cx - w / 2) * frame_w)
            t = int((cy - h / 2) * frame_h)
            r = int((cx + w / 2) * frame_w)
            b = int((cy + h / 2) * frame_h)
            if max(r - l, b - t) < filter_size:
                continue
            all_rois.append(ROI(name, Rect(l, t, r, b), int(cls),
                                frame_w, frame_h, avg_padding))

    # Group by frame and merge overlapping
    by_frame = defaultdict(list)
    for roi in all_rois:
        by_frame[roi.image_name].append(MergedROI([roi], border))

    merged_all = []
    for name in tqdm(sorted(by_frame), desc='Merging ROIs'):
        merged_all.extend(merge_overlapping(by_frame[name], merge_iou))

    return merged_all, frame_w, frame_h


# ─── Save Canvases ────────────────────────────────────────


def save_canvases(canvases, canvas_size, frames_dir, img_dir, lbl_dir,
                  info_dir=None, debug_dir=None, num_debug=20):
    """Render and save packed canvases with labels."""
    img_dir.mkdir(parents=True, exist_ok=True)
    lbl_dir.mkdir(parents=True, exist_ok=True)
    if info_dir:
        info_dir.mkdir(parents=True, exist_ok=True)
    if debug_dir:
        debug_dir.mkdir(parents=True, exist_ok=True)

    frame_cache = {}

    for ci, pinfo in enumerate(tqdm(canvases, desc='Saving canvases')):
        canvas = np.full((canvas_size, canvas_size, 3), 114, dtype=np.uint8)
        label_lines = []
        info_entries = []
        bbox_pixels = []  # for debug visualization

        for (x, y), m in pinfo.items():
            if m.image_name not in frame_cache:
                frame_cache[m.image_name] = cv2.imread(
                    str(frames_dir / f'{m.image_name}.jpg'))

            w, h = m.resized_wh
            canvas[y:y + h, x:x + w] = m.render(frame_cache[m.image_name])

            scale = m.scale
            for roi in m.rois:
                cx = ((roi.bbox.l + roi.bbox.r) / 2 - m.rect.l) * scale \
                     + x + m.border
                cy = ((roi.bbox.t + roi.bbox.b) / 2 - m.rect.t) * scale \
                     + y + m.border
                bw = roi.bbox.width * scale
                bh = roi.bbox.height * scale
                label_lines.append(
                    f'{roi.class_id} '
                    f'{cx / canvas_size:.6f} {cy / canvas_size:.6f} '
                    f'{bw / canvas_size:.6f} {bh / canvas_size:.6f}')
                bbox_pixels.append((
                    int(cx - bw / 2), int(cy - bh / 2),
                    int(cx + bw / 2), int(cy + bh / 2), roi.class_id))

            if info_dir:
                info_entries.append({
                    'pack_x': x, 'pack_y': y,
                    'image_name': m.image_name,
                    'scale': round(scale, 4),
                    'roi_rect': [m.rect.l, m.rect.t, m.rect.r, m.rect.b],
                    'bboxes': [[r.bbox.l, r.bbox.t, r.bbox.r, r.bbox.b]
                               for r in m.rois],
                    'border': m.border,
                    'resized_w': m.resized_wh[0],
                    'resized_h': m.resized_wh[1],
                })

        cv2.imwrite(str(img_dir / f'canvas_{ci:05d}.jpg'), canvas)
        (lbl_dir / f'canvas_{ci:05d}.txt').write_text('\n'.join(label_lines))

        if info_dir and info_entries:
            with open(info_dir / f'canvas_{ci:05d}.json', 'w') as f:
                json.dump(info_entries, f)

        # Save debug visualization for first N canvases
        if debug_dir and ci < num_debug:
            dbg = canvas.copy()
            for x1, y1, x2, y2, cls in bbox_pixels:
                cv2.rectangle(dbg, (x1, y1), (x2, y2), (0, 255, 0), 1)
            cv2.imwrite(str(debug_dir / f'canvas_{ci:05d}.jpg'), dbg)

        if len(frame_cache) > 200:
            frame_cache.clear()


# ─── Modes ────────────────────────────────────────────────


def do_train_or_val(mode, image_names, args, frames_dir, labels_dir):
    target_sizes = args.target_sizes
    tc = args.num_train_canvases if mode == 'train' else args.num_val_canvases

    merged, fw, fh = load_rois(
        image_names, frames_dir, labels_dir,
        args.border, args.avg_padding, args.filter_size, args.iou_thres)

    # Interleave sizes
    shuffled = list(merged)
    random.shuffle(shuffled)
    resized = [m.resized_to(ts) for m in shuffled for ts in target_sizes]

    print(f'[{mode}] {len(merged)} merged ROIs '
          f'x {len(target_sizes)} sizes = {len(resized)} resized ROIs')
    print(f'[{mode}] Target: {tc} canvases')

    canvases = pack_rois(resized, args.canvas_size, tc)
    print(f'[{mode}] Packed into {len(canvases)} canvases')

    out = Path(args.input) / 'packed' / mode / str(args.canvas_size)
    debug_dir = Path(args.input) / 'debug' / f'packed_{mode}'
    save_canvases(canvases, args.canvas_size, frames_dir,
                  out / 'images', out / 'labels', debug_dir=debug_dir)
    print(f'[{mode}] Saved to {out}')


def do_scale(image_names, args, frames_dir, labels_dir):
    scales = args.target_scales

    # For scale mode: no merging, keep individual ROIs
    sample = cv2.imread(str(frames_dir / f'{image_names[0]}.jpg'))
    frame_h, frame_w = sample.shape[:2]

    all_rois = []
    for name in image_names:
        lp = labels_dir / f'{name}.txt'
        if not lp.exists() or lp.stat().st_size == 0:
            continue
        try:
            labels = np.loadtxt(str(lp)).reshape(-1, 5)
        except Exception:
            continue
        for cls, cx, cy, w, h in labels:
            l = int((cx - w / 2) * frame_w)
            t = int((cy - h / 2) * frame_h)
            r = int((cx + w / 2) * frame_w)
            b = int((cy + h / 2) * frame_h)
            if max(r - l, b - t) < args.filter_size:
                continue
            all_rois.append(ROI(name, Rect(l, t, r, b), int(cls),
                                frame_w, frame_h, avg_padding=args.avg_padding))

    # Create MergedROI (single ROI each) at each scale
    resized = []
    for roi in all_rois:
        m = MergedROI([roi], args.border)
        for s in scales:
            r = m.rescaled_to(s)
            # Filter: skip if resized too small
            wi, hi = r.resized_wh_inner
            if max(wi, hi) < args.filter_size and s < 0.99:
                continue
            resized.append(r)

    random.shuffle(resized)
    print(f'[scale] {len(all_rois)} ROIs '
          f'x {len(scales)} scales = {len(resized)} resized ROIs')

    canvases = pack_rois(resized, args.canvas_size)
    print(f'[scale] Packed into {len(canvases)} canvases')

    out = Path(args.input) / 'packed' / 'scale' / str(args.canvas_size)
    debug_dir = Path(args.input) / 'debug' / 'packed_scale'
    save_canvases(canvases, args.canvas_size, frames_dir,
                  out / 'images', out / 'labels',
                  info_dir=out / 'info', debug_dir=debug_dir)
    print(f'[scale] Saved to {out}')


# ─── Main ─────────────────────────────────────────────────


def main():
    parser = argparse.ArgumentParser(
        description='Pack labeled ROIs into training canvases',
        formatter_class=argparse.ArgumentDefaultsHelpFormatter)
    parser.add_argument('--input', required=True,
                        help='Workspace with frames/ and labels/')
    parser.add_argument('--canvas-size', type=int, required=True,
                        help='Output canvas size in pixels')
    parser.add_argument('--mode', required=True,
                        choices=['train', 'val', 'scale', 'all'],
                        help='Packing mode')
    # Train/val options
    parser.add_argument('--target-sizes', nargs='+', type=int,
                        default=[32, 64, 96, 128],
                        help='Target ROI sizes for train/val mode')
    parser.add_argument('--num-train-canvases', type=int, default=500)
    parser.add_argument('--num-val-canvases', type=int, default=100)
    parser.add_argument('--val-ratio', type=float, default=0.2,
                        help='Fraction of frames for validation')
    # Scale options
    parser.add_argument('--target-scales', nargs='+', type=float,
                        default=[0.1, 0.2, 0.3, 0.4, 0.5,
                                 0.6, 0.7, 0.8, 0.9, 1.0],
                        help='Target scales for scale mode')
    # Packing parameters
    parser.add_argument('--iou-thres', type=float, default=0.3)
    parser.add_argument('--border', type=int, default=2)
    parser.add_argument('--avg-padding', type=int, default=20)
    parser.add_argument('--filter-size', type=int, default=20)
    parser.add_argument('--seed', type=int, default=42)
    args = parser.parse_args()

    random.seed(args.seed)
    np.random.seed(args.seed)

    ws = Path(args.input)
    frames_dir = ws / 'frames'
    labels_dir = ws / 'labels'
    assert frames_dir.is_dir(), f'frames/ not found in {ws}'
    assert labels_dir.is_dir(), f'labels/ not found in {ws}'

    with open(ws / 'frame_map.json') as f:
        frame_map = json.load(f)

    all_names = sorted(frame_map.keys())

    modes = ['train', 'val', 'scale'] if args.mode == 'all' else [args.mode]

    # Train/val split (deterministic)
    if 'train' in modes or 'val' in modes:
        split_path = ws / 'split.json'
        if split_path.exists():
            with open(split_path) as f:
                split = json.load(f)
            train_names = split['train']
            val_names = split['val']
            print(f'Loaded existing split: '
                  f'{len(train_names)} train, {len(val_names)} val')
        else:
            video_groups = defaultdict(list)
            for name, (vpath, _) in frame_map.items():
                video_groups[vpath].append(name)
            videos = sorted(video_groups.keys())

            rng = random.Random(args.seed)
            if len(videos) >= 3:
                n_val = max(1, int(len(videos) * args.val_ratio))
                shuffled = list(videos)
                rng.shuffle(shuffled)
                train_names = [n for v in shuffled[n_val:]
                               for n in sorted(video_groups[v])]
                val_names = [n for v in shuffled[:n_val]
                             for n in sorted(video_groups[v])]
            else:
                shuffled = list(all_names)
                rng.shuffle(shuffled)
                cut = int(len(shuffled) * (1 - args.val_ratio))
                train_names = shuffled[:cut]
                val_names = shuffled[cut:]

            with open(split_path, 'w') as f:
                json.dump({'train': train_names, 'val': val_names}, f)
            print(f'Created split: '
                  f'{len(train_names)} train, {len(val_names)} val')

    for mode in modes:
        if mode in ('train', 'val'):
            names = train_names if mode == 'train' else val_names
            do_train_or_val(mode, names, args, frames_dir, labels_dir)
        elif mode == 'scale':
            do_scale(all_names, args, frames_dir, labels_dir)


if __name__ == '__main__':
    main()
