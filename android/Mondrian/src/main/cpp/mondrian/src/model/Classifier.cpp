#include "mondrian/model/Classifier.hpp"

#include <set>

#include "opencv2/imgproc.hpp"

#include "mondrian/Log.hpp"
#include "mondrian/Utils.hpp"

namespace md {

Classifier::Classifier(const int numLabels, const int inputSize, const int outputSize,
                       const float confThres, const float iouThres)
    : numLabels(numLabels), inputSize(inputSize, inputSize), outputSize(outputSize),
      confThres(confThres), iouThres(iouThres) {}

std::vector<BoundingBox> Classifier::recognizeImage(const cv::Mat& rgbMat) {
  time_us start = NowMicros();
  cv::Mat inputTensor = preprocess(rgbMat);
  time_us preprocessTime = NowMicros();
  inference(inputTensor);
  time_us inferenceTime = NowMicros();
  std::vector<BoundingBox> boxesAll = postprocess(rgbMat.cols, rgbMat.rows);
  time_us postprocessTime = NowMicros();
  std::vector<BoundingBox> boxesNms = nms(boxesAll, numLabels, iouThres);
  time_us nmsTime = NowMicros();

  LOGD("[InferenceEngine] Latency with %dx%d input on %s "
       "// total=%lld preprocess=%lld inference=%lld postprocess=%lld nms=%lld",
       inputSize.width, inputSize.height, str(device()).c_str(),
       nmsTime - start,
       preprocessTime - start,
       inferenceTime - preprocessTime,
       postprocessTime - inferenceTime,
       nmsTime - postprocessTime);

  return boxesNms;
}

} // namespace md
