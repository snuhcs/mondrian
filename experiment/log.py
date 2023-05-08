import json
import math
from typing import List, Tuple
from pathlib import Path

from metrics.BoundingBox import BoundingBox
from metrics.BoundingBoxes import BoundingBoxes
from metrics.utils import BBFormat, BBType

from dataset import VideoData, PERSON


class ConfigJson:
    def __init__(self, config_json: Path):
        assert config_json.exists()
        with config_json.open('r') as f:
            self.config = json.load(f)

    @property
    def dataset(self):
        assert self.config['roi_resizer']['dataset'] == self.config['inference_engine']['dataset']
        return self.config['roi_resizer']['dataset']

    @property
    def video_names(self):
        return [Path(video_config['path']).stem
                for video_config in self.config['video_configs']]


class BoxesTxt:
    def __init__(self, boxes_txt: Path, video_names: List[str]):
        assert boxes_txt.exists()
        self.boxes_txt = boxes_txt
        self.video_names = video_names

    @property
    def time_boxes(self) -> Tuple[List[int], BoundingBoxes]:
        time_us_list, boxes = [], BoundingBoxes()
        with self.boxes_txt.open('r') as f:
            for line in f:
                time_us, frame_boxes = \
                    BoxesTxt.parse_a_line(line, self.video_names)
                time_us_list.append(time_us)
                boxes.concatBoundingBoxes(frame_boxes)
        return time_us_list, boxes

    @staticmethod
    def parse_a_line(line: str,
                     video_names: List[str],
                     num_elems_per_box=9) -> Tuple[int, BoundingBoxes]:
        """
        Mondrian Log file format
            elems = line.split(',')
            elems[0] : video path
            elems[1] : relative frame index (0 ~ length)
            elems[2] : time stamp (us)
            elems[3:12] : first bounding box
                +-------+------+-----+-------+--------+------------+--------+-------------+-------+
                | 0     | 1    | 2   | 3     | 4      | 5          | 6      | 7           | 8     |
                +-------+------+-----+-------+--------+------------+--------+-------------+-------+
                | boxId | left | top | right | bottom | confidence | origin | choiceOfBox | class |
                +-------+------+-----+-------+--------+------------+--------+-------------+-------+
            elems[12:21] : second bounding box
            elems[21:30] : ...
        """
        line = line.strip()
        line = line[:-1] if line.endswith(',') else line
        elems = line.split(',')
        assert (len(elems) - 3) % num_elems_per_box == 0, line
        vid, fid_rel, time_us = int(elems[0]), int(elems[1]), int(elems[2])
        fid = fid_rel + VideoData(video_names[vid]).frame_range[0]
        boxes = BoundingBoxes()
        for box_id, sidx in enumerate(range(3, len(elems), num_elems_per_box)):
            label = elems[sidx + 8]
            l = float(elems[sidx + 1])
            t = float(elems[sidx + 2])
            r = float(elems[sidx + 3])
            b = float(elems[sidx + 4])
            confidence = float(elems[sidx + 5])
            assert label == PERSON
            assert all(not math.isnan(v) for v in [l, t, r, b, confidence])
            assert l <= r and t <= b, f'{l} {t} {r} {b}'
            assert 0 <= confidence <= 1, \
                f'Original Line: {line}\n' \
                f'Splitted Elems: {str(elems)}\n' \
                f'{box_id}th Box: {l} {t} {r} {b} {confidence}'
            boxes.addBoundingBox(BoundingBox(
                (vid, fid), label,
                l, t, r, b,
                bbType=BBType.Detected,
                classConfidence=confidence,
                format=BBFormat.XYX2Y2))
        return time_us, boxes
