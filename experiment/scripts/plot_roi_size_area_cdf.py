import argparse
from pathlib import Path
import re

import matplotlib.pyplot as plt
import numpy as np
import pandas as pd


def plot_cdf(values, filename, xlabel, xlim, xticks, fig_ax=None, save=True, label=None):
    labelfontsize = 50
    ticklabelfontsize = 30
    lw = 4

    if fig_ax is None:
        fig, ax = plt.subplots(figsize=(8, 10))
    else:
        fig, ax = fig_ax

    ax.plot(np.sort(values), np.linspace(0, 1, values.shape[0]), linewidth=lw, label=label)

    ax.grid(axis='both', linestyle='--')
    ax.set_axisbelow(True)

    ax.set_xlim(xlim)
    ax.set_ylim(0, 1)
    ax.set_xticks(xticks)
    ax.set_yticks([i * 0.2 for i in range(6)])

    ax.set_xlabel(xlabel, fontsize=labelfontsize, fontweight='bold')
    ax.set_ylabel('CDF',  fontsize=labelfontsize, fontweight='bold')

    ax.spines['left'].set_linewidth(lw)
    ax.spines['top'].set_visible(False)
    ax.spines['right'].set_visible(False)
    ax.spines['bottom'].set_linewidth(lw)

    ax.tick_params('both', which='major', direction='in', length=6, width=lw)

    ticklabels = ax.get_xticklabels() + ax.get_yticklabels()
    [ticklabel.set_fontsize(ticklabelfontsize) for ticklabel in ticklabels]
    [ticklabel.set_fontweight('bold') for ticklabel in ticklabels]

    ax.legend(ncol=1, prop={'size': 30, 'weight': 'bold'})

    if save:
        fig.savefig(f'figures/{filename}.png', bbox_inches='tight')
        fig.savefig(f'figures/{filename}.eps', format='eps', bbox_inches='tight')


def id_of(log_path: Path):
    m = re.search(r'_(\d+).[a-z]{3,4}$', str(log_path))
    return int(m.group(1)) if m is not None else 0


def get_max_lengths_areas(dir: Path):
    roi_max_lengths, roi_areas = [], []
    for config_json in dir.glob('*/config.json'):
        roi_csv = sorted(config_json.parent.glob('roi*.csv'),
                         key=id_of, reverse=True)[0]
        df = pd.read_csv(roi_csv, delimiter='\t')
        print(df.shape)
        ltrbs = df[[
            'paddedLoc_l',
            'paddedLoc_t',
            'paddedLoc_r',
            'paddedLoc_b',
            'targetScale',
        ]].values
        widths = ltrbs[:, 2] - ltrbs[:, 0]
        heights = ltrbs[:, 3] - ltrbs[:, 1]
        targetScales = ltrbs[:, 4]
        roi_max_lengths += list(np.maximum(widths, heights) * targetScales)
        roi_areas = widths * heights * targetScales ** 2
    return np.array(roi_max_lengths), np.array(roi_areas)


def main(dir: Path, reactive_dir: Path, proactive_dir: Path):
    roi_max_lengths, roi_areas = get_max_lengths_areas(dir)
    print(roi_max_lengths.shape, np.max(roi_max_lengths), np.min(roi_max_lengths))
    print(roi_areas.shape, np.max(roi_areas), np.min(roi_areas))
    plot_cdf(roi_max_lengths, 'approach-size-cdf-mta', 'Rescaled ROI Size', (0, 300), [0, 100, 200, 300])
    plot_cdf(roi_areas, 'approach-area-cdf-mta', 'Rescaled ROI Area', (0, 3000), [0, 1000, 2000, 3000])

    if reactive_dir is not None and proactive_dir is not None:
        reactive_roi_max_lengths, reactive_roi_areas = get_max_lengths_areas(reactive_dir)
        proactive_roi_max_lengths, proactive_roi_areas = get_max_lengths_areas(proactive_dir)
        fig, ax = plt.subplots(figsize=(10, 10))
        print(np.mean(roi_areas), np.std(roi_areas))
        print(np.mean(reactive_roi_areas), np.std(reactive_roi_areas))
        print(np.mean(proactive_roi_areas), np.std(proactive_roi_areas))
        plot_cdf(roi_areas,
                 'approach-area-cdf-mta',
                 'Rescaled ROI Area',
                 (0, 3000),
                 [0, 1000, 2000, 3000],
                 fig_ax=(fig, ax), save=False, label='Mondrian')
        plot_cdf(reactive_roi_areas,
                 'eval-scheduler-area-cdf-mta',
                 'Rescaled ROI Area',
                 (0, 6000),
                 [0, 2000, 4000, 6000],
                 fig_ax=(fig, ax), save=False, label='Reactive Only')
        plot_cdf(proactive_roi_areas,
                 'eval-scheduler-area-cdf-mta',
                 'Rescaled ROI Area',
                 (0, 6000),
                 [0, 2000, 4000, 6000],
                 fig_ax=(fig, ax), save=True, label='Proactive Only')


if __name__ == '__main__':
    parser = argparse.ArgumentParser()
    parser.add_argument('-d', '--dir', required=True, type=Path)
    parser.add_argument('-r', '--reactive-dir', type=Path)
    parser.add_argument('-p', '--proactive-dir', type=Path)
    args = parser.parse_args()

    main(args.dir, args.reactive_dir, args.proactive_dir)
