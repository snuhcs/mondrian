import logging
import os
from pathlib import Path
import re
import subprocess
import time

from dataset import VideoData


def get_system_commit_hash(system_path):
    base_dir = os.getcwd()
    os.chdir(system_path)
    hash = subprocess.check_output(['git', 'rev-parse', 'HEAD']).decode('utf-8').strip()[:6]  # noqa
    os.chdir(base_dir)
    return hash


def get_current_time_string(fmt):
    return time.strftime(fmt)


def get_child_logger(setup, parent, logger_name, file_name):
    log_directory = setup.base_directory / "log"
    logger = parent.getChild(logger_name)
    fileHandler = logging.FileHandler(str(log_directory / file_name))
    fileHandler.setFormatter(setup.formatter)
    logger.addHandler(fileHandler)
    return logger


def update_dashboard(setup, msg):
    log_directory = setup.base_directory / "log"
    # msg = get_current_time_string("%y-%m-%d %H:%M:%S") + "\n\n" + msg + "\n"
    msg = msg + "\n"
    with (log_directory / "dashboard.log").open("w") as f:
        f.write(msg)


def is_valid_exec(exec_dir: Path, partial_exec: bool):
    boxes_txt = ExpUtil.boxes_txt_of(exec_dir)
    if boxes_txt is None:
        return False
    with boxes_txt.open('r') as f:
        num_lines = sum([1 for _ in f])
    if partial_exec:
        return num_lines > 0
    else:
        return num_lines == VideoData(exec_dir.name).num_frames


class ExpUtil:
    @staticmethod
    def config_json_of(log_dir: Path) -> Path:
        return log_dir / 'config.json'

    @staticmethod
    def boxes_txt_of(log_dir: Path) -> Path:
        return ExpUtil.log_of('boxes*.txt', log_dir)

    @staticmethod
    def roi_csv_of(log_dir: Path) -> Path:
        return ExpUtil.log_of('roi*.csv', log_dir)

    @staticmethod
    def timeline_csv_of(log_dir: Path) -> Path:
        return ExpUtil.log_of('timeline*.csv', log_dir)

    @staticmethod
    def id_of(log_path: Path):
        m = re.search(r'_(\d+).[a-z]{3,4}$', str(log_path))
        return int(m.group(1))

    @staticmethod
    def log_of(pattern, log_dir: Path) -> Path:
        log_paths = list(Path(log_dir).glob(pattern))
        if len(log_paths) == 0:
            return None
        elif len(log_paths) == 1:
            return log_paths[0]
        else:
            return sorted(log_paths, key=ExpUtil.id_of, reverse=True)[0]
