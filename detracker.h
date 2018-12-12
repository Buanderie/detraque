#pragma once

#include <memory>
#include <thread>

#include <opencv2/opencv.hpp>

#include "detector.h"
#include "temporalbuffer.h"
#include "tracker.h"
#include "track.h"

#define USE_NPD

class DeTracker
{
public:
    DeTracker();
    virtual ~DeTracker();
    void process( cv::Mat& img );
    void drawCurrentTracks(cv::Mat &img );
    std::vector< cv::RotatedRect > getTrackedRegions();

private:
    TemporalBuffer< cv::Mat > * _imageBuffer;
    Detector * _detector;
    std::shared_ptr< GenericTracker > _tracker;

    double _curT;

    std::thread _detectorThread;
    void startDetector();
    void detectorRoutine();

    cv::RotatedRect retrack(cv::RotatedRect det, double det_ts , double &retracked_ts);

    std::mutex _tracksMtx;
    std::vector< Track* > _activeTracks;
    void insertTrack(cv::RotatedRect region, cv::Mat& image);

    void updateTracks(cv::Mat img);

protected:

};
