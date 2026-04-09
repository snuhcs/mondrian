# Fine-Tuning Guide

This guide covers training a custom scale estimator and/or fine-tuning the packed canvas detector on your own data. All steps are optional — Mondrian works out of the box with pre-trained models (see [README.md](README.md)).

## Prerequisites

```bash
cd offline
conda env create -f environment.yml
conda activate mondrian-offline
```

Or manually: Python 3.10 + `pip install -r requirements.txt`. Python 3.10 is required for TFLite export compatibility with the runtime (TFLite 2.8).

## Pipeline Overview

```
Videos
  │
  ▼
Step 1 ─ label_frames ──→ frames/ + labels/ + frame_map.json
  │
  ▼
Step 2 ─ pack_canvases ──→ packed canvases
  │
  ├─ --mode scale ──→ packed/scale/ (for scale estimator)
  ├─ --mode train ──→ packed/train/ (for fine-tuning, optional)
  └─ --mode val   ──→ packed/val/  (for fine-tuning, optional)
  │
  ▼
Step 3 ─ train_scale_estimator ──→ scale_estimator.json
  │       (uses pre-trained YOLO by default)
  │
Step 4 ─ export_tflite ──→ model.tflite

Optional ─ finetune_packed_detector ──→ fine-tuned model.pt
           (requires packed/train + packed/val)
```

| Step | Script | Depends on | Required? |
|------|--------|------------|-----------|
| 1 | `label_frames.py` | Videos | Skip if you provide your own `frames/` + `labels/` + `frame_map.json` |
| 2 | `pack_canvases.py` | Step 1 output | `--mode scale` for Step 3; `--mode train/val` only if fine-tuning |
| 3 | `train_scale_estimator.py` | Step 2 (scale) + videos | Optional — use `scale_estimator_mta.json` as default |
| 4 | `export_tflite.py` | model .pt | Required if using pre-trained model (Option A) |
| F | `finetune_packed_detector.py` | Step 2 (train+val) | Optional — for higher packed canvas accuracy |

## Complete Workflow Examples

Replace `VIDEOS` and `WS` with your paths.

```bash
VIDEOS="/path/to/video1.mp4 /path/to/video2.mp4"
WS=./workspace
```

### Without fine-tuning (scale estimator only)

```bash
# Step 1: Label frames
python label_frames.py --videos $VIDEOS --output $WS

# Step 2-3: For each canvas size
for SIZE in 640 1280; do
  python pack_canvases.py --input $WS --canvas-size $SIZE --mode scale

  python train_scale_estimator.py \
    --scale-dir $WS/packed/scale/$SIZE \
    --videos $VIDEOS \
    --frame-map $WS/frame_map.json \
    --canvas-size $SIZE \
    --output $WS/scale_estimator_$SIZE.json
done

# Step 4: Export pre-trained model
python export_tflite.py --model yolov5mu.pt --imgsz 640
python export_tflite.py --model yolov5mu.pt --imgsz 1280
```

### With fine-tuning (full pipeline)

```bash
# Step 1: Label frames
python label_frames.py --videos $VIDEOS --output $WS

for SIZE in 640 1280; do
  # Step 2: Pack all canvases
  python pack_canvases.py --input $WS --canvas-size $SIZE --mode all

  # Fine-tune detector
  python finetune_packed_detector.py \
    --data-dir $WS \
    --canvas-size $SIZE \
    --epochs 50 \
    --output $WS/models_$SIZE

  MODEL=$WS/models_$SIZE/yolov5mu/weights/best.pt

  # Step 3: Train scale estimator with fine-tuned model
  python train_scale_estimator.py \
    --scale-dir $WS/packed/scale/$SIZE \
    --videos $VIDEOS \
    --frame-map $WS/frame_map.json \
    --model $MODEL \
    --canvas-size $SIZE \
    --output $WS/scale_estimator_$SIZE.json

  # Step 4: Export fine-tuned model
  python export_tflite.py --model $MODEL --imgsz $SIZE
done
```

## Step Reference

### Step 1: Label Frames

Sample frames from videos and auto-label with a large YOLO model.

```bash
python label_frames.py \
  --videos /path/to/video1.mp4 /path/to/video2.mp4 \
  --output ./workspace
```

| | Details |
|---|---|
| **Input** | Video files (any format OpenCV supports) |
| **Output** | `workspace/frames/*.jpg` — sampled frames |
| | `workspace/labels/*.txt` — YOLO labels (one per frame) |
| | `workspace/frame_map.json` — frame-to-video mapping |
| | `workspace/debug/labels/*.jpg` — sample frames with bboxes drawn (first 20) |

**Label format** (per line): `class_id center_x center_y width height` (all normalized to [0,1])

Skip this step if you have your own labels. Place frames as JPEG in `frames/`, labels as YOLO txt in `labels/`, and create a `frame_map.json` mapping `image_name → [video_path, frame_index]`.

| Argument | Default | Description |
|----------|---------|-------------|
| `--videos` | *(required)* | Input video files |
| `--output` | *(required)* | Output workspace directory |
| `--teacher-model` | `yolov5xu` | Large YOLO model for detection |
| `--target-classes` | `person` | COCO class names to detect |
| `--sample-fps` | `1.0` | Frames sampled per second |
| `--conf-thres` | `0.5` | Detection confidence threshold |

### Step 2: Pack Canvases

Generate packed canvas images from labeled frames. Requires Step 1 output (`frames/` + `labels/`).

```bash
python pack_canvases.py --input ./workspace --canvas-size 1280 --mode scale
```

| | Details |
|---|---|
| **Input** | `workspace/frames/` + `workspace/labels/` |
| **Output (train/val)** | `workspace/packed/{train,val}/{size}/images/*.jpg` — canvas images |
| | `workspace/packed/{train,val}/{size}/labels/*.txt` — YOLO labels |
| **Output (scale)** | `workspace/packed/scale/{size}/images/*.jpg` — canvas images |
| | `workspace/packed/scale/{size}/labels/*.txt` — YOLO labels |
| | `workspace/packed/scale/{size}/info/*.json` — packing metadata |
| **Debug** | `workspace/debug/packed_{mode}/*.jpg` — sample canvases with bboxes drawn (first 20) |

| Argument | Default | Description |
|----------|---------|-------------|
| `--input` | *(required)* | Workspace directory |
| `--canvas-size` | *(required)* | Canvas size in pixels |
| `--mode` | *(required)* | `train`, `val`, `scale`, or `all` |
| `--target-sizes` | `32 64 96 128` | ROI sizes for train/val mode |
| `--target-scales` | `0.1 0.2 ... 1.0` | Scales for scale mode |
| `--num-train-canvases` | `500` | Target train canvas count |
| `--num-val-canvases` | `100` | Target val canvas count |
| `--val-ratio` | `0.2` | Fraction of data for validation |

### Step 3: Train Scale Estimator

Train a decision tree that predicts how much each ROI can be downscaled at runtime while still being detected. Requires Step 2 output (`packed/scale/`) and the original videos (for optical flow features).

By default, uses a pre-trained YOLOv5mu model. Pass `--model` to use a fine-tuned model instead.

```bash
python train_scale_estimator.py \
  --scale-dir ./workspace/packed/scale/1280 \
  --videos /path/to/video1.mp4 /path/to/video2.mp4 \
  --frame-map ./workspace/frame_map.json \
  --canvas-size 1280 \
  --output ./workspace/scale_estimator.json
```

| | Details |
|---|---|
| **Input** | Scale canvases (from Step 2) + original videos |
| **Output** | `scale_estimator.json` — decision tree for runtime |

| Argument | Default | Description |
|----------|---------|-------------|
| `--scale-dir` | *(required)* | Scale canvas directory |
| `--videos` | *(required)* | Original videos (for optical flow) |
| `--frame-map` | *(required)* | Path to frame_map.json |
| `--model` | `yolov5mu.pt` | YOLO model for detection (pre-trained or fine-tuned .pt path) |
| `--canvas-size` | *(required)* | Canvas size |
| `--output` | *(required)* | Output scale_estimator.json path |
| `--num-levels` | `5` | Scale quantization levels |
| `--max-depth` | `10` | Decision tree max depth |

### Step 4: Export to TFLite

Convert a YOLO model to TFLite for on-device deployment.

```bash
python export_tflite.py --model yolov5mu.pt --imgsz 1280
```

| | Details |
|---|---|
| **Input** | Model `.pt` file (pre-trained or fine-tuned) |
| **Output** | Model `.tflite` file (same directory as input) |

| Argument | Default | Description |
|----------|---------|-------------|
| `--model` | *(required)* | Path to .pt model |
| `--imgsz` | *(required)* | Input image size |
| `--half` | `True` | FP16 quantization |
| `--int8` | `False` | INT8 quantization |

### Optional: Fine-Tune Detector

Fine-tune a YOLO model on packed canvases for higher accuracy. This teaches the model to detect objects in Mondrian's packed layout (gray background, bordered ROI patches) and adapts it to your target domain.

**When to use:** When pre-trained YOLO accuracy on packed canvases is insufficient for your use case.

```bash
# Generate train/val canvases:
python pack_canvases.py --input ./workspace --canvas-size 1280 --mode train
python pack_canvases.py --input ./workspace --canvas-size 1280 --mode val

# Fine-tune:
python finetune_packed_detector.py \
  --data-dir ./workspace \
  --canvas-size 1280 \
  --epochs 50 \
  --output ./workspace/models
```

| | Details |
|---|---|
| **Input** | `workspace/packed/train/{size}/` + `workspace/packed/val/{size}/` |
| **Output** | `workspace/models/yolov5mu/weights/best.pt` |

After fine-tuning, use the resulting model in Step 3 (`--model`) and Step 4.

| Argument | Default | Description |
|----------|---------|-------------|
| `--data-dir` | *(required)* | Workspace directory (must contain `packed/`) |
| `--canvas-size` | *(required)* | Must match packed canvas size |
| `--model` | `yolov5mu` | YOLO model to fine-tune |
| `--target-classes` | `person` | Class names |
| `--epochs` | `50` | Training epochs |
| `--batch-size` | `16` | Batch size |
| `--output` | *(required)* | Output directory for weights |

## File Formats

All intermediate files use standard formats so you can prepare them yourself:

| File | Format |
|------|--------|
| `frames/*.jpg` | JPEG images |
| `labels/*.txt` | YOLO: `class_id cx cy w h` (normalized, one line per object) |
| `frame_map.json` | `{"image_name": ["video_path", frame_index], ...}` |
| `packed/*/images/*.jpg` | JPEG packed canvas images |
| `packed/*/labels/*.txt` | YOLO labels for packed canvases |
| `packed/scale/*/info/*.json` | Packing metadata (ROI positions, scales) |
| `split.json` | `{"train": [...], "val": [...]}` frame name lists |
| `scale_estimator.json` | Decision tree with feature names and thresholds |

## Debug Visualizations

Each step automatically saves a small number of annotated images to `workspace/debug/` for visual inspection:

- **`debug/labels/`** — Original frames with auto-label bounding boxes (from `label_frames.py`)
- **`debug/packed_train/`** — Packed training canvases with bounding boxes (from `pack_canvases.py`)
- **`debug/packed_val/`** — Packed validation canvases with bounding boxes
- **`debug/packed_scale/`** — Packed scale canvases with bounding boxes

Up to 20 samples are saved per step.
