#pragma once

#include "detector.h"

#include "npd/LearnGAB.h"

#include <iostream>
#include <opencv2/opencv.hpp>

class NPDFaceDetector : public Detector
{
public:
    NPDFaceDetector( const std::string modelPath )
        :_modelPath(modelPath)
    {
        _gab.LoadModel( _modelPath );
    }

    virtual ~NPDFaceDetector()
    {

    }

    virtual std::vector< cv::RotatedRect > detect( cv::Mat& img );

private:
    std::string _modelPath;
    GAB _gab;

protected:


};
