from dataclasses import dataclass
from typing import Tuple

import numpy as np


@dataclass(frozen=True, eq=True, order=True)
class Rect:
    l: float
    t: float
    r: float
    b: float

    def __post_init__(self):
        assert self.l <= self.r
        assert self.t <= self.b

    @property
    def width(self):
        return self.r - self.l

    @property
    def height(self):
        return self.b - self.t
    
    @property
    def hw_ratio(self):
        return self.height / self.width

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

    def overlap_area(self, other: 'Rect') -> 'Rect':
        new_l = max(self.l, other.l)
        new_t = max(self.t, other.t)
        new_r = min(self.r, other.r)
        new_b = min(self.b, other.b)
        if new_r < new_l or new_b < new_t:
            return 0
        return (new_r - new_l) * (new_b - new_t)

    def iou(self, other: 'Rect') -> float:
        if other.r < self.l or other.l > self.r or \
                other.b < self.t or other.t > self.b:
            return 0
        overlap_area = self.overlap_area(other)
        return overlap_area / (self.area + other.area - overlap_area)

    def contains(self, other: 'Rect', ratio: float = 0.995) -> bool:
        return self.overlap_area(other) / other.area >= ratio


@dataclass
class BBox:
    rect: Rect
    conf: float


@dataclass
class ROI:
    video_id: int
    frame_index: int
    rect_ao: Rect      # absolute rect in original frame's view
    src_rect_ao: Rect  # absolute rects in original frame's view
    scale: float
    input_size: int    # input size of packed canvas
    packed_frame_index: int
    pack_l: int        # packed location
    pack_t: int        # packed location
    src_box: BBox
    pred_boxes: Tuple[BBox]


@dataclass
class OFFeatures:
    shifts: np.ndarray
    errs: np.ndarray
    statuses: np.ndarray

    def __post_init__(self):
        assert len(self.shifts) == len(self.errs) == len(self.statuses)
        assert set(self.statuses) <= {0, 1}
        if all(status == 0 for status in self.statuses):
            self.shift_avg = np.zeros(2, dtype=self.shifts.dtype)
            self.shift_std = np.zeros(2)
            self.shift_ncc = 100
            self.avg_err = 100
        else:
            valid_mask = self.statuses == 1
            valid_shifts = self.shifts[valid_mask]
            valid_errs = self.errs[valid_mask]
            filtered_shifts, filtered_errs = \
                self.filter_outlier(valid_shifts, valid_errs)
            self.shift_avg = np.mean(filtered_shifts, axis=0)
            self.shift_std = np.std(filtered_shifts, axis=0)
            self.shift_ncc = self.ncc_of(filtered_shifts)
            self.avg_err = np.mean(filtered_errs)

    def filter_outlier(self, shifts, errs):
        sizes = shifts[:, 0] ** 2 + shifts[:, 1] ** 2
        q1 = np.quantile(sizes, 0.25)
        mask = sizes >= q1
        return shifts[mask], errs[mask]

    def ncc_of(self, shifts):
        if len(shifts) <= 1:
            return 0

        ncc = 0
        for i in range(len(shifts)):
            for j in range(i + 1, len(shifts)):
                xi, yi = shifts[i]
                xj, yj = shifts[j]

                ni = xi*xi + yi*yi
                nj = xj*xj + yj*yj

                if ni == 0 or nj == 0:
                    continue
                ncc += (xi * xj + yi * yj) / np.sqrt(ni*nj)
        return ncc / (len(shifts) * (len(shifts) - 1)/2)
