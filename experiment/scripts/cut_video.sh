#!/bin/bash

set -x

if [ $# -ne 4 ]; then
    echo "Usage : ./cut_video.sh <input_video_path> <output_video_path> <start_frame_index> <end_frame_index>"
    exit -1
fi

input=$1
output=$2
start_index=$3
end_index=$4

ffmpeg -i $input -vf "trim=start_frame=$start_index:end_frame=$end_index, setpts=PTS-STARTPTS" -an $output
