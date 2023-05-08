import json
from multiprocessing import Pool
from pathlib import Path
from typing import List

from tqdm import tqdm


def get_scales(video_dir: Path):
    return sorted([float(s.name) for s in (video_dir / '0' / '0_0_1920_1080').glob('*')])


def get_boxes_json(frame_dir: Path, scale: float):
    return frame_dir / '0_0_1920_1080' / str(scale) / str(int(1920 * scale)) / 'boxes.json'


def get_boxes_jsons(frame_dirs: List[Path], scale: float):
    return [get_boxes_json(frame_dir, scale) for frame_dir in frame_dirs]


def load_boxes_json(boxes_json: Path):
    with boxes_json.open('r') as f:
        return json.load(f)


def load_boxes_jsons(boxes_jsons: List[Path], desc=None):
    with Pool() as pool:
        return list(tqdm(pool.imap(load_boxes_json, boxes_jsons),
                         total=len(boxes_jsons), desc=desc))


def main():
    src_dir = Path('/data1/mondrian/db/full_frame')
    dst_dir = Path('/data1/mondrian/full_boxes')

    for video_dir in sorted(list(src_dir.glob('*'))):
        video_name = video_dir.name
        dst_video_dir = dst_dir / video_name
        dst_video_dir.mkdir(exist_ok=True)
        frame_dirs = sorted(list(video_dir.glob('*')),
                            key=lambda f: int(f.name))

        for scale in get_scales(video_dir):
            save_path = dst_video_dir / f'{int(1920 * scale)}.json'
            if save_path.exists():
                print(f'{save_path} already exists')
                continue

            boxes_jsons = get_boxes_jsons(frame_dirs, scale)
            boxes_list = load_boxes_jsons(
                boxes_jsons, desc=f'{video_name} {scale}')

            video_scale_results = {
                int(frame_dir.name): boxes_list[i]
                for i, frame_dir in enumerate(frame_dirs)
            }

            with save_path.open('w') as f:
                json.dump(video_scale_results, f)
            print(f'{save_path} saved')


if __name__ == '__main__':
    main()
