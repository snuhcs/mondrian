import argparse
from collections import Counter, defaultdict
import json
from pathlib import Path
from typing import List
import re

import numpy as np
import matplotlib as mpl
import matplotlib.pyplot as plt
import pandas as pd


SCALE_LEVELS = {
    'VIRAT': [0.2, 0.4, 0.9, 1.0],
    'MTA': [0.2, 0.3, 0.5, 0.7, 1.0],
}


def main(exp_name, roi_csv: Path, timeline_csv: Path, scale_levels: List[float],
         probe_step_size: float, min_count=0, plot_schedule=False, no_count=False, eps=False):
    timeline_df = pd.read_csv(timeline_csv, delimiter='\t')
    roi_df = pd.read_csv(roi_csv, delimiter='\t')
    print(f'IsProbingROI: {Counter(roi_df["isProbingROI"])}')

    start_indicies = sorted([
        min(timeline_df[timeline_df['scheduledTime'] == schedule_time]['frameIndex'])
        for schedule_time in set(timeline_df['scheduledTime'])
    ])

    roi_ids = [roi_id
               for roi_id, count in Counter(roi_df['id']).items()
               if min_count <= count]
    print(f'{len(roi_ids)} ROI IDs with {min_count} <= occurence')

    figure_dir = Path('figures') / f'{exp_name}_{roi_csv.parent.name}'
    figure_dir.mkdir(exist_ok=True)
    xlim = (min(roi_df['frameIndex']), max(roi_df['frameIndex']))
    for roi_id in roi_ids:
        _roi_df = roi_df[roi_df['id'] == roi_id]
        mask = _roi_df['isProbingROI'].values == 0
        frame_indices = _roi_df['frameIndex'].values
        pred_scales = np.array([scale_levels[int(level)]
                                for level in _roi_df['scaleLevel'].values])
        scales = _roi_df['targetScale'].values

        mask, frame_indices, pred_scales, scales = [
            v[np.argsort(frame_indices)]
            for v in [mask, frame_indices, pred_scales, scales]]

        fig, ax = plt.subplots(figsize=(6, 4))
        # fig, ax = plt.subplots(figsize=((xlim[1] - xlim[0]) / 200, 4))
        if plot_schedule:
            for start_index in start_indicies:
                ax.axvline(x=start_index,
                           color='black', linestyle='--', linewidth=0.5)

        ax.plot(frame_indices, pred_scales, linewidth=1, color='black',
                label='Predicted Scale')
        ax.scatter(frame_indices[mask], scales[mask], s=3, color='blue',
                   label='Estimated Safe Scale')
        ax.scatter(frame_indices[~mask], scales[~mask], s=15, marker='D', color='red',
                   label='Probe Scale')

        for direction in ['left', 'top', 'right', 'bottom']:
            ax.spines[direction].set_linewidth(2)

        ax.tick_params('both', which='major', length=0)
        labels = ax.get_xticklabels() + ax.get_yticklabels()
        [label.set_fontweight('bold') for label in labels]
        [label.set_fontsize(30) for label in labels]

        ax.set_xlabel('Frame Index', fontsize=36, fontweight='bold')
        ax.set_ylabel('Safe Scale', fontsize=36, fontweight='bold')
        # ax.set_xlim(min(frame_indices), max(frame_indices))
        # ax.set_xlim(xlim)
        ax.set_xlim(4538, 5200)
        ax.set_xticks([4600, 5000])
        ax.set_ylim(0, 0.65)
        ax.set_yticks([0, 0.2, 0.4, 0.6])
        # ax.set_ylim(0, 1.0)
        # ax.set_yticks([0, 0.2, 0.4, 0.6, 0.8, 1.0])

        handles, labels = ax.get_legend_handles_labels()
        order = [1, 2, 0]
        leg = ax.legend([handles[idx] for idx in order],
                        [labels[idx] for idx in order],
                        ncol=3,
                        loc=[0.001, 1.025],
                        prop={'size': 24, 'weight': 'bold'})
        leg.get_frame().set_linewidth(0)
        # leg = ax.legend(loc='upper right', prop={'size': 24, 'weight': 'bold'})
        for handle in leg.legendHandles:
            if isinstance(handle, mpl.collections.PathCollection):
                handle.set_sizes([40])

        fig.savefig(figure_dir / f'scale_timeline_{roi_id}.png',
                    bbox_inches='tight')
        if eps:
            fig.savefig(figure_dir / f'scale_timeline_{roi_id}.eps', format='eps',
                        bbox_inches='tight')
        plt.close(fig)
        break

    if no_count:
        return

    # Analyze probing
    counts = defaultdict(lambda: 0)
    for roi_id in roi_ids:
        _roi_df = roi_df[roi_df['id'] == roi_id]
        frame_indices = sorted(list(set(_roi_df['frameIndex'].values)))
        cROI_df = _roi_df[_roi_df['isProbingROI'] == 0]
        scaleLevels = {}
        targetScales = {}
        for frame_index in frame_indices:
            _df = cROI_df[cROI_df['frameIndex'] == frame_index]
            assert _df.shape[0] == 1
            targetScales[frame_index] = _df['targetScale'].values[0]
            scaleLevels[frame_index] = _df['scaleLevel'].values[0]

        for frame_index in frame_indices:
            _df = _roi_df[_roi_df['frameIndex'] == frame_index]
            if _df.shape[0] > 1 \
                and (frame_index + 1) in targetScales \
                    and scaleLevels[frame_index] == scaleLevels[frame_index + 1]:
                targetScale = targetScales[frame_index]
                nextTargetScale = targetScales[frame_index + 1]
                steps = (targetScale - nextTargetScale) / (targetScale * probe_step_size)
                if abs(round(steps) - steps) > 1e-3:
                    print(f'curr: {targetScale}, next: {nextTargetScale} step_size: {targetScale * probe_step_size} steps: {steps}')
                else:
                    counts[round(steps)] += 1
    sorted_counts = [(s, c) for s, c in sorted(counts.items(), key=lambda e: e[0])]
    print(f'Probe Step Count: {sorted_counts}')


if __name__ == '__main__':
    parser = argparse.ArgumentParser()
    parser.add_argument('-d', '--dir', type=Path, required=True)
    parser.add_argument('-m', '--min-count', type=int, default=400)
    parser.add_argument('-s', '--plot-schedule', action='store_true')
    parser.add_argument('-n', '--no-count', action='store_true')
    parser.add_argument('--eps', action='store_true')
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
        probe_step_size = config['roi_resizer']['probe_step_size']
        scale_levels = SCALE_LEVELS[config['roi_resizer']['dataset']]
        scale_levels = [v + config['roi_resizer']['scale_shift']
                        for v in scale_levels]

    main(args.dir.parent.name, roi_csv, timeline_csv, scale_levels,
         probe_step_size, args.min_count, args.plot_schedule,
         no_count=args.no_count, eps=args.eps)
