locType = 'paddedLoc'


class Rect:
    def __init__(self, ltrb):
        l, t, r, b = ltrb
        self.l = l
        self.t = t
        self.r = r
        self.b = b


class ROI:
    def __init__(self, row):
        self.frameIndex = row['frameIndex']
        loc = Rect([int(row[f"{locType}_{i}"]) for i in "ltrb"])
        self.loc = loc
        self.mergedLoc = Rect([int(row[f"mergedLoc_{i}"]) for i in "ltrb"])
        self.packedXY = row['packedXY_x'], row['packedXY_y']


class mergedROI:
    def __init__(self, row):
        self.canvasIndex = row['absolutePackedCanvasIndex']
        self.canvasSize = row['packedCanvasSize']
        self.frameIndex = row['frameIndex']
        self.loc = Rect([int(row[f"mergedLoc_{i}"]) for i in "ltrb"])
        self.packedXY = (row['packedXY_x'], row['packedXY_y'])
        self.mergedScale = row['mergedScale']
        self.rois = []

    def __eq__(self, other):
        cond = \
            self.canvasIndex == other.canvasIndex &\
            self.frameIndex == other.frameIndex &\
            self.loc == other.loc &\
            self.packedXY == other.packedXY &\
            self.mergedScale == other.mergedScale
        return cond

    def __hash__(self) -> int:
        return hash((self.canvasIndex, self.packedXY))

    def register(self, roi: ROI):
        self.rois.append(roi)


class Canvas:
    def __init__(self, index, size):
        self.index = index
        self.size = size
        self.mROIs = []

    def add_mROI(self, mROI):
        self.mROIs.append(mROI)

    def get_frame_index_set(self):
        return set([roi.frameIndex for roi in self.mROIs])
