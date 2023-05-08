import argparse
from collections import OrderedDict
from pathlib import Path
import pickle
from pprint import pprint
import re

import matplotlib.pyplot as plt
import numpy as np
import yaml


def id_of(exp_name):
    return int(re.search(r'^(\d+)_', exp_name).group(1))


def get_first_not_common_index(params):
    for i in range(min([len(param) for param in params])):
        if len(set([param[i] for param in params])) != 1:
            break
    return i


def get_param_type(params):
    assert len(params) > 0
    param_type = params[0][:get_first_not_common_index(params)]
    if param_type.endswith('_'):
        param_type = param_type[:-1]
    return param_type.upper()


def params_to_labels(params):
    assert len(params) > 0
    labels = [float(param[get_first_not_common_index(params):])
              for param in params]
    if all([label == int(label) for label in labels]):
        labels = [int(label) for label in labels]
    return labels


def tradeoff_results_full(dataset: str, result_path: Path):
    with open('scripts/full_frame_results.yaml', 'r') as f:
        results = yaml.load(f, Loader=yaml.FullLoader)
    labels = [int(size) for size in results['fps'].keys()]
    fpss = [results['fps'][int(label)] for label in labels]
    fpss_std = [0] * len(labels)
    aps = [
        np.mean([size_ap[int(label)] for size_ap in results['ap'][dataset].values()])
        for label in labels
    ]
    aps_std = [
        np.std([size_ap[int(label)] for size_ap in results['ap'][dataset].values()])
        for label in labels
    ]
    return labels, fpss, fpss_std, aps, aps_std


def tradeoff_results(dataset: str, result_path: Path):
    if result_path.parent.name == 'tradeoff_full_frame':
        print('FULL Results')
        labels, fpss, fpss_std, aps, aps_std = \
            tradeoff_results_full(dataset, result_path)
        pprint(list(zip(labels, fpss, fpss_std, aps, aps_std)))
        return fpss, aps

    # results[param][video] = fps, [{class: person, AP: ...}]
    with result_path.open('rb') as f:
        results = pickle.load(f)

    exp_names = sorted(list(results.keys()), key=id_of)
    params = [exp_name[exp_name.find('_') + 1:] for exp_name in exp_names]
    param_type = get_param_type(params)
    labels = params_to_labels(params)
    print(f'{param_type} Results')

    fpss, fpss_std, aps, aps_std = [], [], [], []
    for exp_name in exp_names:
        video_results = results[exp_name]
        videos = sorted(list(video_results.keys()))
        video_fpss, video_aps = [], []
        for video in videos:
            fps = video_results[video][1]['fps']
            acc = video_results[video][1]['acc']
            assert len(acc) == 1 and acc[0]['class'] == 'person'
            ap = acc[0]['AP']
            video_fpss.append(fps)
            video_aps.append(ap)
        print(np.mean(video_fpss), np.mean(video_aps))
        fpss.append(np.mean(video_fpss))
        aps.append(np.mean(video_aps))
        fpss_std.append(np.std(video_fpss))
        aps_std.append(np.std(video_aps))

    pprint(list(zip(labels, fpss, fpss_std, aps, aps_std)))
    return fpss, aps


def main(dataset: Path, fig_ax=None, save=True, plot_ylabel=True, plot_legend=True):
    if fig_ax is None:
        fig, ax = plt.subplots(figsize=(15, 8))
    else:
        fig, ax = fig_ax

    tradeoffs = OrderedDict({
        'Mondrian': 'tradeoff_mondrian_scale_shift',
        'Emulated Batching': 'tradeoff_emulated_batch',
        'Full Frame Inference': 'tradeoff_full_frame',
        # 'Ours (Padding)': 'tradeoff_padding',
        # 'StaticScale': 'tradeoff_scaler_static_resize',
        # 'NumProbing': 'tradeoff_scaler_num_probing',
    })

    for label, name in tradeoffs.items():
        ax.plot(*tradeoff_results(
            str(dataset), dataset / name / 'result.pkl'),
            label=label, marker='D', markersize=10, linewidth=4)

    ax.tick_params('x', which='major', direction='in', length=4, width=4)
    ax.tick_params('x', which='minor', direction='in', length=4, width=0)
    ax.tick_params('y', which='major', direction='in', length=4, width=4)
    ax.tick_params('y', which='minor', direction='in', length=4, width=0)
    for direction in ['top', 'right']:
        ax.spines[direction].set_visible(False)
    for direction in ['left', 'bottom']:
        ax.spines[direction].set_linewidth(4)

    xticks = list(range(0, int(ax.get_xlim()[1]) + 9, 8))
    ax.set_xlim(min(xticks), max(xticks))
    ax.set_ylim(0, 0.8)
    ax.set_xticks(xticks)
    ax.set_yticks([0.0, 0.2, 0.4, 0.6, 0.8])

    labels = ax.get_xticklabels() + ax.get_yticklabels()
    [label.set_fontweight('bold') for label in labels]
    [label.set_fontsize(36) for label in labels]
    ax.set_xlabel('FPS', fontsize=40, fontweight='bold')
    if plot_ylabel:
        ax.set_ylabel('AP@0.5', fontsize=40, fontweight='bold')

    if plot_legend:
        leg = ax.legend(ncol=1, loc=[0.45, 0.01], prop={'size': 28, 'weight': 'bold'})
        # leg = ax.legend(ncol=1, loc=[0.33, 0.05], prop={'size': 24, 'weight': 'bold'})
        leg.get_frame().set_linewidth(0)
    if save:
        figname = f'eval-performance-overview-tradeoffs-{dataset}'
        fig.savefig(f'figures/{figname}.png', bbox_inches='tight')
        fig.savefig(f'figures/{figname}.eps', bbox_inches='tight', format='eps')
        plt.close(fig)


if __name__ == '__main__':
    parser = argparse.ArgumentParser()
    parser.add_argument('-d', '--dataset', required=True, type=Path,
                        choices=[Path('mta'), Path('virat')])
    args = parser.parse_args()

    main(args.dataset)

    # fig, axes = plt.subplots(1, 2, figsize=(20, 6))
    # for i, dataset in enumerate(['virat', 'mta']):
    #     main(Path(dataset), fig_ax=(None, axes[i]), save=False, plot_ylabel=i == 0, plot_legend=False)

    # leg = fig.legend(['Mondrian', 'Emulated Batching', 'Frame-wise Inference'],
    #                  ncol=3, loc=[0.025, 0.91], prop={'size': 30, 'weight': 'bold'})
    # leg.get_frame().set_linewidth(0)
    # figname = f'eval-performance-overview-tradeoffs'
    # fig.savefig(f'figures/{figname}.png', bbox_inches='tight')
    # fig.savefig(f'figures/{figname}.eps', bbox_inches='tight', format='eps')
