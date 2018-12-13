
#include "detracker.h"
#include "kcf/kcftracker.hpp"

#ifndef USE_NPD
#include "dlibfacedetector.h"
#else
#include "npdfacedetector.h"
#endif

#include <unistd.h>

#include <opencv2/opencv.hpp>

using namespace cv;
using namespace std;

#define DEBUG_RETRACK

DeTracker::DeTracker()
{
    _imageBuffer = new TemporalBuffer< cv::Mat >( 100 );
    _curT = 0.0;

#ifndef USE_NPD
    _detector = new DlibFaceDetector();
#else
    _detector = new NPDFaceDetector( "../npd_model/620model" );
#endif

    startDetector();
}

DeTracker::~DeTracker()
{

}

void DeTracker::process(cv::Mat &img)
{
    _imageBuffer->push( TimeStamped<cv::Mat>( img, _curT ) );
    // cerr << "RT_last_ts=" << _imageBuffer->last_ts() << endl;
    updateTracks(img);
    _curT += 1.0;
}

void DeTracker::startDetector()
{
    _detectorThread = std::thread( &DeTracker::detectorRoutine, this );
}

void DeTracker::detectorRoutine()
{
    while(true)
    {
        // Try to retrieve the "last" image in buffer
        double lastTs = _imageBuffer->last_ts();
//        cerr << "lastTs=" << lastTs << endl;
        cv::Mat dimg;
//        cerr << "first_ts=" << _imageBuffer->first_ts() << " - lastTs=" << _imageBuffer->last_ts() << endl;
        if( _imageBuffer->sample( lastTs, dimg ) )
        {
            // imshow( "detect_image", dimg );
//            cerr << "size=" << dimg.cols << " - " << dimg.rows << endl;
            std::vector< RotatedRect > dets = _detector->detect( dimg );
            cerr << "DETECTED " << dets.size() << " at t=" << lastTs << endl;

#pragma omp parallel for
            for( int k = 0; k < dets.size(); ++k )
            //for( RotatedRect rr : dets )
            {
                cv::RotatedRect rr = dets[k];
                double retrack_ts;
                cv::RotatedRect retrack_rr = retrack( rr, lastTs, retrack_ts );
                cv::Mat retrack_img;
                if( _imageBuffer->sample( retrack_ts, retrack_img ) )
                {
                    cerr << "RETRACKED UP TO t=" << retrack_ts << endl;
                    insertTrack( retrack_rr, retrack_img );
                }
            }
        }
        // sleep(1);
    }
}

RotatedRect DeTracker::retrack(RotatedRect det, double det_ts, double& retracked_ts )
{
    KCFTracker retracker( true, false, true, false );
    double targetTs = _imageBuffer->last_ts();
    double cur_ts = det_ts;

    cv::Mat cur_img;
    cv::Rect cur_rect = det.boundingRect();
    bool firstTrackingFrame = true;

    cv::Mat img_debug;

    while( cur_ts < _imageBuffer->last_ts() )
    {
        targetTs = _imageBuffer->last_ts();
        if( _imageBuffer->sample( cur_ts, cur_img ) )
        {
            cv::Mat cur_img_gray;

            // Convert to gray_scale if needed
            /*
            if( cur_img.channels() == 3 )
            {
                cvtColor( cur_img, cur_img_gray, CV_RGB2GRAY );
            }
            else
            {
                cur_img_gray = cur_img.clone();
            }
            */

            cur_img_gray = cur_img.clone();

            // First iteration
            if( firstTrackingFrame )
            {
                img_debug = cv::Mat::zeros( cur_img.rows, cur_img.cols, CV_8UC3 );
                retracker.init( cur_rect, cur_img_gray );
                firstTrackingFrame = false;
            }
            else
            {
                img_debug = cur_img_gray.clone();
                cur_rect = retracker.update( cur_img_gray );
#ifdef DEBUG_RETRACK
                cv::rectangle( img_debug, cur_rect, Scalar(0,255,0), 1 );
#endif
            }
#ifdef DEBUG_RETRACK
            imshow( "retrack_dbg", img_debug );
#endif
            cur_ts = cur_ts + 1.0;
        }
    }
    retracked_ts = targetTs;

    cv::RotatedRect ret( cv::Point2f(cur_rect.tl().x + cur_rect.width / 2,
                                     cur_rect.tl().y + cur_rect.height / 2),
                         cv::Size2f( cur_rect.width, cur_rect.height ), 0);
    return ret;
}

void DeTracker::insertTrack(RotatedRect region, Mat &image)
{
    std::unique_lock< std::mutex > lock(_tracksMtx );

    double max_overlap = 0.0;
    Track* max_track = nullptr;

    // Check if it corresponds to an active tracked region.
#pragma omp parallel for
    for( int k = 0; k < _activeTracks.size(); ++k )
    {
        Track* t = _activeTracks[k];
        cv::Rect trr = t->region().boundingRect();
        cv::Rect region_r = region.boundingRect();
        cv::Rect inter_r = trr & region_r;
        cv::Rect union_r = trr | region_r;
        double overlap = (double)(inter_r.area()) / (double)(union_r.area());
        if( overlap >= max_overlap )
        {
            max_overlap = overlap;
            max_track = t;
        }
    }

    // Fuck
    if( max_overlap < 0.2 )
    {
        cerr << "INSERTION - OVERLAP = " << max_overlap << " REGION=" << region.boundingRect() << endl;
        // This means new track
        _activeTracks.push_back( new Track( region, image ) );
    }
    else
    {
        // Not a new track
    }

}

void DeTracker::updateTracks( cv::Mat img )
{
    std::unique_lock< std::mutex > lock(_tracksMtx );
    for (auto it = _activeTracks.begin(); it != _activeTracks.end(); /* NOTHING */)
    {
        Track* t = (*it);
        double pv = t->update( img );
        if( pv < 0.2 )
        {
            // cerr << "deactivate pv=" << pv << endl;
            it = _activeTracks.erase(it);
            // t->set_active(false);
        }
        else
        {
            // t->set_active(true);
            ++it;
        }
    }
    /*
    for( Track* t : _activeTracks )
    {
        double pv = t->update( img );
        if( pv < 0.3 )
        {
            to_delete
        }
    }
    */
}

void DeTracker::drawCurrentTracks(Mat& img)
{
    std::unique_lock< std::mutex > lock(_tracksMtx );
//    cerr << "DRAWING " << _activeTracks.size() << " regions" << endl;
    for( Track* t : _activeTracks )
    {
        cv::Rect r = t->region().boundingRect();
//        cerr << "DRAW r=" << r << endl;
        cv::rectangle( img, r, Scalar(255,0,0), 1 );
    }
}

std::vector<RotatedRect> DeTracker::getTrackedRegions()
{
    std::vector< RotatedRect > ret;
    std::unique_lock< std::mutex > lock(_tracksMtx );
    // cerr << "DRAWING " << _activeTracks.size() << " regions" << endl;
    for( Track* t : _activeTracks )
    {
        if( t->active() )
        {
            ret.push_back( t->region() );
        }
    }
    return ret;
}
