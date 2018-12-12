#pragma once

#include <opencv2/opencv.hpp>

#include "kcf/kcftracker.hpp"

class Track
{
public:
    Track( cv::RotatedRect initialRegion, cv::Mat& initialImage )
    {
        _tracker = new KCFTracker( true, false, true, false );
        _tracker->init( initialRegion.boundingRect(), initialImage.clone() );
        _curRegion = initialRegion;
    }

    virtual ~Track()
    {
        delete _tracker;
    }

    double update( cv::Mat& img )
    {
        cv::Rect tres = _tracker->update( img );
        _curRegion = cv::RotatedRect( cv::Point2f(tres.tl().x + tres.width / 2,
                                                  tres.tl().y + tres.height / 2 ), cv::Size2f( tres.width, tres.height ), 0 );
        return _tracker->last_peak_value;
    }

    cv::RotatedRect region()
    {
        return _curRegion;
    }

    KCFTracker * _tracker;
    cv::RotatedRect _curRegion;

};
