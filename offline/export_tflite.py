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

    # Rename to a descriptive filename
    exported = Path(result)
    if exported.exists():
        model_name = Path(args.model).stem  # yolov5mu, yolov5lu, etc.
        quant = 'int8' if args.int8 else 'fp16'
        dest = args.output or str(exported.parent.parent / f'{model_name}-{args.imgsz}-{quant}.tflite')
        exported.rename(dest)
        print(f'Renamed: {dest}')

    # Fix output shape: newer ultralytics exports [1, 84, N] but the
    # C++ runtime expects [1, N, 84].  Re-write the TFLite model with a
    # Transpose appended to the output so the on-device code stays unchanged.
    if args.format == 'tflite':
        _fix_output_transpose(dest)


def _fix_output_transpose(tflite_path):
    """Transpose output from [1, 84, N] to [1, N, 84] if needed."""
    import tensorflow as tf

    interp = tf.lite.Interpreter(model_path=tflite_path)
    interp.allocate_tensors()
    out_detail = interp.get_output_details()[0]
    shape = out_detail['shape']

    # Already in [1, N, 84] — nothing to do
    if len(shape) == 3 and shape[2] == 84:
        print(f'Output shape {list(shape)} is already correct.')
        return

    # [1, 84, N] → need transpose
    if len(shape) == 3 and shape[1] == 84:
        print(f'Output shape {list(shape)} needs transpose to [1, {shape[2]}, 84].')
    else:
        print(f'Unexpected output shape {list(shape)}, skipping fix.')
        return

    in_detail = interp.get_input_details()[0]
    in_shape = in_detail['shape']

    # Build a minimal model that wraps the TFLite and transposes output
    saved_model_dir = str(Path(tflite_path).with_suffix('')) + '_saved_model'
    if not Path(saved_model_dir).exists():
        # Fallback: find sibling saved_model dirs
        parent = Path(tflite_path).parent
        candidates = sorted(parent.glob('*_saved_model'), key=lambda p: p.stat().st_mtime, reverse=True)
        if candidates:
            saved_model_dir = str(candidates[0])
        else:
            print(f'No saved_model found, cannot fix transpose.')
            return

    loaded = tf.saved_model.load(saved_model_dir)
    concrete = loaded.signatures['serving_default']
    input_key = list(concrete.structured_input_signature[1].keys())[0]
    output_key = list(concrete.structured_outputs.keys())[0]

    @tf.function(input_signature=[tf.TensorSpec(shape=in_shape, dtype=tf.float32)])
    def transposed_fn(x):
        result = concrete(**{input_key: x})
        out = result[output_key]
        return tf.transpose(out, perm=[0, 2, 1])

    # Convert to TFLite
    converter = tf.lite.TFLiteConverter.from_concrete_functions([transposed_fn.get_concrete_function()])
    converter.target_spec.supported_ops = [tf.lite.OpsSet.TFLITE_BUILTINS]
    if 'fp16' in tflite_path:
        converter.optimizations = [tf.lite.Optimize.DEFAULT]
        converter.target_spec.supported_types = [tf.float16]
    tflite_model = converter.convert()

    with open(tflite_path, 'wb') as f:
        f.write(tflite_model)

    # Verify
    interp2 = tf.lite.Interpreter(model_content=tflite_model)
    interp2.allocate_tensors()
    new_shape = interp2.get_output_details()[0]['shape']
    print(f'Fixed output shape: {list(new_shape)}')


if __name__ == '__main__':
    main()
