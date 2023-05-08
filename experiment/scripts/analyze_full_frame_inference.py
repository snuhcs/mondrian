from collections import defaultdict

import numpy as np
import yaml


def main():
    with open('scripts/full_frame_results.yaml', 'r') as f:
        results = yaml.load(f, Loader=yaml.FullLoader)
    input_sizes = sorted(results["fps"].keys())
    for size in input_sizes:
        print(f'FPS for {size}: {results["fps"][size]}')
    for dataset, video_ap in results['ap'].items():
        size_aps = defaultdict(list)
        for video, size_ap in video_ap.items():
            for size, ap in size_ap.items():
                size_aps[size].append(ap)
        for size in input_sizes:
            print(f'AP  for {dataset:<5} {size:<4}: {np.mean(size_aps[size]):<20} {np.std(size_aps[size]):<20}')
            


if __name__ == '__main__':
    main()
