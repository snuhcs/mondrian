import argparse
import json

import cv2
from tqdm import tqdm

def main(video_path, gt_path):
    input_video = cv2.VideoCapture(video_path)
    assert input_video.isOpened()
    with open(gt_path, 'r') as f:
        gt = json.load(f)

    width = int(input_video.get(cv2.CAP_PROP_FRAME_WIDTH))
    height = int(input_video.get(cv2.CAP_PROP_FRAME_HEIGHT))
    length = int(input_video.get(cv2.CAP_PROP_FRAME_COUNT))
    fps = int(input_video.get(cv2.CAP_PROP_FPS))

    assert set([int(i) for i in gt.keys()]) == set(range(length))

    codec = cv2.VideoWriter_fourcc(*'mp4v')
    output_video = cv2.VideoWriter(
        video_path.replace('.mp4', '_labeled.mp4'),
        codec, fps, (width, height))

    for frame_index in tqdm(range(length)):
        ret, image = input_video.read()
        assert image.shape == (1080, 1920, 3)
        if not ret:
            print(f'Failed to read {frame_index} frame')
            break

        for box in gt[str(frame_index)]:
            if box['confidence'] < 0.25:
                continue
            l, t, r, b = [int(v) for v in box['xyxy']]
            if l >= r or t >= b:
                print(f'l >= r or t >= b: {l} >= {r} or {t} >= {b} in frame {frame_index}')
            image = cv2.rectangle(image, (l, t), (r, b), (0, 255, 0) if box['label'] == 0 else (0, 0, 255), 2)

        output_video.write(image)

    input_video.release()
    output_video.release()


if __name__ == '__main__':
    parser = argparse.ArgumentParser()
    parser.add_argument('-v', '--video', type=str, required=True)
    parser.add_argument('-g', '--gt-path', type=str, required=True)
    args = parser.parse_args()

    main(args.video, args.gt_path)

