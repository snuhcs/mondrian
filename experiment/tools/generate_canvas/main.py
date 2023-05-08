from tqdm import tqdm
import pandas as pd
import numpy as np
import os
import cv2
import time

from datatype import Canvas
from util import parse_row, load_frames, get_args

max_canvas_size = 1280
batch_size = 50
bar_format = "{percentage:3.0f}%|{bar:30}| {n_fmt:>7}/{total_fmt:<7} : {desc}"
INVALID_CANVAS_INDEX = -1

def main(args):
    log_dir = args.log_dir
    video_path = args.video_path

    # load roi.csv and parse it into mergedROIs
    df = pd.read_csv(log_dir/"roi.csv", delimiter='\t')
    mergedROIs = {}
    for i, row in tqdm(df.iterrows(), total=len(df),
                       desc="PARSING ROI.CSV", bar_format=bar_format):
        roi, mROI = parse_row(row)
        key = hash(mROI)
        if key not in mergedROIs:
            mergedROIs[key] = mROI
        mergedROIs[key].register(roi)

    # group mergedROIs into canvases
    canvases = {}
    for mROI in mergedROIs.values():
        canvasIndex = mROI.canvasIndex
        canvasSize = mROI.canvasSize
        if canvasIndex == INVALID_CANVAS_INDEX:
            continue
        if canvasIndex not in canvases:
            canvases[canvasIndex] = Canvas(canvasIndex, canvasSize)
        canvases[canvasIndex].add_mROI(mROI)

    # list of canvases sorted by index
    canvas_list = sorted(canvases.values(), key=lambda canvas: canvas.index)

    # create canvas images (in batches for memory efficiency)
    pbar = tqdm(total=len(canvas_list),
                desc="CREATING CANVAS IMAGES", bar_format=bar_format)
    for i in range(0, len(canvas_list), batch_size):
        canvasIndexRange = (i, min(i+batch_size, len(canvas_list)))
        canvas_list_batch = canvas_list[canvasIndexRange[0]:canvasIndexRange[1]]

        # load required video frames to memory
        frame_index_set = set()
        for canvas in canvas_list_batch:
            frame_index_set |= canvas.get_frame_index_set()
        start_index = min(frame_index_set)
        end_index = max(frame_index_set) + 1
        success, frames = load_frames(video_path, start_index, end_index)
        assert success, "Failed to load frames"

        # process each canvas
        for canvas in canvas_list_batch:
            canvasIndex = canvas.index

            # fill background
            canvas_img = np.zeros(
                (max_canvas_size, max_canvas_size, 3), dtype=np.uint8)
            bg = np.ones((canvas.size, canvas.size, 3), dtype=np.uint8)*124
            canvas_img[0:canvas.size, 0:canvas.size] = bg

            # crop mROI image and paste it on canvas
            for mROI in canvas.mROIs:
                pX, pY = mROI.packedXY
                img = frames[mROI.frameIndex][mROI.loc.t:mROI.loc.b,
                                              mROI.loc.l:mROI.loc.r]
                scale = mROI.mergedScale
                newH, newW, _ = map(lambda x: round(scale*x), img.shape)
                # XXX : why are there scales that make 0-sized images?
                if not (newH == 0 or newW == 0):
                    img = cv2.resize(img, dsize=(0, 0), fx=scale, fy=scale)
                canvas_img[pY:pY+newH, pX:pX+newW] = img

            # save canvas image at log_dir/canvas
            if not os.path.exists(log_dir/"canvas"):
                os.makedirs(log_dir/"canvas")
            canvas_img = cv2.cvtColor(canvas_img, cv2.COLOR_RGB2BGR)
            cv2.imwrite(str(log_dir/"canvas")+ f"/{canvasIndex}.jpg", canvas_img)
            pbar.update(1)

        # clear frames from memory
        frames.clear()

if __name__ == "__main__":
    start_time = time.time()
    args = get_args()
    main(args)
    end_time = time.time()
    print(f"Time elapsed: {end_time - start_time:.2f} seconds")