import argparse
from collections import defaultdict
import json
from itertools import chain
from pathlib import Path
from typing import List
import random
import re

import numpy as np
import pandas as pd
import matplotlib as mpl
import matplotlib.pyplot as plt
from matplotlib.collections import PolyCollection
plt.rcParams['hatch.color'] = 'white'


PDID = 'PDExtractorID'
OFID = 'OFExtractorID'

# Columns
frameIndex = 'frameIndex'
inferenceFrameSize = 'inferenceFrameSize'
inferenceDevice = 'inferenceDevice'

enqueueTime = 'enqueueTime'
scheduleTime = 'scheduledTime'
fullStart = 'fullInferenceStartTime'
fullEnd = 'fullInferenceEndTime'
PDStart = 'pixelDiffROIProcessStartTime'
PDEnd = 'pixelDiffROIProcessEndTime'
OFStart = 'opticalFlowROIProcessStartTime'
OFEnd = 'opticalFlowROIProcessEndTime'
resizeStart = 'resizeStartTime'
resizeEnd = 'resizeEndTime'
mergeStart = 'mergeROIStartTime'
mergeEnd = 'mergeROIEndTime'
packingStart = 'packingStartTime'
packingEnd = 'packingEndTime'
inferenceStart = 'packedInferenceStartTime'
inferenceEnd = 'packedInferenceEndTime'
reconstructStart = 'reconstructStartTime'
reconstructEnd = 'reconstructEndTime'
endTime = 'endTime'

START_END_PAIRS = {
    fullStart: fullEnd,
    PDStart: PDEnd,
    OFStart: OFEnd,
    resizeStart: resizeEnd,
    mergeStart: mergeEnd,
    packingStart: packingEnd,
    inferenceStart: inferenceEnd,
    reconstructStart: reconstructEnd,
}


class Component:
    Packing = f'Pack'
    Reconstruct = f'Reconstruct'
    _ROIExtraction = f'E & S & M'
    _Inference = 'Inference'

    @classmethod
    def ROIExtraction(cls, cpu_id):
        return f'{cls._ROIExtraction} {int(cpu_id)}'

    @classmethod
    def isROIExtraction(cls, component):
        return component.find(cls._ROIExtraction) != -1

    @classmethod
    def Inference(cls, device):
        device = int(device)
        assert device in [0, 1]
        device_name = 'GPU' if device == 0 else 'DSP'
        # return f'{device_name} {cls._Inference}'
        return f'{cls._Inference}'

    @classmethod
    def isInference(cls, component):
        return component.find(cls._Inference) != -1

    @classmethod
    def get(cls, row, startCol):
        if startCol in [fullStart, inferenceStart]:
            return Component.Inference(row[inferenceDevice])
        elif startCol == PDStart:
            return Component.ROIExtraction(row[PDID])
        elif startCol in [OFStart, resizeStart, mergeStart]:
            return Component.ROIExtraction(row[OFID])
        elif startCol == packingStart:
            return Component.Packing
        elif startCol == reconstructStart:
            return Component.Reconstruct
        else:
            raise ValueError(f'Wrong startCol: {startCol}')

    @classmethod
    def sort_key(cls, component: 'Component'):
        if Component.isROIExtraction(component):
            return int(re.search(r'(\d+)$', component).group(1))
        elif component == Component.Packing:
            return 10
        elif Component.isInference(component):
            device_name = 'GPU'
            # if 'GPU' in component:
            #     assert 'DSP' not in component
            #     device_name = 'GPU'
            # elif 'DSP' in component:
            #     device_name = 'DSP'
            # else:
            #     raise ValueError(f'Wrong component: {component}')
            return 20 + (0 if device_name == 'GPU' else 1)
        elif component == Component.Reconstruct:
            return 30
        else:
            raise ValueError(f'Wrong component: {component}')


class Segment:
    def __init__(self, row, startCol):
        self.row = row
        self.startTime = row[startCol]
        self.endTime = row[START_END_PAIRS[startCol]]
        self.component = Component.get(row, startCol)

    @classmethod
    def generate(cls, row):
        segments = []
        if row[fullStart] != 0:  # Full frame inference
            assert row[fullEnd] != 0
            for col in [inferenceStart, inferenceEnd, reconstructStart, reconstructEnd]:
                assert row[col] == 0

            segments.append(Segment(row, fullStart))
            if row[PDStart] != 0:
                for col in [PDID, OFID]:
                    assert row[col] != -1
                for col in [PDEnd, OFStart, OFEnd, resizeStart, resizeEnd, mergeStart, mergeEnd]:
                    assert row[col] != 0
                segments.append(Segment(row, PDStart))
                segments.append(Segment(row, OFStart))
                segments.append(Segment(row, resizeStart))
                segments.append(Segment(row, mergeStart))
                segments.append(Segment(row, packingStart))
        else:  # Packed inference
            for col in [fullStart, fullEnd]:
                assert row[col] == 0
            for col in [PDEnd, OFStart, OFEnd, resizeStart, resizeEnd, mergeStart, mergeEnd, inferenceStart, inferenceEnd, reconstructStart, reconstructEnd]:
                assert row[col] != 0
            segments.append(Segment(row, PDStart))
            segments.append(Segment(row, OFStart))
            segments.append(Segment(row, resizeStart))
            segments.append(Segment(row, mergeStart))
            segments.append(Segment(row, packingStart))
            segments.append(Segment(row, inferenceStart))
            segments.append(Segment(row, reconstructStart))
        return segments


def print_times(df):
    times = defaultdict(list)
    for _, row in df.iterrows():
        for startTime, endTime in START_END_PAIRS.items():
            if row[startTime] > 0:
                times[startTime].append((row[startTime], row[endTime]))
    print('=' * 20 + ' Times ' + '=' * 20)
    print(f'{"":<30} {"Avg (ms)":>8} {"Std (ms)":>8}')
    for startTime, start_end_times in times.items():
        counts = defaultdict(lambda: 0)
        for s, _ in start_end_times:
            counts[s] += 1
        durations = [(e - s)/counts[s] for s, e in start_end_times]
        print(f'{startTime:<30} {np.mean(durations)*1000:>8.2f} {np.std(durations)*1000:>8.2f}')
    print('=' * 47)


def plot_timeline(df, fig, ax, cax, segments, time_range, input_sizes,
                  margin=0.2, no_schedule_line=None, start_line=None, end_line=None):
    mapPackedCanvasIndex = {
        endTime: i
        for i, endTime in enumerate(sorted(set(df[inferenceEnd])))
    }
    mapReconIndex = {
        endTime: i
        for i, endTime in enumerate(sorted(set(df[reconstructEnd])))
    }

    xlabelfontsize = 50
    ylabelfontsize = 50
    xticklabelfontsize = 30
    lw = 4

    max_index = int(max([segment.row[frameIndex]
                         for segment in segments
                         if segment.row[reconstructEnd] > time_range[0]]))
    min_index = int(min([segment.row[frameIndex]
                         for segment in segments
                         if segment.row[PDStart] < time_range[1]]))

    def index_color(index, cmap='viridis', color_interval=0):
        if color_interval == 0:
            v = max(0, min(1, (index - min_index) / (max_index - min_index)))
        else:
            v = (index % color_interval) / color_interval
        return plt.get_cmap(cmap)(v)

    def to_box(segment: Segment):
        start, end = segment.startTime, segment.endTime
        height = heights[segment.component]
        return [
            (start, height - (0.5 - margin/2)),
            (start, height + (0.5 - margin/2)),
            (end,   height + (0.5 - margin/2)),
            (end,   height - (0.5 - margin/2)),
        ]

    components = sorted(list(set([segment.component for segment in segments])),
                        key=Component.sort_key)
    heights = {
        component: i
        for i, component in enumerate(components)
    }

    # Plot timeline without InferenceEngine
    for segment in segments:
        if Component.isInference(segment.component):
            color = index_color(mapPackedCanvasIndex[segment.row[inferenceEnd]],
                                cmap='tab10', color_interval=10)
        elif segment.component == Component.Reconstruct:
            color = index_color(mapReconIndex[segment.row[reconstructEnd]],
                                cmap='tab10', color_interval=10)
        else:
            color = index_color(segment.row[frameIndex])
        ax.add_collection(PolyCollection([to_box(segment)], facecolor=color))
        if Component.isInference(segment.component) and time_range[0] <= segment.startTime and segment.endTime < time_range[1]:
            ax.text((segment.startTime + segment.endTime) / 2, heights[segment.component],
                    str(int(segment.row[inferenceFrameSize])), fontsize=30,
                    color='white', ha='center', va='center')

    # Schedule Times / Start Times / End Times
    if not no_schedule_line:
        scheduleTimes = set([segment.row[scheduleTime] for segment in segments])
        for t in scheduleTimes:
            ax.axvline(x=t, color='red', linestyle='--')
    if start_line:
        enqueueTimes = set([segment.row[enqueueTime] for segment in segments])
        for t in enqueueTimes:
            ax.axvline(x=t, color='blue', linestyle='--')
    if end_line:
        endTimes = set([segment.row[enqueueTime] for segment in segments])
        for t in endTimes:
            ax.axvline(x=t, color='black', linestyle='--')

    cb = fig.colorbar(mpl.cm.ScalarMappable(cmap=plt.get_cmap('viridis')), cax=cax, fraction=0.046, pad=0.04, orientation='horizontal')
    cb.ax.set_xticks([0, 0.5, 1])
    cb.ax.set_xticklabels(['Earlier', 'FrameIndex', 'Later'], fontsize=30, fontweight='bold')
    cb.ax.tick_params('both', which='major', length=0)
    cb.ax.xaxis.tick_top()
    cb.ax.xaxis.set_label_position('top')

    ax.autoscale()
    ax.set_xlim(min([segment.startTime for segment in segments]),
                max([segment.endTime for segment in segments]))
    ax.set_ylim(len(components) - 1 + 0.64, -0.64)
    ax.set_yticks(np.arange(len(components)))
    ax.set_yticklabels(components, fontsize=ylabelfontsize, fontweight='bold')
    ax.set_xlabel('Time (s)', fontsize=xlabelfontsize, fontweight='bold')
    ax.tick_params('y', which='major', pad=20)
    
    for spine in cb.ax.spines.values():
        spine.set_linewidth(lw)
        spine.set_linewidth(lw)
        spine.set_linewidth(lw)
        spine.set_linewidth(lw)

    for spine in ax.spines.values():
        spine.set_linewidth(lw)
        spine.set_linewidth(lw)
        spine.set_linewidth(lw)
        spine.set_linewidth(lw)

    ax.tick_params('x', which='minor', direction='in', length=0, width=lw)
    ax.tick_params('y', which='major', direction='in', length=0, width=lw)
    ax.tick_params('x', which='major', direction='in', length=0, width=lw)
    ax.tick_params('y', which='minor', direction='in', length=0, width=lw)

    labels = ax.get_xticklabels()
    [label.set_fontweight('bold') for label in labels]
    [label.set_fontsize(xticklabelfontsize) for label in labels]


def main(timeline_csv: Path, input_sizes, time_range, schedule_range,
         figsize, no_plot, no_schedule_line, start_line, end_line):
    df = pd.read_csv(timeline_csv, delimiter='\t')
    time_columns = [column for column in df.columns if 'Time' in column]
    df[time_columns] = df[time_columns] / 1e6  # us to s
    if time_range is None and schedule_range is not None:
        scheduleTimes = sorted(list(set(df[scheduleTime])))
        time_range = [scheduleTimes[i] for i in schedule_range]

    df = df[(time_range[0] <= df[endTime])
            & (df[enqueueTime] <= time_range[1])]
    segments = list(chain.from_iterable([Segment.generate(row)
                                         for _, row in df.iterrows()]))
    print(f'# Frames in interval: {df.shape[0]}')
    print(f'# Segments to plot: {len(segments)}')

    print_times(df)

    if no_plot:
        return

    figsize = ((time_range[1] - time_range[0]) * 4 if figsize is None else figsize), 7
    print(f'figsize=={figsize}')
    fig, axes = plt.subplots(2, 1, figsize=figsize,
                             gridspec_kw={'height_ratios': [1, 15]})
    plt.subplots_adjust(hspace=0.05)
    plot_timeline(df, fig, axes[1], axes[0], segments, time_range, input_sizes,
                  no_schedule_line=no_schedule_line, start_line=start_line, end_line=end_line)
    axes[1].set_xlim(time_range)
    fig.savefig(f'figures/schedule_timeline.png', bbox_inches='tight')


if __name__ == '__main__':
    parser = argparse.ArgumentParser()
    parser.add_argument('-d', '--dir', type=Path, required=True)
    parser.add_argument('-t', '--time-range', type=float, nargs=2,
                        metavar=('START TIME', 'END TIME'))
    parser.add_argument('-s', '--schedule-range', type=int, nargs=2,
                        default=(0, -1), metavar=('START INTERVAL', 'END INTERVAL'))
    parser.add_argument('-f', '--figsize', type=float, help='WIDTH')
    parser.add_argument('--no-plot', action='store_true')
    parser.add_argument('--no-schedule-line', action='store_true')
    parser.add_argument('--start-line', action='store_true')
    parser.add_argument('--end-line', action='store_true')
    args = parser.parse_args()
    print(args)

    assert (args.time_range is None) != (args.schedule_range is None)

    def id_of(log_path: Path):
        m = re.search(r'_(\d+).[a-z]{3,4}$', str(log_path))
        return int(m.group(1)) if m is not None else 0

    timeline_csv = sorted(args.dir.glob('timeline*.csv'),
                          key=id_of, reverse=True)[0]

    with (args.dir / 'config.json').open('r') as f:
        config = json.load(f)
        input_sizes = config['inference_engine']['input_sizes']
        input_sizes += [config['full_frame_size']]

    main(timeline_csv, input_sizes, args.time_range, args.schedule_range,
         args.figsize, args.no_plot, args.no_schedule_line, args.start_line, args.end_line)
