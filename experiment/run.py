import argparse
import os
import time

from experiment import ExperimentManager
from device import DeviceManager
from setup import Setup


if __name__ == "__main__":
    parser = argparse.ArgumentParser()
    parser.add_argument('-c', '--config', required=True,
                        help='experiment config')
    parser.add_argument('-s', '--system-path',
                        default='system_path',
                        help='android system directory')
    args = parser.parse_args()
    if args.system_path == 'system_path':
        with open('system_path', 'r') as f:
            args.system_path = f.read().strip()
    assert os.path.exists(args.system_path), \
        f'{args.system_path} not exists'

    setup = Setup(args.config, args.system_path)

    dev_manager = DeviceManager(setup)
    exp_manager = ExperimentManager(setup)

    while not (exp_manager.all_finished()):
        dev = dev_manager.get_dev()
        if dev is not None:
            exp = exp_manager.get_exp()
            if exp is not None:
                exp.start(dev)

        while dev_manager.all_devices_are_working(exp_manager.get_idle_devices()):
            time.sleep(setup.check_interval)  # task check interval
