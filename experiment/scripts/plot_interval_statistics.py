import argparse
from pathlib import Path
import re

import numpy as np
import matplotlib.pyplot as plt
import pandas as pd


def num_packed_frames_per_interval(roi_df, timeline_df, schedule_times):
    num_packed_frames_list = []
    for schedule_time in schedule_times:
        df = timeline_df[timeline_df['scheduledTime'] == schedule_time]
        num_full_inferences = len(set(df['fullInferenceStartTime']) - {0})
        num_packed_inferences = len(set(df['mixedInferenceStartTime']) - {0})
        num_packed_frames_list.append(num_full_inferences + num_packed_inferences)
    return num_packed_frames_list


def num_frames_per_interval(roi_df, timeline_df, schedule_times):
    return [sum(timeline_df['scheduledTime'] == schedule_time)
            for schedule_time in schedule_times]


def num_rois_per_interval(roi_df, timeline_df, schedule_times):
    num_rois_list = []
    for schedule_time in schedule_times:
        df = timeline_df[timeline_df['scheduledTime'] == schedule_time]
        frame_indices = df['frameIndex']
        num_rois_list.append(sum(roi_df['frameIndex'].isin(frame_indices)))
    return num_rois_list


def roi_area_per_interval(roi_df, timeline_df, schedule_times):
    roi_areas = []
    for schedule_time in schedule_times:
        _timeline_df = timeline_df[timeline_df['scheduledTime'] == schedule_time]
        frame_indices = _timeline_df['frameIndex']
        _roi_df = roi_df[roi_df['frameIndex'].isin(frame_indices)]
        xyxyss_arr = np.unique(_roi_df[[
            'mergedLoc_l',
            'mergedLoc_t',
            'mergedLoc_r',
            'mergedLoc_b',
            'mergedScale',
            'absolutePackedCanvasIndex']].values, axis=0)
        assert xyxyss_arr.ndim == 2
        roi_area = (xyxyss_arr[:, 2] - xyxyss_arr[:, 0]) \
            * (xyxyss_arr[:, 3] - xyxyss_arr[:, 1]) \
            * xyxyss_arr[:, 4] ** 2
        roi_areas.append(np.sum(roi_area))
    return roi_areas


def inference_area_per_interval(roi_df, timeline_df, schedule_times):
    inference_areas = []
    for schedule_time in schedule_times:
        _timeline_df = timeline_df[timeline_df['scheduledTime'] == schedule_time]
        frame_indices = _timeline_df['frameIndex']
        _roi_df = roi_df[roi_df['frameIndex'].isin(frame_indices)]

        inference_area = 0
        full_start_times = sorted(list(
            set(_timeline_df['fullInferenceStartTime']) - {0}))
        for full_start_time in full_start_times:
            mask = _timeline_df['fullInferenceStartTime'] == full_start_time
            inference_area += _timeline_df[mask]['inferenceFrameSize'].values[0] ** 2

        si_arr = np.unique(_roi_df[[
            'packedCanvasSize',
            'absolutePackedCanvasIndex']].values, axis=0)
        for s, _ in si_arr:
            inference_area += s**2
        inference_areas.append(inference_area)
    return inference_areas


def fillup_ratio_per_interval(roi_df, timeline_df, schedule_times):
    fillup_ratios = []
    for schedule_time in schedule_times:
        _timeline_df = timeline_df[timeline_df['scheduledTime'] == schedule_time]
        frame_indices = _timeline_df['frameIndex']
        _roi_df = roi_df[roi_df['frameIndex'].isin(frame_indices)]
        xyxyss_arr = np.unique(_roi_df[[
            'mergedLoc_l',
            'mergedLoc_t',
            'mergedLoc_r',
            'mergedLoc_b',
            'mergedScale',
            'absolutePackedCanvasIndex']].values, axis=0)
        si_arr = np.unique(_roi_df[[
            'packedCanvasSize',
            'absolutePackedCanvasIndex']].values, axis=0)
        assert xyxyss_arr.ndim == 2 and si_arr.ndim == 2
        roi_area = (xyxyss_arr[:, 2] - xyxyss_arr[:, 0]) \
            * (xyxyss_arr[:, 3] - xyxyss_arr[:, 1]) \
            * xyxyss_arr[:, 4] ** 2
        inference_area = si_arr[:, 0]**2
        fillup_ratios.append(np.sum(roi_area) / np.sum(inference_area)
                             if len(si_arr) > 0 else 0)
    return fillup_ratios


def plot_single_run(dir, fig_axes, statistics):
    roi_csv = sorted(dir.glob('roi*.csv'),
                     key=id_of, reverse=True)[0]
    timeline_csv = sorted(dir.glob('timeline*.csv'),
                          key=id_of, reverse=True)[0]
    roi_df = pd.read_csv(roi_csv, delimiter='\t')
    timeline_df = pd.read_csv(timeline_csv, delimiter='\t')
    schedule_times = sorted(list(set(timeline_df['scheduledTime'])))
    start_indices = [
        min(timeline_df[timeline_df['scheduledTime'] == schedule_time]['frameIndex'])
        for schedule_time in schedule_times
    ]
    for name, (func, ylabel) in statistics.items():
        ax = fig_axes[name][1]
        values = func(roi_df, timeline_df, schedule_times)
        ax.plot(start_indices, values, label=dir.name)

        ax.set_xlim(min(start_indices), max(start_indices))
        ax.set_ylim(0, ax.get_ylim()[1])
        ax.set_xlabel('Frame Index')
        ax.set_ylabel(ylabel)


if __name__ == '__main__':
    parser = argparse.ArgumentParser()
    parser.add_argument('-e', '--exp-dir', type=Path)
    parser.add_argument('-d', '--dir', type=Path)
    parser.add_argument('-f', '--figsize', type=float, nargs=2,
                        default=(20, 5))
    args = parser.parse_args()
    assert (args.exp_dir is None) != (args.dir is None)

    statistics = {
        'num_packed_frames': [
            num_packed_frames_per_interval,
            '# Packed Frames',
        ],
        'num_frames': [
            num_frames_per_interval,
            '# Frames',
        ],
        'num_rois': [
            num_rois_per_interval,
            '# ROIs',
        ],
        'roi_areas': [
            roi_area_per_interval,
            'ROI Area',
        ],
        'inference_areas': [
            inference_area_per_interval,
            'Inference Area',
        ],
        'fillup_ratio': [
            fillup_ratio_per_interval,
            'Fillup Ratio',
        ],
    }

    fig_axes = {
        name: plt.subplots(figsize=args.figsize)
        for name in statistics.keys()
    }

    def smoothing(x, y, window_size=5):
        assert len(x) == len(y)
        xs, ys = [], []
        for i in range(len(x) - window_size + 1):
            xs.append(np.mean(x[i:i+window_size]))
            ys.append(np.mean(y[i:i+window_size]))
        return np.array(xs), np.array(ys)

    def id_of(log_path: Path):
        m = re.search(r'_(\d+).[a-z]{3,4}$', str(log_path))
        return int(m.group(1)) if m is not None else 0

    if args.dir:
        plot_single_run(args.dir, fig_axes, statistics)

    if args.exp_dir:
        dirs = [config_json.parent
                for config_json in Path(args.exp_dir).glob('*/*/config.json')]
        for dir in dirs:
            plot_single_run(dir, fig_axes, statistics)
        for fig, ax in fig_axes.values():
            ax.legend()

    for name, (fig, ax) in fig_axes.items():
        fig.savefig(f'figures/{name}.png', bbox_inches='tight')
