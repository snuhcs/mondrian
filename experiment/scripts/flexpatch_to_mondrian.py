import argparse
import csv

def flexpatch_to_mondrian(log_path):
    # read labels from csv file
    print(f"Converting flexpatch log at {log_path}")
    vid = 0
    csv_file = open(f"{log_path}", "r", encoding="utf-8")
    f = csv.reader(csv_file, delimiter=",", doublequote=True, lineterminator="\n")
    labels = {}
    for row in f:
        frameIndex = int(row[0])
        ms = int(row[1])
        row = row[2:]
        num_box = int(len(row) / 6)
        if num_box == 0:
            if frameIndex not in labels:
                labels[(frameIndex, ms)] = []
        else:
            for i in range(num_box):
                box = row[i*6:i*6+6]
                # r0, r1, r2, r3, score, id
                box = [float(x) for x in box]
                if (frameIndex, ms) not in labels:
                    labels[(frameIndex, ms)] = []
                labels[(frameIndex, ms)].append(box) 

    print(f"Total number of frames: {len(labels.items())}")
    msg = ""
    for (frame_index, ms), boxes in labels.items():
        # vid, frame_index, time
        if len(boxes) == 0:
            continue
        ns = ms * 1000000
        msg += f"{vid},{frame_index},{ns},"
        for box in boxes:
            # id, l, t, r, b, conf, origin, choiceofbox, label_in_string
            msg += f"-2,{box[0]},{box[1]},{box[2]},{box[3]},{box[4]},-2,-2,person,"
        msg = msg[:-1]
        msg += "\n"

    new_log_path = log_path + "_mondrian_boxes.txt"
    with open(new_log_path, "w") as f:
        f.write(msg)
    print(f"Saved mondrian log at {new_log_path}")

if __name__ == "__main__":
    parser = argparse.ArgumentParser()
    parser.add_argument('log_path')
    args = parser.parse_args()
    flexpatch_to_mondrian(args.log_path)
        
