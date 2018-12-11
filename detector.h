#pragma once

#include <opencv2/opencv.hpp>

class Detector
{
public:
    Detector(){}
    virtual ~Detector(){}

    virtual std::vector< cv::RotatedRect > detect( cv::Mat& img )
    {
        std::vector< cv::RotatedRect > ret;
        return ret;
    }

private:

protected:

};
