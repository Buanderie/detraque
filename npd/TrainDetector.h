#include "common.h"
#include <opencv2/core/core.hpp>
/* \breif Wraper for call Detector */
class TrainDetector{
  public:
    /*
     * \breif single detect
     */
    void Detect();
    /*
     * \breif Detect For FDDB
     */
    void FddbDetect();
    /*
     * \breif Detect face from camera
     */
    void Live();

    void DetectOpenCV( cv::Mat& img );

};
