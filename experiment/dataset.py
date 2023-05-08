from pathlib import Path
from typing import Tuple
import warnings

import pandas as pd
import numpy as np
from metrics.utils import BBFormat, BBType, CoordinatesType
from metrics.BoundingBoxes import BoundingBoxes
from metrics.BoundingBox import BoundingBox
warnings.filterwarnings('ignore')


PERSON = 'person'
DATASET_INFO = {
    'mta': {
        'width': 1920,
        'height': 1080,
        'fps': 41,
        'videos': {
            'mta_train_cam_0': [0, 4922],
            'mta_train_cam_1': [0, 4922],
            'mta_train_cam_2': [0, 4922],
            'mta_train_cam_3': [0, 4922],
            'mta_train_cam_4': [0, 4922],
            'mta_train_cam_5': [0, 4922],
            'mta_test_cam_0': [0, 4922],
            'mta_test_cam_1': [0, 4922],
            'mta_test_cam_2': [0, 4922],
            'mta_test_cam_3': [0, 4922],
            'mta_test_cam_4': [0, 4922],
            'mta_test_cam_5': [0, 4922],
        },
    },
    'virat': {
        'width': 1920,
        'height': 1080,
        'fps': 30,
        'videos': {
            'virat_train_cam_1': [0, 10327],
            'virat_train_cam_2': [0, 4537],
            'virat_train_cam_3': [0, 10470],
            'virat_train_cam_4': [0, 8820],
            'virat_train_cam_6': [0, 8920],
            'virat_test_cam_1': [10327, 20655],
            'virat_test_cam_2': [4537, 9075],
            'virat_test_cam_3': [10470, 20940],
            'virat_test_cam_4': [8820, 17640],
            'virat_test_cam_6': [8920, 17805],
        },
    },
}


def dataset_of(video_name: str):
    return video_name.split('_')[0]


def test_dataset_info():
    keys = set(next(iter(DATASET_INFO.values())).keys())
    for dataset, info in DATASET_INFO.items():
        assert set(info.keys()) == keys
    for dataset, info in DATASET_INFO.items():
        for video_name in info['videos'].keys():
            assert dataset == dataset_of(video_name)


test_dataset_info()


class VideoData:
    def __init__(self, video_name: str, vid: int = 0):
        self.video_name = video_name
        self.vid = vid
        self.dataset = dataset_of(video_name)
        self.width = DATASET_INFO[self.dataset]['width']
        self.height = DATASET_INFO[self.dataset]['height']
        self.frame_range = DATASET_INFO[self.dataset]['videos'][video_name]
        self.num_frames = self.frame_range[1] - self.frame_range[0]

    @property
    def boxes(self) -> BoundingBoxes:
        if self.dataset == 'mta':
            return self.mta_gt_of()
        elif self.dataset == 'virat':
            return self.virat_gt_of()
        else:
            raise NotImplementedError(self.video_name)

    def mta_gt_of(self) -> BoundingBoxes:
        ann = pd.read_csv(f'ground_truths/mta/{self.video_name}.csv')
        boxes = BoundingBoxes()
        for frame_index in range(*self.frame_range):
            xyxys = ann[ann['frame_no_cam'] == frame_index][[
                'x_top_left_BB',
                'y_top_left_BB',
                'x_bottom_right_BB',
                'y_bottom_right_BB']].values.reshape(-1, 4)
            for xyxy in xyxys:
                boxes.addBoundingBox(BoundingBox(
                    (self.vid, frame_index), PERSON,
                    *xyxy,
                    bbType=BBType.GroundTruth,
                    format=BBFormat.XYX2Y2))
        return boxes

    def virat_gt_of(self) -> BoundingBoxes:
        video_id = int(self.video_name[-1])
        width = DATASET_INFO['virat']['width']
        height = DATASET_INFO['virat']['height']
        base_dir = Path('ground_truths/VIRAT_YOLOv5x_combined_labels/')
        boxes = BoundingBoxes()
        for frame_index in range(*self.frame_range):
            labels = np.genfromtxt(base_dir / f'virat_cam_{video_id}_{frame_index:05d}.txt',
                                   delimiter=' ', dtype=np.float32)
            if labels.size == 0:
                continue
            if labels.ndim == 1:
                labels = labels.reshape(1, -1)
            if labels.shape[1] == 6:
                labels = labels[:, :5]
            for c, x, y, w, h in labels:
                assert c == 0
                l = (x - w / 2) * width
                t = (y - h / 2) * height
                r = (x + w / 2) * width
                b = (y + h / 2) * height
                boxes.addBoundingBox(BoundingBox(
                    (self.vid, frame_index), PERSON,
                    l, t, r, b,
                    typeCoordinates=CoordinatesType.Absolute,
                    bbType=BBType.GroundTruth,
                    format=BBFormat.XYX2Y2))
        return boxes
