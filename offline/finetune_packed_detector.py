#!/usr/bin/env python3
"""
Step 3: Fine-tune a YOLO detector on packed canvases.

Input:  Packed canvas directories (train + val, in YOLOv5 training format)
Output: Fine-tuned model weights (.pt)

Expected input structure:
    packed/
    ├── train/{canvas_size}/images/*.jpg + labels/*.txt
    └── val/{canvas_size}/images/*.jpg + labels/*.txt
"""

import argparse
from pathlib import Path


def main():
    parser = argparse.ArgumentParser(
        description='Fine-tune YOLO on packed canvases',
        formatter_class=argparse.ArgumentDefaultsHelpFormatter)
    parser.add_argument('--data-dir', required=True,
                        help='Directory containing packed/ with train and val')
    parser.add_argument('--canvas-size', type=int, required=True,
                        help='Canvas size (must match packed canvas size)')
    parser.add_argument('--model', default='yolov5mu',
                        help='YOLO model name to fine-tune (e.g., yolov5mu)')
    parser.add_argument('--target-classes', nargs='+', default=['person'],
                        help='Class names')
    parser.add_argument('--epochs', type=int, default=50)
    parser.add_argument('--batch-size', type=int, default=16)
    parser.add_argument('--output', required=True,
                        help='Output directory for model weights')
    args = parser.parse_args()

    from ultralytics import YOLO

    data_dir = Path(args.data_dir)
    packed_dir = data_dir / 'packed'
    cs = args.canvas_size

    train_img = packed_dir / 'train' / str(cs) / 'images'
    val_img = packed_dir / 'val' / str(cs) / 'images'
    assert train_img.is_dir(), f'Train images not found: {train_img}'
    assert val_img.is_dir(), f'Val images not found: {val_img}'

    # Create dataset.yaml
    dataset_yaml = data_dir / 'dataset.yaml'
    names_str = ', '.join(f"'{c}'" for c in args.target_classes)
    dataset_yaml.write_text(
        f'path: {packed_dir.resolve()}\n'
        f'train: train/{cs}/images\n'
        f'val: val/{cs}/images\n'
        f'nc: {len(args.target_classes)}\n'
        f'names: [{names_str}]\n'
    )

    output = Path(args.output)
    print(f'Fine-tuning {args.model} for {args.epochs} epochs '
          f'(canvas={cs}, classes={args.target_classes})')

    model = YOLO(f'{args.model}.pt')
    model.train(
        data=str(dataset_yaml),
        epochs=args.epochs,
        imgsz=cs,
        batch=args.batch_size,
        project=str(output),
        name=args.model,
        exist_ok=True,
    )
    best_pt = output / args.model / 'weights' / 'best.pt'
    print(f'Done: {best_pt}')


if __name__ == '__main__':
    main()
