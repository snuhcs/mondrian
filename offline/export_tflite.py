#!/usr/bin/env python3
"""
Step 5: Export a YOLO model to TFLite format.

Input:  Model .pt file
Output: Model .tflite file
"""

import argparse
from pathlib import Path


def main():
    parser = argparse.ArgumentParser(
        description='Export YOLO model to TFLite',
        formatter_class=argparse.ArgumentDefaultsHelpFormatter)
    parser.add_argument('--model', required=True,
                        help='Path to .pt model file')
    parser.add_argument('--imgsz', type=int, required=True,
                        help='Input image size')
    parser.add_argument('--format', default='tflite',
                        choices=['tflite', 'onnx', 'torchscript'],
                        help='Export format')
    parser.add_argument('--half', action='store_true', default=True,
                        help='FP16 quantization')
    parser.add_argument('--int8', action='store_true',
                        help='INT8 quantization (overrides --half)')
    parser.add_argument('--output', default=None,
                        help='Output path (default: same directory as model)')
    args = parser.parse_args()

    from ultralytics import YOLO

    model = YOLO(args.model)
    export_kwargs = dict(format=args.format, imgsz=args.imgsz)

    if args.int8:
        export_kwargs['int8'] = True
    elif args.half:
        export_kwargs['half'] = True

    result = model.export(**export_kwargs)
    print(f'Exported: {result}')


if __name__ == '__main__':
    main()
