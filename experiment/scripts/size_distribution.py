import json

import numpy as np


videos = [f'test_cam_{i}' for i in range(6)]
xyxys_dict = {}
for video in videos:
    args = [(video, frame_index) for frame_index in range(4922)]
    with open(f'/data1/mondrian/full_boxes/{video}/1920.json', 'r') as f:
        boxes_dict = json.load(f)
    xyxys, confs, labels = [], [], []
    for boxes in boxes_dict.values():
        xyxys += [box['xyxy'] for box in boxes]
        confs += [box['confidence'] for box in boxes]
        labels += [box['label'] for box in boxes]
    xyxys = np.array(xyxys).astype(np.float32)
    confs = np.array(confs).reshape(-1, 1).astype(np.float32)
    labels = np.array(labels).reshape(-1, 1).astype(np.float32)
    xyxys_dict[video] = np.hstack([xyxys, confs, labels])

for video in videos:
    xyxys = xyxys_dict[video]
    whs = xyxys[:, [2, 3]] - xyxys[:, [0, 1]]
    print(video)
    print(f'max: {np.max(whs):.2f} min: {np.min(whs):.2f}')
    print(f'0.99: {np.quantile(whs, 0.99):.2f} 0.01: {np.quantile(whs, 0.01):.2f}')
    print()

xyxys_acc = np.vstack([xyxys_dict[video] for video in videos])
whs = xyxys[:, [2, 3]] - xyxys[:, [0, 1]]
print('All')
print(f'max: {np.max(whs):.2f} min: {np.min(whs):.2f}')
print(f'0.999: {np.quantile(whs, 0.999):.2f} 0.001: {np.quantile(whs, 0.001):.2f}')
print()
