from dataclasses import dataclass, fields, asdict
from collections import namedtuple
from typing import List, get_args


Diff = namedtuple("Diff", ["key", "val"])


class SubConfig:
    pass


@dataclass
class VideoCfg(SubConfig):
    path: str = ""
    fps: int = 0


@dataclass
class ROIExtractorCfg(SubConfig):
    max_queue_size: int = 200
    num_workers: int = 2
    extraction_resize_width: float = 960.0
    extraction_resize_height: float = 540.0
    max_pd_roi_size: float = 206.0
    min_pd_roi_size: float = 5.0
    eat_pd: bool = True
    roi_padding: float = 20.0
    roi_border: int = 2
    of_conf_threshold: float = 0.0
    pd_filter_threshold: float = 0.5
    pd_interval: int = 5
    merge: bool = True
    no_downsampling_for_last_frame: bool = True


@dataclass
class ROIResizerCfg(SubConfig):
    dataset: str = ""
    voting_window: int = 5
    scale_shift: float = 0.0
    area_shift: float = 0.0
    static_scale: bool = False
    static_target_scale: float = 1.0
    max_of_roi_size: float = 206.0
    probe_step_size: float = 0.1
    num_probe_steps: int = 5
    probe_conf_threshold: float = 0.0
    probe_iou_threshold: float = 0.5


@dataclass
class InferenceEngineCfg(SubConfig):
    draw_inference_result: bool = True
    dataset: str = ""
    model: str = "YOLO_V5"
    runtime: str = "TFLITE"
    use_tiny: bool = False
    conf_threshold: float = 0.5
    iou_threshold: float = 0.5
    profile_warmups: int = 5
    profile_runs: int = 5
    full_frame_size: int = 1024
    input_sizes: List[int] = None
    devices: List[str] = None


@dataclass
class PatchReconstructorCfg(SubConfig):
    frame_boxes_iou_threshold: float = 0.5
    box_filter_overlap_threshold: float = 0.8
    id_mapping_iou_threshold: float = 0.1


@dataclass
class ConfigFrame:
    video_configs: List[VideoCfg] = None
    execution_type: str = "mondrian"
    log_execution: bool = True
    log_roi: bool = True
    interpolation_threshold: float = 1.0
    full_frame_interval: int = 3
    full_frame_size: int = 1024
    full_device: str = "GPU"
    buffer_size: int = 200
    latency_slo_ms: int = 5000
    roi_size: int = 64

    roi_extractor: ROIExtractorCfg = ROIExtractorCfg()
    roi_resizer: ROIResizerCfg = ROIResizerCfg()
    inference_engine: InferenceEngineCfg = InferenceEngineCfg()
    patch_reconstructor: PatchReconstructorCfg = PatchReconstructorCfg()

    def __post_init__(self):
        self.inference_engine.input_sizes = [640, 1280]
        self.inference_engine.devices = ['GPU']
        self.originals = []

    def _apply(self, diffs, isReset=False):
        for (keys, val) in diffs:
            modified = False
            handle = self
            ks = keys.split(";")
            for k, nk in zip(ks, ks[1:] + [None]):
                if isinstance(handle, list):
                    assert k.isdigit(), f'\n\n\tCONFIG ERROR:\n\t\tIndex "{k}" is not number\n'
                    attr = handle[int(k)]
                else:
                    assert hasattr(
                        handle, k
                    ), f'\n\n\tCONFIG ERROR:\n\t\tKey "{k}" does not exist\n'
                    attr = getattr(handle, k)
                if issubclass(type(attr), SubConfig) or \
                        (isinstance(attr, list) and nk is not None and nk.isdigit()):
                    handle = attr
                else:
                    if not isReset:
                        orig = handle[int(k)] if isinstance(handle, list) \
                            else getattr(handle, k)
                        self.originals.append(Diff(keys, orig))
                    setattr(handle, k, val)
                    modified = True
            assert modified, f'\n\n\tCONFIG ERROR:\n\t\tKey path "{keys}" is invalid\n'

    def _check_type(self, obj, passed=True):
        field_types = {field.name: field.type for field in fields(type(obj))}
        err = ""
        for name in field_types:
            attr = getattr(obj, name)
            attr_type = field_types[name]
            if hasattr(attr_type, "__origin__"):  # List
                if not isinstance(attr, list):
                    err += f"\t\t{name} is not list\n"
                    passed &= False
                else:
                    list_type = get_args(attr_type)[0]
                    if len(attr) == 0:
                        err += f"\t\t{name} is empty list\n"
                        passed &= False
                    for i in attr:
                        type_matched = type(i) == list_type
                        if not type_matched:
                            err += f"\t\t{name} must be list[{list_type}], but has element of type {type(i)}\n"
                            passed &= False
            else:
                attr = getattr(obj, name)
                type_matched = isinstance(attr, attr_type)
                if not type_matched:
                    err += f"\t\t{name} must be {attr_type}, not {type(attr)}\n"
                    passed &= False
            if issubclass(type(attr), SubConfig):
                _passed, _err = self._check_type(attr)
                passed &= _passed
                err += _err
        return (passed, err)

    def generate(self, diffs):
        self._apply(diffs)
        passed, err = self._check_type(self)
        assert passed, f"\n\n\tCONFIG ERROR:\n{err}\n"
        ret = asdict(self)
        self._apply(self.originals, True)
        return ret


config_frame = ConfigFrame()
