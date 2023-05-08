import argparse
from collections import Counter, defaultdict
import json
from pathlib import Path
import re

import numpy as np
import matplotlib.pyplot as plt
import pandas as pd
from tqdm import tqdm


def id_of(log_path: Path):
    m = re.search(r'_(\d+).[a-z]{3,4}$', str(log_path))
    return int(m.group(1)) if m is not None else 0


def plot_num_windows_to_min_scale(dir: Path):
    exp_dirs = list(dir.glob('*_num_probe_steps_*'))
    assert len(exp_dirs) > 0

    num_windows_to_min_scale_dict = {}
    for exp_dir in exp_dirs:
        config_jsons = list(exp_dir.glob('*/config.json'))
        assert len(config_jsons) == 1
        config_json = config_jsons[0]
        with config_json.open('r') as f:
            config = json.load(f)
        num_probe_steps = config['roi_resizer']['num_probe_steps']
        video_dir = config_json.parent
        roi_df = pd.read_csv(sorted(video_dir.glob('roi*.csv'),
                                    key=id_of, reverse=True)[0], delimiter='\t')
        timeline_df = pd.read_csv(sorted(video_dir.glob('timeline*.csv'),
                                         key=id_of, reverse=True)[0], delimiter='\t')
        to_schedule_id = {
            row['frameIndex']: row['scheduleID']
            for _, row in timeline_df.iterrows()
        }
        roi_df['scheduleID'] = roi_df['frameIndex'].map(to_schedule_id)
        roi_ids = sorted(list(set(roi_df['id'])))

        # The number of windows to arrive min scale
        num_windows_to_min_scale = []
        for roi_id in tqdm(roi_ids, total=len(roi_ids), desc='Num Windows'):
            _roi_df = roi_df[roi_df['id'] == roi_id]
            _roi_df = _roi_df[_roi_df['isProbingROI'] == 0]
            _roi_df = _roi_df.sort_values('frameIndex')
            assert np.all(np.diff(_roi_df['frameIndex'].values) == 1)
            assert np.all(np.diff(_roi_df['scheduleID'].values) >= 0)

            targetScales = {}
            for schedule_id in sorted(list(set(_roi_df['scheduleID']))):
                targetScales[schedule_id] = Counter(_roi_df[_roi_df['scheduleID'] == schedule_id]['targetScale']).most_common(1)[0][0]

            if max(targetScales.values()) - min(targetScales.values()) <= 0.2:
                continue

            min_scale = min(targetScales.values())
            for i, schedule_id in enumerate(sorted(targetScales.keys())):
                if min_scale == targetScales[schedule_id]:
                    num_windows_to_min_scale.append(i)
                    break
        num_windows_to_min_scale_dict[num_probe_steps] = np.mean(num_windows_to_min_scale)

    num_probe_steps_list = sorted(list(set(num_windows_to_min_scale_dict.keys())))


    lw = 2

    fig, ax = plt.subplots(figsize=(12, 4))

    ax.plot(num_probe_steps_list, [
        num_windows_to_min_scale_dict[num_probe_steps]
        for num_probe_steps in num_probe_steps_list], marker='x')

    # ax.set_xlim(0, 4)
    # ax.set_xticks([0, 1, 2, 3, 4])
    # ax.set_ylim(0, 120000)
    # ax.set_yticks([0, 40000, 80000, 120000])

    ax.set_xlabel('Scheduling Window Index', fontsize=30, fontweight='bold')
    ax.set_ylabel('Num Windows to Min Scale', fontsize=30, fontweight='bold')
    # ax.tick_params('x', which='major', pad=20)

    ax.spines['left'].set_linewidth(lw)
    ax.spines['top'].set_visible(False)
    ax.spines['right'].set_linewidth(False)
    ax.spines['bottom'].set_linewidth(lw)

    ax.tick_params('both', which='major', direction='in', length=6, width=lw)

    ticklabels = ax.get_xticklabels() + ax.get_yticklabels()
    [ticklabel.set_fontsize(24) for ticklabel in ticklabels]
    [ticklabel.set_fontweight('bold') for ticklabel in ticklabels]

    leg = ax.legend(ncol=1, loc='upper right', prop={'size': 24, 'weight': 'bold'})
    leg.get_frame().set_linewidth(0)
    
    figname = 'eval-scaler-num-windows-to-min-scale'
    fig.savefig(f'figures/{figname}.png', bbox_inches='tight')
    fig.savefig(f'figures/{figname}.eps', bbox_inches='tight', format='eps')


def plot_area(dir: Path):
    exp_dirs = list(dir.glob('*_num_probe_steps_*'))
    assert len(exp_dirs) > 0

    probe_area_dict = {}
    for exp_dir in exp_dirs:
        config_jsons = list(exp_dir.glob('*/config.json'))
        assert len(config_jsons) == 1
        config_json = config_jsons[0]
        with config_json.open('r') as f:
            config = json.load(f)
        num_probe_steps = config['roi_resizer']['num_probe_steps']
        video_dir = config_json.parent
        roi_df = pd.read_csv(sorted(video_dir.glob('roi*.csv'),
                                    key=id_of, reverse=True)[0], delimiter='\t')
        timeline_df = pd.read_csv(sorted(video_dir.glob('timeline*.csv'),
                                         key=id_of, reverse=True)[0], delimiter='\t')
        to_schedule_id = {
            row['frameIndex']: row['scheduleID']
            for _, row in timeline_df.iterrows()
        }
        roi_df['scheduleID'] = roi_df['frameIndex'].map(to_schedule_id)
        roi_ids = sorted(list(set(roi_df['id'])))

        # Probing area for windows
        areas = defaultdict(list)
        target_roi_id = 0
        for roi_id in roi_ids:
            if roi_id != target_roi_id:
                continue
            _roi_df = roi_df[roi_df['id'] == roi_id]
            schedule_ids = np.unique(_roi_df['scheduleID'].values)
            assert np.all(np.diff(schedule_ids) == 1)

            min_schedule_id = np.min(schedule_ids)
            for schedule_id in schedule_ids:
                schedule_index = schedule_id - min_schedule_id
                _probe_df = _roi_df[(_roi_df['scheduleID'] == schedule_id)
                                    & (_roi_df['isProbingROI'] == 1)]
                ltrbs = _probe_df[[
                    'paddedLoc_l',
                    'paddedLoc_t',
                    'paddedLoc_r',
                    'paddedLoc_b',
                    'targetScale'
                ]].values
                widths = ltrbs[:, 2] - ltrbs[:, 0]
                heights = ltrbs[:, 3] - ltrbs[:, 1]
                scales = ltrbs[:, 4]
                areas[schedule_index].append(np.sum(widths * heights * scales ** 2))

        probe_area_dict[num_probe_steps] = [
            np.mean(areas[schedule_index])
            for schedule_index in sorted(areas.keys())
        ]


    num_probe_steps_list = sorted(list(set(probe_area_dict.keys())))

    lw = 4

    fig, ax = plt.subplots(figsize=(12, 4))

    for num_probe_steps in num_probe_steps_list:
        if num_probe_steps in [0, 1, 9]:
            continue
        x = [i for i, area in enumerate(probe_area_dict[num_probe_steps])
             if area > 0]
        ax.plot(np.arange(len(x)), [v for v in probe_area_dict[num_probe_steps]
                    if v > 0], label=f'{num_probe_steps} Probes',
                marker='D', linewidth=2, linestyle='-', ms=10)

    ax.set_xlim(0, 5)
    ax.set_xticks([0, 1, 2, 3, 4, 5])
    ax.set_ylim(0, 12000)
    ax.set_yticks([0, 4000, 8000, 12000])

    ax.set_xlabel('Schedule Event Index', fontsize=24, fontweight='bold')
    ax.set_ylabel('Probing Overhead\n(Probe ROIs Area)', fontsize=25, fontweight='bold')
    # ax.tick_params('x', which='major', pad=20)

    ax.spines['left'].set_linewidth(lw)
    ax.spines['top'].set_visible(False)
    ax.spines['right'].set_linewidth(False)
    ax.spines['bottom'].set_linewidth(lw)

    ax.tick_params('both', which='major', direction='in', length=6, width=lw)

    ticklabels = ax.get_xticklabels() + ax.get_yticklabels()
    [ticklabel.set_fontsize(24) for ticklabel in ticklabels]
    [ticklabel.set_fontweight('bold') for ticklabel in ticklabels]

    leg = ax.legend(ncol=3, loc='upper right',
                    prop={'size': 20, 'weight': 'bold'})
    leg.get_frame().set_linewidth(0)

    figname = 'eval-scaler-probing-overhead'
    fig.savefig(f'figures/{figname}.png', bbox_inches='tight')
    fig.savefig(f'figures/{figname}.eps', bbox_inches='tight', format='eps')


if __name__ == '__main__':
    parser = argparse.ArgumentParser()
    parser.add_argument('-d', '--dir', type=Path)
    args = parser.parse_args()

    plot_area(args.dir)
    # plot_num_windows_to_min_scale(args.dir)
