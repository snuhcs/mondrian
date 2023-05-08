import pathlib
from pprint import pprint
import logging
import shutil

import yaml

from config import Diff as Diff
from util import get_current_time_string, get_system_commit_hash


class Setup:
    app_name = "hcs.offloading.mondrian"
    use_stdout = True
    num_videos = 1
    check_interval = 3
    rest_count = 4
    no_reuse = False

    def __init__(self, yaml_path, system_path):
        with open(yaml_path, 'r') as f:
            exp_configs = yaml.safe_load(f)
        self.config_diffs = {
            exp_name: [
                Diff(key, value)
                for key, value in key_values.items()
            ] if key_values is not None else []
            for exp_name, key_values in exp_configs['config_diffs'].items()
        }
        self.datasets = exp_configs['datasets']
        pprint(self.config_diffs)
        pprint(self.datasets)

        self.system_hash = get_system_commit_hash(system_path)
        self.base_directory = pathlib.Path(
            f'result_{get_current_time_string("%y%m%d_%H%M%S")}_{self.system_hash}')
        self.base_directory.mkdir()
        shutil.copy(yaml_path, self.base_directory)

        self.formatter = logging.Formatter(
            '[%(asctime)s] {%(name)s} %(message)s', '%Y-%m-%d %H:%M:%S'
        )
        self.logger = self.init_logger()

        with open('.result', 'w') as f:
            f.write(str(self.base_directory))

    def init_logger(self):
        log_directory = self.base_directory / 'log'
        log_directory.mkdir()
        use_stdout = self.use_stdout
        logger = logging.getLogger('/')
        logger.setLevel(level=logging.INFO)
        if use_stdout:
            streamHandler = logging.StreamHandler()
            streamHandler.setFormatter(self.formatter)
            logger.addHandler(streamHandler)
        fileHandler = logging.FileHandler(str(log_directory / 'root.log'))
        fileHandler.setFormatter(self.formatter)
        logger.addHandler(fileHandler)
        return logger
