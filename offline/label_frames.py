#!/usr/bin/env python3
"""
Step 1: Sample frames from videos and auto-label with a large YOLO model.

Input:  Video files
Output: workspace/frames/*.jpg + workspace/labels/*.txt (YOLO format)

Label format (per line): class_id center_x center_y width height
  - All values normalized to [0, 1]
  - class_id is remapped to 0, 1, 2, ... based on --target-classes order
"""

import argparse
import json
from pathlib import Path

import cv2
from tqdm import tqdm


def main():
    parser = argparse.ArgumentParser(
        description='Auto-label video frames with a YOLO teacher model',
        formatter_class=argparse.ArgumentDefaultsHelpFormatter)
    parser.add_argument('--videos', nargs='+', required=True,
                        help='Input video files')
    parser.add_argument('--output', required=True,
                        help='Output workspace directory')
    parser.add_argument('--teacher-model', default='yolov5xu',
                        help='Large YOLO model for detection')
    parser.add_argument('--target-classes', nargs='+', default=['person'],
                        help='COCO class names to detect')
    parser.add_argument('--sample-fps', type=float, default=1.0,
                        help='Frames sampled per second from each video')
    parser.add_argument('--conf-thres', type=float, default=0.5,
                        help='Detection confidence threshold')
    args = parser.parse_args()

    from ultralytics import YOLO

    output = Path(args.output)
    frames_dir = output / 'frames'
    labels_dir = output / 'labels'
    frames_dir.mkdir(parents=True, exist_ok=True)
    labels_dir.mkdir(parents=True, exist_ok=True)

    # Load model and resolve class mapping
    model = YOLO(args.teacher_model)
    coco_names = model.names
    coco_to_local = {}
    for local_id, tc in enumerate(args.target_classes):
        for coco_id, name in coco_names.items():
            if name == tc:
                coco_to_local[coco_id] = local_id
                break
        else:
            raise ValueError(
                f"Class '{tc}' not in model. "
                f"Available: {sorted(set(coco_names.values()))}")
    target_coco_ids = list(coco_to_local.keys())

    # Sample frames
    frame_map = {}  # image_name -> [video_path, frame_index]
    for vp in args.videos:
        vp = Path(vp)
        cap = cv2.VideoCapture(str(vp))
        fps = cap.get(cv2.CAP_PROP_FPS)
        total = int(cap.get(cv2.CAP_PROP_FRAME_COUNT))
        interval = max(1, int(fps / args.sample_fps))
        indices = list(range(0, total, interval))
        print(f'{vp.name}: {total} frames, sampling {len(indices)} '
              f'(interval={interval}, fps={fps:.1f})')
        for fi in tqdm(indices, desc=f'Extracting {vp.stem}'):
            cap.set(cv2.CAP_PROP_POS_FRAMES, fi)
            ret, frame = cap.read()
            if not ret:
                break
            name = f'{vp.stem}_{fi:06d}'
            cv2.imwrite(str(frames_dir / f'{name}.jpg'), frame)
            frame_map[name] = [str(vp), fi]
        cap.release()

    with open(output / 'frame_map.json', 'w') as f:
        json.dump(frame_map, f)

    # Run detection
    print(f'Running {args.teacher_model} on {len(frame_map)} frames...')
    results = model.predict(
        source=str(frames_dir),
        conf=args.conf_thres,
        classes=target_coco_ids,
        save=False,
        verbose=False,
        stream=True,
    )

    n_det = 0
    for result in tqdm(results, total=len(frame_map), desc='Detecting'):
        img_name = Path(result.path).stem
        boxes = result.boxes
        if len(boxes) == 0:
            continue
        lines = []
        for box in boxes:
            coco_cls = int(box.cls)
            local_cls = coco_to_local[coco_cls]
            x, y, w, h = box.xywhn[0].tolist()
            lines.append(f'{local_cls} {x:.6f} {y:.6f} {w:.6f} {h:.6f}')
        (labels_dir / f'{img_name}.txt').write_text('\n'.join(lines))
        n_det += len(lines)

    print(f'Done: {n_det} detections in {len(frame_map)} frames')

    # Save debug visualizations (sample of labeled frames with bboxes)
    debug_dir = output / 'debug' / 'labels'
    debug_dir.mkdir(parents=True, exist_ok=True)
    label_files = sorted(labels_dir.glob('*.txt'))
    sample_files = label_files[:min(20, len(label_files))]
    for lf in sample_files:
        name = lf.stem
        img = cv2.imread(str(frames_dir / f'{name}.jpg'))
        if img is None:
            continue
        h, w = img.shape[:2]
        labels = []
        try:
            labels = __import__('numpy').loadtxt(str(lf)).reshape(-1, 5)
        except Exception:
            pass
        for cls, cx, cy, bw, bh in labels:
            x1 = int((cx - bw / 2) * w)
            y1 = int((cy - bh / 2) * h)
            x2 = int((cx + bw / 2) * w)
            y2 = int((cy + bh / 2) * h)
            cv2.rectangle(img, (x1, y1), (x2, y2), (0, 255, 0), 2)
            cv2.putText(img, f'{int(cls)}', (x1, y1 - 5),
                        cv2.FONT_HERSHEY_SIMPLEX, 0.5, (0, 255, 0), 1)
        cv2.imwrite(str(debug_dir / f'{name}.jpg'), img)
    print(f'Debug: {len(sample_files)} labeled samples in {debug_dir}')
    print(f'Output: {output}')


if __name__ == '__main__':
    main()
