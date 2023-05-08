from ppadb.client import Client as AdbClient
from util import get_child_logger


class DeviceManager:
    def _check_device(self, device):
        assert device.is_installed(
            self.setup.app_name
        ), f"\n\n[PREREQUISITE] App is not installed in device {device.serial}!\n"
        device.shell(f"am force-stop {self.setup.app_name}")
        """ TODO
        * Copy videos only for pixel4
        * Note that type(self.setup.datasets) is changed to Dict[str, Dict[str, List[int]]]
        """
        # device.shell(f"run-as {self.setup.app_name} mkdir video")
        # for dataset, video_names in self.setup.datasets.items():
        #     for video_name in video_names:
        #         assert video_name in device.shell(
        #             f"ls /data/local/tmp/video/{dataset}"
        #         ), f"\n\n[PREREQUISITE] Video {video_name} is missing in device {device.serial}!\n"
        #         device.shell(f"run-as {self.setup.app_name} mkdir video/{dataset}")
        #         device.shell(
        #             f"run-as {self.setup.app_name} cp /data/local/tmp/video/{dataset}/{video_name} video/{dataset}"
        #         )
        self.logger.info(f"=> Checked.")

    def __init__(self, setup):
        self.setup = setup
        self.client = AdbClient(host="localhost", port=5037)
        self.devices = []
        self.serials = []
        self.serial_counts = {}
        self.logger = get_child_logger(setup, setup.logger, "DM", "dev_manager.log")
        self.logger.info("Scanning devices...")
        self.serial_index = 0
        self.rest_count = setup.rest_count
        self.scan()

    def scan(self):
        scanned = self.client.devices()
        for device in scanned:
            if device.serial not in self.serials:
                self.logger.info(f"Device found: {device.serial}")
                self._check_device(device)
                self.devices.append(device)
        self.serials += [device.serial for device in self.devices]

        for serial in self.serials:
            if serial not in self.serial_counts:
                self.serial_counts[serial] = self.rest_count

        for serial in self.serial_counts:
            self.serial_counts[serial] += 1
            if self.serial_counts[serial] >= self.rest_count:
                self.serial_counts[serial] = self.rest_count

        with open(".device", "w") as f:
            f.write(f"{self.serials[self.serial_index]}")
        self.serial_index = (self.serial_index + 1) % len(self.serials)

    def get_dev(self):
        ret = None
        for dev in self.devices:
            if self.serial_counts[dev.serial] >= self.rest_count:
                ret = dev
                break
        self.devices.remove(dev)
        return ret

    def all_devices_are_working(self, idle_devices):
        self.scan()
        self.devices += idle_devices
        for dev in idle_devices:
            self.serial_counts[dev.serial] = 0
        busy = len(self.devices) == 0

        tired = True
        for dev in self.devices:
            if self.serial_counts[dev.serial] >= self.rest_count:
                tired = False
                break

        if (not busy) and (not tired):
            for dev in self.devices:
                if self.serial_counts[dev.serial] >= self.rest_count:
                    self.logger.info(f"Idle device found: device {dev.serial}")
                    break
        return (busy or tired)
