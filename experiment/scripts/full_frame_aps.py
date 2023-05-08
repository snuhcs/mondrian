import argparse
from collections import defaultdict
import json
from pathlib import Path

import numpy as np
from tqdm import tqdm

import sys
sys.path.append(str(Path(sys.path[0]).parent))

from metrics.BoundingBox import BoundingBox
from metrics.BoundingBoxes import BoundingBoxes
from metrics.Evaluator import Evaluator
from metrics.utils import BBFormat, BBType


PERSON_LABEL = 0


def main(dataset):
    target_sizes = [128 * i for i in range(1, 16)]
    if dataset == 'virat':
        videos = [
            'VIRAT_S_000001',
            'VIRAT_S_000002',
            'VIRAT_S_000003',
            'VIRAT_S_000004',
            'VIRAT_S_000006',
        ]
        start_end_indices = {
            'VIRAT_S_000001': [10327, 20655],
            'VIRAT_S_000002': [4537, 9075],
            'VIRAT_S_000003': [10470, 20940],
            'VIRAT_S_000004': [8820, 17640],
            'VIRAT_S_000006': [8902, 17805],
        }
    else:
        videos = [f'test_cam_{i}' for i in range(6)]
        start_end_indices = {video: (0, 2461) for video in videos}

    size_video_aps = defaultdict(dict)
    for video in videos:
        base_dir = Path(f'/data1/mondrian/full_boxes') / video
        full_boundingBoxes = BoundingBoxes()
        sidx, eidx = start_end_indices[video]
        with (base_dir / '1920.json').open('r') as f:
            boxes_dict_1920 = json.load(f)
        for frame_index, boxes in boxes_dict_1920.items():
            for box in boxes:
                if box['label'] == PERSON_LABEL:
                    full_boundingBoxes.addBoundingBox(BoundingBox(
                        str(frame_index), box['label'],
                        *box['xyxy'],
                        bbType=BBType.GroundTruth,
                        format=BBFormat.XYX2Y2
                    ))

        for target_size in target_sizes:
            evaluator = Evaluator()
            boundingBoxes = BoundingBoxes()
            boundingBoxes.concatBoundingBoxes(full_boundingBoxes)

            with (base_dir / f'{target_size}.json').open('r') as f:
                boxes_dict = json.load(f)
            for frame_index, boxes in boxes_dict.items():
                for box in boxes:
                    if box['label'] == PERSON_LABEL:
                        boundingBoxes.addBoundingBox(BoundingBox(
                            str(frame_index), box['label'],
                            *box['xyxy'],
                            bbType=BBType.Detected,
                            format=BBFormat.XYX2Y2,
                            classConfidence=box['confidence']))
            result = evaluator.GetPascalVOCMetrics(boundingBoxes)
            assert len(result) == 1
            assert result[0]['class'] == PERSON_LABEL
            ap = result[0]['AP']
            print(f'{video} {target_size}: {ap} AP')
            size_video_aps[target_size][video] = ap

    print('='*10, 'Average', '='*10)
    for target_size, video_aps in size_video_aps.items():
        print(f'Target Size: {target_size:<4} Average AP: {np.mean(list(video_aps.values()))}')
    print()

    for video in videos:
        print('='*10, video, '='*10)
        for target_size in target_sizes:
            print(f'Target Size: {target_size:<4} AP: {size_video_aps[target_size][video]}')
        print()
    print('='*30)


if __name__ == '__main__':
    parser = argparse.ArgumentParser()
    parser.add_argument('-d', '--dataset', required=True, choices=['mta', 'virat'])
    args = parser.parse_args()

    main(args.dataset)
