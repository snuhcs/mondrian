from collections import deque
from enum import Enum
import json
from pathlib import Path
import shutil
import signal
import sys
import time

from tabulate import tabulate

from config import config_frame, Diff, VideoCfg
from dataset import VideoData
from util import get_child_logger, update_dashboard, is_valid_exec


def try_reuse_existing_results(exp_path: Path, system_hash: str):
    with (exp_path / 'config.json').open('r') as f:
        exp_config = json.load(f)
    existing_config_jsons = Path('.').glob('result_*/*/*/config.json')
    for existing_config_json in existing_config_jsons:
        if existing_config_json.parent == exp_path:
            continue
        exp_root_dir = existing_config_json.parent.parent.parent
        if str(exp_root_dir).split('_')[-1] != system_hash:
            continue
        with existing_config_json.open('r') as f:
            existing_config = json.load(f)
        if exp_config != existing_config:
            continue
        assert len(existing_config['video_configs']) == 1
        if not is_valid_exec(existing_config_json.parent, partial_exec=False):
            continue
        print(f'Reuse {existing_config_json} for exp {exp_path}')
        shutil.copytree(existing_config_json.parent, exp_path,
                        dirs_exist_ok=True)
        return True
    return False


class Experiment:
    class Status(Enum):
        READY = 0
        STARTED = 1
        RUNNING = 2
        HALTED = 3
        DONE = 4
        ERROR = 5

    def __init__(self, setup, parent_logger, dataset, exp_path, num_frames, no_reuse):

        self.setup = setup
        self.parent_logger = parent_logger
        self.dataset = dataset
        self.exp_path = exp_path
        self.num_frames = num_frames

        info = str(exp_path).split("/")
        self.name = f"{info[-2][info[-2].find('_') + 1:]}/{info[-1]}"
        if not no_reuse and try_reuse_existing_results(exp_path, setup.system_hash):
            self.status = self.Status.DONE
        else:
            self.status = self.Status.READY
        self.trial = 0
        self.prev_frame_count = deque([0])
        self.app_name = setup.app_name
        self.progress = 0
        self.device = None

    def start(self, device):
        self.device = device
        self.logger = get_child_logger(
            self.setup,
            self.parent_logger,
            f"{self.name:^30}",
            f"device_{self.device.serial}.log",
        )

        if self.status == Experiment.Status.READY:
            self._log("info", f"Starting experiment {self.name}")
        else:
            self._log("info", f"Restarting experiment {self.name}")
        self.trial += 1

        success_msg = f"Starting: Intent {{ cmp={self.app_name}/.MainActivity }}\n"
        self.device.shell(
            f"run-as {self.app_name} rm /data/data/{self.app_name}/boxes.txt"
        )
        self.device.shell(
            f"run-as {self.app_name} rm /data/data/{self.app_name}/timeline.csv"
        )
        self.device.shell(
            f"run-as {self.app_name} rm /data/data/{self.app_name}/roi.csv"
        )
        self.device.push(self.exp_path / "config.json", "/data/local/tmp/config.json")
        start_msg = self.device.shell(
            f"am start -n {self.app_name}/{self.app_name}.MainActivity"
        )
        self.start_time = time.time()
        for _ in range(60):
            if not self.device.shell('pidof hcs.offloading.mondrian').strip():
                time.sleep(1)
            else:
                break

        if start_msg == success_msg:
            self.status = self.Status.STARTED
            self._log("info", f"App started.")
        else:
            self._log("error", f"App start failed.")
            self._log("error", start_msg)
            self.status = self.Status.HALTED

    def stop(self, status):
        stop_msg = self.device.shell(f"am force-stop {self.app_name}")
        if status != None:
            self.status = status
            self._log("info", f"App stopped.")

    def save(self, log, name):
        with (self.exp_path / name).open("w") as f:
            f.write(log)
        self._log("info", f"{name} saved.")

    def check(self):
        """
        if interval of call on this function is too short,
        halt condition may be triggered.
        2 seconds were not enough
        """
        try:
            test_log = self.device.shell(
                f"run-as {self.app_name} cat /data/user/0/{self.app_name}/boxes.txt"
            )
        except:
            self._log("error", f"Connection lost for device {self.device.serial}")
            return []

        frame_count = test_log.count("\n")
        frame_count = 0 if frame_count == 1 else frame_count

        def is_halted():
            log = self.device.shell('pidof hcs.offloading.mondrian').strip()
            if not log:
                self._log("error", f"App is not running")
                return True
            if frame_count == self.prev_frame_count[0] and frame_count != 0:
                self._log("error", f"frame_count not updated")
                return True
            return False

        if frame_count >= self.num_frames:  # DONE
            self.stop(self.Status.DONE)
            self._log("info", f"Experiment done")
        elif is_halted():  # HALTED
            if self.trial > 2:
                self.stop(self.Status.ERROR)
                self._log("error", f"Something is wrong with this experiment (3 trial)")
            else:
                self.stop(self.Status.HALTED)
                self._log("error", f"Something went wrong")
        else:
            self.progress = int(10000 * frame_count / self.num_frames) / 100
            if frame_count == 0:
                self._log("info", f"App loading...")
            else:
                self.status = self.Status.RUNNING
                self._log("info", f"Progress : {self.progress:.2f}%")
            self.prev_frame_count.append(frame_count)  # still RUNNING
            if len(self.prev_frame_count) >= 10:
                self.prev_frame_count.popleft()
            return []

        execution_log = self.device.shell(
            f"run-as {self.app_name} cat /data/user/0/{self.app_name}/timeline.csv"
        )
        roi_log = self.device.shell(
            f"run-as {self.app_name} cat /data/user/0/{self.app_name}/roi.csv"
        )
        self.save(test_log, f"boxes_{self.device.serial}_{self.trial}.txt")
        self.save(execution_log, f"timeline_{self.device.serial}_{self.trial}.csv")
        self.save(roi_log, f"roi_{self.device.serial}_{self.trial}.csv")
        device = self.device
        return [device]

    def _log(self, level, msg):
        msg = f"({self.device.serial:^15}) [{self.status.name:^9}] {msg}"
        getattr(self.logger, level)(msg)


class ExperimentManager:
    def __init__(self, setup):
        self.setup = setup
        self.base_directory = setup.base_directory
        signal.signal(signal.SIGINT, self.stop_all_handler)

        self.experiments = []
        self.logger = get_child_logger(setup, setup.logger, "EM", "exp_manager.log")

        for dataset, video_names in setup.datasets.items():
            for video_name in video_names:
                num_frames = VideoData(video_name).num_frames
                for i, (exp_name, diffs) in enumerate(setup.config_diffs.items()):
                    video_configs = [
                        VideoCfg(
                            path=f"/data/local/tmp/video/{video_name}.mp4",
                            fps=0,
                        ) for _ in range(setup.num_videos)
                    ]
                    setattr(config_frame, "video_configs", video_configs)
                    diffs += [
                        Diff("roi_resizer;dataset", dataset),
                        Diff("inference_engine;dataset", dataset),
                    ]
                    exp_config = config_frame.generate(diffs)

                    exp_path = (
                        self.base_directory
                        / f"{i}_{exp_name}"
                        / video_name.replace(".mp4", "")
                    )
                    exp_path.mkdir(parents=True, exist_ok=True)

                    with (exp_path / "config.json").open("w") as f:
                        json.dump(exp_config, f, indent=4)

                    experiment = Experiment(setup,
                                            self.logger,
                                            dataset,
                                            exp_path,
                                            num_frames,
                                            setup.no_reuse)
                    self.experiments.append(experiment)
        self._update_dashboard()

    def get_exp(self):
        """
        priority: READY > HALTED
        """
        readys = list(
            filter(lambda exp: exp.status == Experiment.Status.READY, self.experiments)
        )
        halteds = list(
            filter(lambda exp: exp.status == Experiment.Status.HALTED, self.experiments)
        )

        if readys or halteds:
            exp = readys[0] if readys else halteds[0]
        else:
            exp = None
        self._update_dashboard()
        return exp

    def all_finished(self):
        stasuses = [exp.status for exp in self.experiments]
        done_count = stasuses.count(Experiment.Status.DONE)
        err_count = stasuses.count(Experiment.Status.ERROR)
        self._update_dashboard()
        return (done_count + err_count) == len(self.experiments)

    def stop_all(self):
        for exp in self.experiments:
            if (exp.status == Experiment.Status.RUNNING) or (
                exp.status == Experiment.Status.STARTED
            ):
                exp.stop(None)
            elif exp.status != Experiment.Status.READY:
                self.logger.info(f"{exp.name} : {exp.status.name}")
        self.logger.info("Stopped all experiments")

    def get_idle_devices(self):
        idle_devices = []
        for exp in self.experiments:
            if exp.status == Experiment.Status.ERROR:
                idle_devices += [exp.device]
            if (exp.status == Experiment.Status.RUNNING) or (exp.status == Experiment.Status.STARTED):
                idle_devices += exp.check()

        self._update_dashboard()
        return idle_devices

    def stop_all_handler(self, singal, frame):
        print()
        self.logger.error("Stopping all experiments with CTRL-C")
        self.stop_all()
        self._update_dashboard(True)
        sys.exit(0)

    def _update_dashboard(self, stopped=False):

        STARTED = []
        RUNNING = []
        HALTED = []
        DONE = []

        exps = [exp for exp in self.experiments if exp.status != None]

        for exp in exps:
            if (exp.status.name in locals()) and (exp.device != None):
                locals()[exp.status.name].append(exp.device.serial)

        HALTED = list(set(HALTED))
        HALTED = [d for d in HALTED if (d not in STARTED) and (d not in RUNNING)]
        DONE = [d for d in DONE if (d not in STARTED) and (d not in RUNNING)]

        if stopped:
            msg = "!!! STOPPED !!!\n\n"
            msg += "DEVICES\n"
        else:
            msg = "DEVICES\n"
        msg += f"\t{'STARTED':^9}: {' '.join(STARTED)}\n"
        msg += f"\t{'RUNNING':^9}: {' '.join(RUNNING)}\n"
        msg += f"\t{'HALTED':^9}: {' '.join(HALTED)}\n"
        msg += f"\t{'DONE':^9}: {' '.join(DONE)}\n"
        msg += "\n"

        header = ["EXPERIMENT", "STATUS", "TRIAL", "DEVICE", "PROGRESS"]
        values = [
            [
                exp.name,
                exp.status.name,
                exp.trial,
                "-" if exp.device is None else exp.device.serial,
                f"{exp.progress}%"
                if (exp.status == Experiment.Status.RUNNING)
                or (exp.status == Experiment.Status.HALTED)
                else "-",
            ]
            for exp in exps
        ]
        update_dashboard(self.setup, msg + tabulate(values, header, tablefmt="grid"))
