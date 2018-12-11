#pragma once

#include "detector.h"

#include <dlib/image_processing/frontal_face_detector.h>
#include <dlib/gui_widgets.h>
#include <dlib/image_io.h>
#include <dlib/opencv/cv_image.h>

#include <iostream>

#include <opencv2/opencv.hpp>

class DlibFaceDetector : public Detector
{
public:
    DlibFaceDetector()
    {
        _detector = dlib::get_frontal_face_detector();
    }

    virtual ~DlibFaceDetector()
    {

    }

    virtual std::vector< cv::RotatedRect > detect( cv::Mat& img );

private:
    dlib::frontal_face_detector _detector;

protected:


};
