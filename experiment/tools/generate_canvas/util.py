from datatype import ROI, mergedROI
import cv2
from argparse import ArgumentParser
from pathlib import Path


def parse_row(row):
    return ROI(row), mergedROI(row)


def load_frames(video_path, start, end):
    frames = {}
    vidcap = cv2.VideoCapture(str(video_path))
    vidcap.set(cv2.CAP_PROP_POS_FRAMES, start)
    total_success = True
    for i in range(start, end):
        success, image = vidcap.read()
        image = cv2.cvtColor(image, cv2.COLOR_BGR2RGB)
        frames[i] = image
        total_success &= success
    return total_success, frames


def get_args():
    args = ArgumentParser()
    args.add_argument("--log-dir", "-d", type=str,
                      required=True, help="Path to log directory")
    args.add_argument("--video_path", "-v", type=str,
                      required=True, help="Path to video file")
    args = args.parse_args()

    args.log_dir = Path(args.log_dir)
    args.video_path = Path(args.video_path)

    return args
