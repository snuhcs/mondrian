from pathlib import Path
import re

import numpy as np
import matplotlib.pyplot as plt
import pandas as pd
from tqdm import tqdm


def id_of(log_path: Path):
    m = re.search(r'_(\d+).[a-z]{3,4}$', str(log_path))
    return int(m.group(1)) if m is not None else 0


def load_video_fillup_values(video_dir: Path):
    roi_csv = sorted(video_dir.glob('roi*.csv'),
                     key=id_of, reverse=True)[0]
    timeline_csv = sorted(video_dir.glob('timeline*.csv'),
                          key=id_of, reverse=True)[0]
    roi_df = pd.read_csv(roi_csv, delimiter='\t')
    timeline_df = pd.read_csv(timeline_csv, delimiter='\t')
    to_schedule_id = {
        row['frameIndex']: row['scheduleID']
        for _, row in timeline_df.iterrows()
    }
    roi_df['scheduleID'] = roi_df['frameIndex'].map(to_schedule_id)
    schedule_ids = sorted(list(set(roi_df['scheduleID'])))
    assert np.all(np.diff(schedule_ids) == 1)

    values = []
    for schedule_id in tqdm(schedule_ids, total=len(schedule_ids), desc=video_dir.name):
        _roi_df = roi_df[roi_df['scheduleID'] == schedule_id]
        packed_frame_sizes = {}
        for _, row in _roi_df.iterrows():
            packed_frame_sizes[row['absolutePackedCanvasIndex']] = row['packedCanvasSize']
        packed_frame_area = sum([s**2 for s in packed_frame_sizes.values()])
        ltrbsi_arr = np.unique(_roi_df[[
            'mergedLoc_l',
            'mergedLoc_t',
            'mergedLoc_r',
            'mergedLoc_b',
            'mergedScale',
            'absolutePackedCanvasIndex',
        ]].values, axis=0)
        widths = ltrbsi_arr[:, 2] - ltrbsi_arr[:, 0]
        heights = ltrbsi_arr[:, 3] - ltrbsi_arr[:, 1]
        scales = ltrbsi_arr[:, 4]
        roi_area = np.sum(widths * heights * scales ** 2)
        fillup_ratio = roi_area / packed_frame_area
        assert 0 <= fillup_ratio <= 1
        values.append(fillup_ratio)
    return np.array(values)


def load_exp_fillup_values(dir: Path):
    video_dirs = list(dir.glob('*'))
    return np.hstack([load_video_fillup_values(video_dir)
                      for video_dir in video_dirs])


def main():
    dirs = {
        'VIRAT': Path('result_221209_165653_b7e1db/4_roi_size_64/test_cam_2'),
        'MTA': Path('result_221209_224457_mta_e2e_b7e1db/0_mondrian/test_cam_2'),
    }

    xlabelfontsize = 40
    ylabelfontsize = 50
    ticklabelfontsize = 40
    legendfontsize = 40
    lw = 4

    fig, ax = plt.subplots(figsize=(10, 10))
    for dataset in ['VIRAT', 'MTA']:
        print(dataset, np.mean(load_video_fillup_values(dirs[dataset])))
    for dataset in ['MTA']:
        cache_path = Path(f'fillup_ratio_{dataset}.npy')
        if cache_path.exists():
            values = np.load(str(cache_path))
        else:
            values = load_exp_fillup_values(dirs[dataset])
            np.save(str(cache_path), values)
        ax.plot(np.sort(values), np.linspace(0, 1, values.shape[0]),
                linewidth=lw, label=dataset)

    ax.grid(axis='both', linestyle='--')
    ax.set_axisbelow(True)

    ax.set_xlim(0.5, 1.0)
    # ax.set_xticks()
    ax.set_ylim(0, 1)
    ax.set_yticks([i * 0.2 for i in range(6)])

    ax.set_xlabel('Fillup Ratio', fontsize=xlabelfontsize, fontweight='bold')
    ax.set_ylabel('CDF',  fontsize=ylabelfontsize, fontweight='bold')

    ax.spines['left'].set_linewidth(lw)
    ax.spines['top'].set_visible(False)
    ax.spines['right'].set_visible(False)
    ax.spines['bottom'].set_linewidth(lw)

    ax.tick_params('both', which='major', direction='in', length=6, width=lw)

    ticklabels = ax.get_xticklabels() + ax.get_yticklabels()
    [ticklabel.set_fontsize(ticklabelfontsize) for ticklabel in ticklabels]
    [ticklabel.set_fontweight('bold') for ticklabel in ticklabels]

    leg = ax.legend(prop={'size': legendfontsize, 'weight': 'bold'})
    leg.get_frame().set_visible(False)

    filename = 'eval-scheduler-fillup-ratio'
    fig.savefig(f'figures/{filename}.png', bbox_inches='tight')
    fig.savefig(f'figures/{filename}.eps', format='eps', bbox_inches='tight')


if __name__ == '__main__':
    main()
