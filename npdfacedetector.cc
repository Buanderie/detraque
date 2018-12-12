
#include "npdfacedetector.h"
#include "utils.h"

using namespace cv;

std::vector<cv::RotatedRect> NPDFaceDetector::detect(cv::Mat &img)
{
    std::vector< cv::RotatedRect > ret;

    cv::Mat frame;
    float ar;
    frame = aspectResize( img, cv::Size(640,480), ar );

    cv::Mat gray;
    cvtColor(frame, gray, CV_BGR2GRAY);
    vector<Rect> rects;
    vector<float> scores;
    vector<int> index;
    index = _gab.DetectFace(gray,rects,scores);

    for (int i = 0; i < index.size(); i++) {
        if(scores[index[i]]>100)
        {
            cv::Rect drect = rects[index[i]];
            cv::RotatedRect drr = rectToRotatedRect(drect);
            drr.center /= ar;
            drr.size /= ar;
            ret.push_back( drr );
        }
    }

    return ret;

}
