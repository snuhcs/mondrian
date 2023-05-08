import argparse
from pathlib import Path
import re
import subprocess
import time

from tqdm import tqdm


def graph_path_of(model, input_size):
    return f'/data/local/tmp/models/yolov5{model}-{input_size}-fp16.tflite'


def main(
        sleep_secs: int
) -> None:
    log_dir = Path('logs/latency_profile/')
    benchmark_binary = '/data/local/tmp/benchmark/benchmark_model_v2.8.0'
    common_args = '--warmup_runs=1 --num_runs=1 --warmup_min_secs=3 --min_secs=3 --use_gpu=true'
    models = ['n', 's', 'm', 'l', 'x']
    input_sizes = [64 * i for i in range(1, 31)]

    for model in models:
        (log_dir / model).mkdir(parents=True, exist_ok=True)

    model_input_sizes = [
        (model, input_size)
        for model in models
        for input_size in input_sizes
    ]

    for model, input_size in tqdm(model_input_sizes, total=len(model_input_sizes)):
        log_path = log_dir / model / f'{input_size}.log'
        if log_path.exists():
            continue
        with log_path.open('w') as f:
            subprocess.run([
                '/opt/platform-tools/adb',
                'shell',
                f'{benchmark_binary} --graph={graph_path_of(model, input_size)} {common_args}'
            ], stdout=f, stderr=f)
        time.sleep(sleep_secs)

    for model, input_size in tqdm(model_input_sizes, total=len(model_input_sizes)):
        log_path = log_dir / model / f'{input_size}.log'
        if not log_path.exists():
            continue
        with log_path.open('r') as f:
            content = f.read()
        avg_latency_ms_list = [float(v) for v in re.findall(r'avg=([0-9.+e]+)\s', content)]
        std_latency_ms_list = [float(v) for v in re.findall(r'std=([0-9.+e]+)\s', content)]
        assert len(avg_latency_ms_list) == len(std_latency_ms_list) == 2
        avg_latency_ms = avg_latency_ms_list[1]
        std_latency_ms = std_latency_ms_list[1]
        print(model, input_size, avg_latency_ms, std_latency_ms)


if __name__ == '__main__':
    parser = argparse.ArgumentParser()
    parser.add_argument('-s', '--sleep-secs', type=int, default=3)
    args = parser.parse_args()

    main(**vars(args))
