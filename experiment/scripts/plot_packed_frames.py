import argparse
import decimal
import json
from pathlib import Path
from typing import Dict, Tuple
import re

import cv2
import numpy as np
import pandas as pd
from tqdm import tqdm


def load_frames(video_path: Path, start_index, end_index) -> Dict[Tuple[str, int], list]:
    cap = cv2.VideoCapture(str(video_path))
    assert cap.isOpened()
    cap.set(cv2.CAP_PROP_POS_FRAMES, start_index)
    frames = {}
    num_images = end_index - start_index
    for frame_index in tqdm(range(start_index, end_index), total=num_images,
                            desc=f'Loading {num_images} images from video {video_path.name}'):
        ret, frame = cap.read()
        if ret:
            frames[frame_index] = frame
        else:
            break
    cap.release()
    return frames


context = decimal.getcontext()
context.rounding = decimal.ROUND_HALF_UP


def main(roi_csv: Path, timeline_csv: Path, video_path: Path, interval_index, roi_padding):
    roi_df = pd.read_csv(roi_csv, delimiter='\t')
    timeline_df = pd.read_csv(timeline_csv, delimiter='\t')
    schedule_times = sorted(list(set(timeline_df['scheduledTime']) - {0}))

    utils = []
    for schedule_time in schedule_times:
        _timeline_df = timeline_df[timeline_df['scheduledTime'] == schedule_time]
        mixed = np.unique(_timeline_df[[
            'mixedInferenceStartTime',
            'mixedInferenceEndTime']].values, axis=0)
        mixed = mixed[mixed[:,0] != 0]
        full = np.unique(_timeline_df[[
            'fullInferenceStartTime',
            'fullInferenceEndTime']].values, axis=0)
        full = full[full[:,0] != 0]
        assert full.shape[0] in [0, 1]
        start_time = min(mixed[:,0])
        end_time = max(mixed[:,1])
        if full.shape[0] > 0:
            full_start, full_end = full.reshape(2)
            start_time = min(full_start, start_time)
            end_time = max(full_end, end_time)
        total_time = end_time - start_time
        gpu_time = 0
        for s, e in mixed:
            gpu_time += e - s
        for s, e in full:
            gpu_time += e - s
        util = gpu_time / total_time
        utils.append(util)
    print(utils)
    print(np.mean(utils), np.std(utils))


    _timeline_df = timeline_df[timeline_df['scheduledTime'] == schedule_times[interval_index]]
    frame_indices = sorted(list(set(_timeline_df['frameIndex'])))
    roi_df = roi_df[roi_df['frameIndex'].isin(frame_indices)]
    roi_df
    frames = load_frames(video_path, min(frame_indices), max(frame_indices) + 1)
    is_arr = np.unique(roi_df[[
        'absolutePackedCanvasIndex',
        'packedCanvasSize']].values, axis=0)

    estimated_areas = []
    for packedFrameIndex, packedFrameSize in is_arr:
        if packedFrameSize == -1:
            continue
        _roi_df = roi_df[roi_df['absolutePackedCanvasIndex'] == packedFrameIndex]
        iltrbxysp_arr = np.unique(_roi_df[[
            'frameIndex',
            'mergedLoc_l',
            'mergedLoc_t',
            'mergedLoc_r',
            'mergedLoc_b',
            'packedXY_x',
            'packedXY_y',
            'mergedScale',
            'isProbingROI',
        ]].values, axis=0)

        packedFrameArea = packedFrameSize ** 2
        roi_areas = (iltrbxysp_arr[:, 3] - iltrbxysp_arr[:, 1]) \
            * (iltrbxysp_arr[:, 4] - iltrbxysp_arr[:, 2]) \
            * iltrbxysp_arr[:, 7]**2
        assert roi_areas.ndim == 1
        left_area = packedFrameArea - np.sum(roi_areas)
        print(f'packedFrame: {packedFrameArea}, rois: {np.sum(roi_areas)}, left area: {left_area}, {roi_areas.shape[0]} ROIs')

        sort_indices = np.argsort((iltrbxysp_arr[:, 3] - iltrbxysp_arr[:, 1])
                                  * (iltrbxysp_arr[:, 4] - iltrbxysp_arr[:, 2]))
        for idx, (i, l, t, r, b, x, y, s, p) in enumerate(iltrbxysp_arr[sort_indices]):
            area = (r - l) * (b - t)
            delta_area = area * (1 - s ** 2)
            left_area -= delta_area
            if left_area < 0:
                estimated_edge_length = np.sqrt(area) - 2 * roi_padding
                estimated_area = estimated_edge_length ** 2
                estimated_areas.append(estimated_area)
                print(f'Stopped at ROI {idx} with ROI Area {area}')
                print(f'Estimated edge length: {estimated_edge_length}')
                print(f'Estimated roi area: {estimated_area}')
                break

        image = np.full((packedFrameSize, packedFrameSize, 3), 114, dtype=np.uint8)
        lw = 2
        # Draw ROIs
        for iltrbxysp in iltrbxysp_arr:
            i, l, t, r, b, x, y, s, p = iltrbxysp
            frame = frames[int(i)]
            frame_h, frame_w, _ = frame.shape
            l, t, r, b, x, y = [int(v) for v in [l, t, r, b, x, y]]
            l, r = [max(0, min(v, frame_w)) for v in [l, r]]
            t, b = [max(0, min(v, frame_h)) for v in [t, b]]
            rw = min(max(1, round(decimal.Decimal((r - l) * s))), packedFrameSize - x)
            rh = min(max(1, round(decimal.Decimal((b - t) * s))), packedFrameSize - y)
            roi = cv2.resize(np.copy(frame[t:b, l:r]), dsize=(rw, rh))
            assert 0 <= y + rh <= packedFrameSize and 0 <= x + rw <= packedFrameSize, \
                f'{y + rh} < {packedFrameSize} or {packedFrameSize} < {x + rw}'
            image[y:y+rh, x:x+rw] = roi
            if p != 0:
                assert p == 1
                image = cv2.rectangle(image, (x, y), (x + rw, y + rh), (0, 0, 255), lw)

        # Draw Boxes
        def is_valid(e): return e != 'X'
        for _, row in _roi_df.iterrows():
            if is_valid(row['box']) and is_valid(row['probingBox']):
                column = 'box'
                color = (255, 100, 100)
            if is_valid(row['box']) and not is_valid(row['probingBox']):
                column = 'box'
                color = (0, 255, 0)
            elif is_valid(row['probingBox']):
                column = 'probingBox'
                color = (255, 255, 0)
            else:
                continue
            pl, pt, x, y, s = row[[
                'mergedLoc_l',
                'mergedLoc_t',
                'packedXY_x',
                'packedXY_y',
                'mergedScale']].values
            box_id, l, t, r, b, conf, origin, choice, label = \
                row[column].split(',')
            assert label == 'person'
            l, t, r, b = [float(v) for v in [l, t, r, b]]
            l, r = [max(0, min(int((v - pl) * s + x), packedFrameSize))
                    for v in [l, r]]
            t, b = [max(0, min(int((v - pt) * s + y), packedFrameSize))
                    for v in [t, b]]
            image = cv2.rectangle(image, (l, t), (r, b), color, lw)

        # Draw Probed ROI
        for _, row in _roi_df.iterrows():
            if is_valid(row['box']) and is_valid(row['probingBox']):
                l, t, r, b, pl, pt, x, y, s = [float(v) for v in row[[
                    'paddedLoc_l',
                    'paddedLoc_t',
                    'paddedLoc_r',
                    'paddedLoc_b',
                    'mergedLoc_l',
                    'mergedLoc_t',
                    'packedXY_x',
                    'packedXY_y',
                    'mergedScale']].values]
                l, r = [max(0, min(int((v - pl) * s + x), packedFrameSize))
                        for v in [l, r]]
                t, b = [max(0, min(int((v - pt) * s + y), packedFrameSize))
                        for v in [t, b]]
                image = cv2.rectangle(image, (l, t), (r, b), (255, 255, 255), lw)

        cv2.imwrite(f'figures/packed_frame_{packedFrameIndex}.png', image)
    print(f'Average: {np.mean(estimated_areas):.2f}')


if __name__ == '__main__':
    parser = argparse.ArgumentParser()
    parser.add_argument('-d', '--dir', type=Path, required=True)
    parser.add_argument('-v', '--video-dir', type=Path, required=True)
    parser.add_argument('-i', '--interval-index', type=int, default=2)
    args = parser.parse_args()

    def id_of(log_path: Path):
        m = re.search(r'_(\d+).[a-z]{3,4}$', str(log_path))
        return int(m.group(1)) if m is not None else 0

    roi_csv = sorted(args.dir.glob('roi*.csv'),
                     key=id_of, reverse=True)[0]
    timeline_csv = sorted(args.dir.glob('timeline*.csv'),
                          key=id_of, reverse=True)[0]
    with (args.dir / 'config.json').open('r') as f:
        config = json.load(f)
        roi_padding = config['roi_extractor']['roi_padding']
        video_configs = config['video_configs']
        assert len(video_configs) == 1
        runtime_video_path = Path(video_configs[0]['path'])
        dataset, video_name = runtime_video_path.parts[-2:]
        video_path = args.video_dir / dataset / video_name
    main(roi_csv, timeline_csv, video_path, args.interval_index, roi_padding)
