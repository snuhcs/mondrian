import argparse
from collections import defaultdict
import math
from pathlib import Path
import pickle
from time import time
from typing import Dict, List, Tuple, Union
import warnings

import matplotlib.pyplot as plt
from metrics.BoundingBoxes import BoundingBoxes
from metrics.Evaluator import Evaluator
import numpy as np
from tqdm import tqdm

from dataset import dataset_of, PERSON, VideoData
from log import ConfigJson, BoxesTxt
from util import ExpUtil, is_valid_exec
warnings.filterwarnings('ignore')


def frame_ranges_of(boxes_det: BoundingBoxes):
    frame_indices = defaultdict(list)
    for box in boxes_det.getBoundingBoxes():
        vid, fid = box.getImageName()
        frame_indices[vid].append(fid)
    return {vid: (min(fids), max(fids) + 1) for vid, fids in frame_indices.items()}


def filter_gt_w_fid(boxes_gt: BoundingBoxes, frame_range: Tuple[int, int]) -> BoundingBoxes:
    boxes_gt_filtered = BoundingBoxes()
    for box in boxes_gt.getBoundingBoxes():
        if frame_range[0] <= box.getImageName()[1] < frame_range[1]:
            boxes_gt_filtered.addBoundingBox(box)
    return boxes_gt_filtered


def concat_boxes(*args):
    assert all([isinstance(arg, BoundingBoxes) for arg in args])
    boxes = BoundingBoxes()
    for arg in args:
        boxes.concatBoundingBoxes(arg)
    return boxes


def fps_of(times_or_times_list: Union[List[int], List[List[int]]]) -> float:
    assert isinstance(times_or_times_list, list)
    if isinstance(times_or_times_list[0], int):
        times_list = [times_or_times_list]
    elif isinstance(times_or_times_list[0], list):
        times_list = times_or_times_list
    else:
        raise TypeError
    time_elapsed, num_frames = 0, 0
    for times in times_list:
        time_elapsed += max(times) - min(times)
        num_frames += len(times) - 1
    return num_frames / (time_elapsed / 1e6)


def mAP_of(boxes_or_boxes_list: Union[BoundingBoxes, List[BoundingBoxes]],
           save_dir: Path = None) -> float:
    if isinstance(boxes_or_boxes_list, BoundingBoxes):
        boxes = boxes_or_boxes_list
    elif isinstance(boxes_or_boxes_list, list):
        boxes = BoundingBoxes()
        for _boxes in boxes_or_boxes_list:
            boxes.concatBoundingBoxes(_boxes)
    else:
        raise TypeError
    evaluator = Evaluator()
    if save_dir is not None:
        results = evaluator.PlotPrecisionRecallCurve(
            boxes, savePath=save_dir, showInterpolatedPrecision=True)
    else:
        results = evaluator.GetPascalVOCMetrics(boxes)
    person_results = [r for r in results if r['class'] == PERSON]
    assert len(person_results) == 1, str(person_results)
    person_result = person_results[0]
    return person_result['AP']


class EvalFile:
    def __init__(self,
                 base_dir: Path,
                 boxes_txt: Path,
                 config_json: Path,
                 video_names: List[str],
                 partial_exec: bool,
                 plot_ap_curve: bool):
        if base_dir:
            assert boxes_txt is None and config_json is None
            boxes_txt = ExpUtil.boxes_txt_of(base_dir)
            config_json = ExpUtil.config_json_of(base_dir)
        if config_json:
            assert video_names is None
            video_names = ConfigJson(config_json).video_names
        assert boxes_txt is not None and video_names is not None
        self.boxes_txt = boxes_txt
        self.video_names = video_names
        self.partial_exec = partial_exec
        if plot_ap_curve:
            self.save_dir = self.boxes_txt.parent
        else:
            self.save_dir = None

    def eval(self) -> None:
        times, boxes_det = BoxesTxt(self.boxes_txt, self.video_names).time_boxes  # noqa
        boxes_list_gt = [VideoData(video_name, vid).boxes
                         for vid, video_name in enumerate(self.video_names)]
        if self.partial_exec:
            frame_ranges = frame_ranges_of(boxes_det)
            boxes_list_gt = [filter_gt_w_fid(boxes_gt, frame_ranges[vid])
                             for vid, boxes_gt in enumerate(boxes_list_gt)]
        else:
            total_num_frames = 0
            for video_name in self.video_names:
                total_num_frames += VideoData(video_name).num_frames
            assert len(times) == total_num_frames
        mAP = mAP_of([boxes_det] + boxes_list_gt, save_dir=self.save_dir)
        fps = fps_of(times)
        print(f'===== {self.boxes_txt} =====')
        print(f'mAP: {mAP:.4f}')
        print(f'FPS: {fps:.4f}')


class EvalExp:
    MAP_INDEX = 0
    FPS_INDEX = 1

    def __init__(self,
                 base_dir: Path,
                 partial_exec: bool,
                 no_all_exps: bool,
                 no_all_videos: bool,
                 reset: bool,
                 labels: List[str]):
        self.base_dir = base_dir
        self.partial_exec = partial_exec
        self.no_all_exps = no_all_exps
        self.no_all_videos = no_all_videos
        self.reset = reset
        self.labels = labels
        self.exps, self.video_names = self.valid_exps_video_names()

    def valid_exps_video_names(self):
        all_configs = list(self.base_dir.glob('*/*/config.json'))
        all_exps = sorted(set(config_json.parent.parent.name
                              for config_json in all_configs))
        all_video_names = sorted(set(config_json.parent.name
                                     for config_json in all_configs))
        assert not self.no_all_exps or not self.no_all_videos
        if self.no_all_exps:
            assert not self.no_all_videos
            video_names = all_video_names
            exps = [exp for exp in all_exps
                    if all([is_valid_exec(self.base_dir / exp / video_name, self.partial_exec)
                            for video_name in all_video_names])]
        elif self.no_all_videos:
            assert not self.no_all_exps
            exps = all_exps
            video_names = [video_name for video_name in all_video_names
                           if all([is_valid_exec(self.base_dir / exp / video_name, self.partial_exec)
                                   for exp in all_exps])]
        else:
            exps = all_exps
            video_names = all_video_names
        print(f'Videos          : {video_names}')
        print(f'All Videos      : {all_video_names}')
        print(f'Experiments     : {exps}')
        print(f'All Experiments : {all_exps}')
        return exps, video_names

    def eval(self) -> None:
        save_path = self.base_dir / 'result.pkl'
        if not save_path.exists() or self.reset:
            results, results_aggr = self.eval_exps()
            with save_path.open('wb') as f:
                pickle.dump((results, results_aggr), f)
        else:
            with save_path.open('rb') as f:
                results, results_aggr = pickle.load(f)
        self.print(results, results_aggr)
        self.plot(results, results_aggr)

    def eval_exps(self):
        results, results_aggr = {}, {}
        for exp in self.exps:
            exp_dir = self.base_dir / exp
            results[exp], results_aggr[exp] = \
                self.eval_exp(exp_dir)
        return results, results_aggr

    def eval_exp(self, exp_dir: Path):
        results, boxes_aggr, times_aggr = {}, [], []
        for video_name in tqdm(self.video_names, desc=f'Evaluating each video'):
            exec_dir = exp_dir / video_name
            config_json = ConfigJson(ExpUtil.config_json_of(exec_dir))
            assert config_json is not None
            times, boxes_det = BoxesTxt(ExpUtil.boxes_txt_of(exec_dir),
                                        config_json.video_names).time_boxes
            frame_ranges = frame_ranges_of(boxes_det)
            assert len(frame_ranges) == 1
            frame_range = next(iter(frame_ranges.values()))
            video_data = VideoData(video_name)
            boxes_gt = video_data.boxes
            if self.partial_exec:
                boxes_gt = filter_gt_w_fid(boxes_gt, frame_range)
            else:
                assert len(times) == video_data.num_frames
            boxes = concat_boxes(boxes_det, boxes_gt)
            boxes_aggr.append(boxes)
            times_aggr.append(times)
            results[video_name] = mAP_of(boxes), fps_of(times)
        results_aggr = mAP_of(boxes_aggr), fps_of(times_aggr)
        return results, results_aggr

    def print(self, results, results_aggr) -> None:
        # Print video-wise results
        for exp in self.exps:
            for video_name in sorted(results[exp].keys()):
                mAP, fps = results[exp][video_name]
                print(f'{exp:<20} {video_name:<20} | mAP: {mAP:>6.4f}  FPS: {fps:<6.4f}')  # noqa
        # Print aggregation results
        for exp in self.exps:
            mAP, fps = results_aggr[exp]
            print(
                f'{exp:<20} {"AGGREGATED":<20} | mAP: {mAP:>6.4f}  FPS: {fps:<6.4f}')

    def plot(self, results, results_aggr) -> None:

        def decorate(ax1, ax2, xticks, xticklabels, yticks, legend=True):
            for ax in [ax1, ax2]:
                ax.tick_params('x', which='major', direction='in', length=4, width=0)  # noqa
                ax.tick_params('x', which='minor', direction='in', length=4, width=0)  # noqa
                ax.tick_params('y', which='major', direction='in', length=4, width=4)  # noqa
                ax.tick_params('y', which='minor', direction='in', length=4, width=0)  # noqa
            for direction in ['top']:
                ax1.spines[direction].set_visible(False)
                ax2.spines[direction].set_visible(False)
            for direction in ['left', 'right', 'bottom']:
                ax1.spines[direction].set_linewidth(4)
                ax2.spines[direction].set_linewidth(4)
            ticklabels = ax1.get_yticklabels() + ax2.get_yticklabels()
            [label.set_fontweight('bold') for label in ticklabels]
            [label.set_fontsize(36) for label in ticklabels]
            ax1.set_ylabel('FPS', fontsize=40, fontweight='bold')
            ax2.set_ylabel('AP', fontsize=40, fontweight='bold')
            ax1.set_xticks(xticks)
            ax2.set_ylim(0, 1)
            ax1.set_yticks(yticks)
            ax2.set_yticks(np.arange(0, 1.2, 0.2))
            ax1.set_xticklabels(xticklabels, fontsize=28, fontweight='bold')
            if legend:
                ax1.legend(ncol=1, loc='lower left',
                           prop={'size': 12, 'weight': 'bold'})

        def to_label(video_name, delim=' '):
            dataset = dataset_of(video_name)
            index = int(video_name[-1])
            return f'{dataset}{delim}{index}'

        # Plot aggregated result
        fig, ax1 = plt.subplots(figsize=(12, 8))
        ax1.grid(axis='y', linestyle='--')
        ax1.set_axisbelow(True)
        ax2 = ax1.twinx()
        width = (1 - 0.2) / len(self.exps)
        xticks = [exp_id * width for exp_id in range(len(self.exps))]
        for exp_id, exp in enumerate(self.exps):
            ax1.bar(xticks[exp_id], results_aggr[exp][EvalExp.FPS_INDEX], width=width,
                    label=self.labels[exp_id] if self.labels else exp.upper())
        ax2.plot(xticks, [results_aggr[exp][EvalExp.MAP_INDEX]
                          for exp in self.exps], marker='x', markersize=5, color='red')
        ax1.set_xlim(-width, 1 - width)
        max_fps = max(results_aggr[exp][EvalExp.FPS_INDEX]
                      for exp in self.exps)
        yticks = np.arange(0, max_fps + 4, 4)
        decorate(ax1, ax2, xticks, self.labels, yticks, legend=not self.labels)
        fig.tight_layout()
        fig.savefig(self.base_dir / 'all.png')

        # Plot results for each video
        fig, ax1 = plt.subplots(figsize=(20, 7))
        ax2 = ax1.twinx()
        width = (1 - 0.2) / len(self.exps)
        for exp_id, exp in enumerate(self.exps):
            x, fpss = [], []
            for video_id, video_name in enumerate(self.video_names):
                x.append(video_id + exp_id * width)
                if video_name in results[exp]:
                    fpss.append(results[exp][video_name][EvalExp.FPS_INDEX])
                else:
                    fpss.append(0)
            ax1.bar(x, fpss,
                    width=width,
                    label=self.labels[exp_id] if self.labels else exp.upper())
        for video_id, video_name in enumerate(self.video_names):
            x, aps = [], []
            for exp_id, exp in enumerate(self.exps):
                x.append(video_id + exp_id * width)
                if video_name in results[exp]:
                    aps.append(results[exp][video_name][EvalExp.MAP_INDEX])
                else:
                    aps.append(0)
            ax2.plot(x, aps,
                     marker='x', markersize=5, color='red',
                     label=to_label(video_name))
        xticks = np.arange(len(self.video_names)) + \
            (len(self.exps) - 1) * width / 2
        xticklabels = [to_label(video_name, delim='\n')
                       for video_name in self.video_names]
        yticks = np.arange(0, math.ceil(ax1.get_ylim()[1] / 4) * 4, 4)
        decorate(ax1, ax2, xticks, xticklabels, yticks, legend=True)
        fig.tight_layout()
        fig.savefig(self.base_dir / 'videos.png')
        plt.close(fig)


class EvalBaseline:
    def __init__(self,
                 base_dir: Path,
                 reset: bool):
        self.base_dir = base_dir
        self.reset = reset

    def eval(self) -> None:
        cache_path = self.base_dir / 'cache.pkl'
        if not self.reset and cache_path.exists():
            print(f'Loading from cache: {cache_path}')
            with open(cache_path, 'rb') as f:
                results, results_aggr = pickle.load(f)
        else:
            results, results_aggr = self.eval_baseline()
            with open(cache_path, 'wb') as f:
                pickle.dump((results, results_aggr), f)

        video_names = sorted(results.keys())
        for video_name in video_names:
            mAP, fps = results[video_name]
            print(f'{video_name:<20} | mAP: {mAP:.4f}  FPS: {fps:.4f}')
        print(f'{"AGGREGATED":<20} | mAP: {results_aggr[0]:.4f}  FPS: {results_aggr[1]:.4f}')  # noqa

    def eval_baseline(self) -> Tuple[Dict[str, Tuple[float, float]], Tuple[float, float]]:
        boxes_txts = sorted(self.base_dir.glob('*boxes.txt'))
        results, boxes_aggr, times_aggr = {}, [], []
        for boxes_txt in boxes_txts:
            video_name = boxes_txt.name[:boxes_txt.name.find('.mp4')]
            print(f'Evaluating {video_name}...', end='', flush=True)
            start_time = time()

            times, boxes = BoxesTxt(boxes_txt, [video_name]).time_boxes
            video_data = VideoData(video_name)
            boxes.concatBoundingBoxes(video_data.boxes)  # GT
            mAP = mAP_of(boxes)
            fps = fps_of(times) * 1000  # FlexPatch use ms

            elapsed = time() - start_time
            results[video_name] = mAP, fps
            print(f'Done ({elapsed:.2f}s) mAP: {mAP:.4f}  FPS: {fps:.4f}')

            boxes_aggr.append(boxes)
            times_aggr.append(times)

        print(f'Evaluating ALL videos...', end='', flush=True)
        start_time = time()
        mAP_aggr = mAP_of(boxes_aggr)
        fps_aggr = fps_of(times_aggr) * 1000  # FlexPatch use ms
        elapsed = time() - start_time
        results_aggr = mAP_aggr, fps_aggr
        print(f'Done ({elapsed:.2f}s) mAP: {mAP_aggr:.4f}  FPS: {fps_aggr:.4f}')
        return results, results_aggr


if __name__ == "__main__":
    parser = argparse.ArgumentParser()
    subparsers = parser.add_subparsers(dest='type')

    parser_file = subparsers.add_parser('file')
    parser_file.add_argument('-d', '--base-dir', type=Path)
    parser_file.add_argument('-b', '--boxes-txt', type=Path)
    parser_file.add_argument('-c', '--config-json', type=Path)
    parser_file.add_argument('-v', '--video-names', nargs='*')
    parser_file.add_argument('-p', '--partial-exec', action='store_true')
    parser_file.add_argument('--plot-ap-curve', action='store_true')

    parser_exp = subparsers.add_parser('exp')
    parser_exp.add_argument('-d', '--base-dir', type=Path, required=True)
    parser_exp.add_argument('-p', '--partial-exec', action='store_true')
    parser_exp.add_argument('-e', '--no-all-exps', action='store_true')
    parser_exp.add_argument('-v', '--no-all-videos', action='store_true')
    parser_exp.add_argument('-r', '--reset', action='store_true')
    parser_exp.add_argument('-l', '--labels', nargs='*', default=[])

    parser_baseline = subparsers.add_parser('baseline')
    parser_baseline.add_argument('-d', '--base-dir', type=Path)
    parser_baseline.add_argument('-r', '--reset', action='store_true')

    args = parser.parse_args()
    exp_type = args.type
    del args.type
    print(args)

    if exp_type == 'file':
        EvalFile(**vars(args)).eval()
    elif exp_type == 'exp':
        EvalExp(**vars(args)).eval()
    elif exp_type == 'baseline':
        EvalBaseline(**vars(args)).eval()
    else:
        raise ValueError(f'Unknown type: {args.type}')
