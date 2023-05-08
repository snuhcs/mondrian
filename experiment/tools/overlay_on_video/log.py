import argparse
import sys
from icecream import ic


type_map = {
    1: 'OF',
    2: 'PD',
}
origin_map = {
    0: 'NU',
    1: 'FF',
    2: 'BB',
    3: 'PD',
    4: 'IP',
    5: 'NF',
    6: 'NM',
}


class ROILog():
    attrs = {
        'key': 'name of the video',
        'keyCount': 'to differentiate key when using identical videos',
        'frameIndex': '.',
        'origLoc': '(left, top, right, bottom)',
        'paddedLoc': '(left, top, right, bottom) with padding',
        'roi_type': 'type of the ROI (1: OF, 2: PD)',
        'origin': 'origin of the ROI (2: BB, 3: PD)',
        'wh': '(width, height) of the ROI',
        'xyRatio': '.',
        'shiftAvg': '(shiftAvgX, shiftAvgY)',
        'shiftSize': '.',
        'shiftStd': '(shiftStdX, shiftStdY)',
        'shiftNcc': '.',
        'avgErr': '.',
        'numProbingROIs': '.',
        'id': '.',
        'mergedLoc': '(left, top, right, bottom) with merged ROI',
        'packedXY': '(packedXY_x, packedXY_y)',
        'mergedScale': '.',
        'isPacked': '.',
        'absolutePackedCanvasIndex': 'Absolute index of the mixed frame ROI is pacekd into',
        'packedCanvasSize': 'Size of the packed canvas',
        'maxEdgeLength': '.',
        'targetScale': '.',
        'scaleLevel': '.',
    }

    def __init__(self, header, log):
        raw = argparse.Namespace(**dict(zip(header, log)))
        raw.key = int(raw.videoId)

        raw.frameIndex = int(raw.frameIndex)

        l = round(float(raw.origLoc_l))
        t = round(float(raw.origLoc_t))
        r = round(float(raw.origLoc_r))
        b = round(float(raw.origLoc_b))
        raw.origLoc = (l, t, r, b)
        del raw.origLoc_l
        del raw.origLoc_t
        del raw.origLoc_r
        del raw.origLoc_b

        l = round(float(raw.paddedLoc_l))
        t = round(float(raw.paddedLoc_t))
        r = round(float(raw.paddedLoc_r))
        b = round(float(raw.paddedLoc_b))
        raw.paddedLoc = (l, t, r, b)
        del raw.paddedLoc_l
        del raw.paddedLoc_t
        del raw.paddedLoc_r
        del raw.paddedLoc_b

        raw.roi_type = type_map[int(raw.type)]
        del raw.type
        raw.origin = origin_map[int(raw.origin)]
        assert raw.origin in ['BB', 'PD']

        width = round(float(raw.width))
        height = round(float(raw.height))
        raw.wh = (width, height)
        del raw.width
        del raw.height

        raw.xyRatio = floatdot2f(raw.xyRatio)

        shiftAvgX = floatdot2f(raw.shiftAvgX)
        shiftAvgY = floatdot2f(raw.shiftAvgY)
        raw.shiftAvg = (shiftAvgX, shiftAvgY)
        raw.shiftSize = shiftAvgX*shiftAvgX + shiftAvgY*shiftAvgY
        del raw.shiftAvgX
        del raw.shiftAvgY

        shiftStdX = floatdot2f(raw.shiftStdX)
        shiftStdY = floatdot2f(raw.shiftStdY)
        raw.shiftStd = (shiftStdX, shiftStdY)
        del raw.shiftStdX
        del raw.shiftStdY

        raw.shiftNcc = floatdot2f(raw.shiftNcc)
        raw.avgErr = floatdot2f(raw.avgErr)
        raw.numProbingROIs = int(raw.numProbingROIs)
        raw.id = int(raw.id)

        l = round(float(raw.mergedLoc_l))
        t = round(float(raw.mergedLoc_t))
        r = round(float(raw.mergedLoc_r))
        b = round(float(raw.mergedLoc_b))
        raw.mergedLoc = (l, t, r, b)
        del raw.mergedLoc_l
        del raw.mergedLoc_t
        del raw.mergedLoc_r
        del raw.mergedLoc_b

        packedXY_x = round(float(raw.packedXY_x))
        packedXY_y = round(float(raw.packedXY_y))
        raw.packedXY = (packedXY_x, packedXY_y)
        raw.isPacked = True if (raw.packedXY != (-1,-1)) else False
        del raw.packedXY_x
        del raw.packedXY_y

        raw.mergedScale = float(raw.mergedScale)
        raw.absolutePackedCanvasIndex = int(raw.absolutePackedCanvasIndex)
        del raw.absolutePackedCanvasIndex
        raw.packedCanvasSize = int(raw.packedCanvasSize)
        raw.maxEdgeLength = round(float(raw.maxEdgeLength))
        raw.targetScale = float(raw.targetScale)
        raw.scaleLevel = int(raw.scaleLevel)

        self.log = raw

    def __getattr__(self, name):
        return getattr(self.log, name)

    @classmethod
    def hasattr(cls, name):
        return name in cls.attrs


class BoxLog:
    attrs = {
        'key': 'name of the video',
        'keyCount': 'to differentiate key when using identical videos',
        'frameIndex': '.',
        'id': '.',
        'location': '(left, top, right, bottom)',
        'confidence': '.',
        'origin': 'origin of the Box (0: Null, 1: fromFF, 2: fromBB, 3: fromPD, 4: fromIP)',
        'choiceofBox': ',',
        'label': '.',
    }

    def __init__(self, key, frameIndex, info):
        ns = argparse.Namespace()
        # TODO: read config.json and use key as index to video name
        #ns.keyCount = int(key.split('.')[-1][3:])
        ns.keyCount = 1
        #ns.key = key.split('/')[-1].split('.')[0] + '.mp4'
        ns.key = int(key)
        ns.frameIndex = int(frameIndex)
        ns.id = int(info[0])
        try:
            ns.location = tuple(map(lambda x:round(float(x)),[info[1], info[2], info[3], info[4]]))
        except:
            ns.location = (0,0,0,0)
        ns.confidence = floatdot2f(info[5])
        ns.origin = origin_map[int(info[6])]
        # assert ns.origin != 'NU'
        ns.choiceofBox = int(info[7])
        ns.label = info[8]
        self.ns = ns

    def __getattr__(self, name):
        return getattr(self.ns, name)

    @classmethod
    def hasattr(cls, name):
        return name in cls.attrs


def floatdot2f(val):
    return int(float(val)*100)/100

def floatdot4f(val):
    return int(float(val)*10000)/10000
