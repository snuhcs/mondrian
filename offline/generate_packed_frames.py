import argparse
from collections import defaultdict
from functools import partial
import json
from pathlib import Path
import random
import shutil
from typing import Dict, List, Tuple

import cv2
import numpy as np
import pandas as pd
from tqdm import tqdm


class Rect:
    def __init__(self, left: float, top: float, right: float, bottom: float):
        assert left <= right and top <= bottom
        self.l = left
        self.t = top
        self.r = right
        self.b = bottom

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

    def clip(self, clip_to: 'Rect') -> 'Rect':
        return Rect(
            max(self.l, clip_to.l),
            max(self.t, clip_to.t),
            min(self.r, clip_to.r),
            min(self.b, clip_to.b))

    def merge(self, other: 'Rect') -> 'Rect':
        return Rect(
            min(self.l, other.l),
            min(self.t, other.t),
            max(self.r, other.r),
            max(self.b, other.b))

    def pad(self, padding) -> 'Rect':
        if isinstance(padding, int):
            padding = [padding] * 4
        elif isinstance(padding, tuple) or isinstance(padding, list):
            if len(padding) == 1:
                padding = [padding[0]] * 4
            elif len(padding) == 2:
                padding = [padding[0]] * 2 + [padding[1]] * 2
            elif len(padding) == 4:
                pass
            else:
                raise ValueError(f'Invalid padding {padding}')
        else:
            raise ValueError(f'Invalid padding {padding}')
        return Rect(
            self.l - padding[0],
            self.t - padding[1],
            self.r + padding[2],
            self.b + padding[3])

    def intersects(self, other: 'Rect') -> bool:
        return self.l < other.r and \
            self.r > other.l and \
            self.t < other.b and \
            self.b > other.t

    def intersection(self, other: 'Rect') -> 'Rect':
        return Rect(
            max(self.l, other.l),
            max(self.t, other.t),
            min(self.r, other.r),
            min(self.b, other.b))

    def iou(self, other: 'Rect') -> float:
        if self.intersects(other):
            intersection = self.intersection(other)
            return intersection.area / (self.area + other.area - intersection.area)
        else:
            return 0


FRAME = Rect(0, 0, 1920, 1080)


class ROI:
    def __init__(self,
                 video_path: Path,
                 frame_index: int,
                 object_id: int,
                 bbox: Rect,
                 avg_padding: int) -> None:
        self.video_path = video_path
        self.frame_index = frame_index
        self.object_id = object_id
        self.padding = [
            np.random.randint(avg_padding // 2, avg_padding * 3 // 2)
            for _ in range(4)]
        self.bbox = bbox.clip(FRAME)
        self.rect = bbox.pad(self.padding).clip(FRAME)


class MergedROI:
    def __init__(self,
                 rois: List[ROI],
                 border: int,
                 target_size: int = None,
                 target_scale: float = None) -> None:
        assert len(rois) > 0
        assert set([roi.video_path for roi in rois]) == {rois[0].video_path}
        assert set([roi.frame_index for roi in rois]) == {rois[0].frame_index}
        assert target_size is None or target_scale is None
        self.video_path = rois[0].video_path
        self.frame_index = rois[0].frame_index
        self.rois = rois
        self.rect = rois[0].rect
        for roi in rois[1:]:
            self.rect = self.rect.merge(roi.rect)
        self.border = border
        if target_size is not None:
            assert target_size > 0
            self._target_size = target_size
        elif target_scale is not None:
            target_size = int(round(self.min_roi_max_length * target_scale))
            self._target_size = max(1, target_size)
        else:
            self._target_size = self.min_roi_max_length

    @classmethod
    def merge(cls, m1: 'MergedROI', m2: 'MergedROI') -> 'MergedROI':
        assert m1.border == m2.border
        return MergedROI(m1.rois + m2.rois, m1.border)

    @property
    def id(self):
        return f'{self.video_path.stem}_' \
               f'{self.frame_index}_' \
               f'{self.rect.l}_{self.rect.t}_{self.rect.r}_{self.rect.b}_' \
               f'{self.scale}'

    @property
    def src_boxes(self):
        return '_'.join([
            f'{roi.bbox.l}_{roi.bbox.t}_{roi.bbox.r}_{roi.bbox.b}'
            for roi in self.rois
        ])

    @property
    def min_roi_max_length(self):
        return min([r.rect.max_length for r in self.rois])

    @property
    def target_size(self):
        return min(self._target_size, self.rect.max_length)

    @property
    def scale(self):
        return min(1.0, self.target_size / self.min_roi_max_length)

    @property
    def resized_wh_wo_border(self):
        w_resized = max(1, int(self.rect.width * self.scale))
        h_resized = max(1, int(self.rect.height * self.scale))
        return w_resized, h_resized

    @property
    def resized_wh(self):
        w_resized, h_resized = self.resized_wh_wo_border
        w_border = w_resized + 2 * self.border
        h_border = h_resized + 2 * self.border
        return w_border, h_border

    @property
    def area(self):
        w_border, h_border = self.resized_wh
        return w_border * h_border

    @property
    def valid_area(self) -> float:
        w_resized, h_resized = self.resized_wh_wo_border
        return w_resized * h_resized

    def resized_to(self, target_size):
        return MergedROI(self.rois, self.border, target_size=target_size)

    def rescaled_to(self, target_scale):
        return MergedROI(self.rois, self.border, target_scale=target_scale)

    def resized_image(self, frames):
        r = self.rect
        merged_roi = frames[self.frame_index][r.t:r.b, r.l:r.r]

        w, h = self.resized_wh_wo_border
        resized_roi = cv2.resize(merged_roi, (w, h))

        wb, hb = self.resized_wh
        canvas = np.full((hb, wb, 3), 255, dtype=np.uint8)
        canvas[self.border:self.border + h,
               self.border:self.border + w] = resized_roi
        return canvas


PackingInfo = Dict[Tuple[int, int], MergedROI]


def seed(random_state):
    random.seed(random_state)
    np.random.seed(random_state)


def load_frames(video_path, frame_indices) -> Dict[int, np.ndarray]:
    frames = {}
    cap = cv2.VideoCapture(str(video_path))
    if np.all(np.diff(frame_indices) == 1):
        start_index, end_index = frame_indices[0], frame_indices[-1] + 1
        cap.set(cv2.CAP_PROP_POS_FRAMES, start_index)
        pbar = tqdm(range(start_index, end_index),
                    desc=f'{f"Loading {video_path.name}":<40}')
        for index in pbar:
            ret, frame = cap.read()
            assert ret
            frames[index] = frame
    else:
        pbar = tqdm(frame_indices,
                    desc=f'{f"Loading {video_path.name}":<40}')
        for frame_index in pbar:
            cap.set(cv2.CAP_PROP_POS_FRAMES, frame_index)
            ret, frame = cap.read()
            assert ret
            frames[frame_index] = frame
    cap.release()
    return frames


def type_of(video_path: Path) -> str:
    train, test = 'train', 'test'
    if train in str(video_path):
        assert test not in str(video_path)
        return train
    elif test in str(video_path):
        assert train not in str(video_path)
        return test
    else:
        raise ValueError()


def frame_indices_of(dataset, data_type, video_path, full):
    if data_type == 'test':
        cap = cv2.VideoCapture(str(video_path))
        frame_count = int(cap.get(cv2.CAP_PROP_FRAME_COUNT))
        cap.release()
        return list(range(frame_count))

    if dataset == 'mta':
        FPS = 41
        if data_type == 'train':
            return [i * FPS for i in range(11)]
        elif data_type == 'scale':
            stime, etime, interval = 40, 50, 1
        elif data_type == 'val':
            val_interval = 20 if full else 5
            stime, etime, interval = 90, 120, val_interval
        else:
            raise ValueError()
        return list(range(stime * FPS, etime * FPS, interval))
    elif dataset == 'virat':
        FPS = 30
        video_id = int(Path(video_path).stem[-1])
        if data_type == 'train':
            if video_id == 1:
                return [3700 + i * FPS for i in range(11)]
            elif video_id in [2, 3, 4, 6]:
                return [10 + i * FPS for i in range(11)]
            else:
                raise ValueError()
        elif data_type == 'scale':
            if video_id == 1:
                return list(range(4900, 5200))
            elif video_id in [2, 3, 4, 6]:
                return list(range(1210, 1510))
            else:
                raise ValueError()
        elif data_type == 'val':
            if video_id == 1:
                return list(range(6400, 7300, 20 if full else 5))
            elif video_id in [2, 3, 4, 6]:
                return list(range(2710, 3610, 20 if full else 5))
            else:
                raise ValueError()
        else:
            raise ValueError()
    else:
        raise ValueError()


def mta_video_paths():
    base_dir = Path('/data1/mondrian/dataset/mta/MTA_ext_short/')
    return sorted(base_dir.glob(f'train/cam_*/cam_*.mp4'))


def mta_annotation_path_of(video_path):
    video_id = video_id_of(video_path)
    return Path(f'/data1/mondrian/dataset/mta/MTA_ext_short/train/cam_{video_id}/coords_fib_cam_{video_id}.csv')


def virat_video_paths():
    base_dir = Path('/data1/mondrian/dataset/virat/video/train/')
    return sorted(base_dir.glob('*.mp4'))


def virat_annotation_path_of(video_path, frame_index):
    video_id = video_id_of(video_path)
    annotation_base_dir = Path(
        '/data1/mondrian/dataset/virat/labels_combined_VIRAT_YOLOv5x/')
    src_path = annotation_base_dir / f'cam_{video_id}_{frame_index:05d}.txt'
    return src_path


def video_id_of(video_path):
    return int(Path(video_path).stem[-1])


def video_paths_of(dataset):
    if dataset == 'mta':
        return mta_video_paths()
    elif dataset == 'virat':
        return virat_video_paths()
    else:
        raise ValueError()


def load_rois_from_a_video(dataset, data_type, video_path, avg_padding):
    frame_indices = frame_indices_of(dataset, data_type, video_path, False)
    if dataset == 'mta':
        annotation_path = mta_annotation_path_of(video_path)
        ann = pd.read_csv(annotation_path)
        if np.all(np.diff(frame_indices) == 1):
            start_index, end_index = frame_indices[0], frame_indices[-1] + 1
            ann = ann[ann['frame_no_cam'].between(start_index, end_index,
                                                  inclusive='left')]
        else:
            ann = ann[ann['frame_no_cam'].isin(frame_indices)]
        pbar = tqdm(ann.iterrows(), total=len(ann),
                    desc=f'{f"Loading ROIs from {video_path}":<40}')
        for _, row in pbar:
            yield ROI(video_path,
                      row['frame_no_cam'],
                      row['person_id'],
                      Rect(row['x_top_left_BB'],
                           row['y_top_left_BB'],
                           row['x_bottom_right_BB'],
                           row['y_bottom_right_BB']),
                      avg_padding)
    elif dataset == 'virat':
        for frame_index in frame_indices:
            annotation_path = virat_annotation_path_of(video_path, frame_index)
            if not annotation_path.exists():
                continue
            ann = np.genfromtxt(annotation_path, delimiter=' ')
            if ann.size == 0:
                continue
            if ann.ndim == 1:
                assert ann.size == 5 or ann.size == 6
                ann = ann.reshape(1, ann.size)
            if ann.shape[1] == 6:
                ann = ann[:, :5]
            for c, x, y, w, h in ann:
                assert c == 0, str(ann)
                l = int((x - w / 2) * FRAME.width)
                t = int((y - h / 2) * FRAME.height)
                r = int((x + w / 2) * FRAME.width)
                b = int((y + h / 2) * FRAME.height)
                yield ROI(video_path,
                          frame_index,
                          -1,
                          Rect(l, t, r, b),
                          avg_padding)
    else:
        raise ValueError()


def load_rois_with_detection(dataset, data_type, video_path, avg_padding):
    assert 'train' in str(video_path)
    frame_indices = frame_indices_of(dataset, data_type, video_path, False)
    video_id = int(video_path.stem[-1])
    base_dir = Path('/data1/mondrian/runs/detect')
    if dataset == 'mta':
        label_dir = base_dir / 'mta_scale_full_yolov5x_640' / 'labels'
    elif dataset == 'virat':
        label_dir = base_dir / 'virat_scale_full_yolov5x_640' / 'labels'
    else:
        raise ValueError()
    for frame_index in frame_indices:
        if dataset == 'mta':
            label_path = label_dir / f'cam_{video_id}_{frame_index:05d}.txt'
        elif dataset == 'virat':
            label_path = label_dir / f'train_cam_{video_id}_{frame_index:05d}.txt'
        else:
            raise ValueError()
        labels = np.genfromtxt(label_path, delimiter=' ').reshape(-1, 6)
        for label in labels:
            _, x, y, w, h, _ = label
            l = int((x - w / 2) * FRAME.width)
            t = int((y - h / 2) * FRAME.height)
            r = int((x + w / 2) * FRAME.width)
            b = int((y + h / 2) * FRAME.height)
            yield ROI(video_path,
                      frame_index,
                      -1,
                      Rect(l, t, r, b),
                      avg_padding)


def load_rois(dataset,
              data_type,
              avg_padding,
              filter_size) -> List[ROI]:
    video_paths = video_paths_of(dataset)
    if data_type in ['train', 'val']:
        rois = [roi
                for video_path in video_paths
                for roi in load_rois_from_a_video(
                    dataset, data_type, video_path, avg_padding)]
    elif data_type == 'scale':
        rois = [roi
                for video_path in video_paths
                for roi in load_rois_with_detection(
                    dataset, data_type, video_path, avg_padding)]
    else:
        raise ValueError()
    return [roi for roi in rois if roi.bbox.max_length >= filter_size]


def merge_rois_in_a_frame(merged_rois: List[MergedROI],
                          iou_thres: float) -> List[MergedROI]:
    for i, m1 in enumerate(merged_rois):
        for j, m2 in enumerate(merged_rois):
            if i >= j:
                continue
            if m1.rect.iou(m2.rect) > iou_thres:
                merged_rois[i] = MergedROI.merge(m1, m2)
                merged_rois.pop(j)
                return merge_rois_in_a_frame(merged_rois, iou_thres)
    return merged_rois


def merge_rois(rois: List[ROI], iou_thres: float, border: int) -> List[MergedROI]:
    frame_wise_merged_rois = defaultdict(list)
    for roi in rois:
        group_id = roi.video_path, roi.frame_index
        frame_wise_merged_rois[group_id].append(MergedROI([roi], border))
    pbar = tqdm(sorted(frame_wise_merged_rois.keys()),
                total=len(frame_wise_merged_rois),
                desc=f'{"Merging ROIs":<40}')
    return [merged_roi
            for group_id in pbar
            for merged_roi in merge_rois_in_a_frame(
                frame_wise_merged_rois[group_id], iou_thres)]


def group_rois(resized_rois: List[MergedROI]) -> List[List[ROI]]:
    grouped_rois_dict = defaultdict(list)
    for resized_roi in resized_rois:
        grouped_rois_dict[resized_roi.video_path].append(resized_roi)
    return list(grouped_rois_dict.values())


def pack_rois(merged_rois: List[MergedROI],
              canvas_size: int,
              sort_by: List[str],
              num_repeats: int) -> List[PackingInfo]:

    def can_fit(f: Rect, w: int, h: int):
        return f.width >= w and f.height >= h

    def split_rect(f: Rect, w: int, h: int) -> List[Rect]:
        if f.width >= f.height:
            r0 = Rect(f.l + w, f.t,     f.r,     f.b)
            r1 = Rect(f.l,     f.t + h, f.l + w, f.b)
        else:
            r0 = Rect(f.l,     f.t + h, f.r,    f.b)
            r1 = Rect(f.l + w, f.t,     f.r,    f.t + h)
        rects = []
        if r0.area > 0:
            rects.append(r0)
        if r1.area > 0:
            rects.append(r1)
        return rects

    def sort_key(roi: MergedROI):
        key = []
        for s in sort_by:
            if s == 'frame':
                key.append(roi.frame_index)
            elif s == 'area':
                key.append(roi.area)
            else:
                raise ValueError(f'Unknown sort_by: {s}')
        return tuple(key)

    merged_rois = sorted(merged_rois, key=sort_key, reverse=True)
    merged_rois *= num_repeats

    packing_infos = [{}]
    free_rects_list = [Rect(0, 0, canvas_size, canvas_size)]
    pbar = tqdm(merged_rois, total=len(merged_rois),
                desc=f'{f"Packing {len(merged_rois)} ROIs":<40}')
    for m in pbar:
        w, h = m.resized_wh
        if w > canvas_size or h > canvas_size:
            continue

        rect_idx, min_remaining_area = -1, canvas_size ** 2
        while rect_idx == -1:
            for ridx, free_rect in enumerate(free_rects_list):
                if can_fit(free_rect, w, h):
                    remaining_area = free_rect.area - w * h
                    if remaining_area < min_remaining_area:
                        min_remaining_area = remaining_area
                        rect_idx = ridx
            if rect_idx == -1:
                packing_infos.append({})
                free_rects_list = [Rect(0, 0, canvas_size, canvas_size)]

        selected_rect = free_rects_list[rect_idx]
        x, y = selected_rect.l, selected_rect.t
        packing_infos[-1][x, y] = m
        free_rects_list.pop(rect_idx)
        free_rects_list += split_rect(selected_rect, w, h)
    pbar.close()
    print(f'{len(merged_rois)} ROIs ({num_repeats} repeats) '
          f'are packed into {len(packing_infos)} packed canvases')
    return packing_infos


def bounding_boxes_of(canvas_size: int, packing_info: PackingInfo) -> np.ndarray:
    bounding_boxes = []
    for (x, y), m in packing_info.items():
        scale = m.scale
        for roi in m.rois:
            bounding_boxes.append((
                0,  # Person
                (((roi.bbox.l + roi.bbox.r) / 2 - m.rect.l) * scale + x + m.border) / canvas_size,  # noqa
                (((roi.bbox.t + roi.bbox.b) / 2 - m.rect.t) * scale + y + m.border) / canvas_size,  # noqa
                roi.bbox.width * scale / canvas_size,
                roi.bbox.height * scale / canvas_size,
            ))
    return np.array(bounding_boxes, dtype=np.float32)


def image_of(canvas_size: int,
             packing_info: PackingInfo,
             frames: Dict[int, np.ndarray]) -> np.ndarray:
    canvas = np.full((canvas_size, canvas_size, 3), 114, dtype=np.uint8)
    for (x, y), m in packing_info.items():
        w, h = m.resized_wh
        assert np.all(canvas[y:y+h, x:x+w] == 114)
        canvas[y:y+h, x:x+w] = m.resized_image(frames)
    return canvas


def label_image(image, bounding_boxes) -> np.ndarray:
    height, width, _ = image.shape
    labeled_image = np.copy(image)
    for cxywh in bounding_boxes:
        _, x, y, w, h = cxywh
        x1, y1 = int((x - w / 2) * width), int((y - h / 2) * height)
        x2, y2 = int((x + w / 2) * width), int((y + h / 2) * height)
        labeled_image = cv2.rectangle(
            labeled_image, (x1, y1), (x2, y2), (0, 255, 0))
    return labeled_image


def save_canvas(save_name: str,
                packing_info: PackingInfo,
                canvas_size: int,
                frames: Dict[int, np.ndarray],
                image_dir: Path,
                label_dir: Path) -> None:
    canvas_path = image_dir / f'{save_name}.png'
    label_path = label_dir / f'{save_name}.txt'
    labeled_canvas_path = label_dir / f'{save_name}.png'
    packing_info_path = label_dir / f'{save_name}.json'

    bounding_boxes = bounding_boxes_of(canvas_size, packing_info)
    packing_info_dict = {
        f'{x}_{y}': f'{m.id}_{m.src_boxes}'
        for (x, y), m in packing_info.items()
    }

    already_exists = all([p.exists() for p in [canvas_path,
                                               label_path,
                                               labeled_canvas_path,
                                               packing_info_path]])
    if already_exists:
        saved_bounding_boxes = np.genfromtxt(str(label_path), delimiter=' ')
        with packing_info_path.open('r') as f:
            saved_packing_info_dict = json.load(f)
        assert np.allclose(saved_bounding_boxes, bounding_boxes, atol=1e-4)  # noqa
        assert saved_packing_info_dict == packing_info_dict
    else:
        canvas = image_of(canvas_size, packing_info, frames)
        labeled_canvas = label_image(canvas, bounding_boxes)

        cv2.imwrite(str(canvas_path), canvas)
        cv2.imwrite(str(labeled_canvas_path), labeled_canvas)
        np.savetxt(str(label_path), bounding_boxes, fmt='%.6f', delimiter=' ')
        with packing_info_path.open('w') as f:
            json.dump(packing_info_dict, f, indent=4)


def save_canvases_of_video(args: Tuple[Path, List[PackingInfo]],
                           canvas_size: int = None,
                           image_dir: Path = None,
                           label_dir: Path = None) -> None:
    video_path, packing_infos_of_video = args
    frame_indices = sorted(set(
        m.frame_index
        for packinginfo in packing_infos_of_video
        for m in packinginfo.values()))
    frames = load_frames(video_path, frame_indices)
    for canvas_idx, packing_info in enumerate(packing_infos_of_video):
        save_name = f'{video_path.stem}_{canvas_idx:05d}'
        save_canvas(save_name, packing_info, canvas_size,
                    frames, image_dir, label_dir)


def group_by_video(packing_infos: List[PackingInfo]) -> Dict[Path, List[PackingInfo]]:

    def video_path_of(packing_info) -> Path:
        assert len(set([m.video_path for m in packing_info.values()])) == 1
        return next(iter(packing_info.values())).video_path

    video_paths = sorted(list(set([m.video_path
                                   for packinginfo in packing_infos
                                   for m in packinginfo.values()])))
    return {
        video_path: [packing_info
                     for packing_info in packing_infos
                     if video_path_of(packing_info) == video_path]
        for video_path in video_paths
    }


def measure_and_print_packing_efficiency(packing_infos, canvas_size):
    canvas_area = canvas_size ** 2
    for video_path, grouped_packing_infos in group_by_video(packing_infos).items():
        packing_efficiencies = []
        for packing_info in grouped_packing_infos:
            packing_area = sum([m.valid_area
                                for m in packing_info.values()])
            packing_efficiency = packing_area / canvas_area
            packing_efficiencies.append(packing_efficiency)
        print(canvas_size, video_path,
              np.mean(packing_efficiencies),
              np.std(packing_efficiencies))


def save_full_frames(dataset, data_type, base_dir):
    image_dir = base_dir / 'images'
    label_dir = base_dir / 'labels'
    image_dir.mkdir(parents=True, exist_ok=True)
    label_dir.mkdir(parents=True, exist_ok=True)

    def save_name_of(video_path, frame_index):
        return f'{video_path.stem}_{frame_index:05d}'
    
    def save_frames(video_path, frame_indices):
        frames = load_frames(video_path, frame_indices)
        for frame_index in tqdm(frame_indices, desc=f"Saving {video_path.name}"):
            save_name = save_name_of(video_path, frame_index)
            cv2.imwrite(
                str(image_dir / f'{save_name}.png'), frames[frame_index])

    video_paths = video_paths_of(dataset)
    if dataset == 'mta':
        for video_path in video_paths:
            frame_indices = frame_indices_of(
                dataset, data_type, video_path, True)
            save_frames(video_path, frame_indices)
            annotation_path = mta_annotation_path_of(video_path)
            ann = pd.read_csv(annotation_path)
            for frame_index in tqdm(frame_indices, desc=str(video_path)):
                xyxy = ann[ann['frame_no_cam'] == frame_index][[
                    'x_top_left_BB',
                    'y_top_left_BB',
                    'x_bottom_right_BB',
                    'y_bottom_right_BB']].values.reshape(-1, 4)
                xywh = xyxy.astype(np.float32)
                xywh[:, 2:] = xyxy[:, 2:] - xyxy[:, :2]
                xywh[:, :2] = (xyxy[:, :2] + xyxy[:, 2:]) / 2
                xywh /= np.array([[FRAME.width, FRAME.height, FRAME.width, FRAME.height]])
                cxywh = np.hstack(
                    [np.zeros((len(xywh), 1), dtype=np.float32), xywh])
                save_name = save_name_of(video_path, frame_index)
                np.savetxt(str(label_dir / f'{save_name}.txt'),
                           cxywh, fmt='%.6f', delimiter=' ')
    elif dataset == 'virat':
        for video_path in video_paths:
            frame_indices = frame_indices_of(
                dataset, data_type, video_path, True)
            save_frames(video_path, frame_indices)
            for frame_index in tqdm(frame_indices, desc=str(video_path)):
                save_name = save_name_of(video_path, frame_index)
                save_path = label_dir / f'{save_name}.txt'
                src_path = virat_annotation_path_of(video_path, frame_index)
                if src_path.exists():
                    shutil.copy(src_path, save_path)
                else:
                    save_path.touch()
    else:
        raise ValueError(f'Unknown dataset: {dataset}')


def main(
        only_measure_packing_efficiency=None,
        dataset=None,
        full=None,
        canvas_size=None,
        data_type=None,
        iou_thres=None,
        sort_by=None,
        size_range=None,
        max_roi_quantile=None,
        scale_range=None,
        scale_min_size=None,
        avg_padding=None,
        border=None,
        filter_size=None,
        random_state=None,
) -> None:
    seed(random_state)
    num_repeats = 5 if data_type == 'train' else 1

    dir_name = 'full_frames' if full else 'packed_frames'
    if data_type == 'train':
        base_dir = Path(f'/data1/mondrian/{dir_name}/{dataset}_{data_type}')  # noqa
    else:
        base_dir = Path(f'/data1/mondrian/{dir_name}/{dataset}_{data_type}')

    if full:
        save_full_frames(dataset, data_type, base_dir)
        return

    rois = load_rois(dataset, data_type, avg_padding, filter_size)
    print(f'Total {len(rois)} ROIs loaded')

    if data_type in ['train', 'val']:
        merged_rois = merge_rois(rois, iou_thres, border)
        resized_rois = [merged_roi.resized_to(target_size)
                        for merged_roi in merged_rois
                        for target_size in np.arange(*size_range)]
        grouped_rois_list = group_rois(resized_rois)
    elif data_type == 'scale':  # No merge, use scale
        merged_rois = [MergedROI([roi], border) for roi in rois]
        max_roi_size = np.quantile([roi.min_roi_max_length for roi in merged_rois], max_roi_quantile).item()
        resized_rois = [merged_roi.rescaled_to(target_scale)
                        for merged_roi in merged_rois
                        for target_scale in np.arange(*scale_range)]

        def is_valid(r):
            return (r.scale >= 0.99 or max(r.resized_wh_wo_border) >= scale_min_size) \
                and r.min_roi_max_length <= max_roi_size

        resized_rois = [resized_roi
                        for resized_roi in resized_rois
                        if is_valid(resized_roi)]
        grouped_rois_list = group_rois(resized_rois)
    else:
        raise ValueError()

    packing_infos = [
        packing_info  # corresponds to a canvas
        for grouped_rois in grouped_rois_list
        for packing_info in pack_rois(grouped_rois, canvas_size, sort_by, num_repeats)
    ]
    print(f'Total {len(resized_rois)} MergedROIs, '
          f'{len(packing_infos)} canvases ({num_repeats} repeats)')

    if only_measure_packing_efficiency:
        measure_and_print_packing_efficiency(packing_infos, canvas_size)
        return

    image_dir = base_dir / str(canvas_size) / 'images'
    label_dir = base_dir / str(canvas_size) / 'labels'
    image_dir.mkdir(exist_ok=True, parents=True)
    label_dir.mkdir(exist_ok=True, parents=True)

    packing_infos_of_video = group_by_video(packing_infos)
    runnable = partial(save_canvases_of_video,
                       canvas_size=canvas_size,
                       image_dir=image_dir,
                       label_dir=label_dir)
    list(tqdm(map(runnable, packing_infos_of_video.items()),
              total=len(packing_infos_of_video)))


if __name__ == '__main__':
    parser = argparse.ArgumentParser()
    parser.add_argument('-e', '--only-measure-packing-efficiency',
                        action='store_true')
    parser.add_argument('-d', '--dataset', required=True,
                        choices=['mta', 'virat'])
    parser.add_argument('-f', '--full', action='store_true')
    parser.add_argument('-c', '--canvas-size', type=int, default=None)
    parser.add_argument('-t', '--data-type', required=True,
                        choices=[
                            'train',
                            'val',
                            'scale',
                            'test',
                        ])
    parser.add_argument('-i', '--iou-thres', type=float, default=0.3)
    parser.add_argument('--sort-by', choices=['frame', 'area'],
                        nargs='+', default=['frame', 'area'])
    parser.add_argument('--size-range', type=int, nargs=3,
                        default=[20, 120, 10],
                        help='Valid for train or val or test or dev data_types. '
                             'Target size range. (min, max, step)')
    parser.add_argument('--scale-range', type=int, nargs=3,
                        default=[0.1, 1.1, 0.1],
                        help='Valid for the scale data_type. '
                             'Target scale range. (min, max, step)')
    parser.add_argument('--max-roi-quantile', type=float, default=0.99)
    parser.add_argument('--scale-min-size', type=int, default=20)
    parser.add_argument('--avg-padding', type=int, default=20)
    parser.add_argument('--border', type=int, default=2)
    parser.add_argument('--filter-size', type=int, default=20)
    parser.add_argument('--random-state', type=int, default=42)
    args = parser.parse_args()

    assert args.full or args.canvas_size is not None

    main(**vars(args))
