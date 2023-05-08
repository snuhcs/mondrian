import util
import argparse
import sys


def debug(frame_index, roi_id, box_id, roi_logs, box_logs):
    print(f"[frame {frame_index}, roi {roi_id}, box {box_id}]")
    failed = False
    try:
        roi_ = list(filter(lambda log: (log.id == roi_id) and (log.frameIndex == frame_index), roi_logs))[0]
    except:
        print(f"roi {roi_id} not found at frame {frame_index}")
        failed = True
    try:
        box_ = list(filter(lambda log: (log.id == box_id) and (log.frameIndex == frame_index), box_logs))[0]
    except:
        print(f"box {box_id} not found at frame {frame_index}")
        failed = True
    if failed:
        sys.exit(1)

    print(f"[frame {frame_index}, roi {roi_id}, box {box_id}]")
    print()

    print('roi.origLoc:',roi_.origLoc)
    print('roi.paddLoc:', roi_.paddedLoc)
    print('box.location:',box_.location)
    print()

    l,t,r,b = roi_.origLoc
    l2,t2,r2,b2 = roi_.paddedLoc
    bl,bt,br,bb = box_.location

    roiarea = (r-l)*(b-t)
    roiarea2 = (r2-l2)*(b2-t2)
    boxarea = (br-bl)*(bb-bt)
    print('box area:', boxarea)
    print('roi origLoc area:', roiarea)
    print('roi paddedLoc area:', roiarea2)
    print()

    xw = min(r, br) - max(l, bl)
    xh = min(b, bb) - max(t, bt)
    intersection = float(xw*xh)
    print('intersection with orig, inter/box:', intersection, intersection/float(boxarea))
    print('IoU btn box & origLoc', intersection/float(roiarea + boxarea - intersection))
    print()

    xw = min(r2, br) - max(l2, bl)
    xh = min(b2, bb) - max(t2, bt)
    intersection = float(xw*xh)
    print('intersection with padded, inter/box:', intersection, intersection/float(boxarea))
    print('IoU btn box & paddedLoc', intersection/float(roiarea2 + boxarea - intersection))
    print()



if __name__=="__main__":
    parser = argparse.ArgumentParser()
    parser.add_argument('--name', '-n', required=True, default=None)
    parser.add_argument('--frame', '-f', required=True, default=None)
    parser.add_argument('--roi', '-r', required=True, default=None)
    parser.add_argument('--box', '-b', required=True, default=None)
    args = parser.parse_args()

    roi_logs = util.load_roi_log(f"{args.name}.csv")
    box_logs = util.load_box_log(f"{args.name}.log")

    for roi_log in roi_logs:
        if not isinstance(roi_log, float):
            print(roi_log.priority)

    frame_index = int(args.frame)
    roi_id = int(args.roi)
    box_id = int(args.box)

    #debug(frame_index, roi_id, box_id, roi_logs, box_logs)

