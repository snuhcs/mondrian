from collections import defaultdict
from pathlib import Path

import numpy as np
import pandas as pd

enqueueTime = 'enqueueTime'
scheduleTime = 'scheduledTime'
fullStart = 'fullInferenceStartTime'
fullEnd = 'fullInferenceEndTime'
PDStart = 'pixelDiffROIProcessStartTime'
PDEnd = 'pixelDiffROIProcessEndTime'
OFStart = 'opticalFlowROIProcessStartTime'
OFEnd = 'opticalFlowROIProcessEndTime'
resizeStart = 'resizeStartTime'
resizeEnd = 'resizeEndTime'
mergeStart = 'mergeROIStartTime'
mergeEnd = 'mergeROIEndTime'
mixingStart = 'mixingStartTime'
mixingEnd = 'mixingEndTime'
inferenceStart = 'mixedInferenceStartTime'
inferenceEnd = 'mixedInferenceEndTime'
reconstructStart = 'reconstructStartTime'
reconstructEnd = 'reconstructEndTime'
endTime = 'endTime'

START_END_PAIRS = {
    fullStart: fullEnd,
    PDStart: PDEnd,
    OFStart: OFEnd,
    resizeStart: resizeEnd,
    mergeStart: mergeEnd,
    mixingStart: mixingEnd,
    inferenceStart: inferenceEnd,
    reconstructStart: reconstructEnd,
}


# print(list(Path('result_221209_224453_virat_e2e_b7e1db/0_mondrian').glob('*/timeline*')))
timeline_csvs = list(Path('result_221209_224453_virat_e2e_b7e1db/0_mondrian').glob('*/timeline*'))
assert len(timeline_csvs) == 5, f'{len(timeline_csvs)} {timeline_csvs}'

times = defaultdict(list)
for timeline_csv in timeline_csvs:
    timeline_df = pd.read_csv(timeline_csv, delimiter='\t')
    for s, e in START_END_PAIRS.items():
        se = timeline_df[[s, e]].values
        mask = (se[:,0] != 0) & (se[:,1] != 0)
        se = np.unique(se[mask], axis=0)
        avg = np.sum(se[:,1] - se[:,0]) / sum(mask)
        times[s] += [avg for _ in range(sum(mask))]

for s in START_END_PAIRS.keys():
    print(s, np.mean(times[s]), np.std(times[s]))

