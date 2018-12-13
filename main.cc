
#include <iostream>
#include <unistd.h>

#include <opencv2/opencv.hpp>

#include "utils.h"
#include "dlibfacedetector.h"
#include "detracker.h"

using namespace std;
using namespace cv;

cv::Rect readjustRect( cv::Rect r , cv::Size imgSize )
{
    cv::Rect ret = r;
    ret.x = max( r.x, 0 );
    ret.y = max( r.y, 0 );
    if( r.br().x > imgSize.width )
    {
        ret.width -= (r.br().x - imgSize.width);
    }
    if( r.br().y > imgSize.height )
    {
        ret.height -= (r.br().y - imgSize.height);
    }
    return ret;
}

int main( int argc, char** argv )
{

    srand(time(NULL));

    namedWindow( "frame" );

    cv::VideoCapture cap( argv[1] );
    bool isPlaying = true;

    cv::Mat curFrame;
    float ar;

//    DlibFaceDetector dlibd;

    DeTracker dtrack;

    for(;;)
    {
        cv::Mat frame;

        if( isPlaying )
        {
            cap >> frame;
            if( frame.cols <= 0 || frame.rows <= 0 )
                continue;
            curFrame = aspectResize( frame, cv::Size(800,600), ar );
        }

        /*
        std::vector< cv::RotatedRect > dets = dlibd.detect( curFrame );
        for( cv::RotatedRect rrect : dets )
        {
            cv::Rect r = rrect.boundingRect();
            cv::rectangle( curFrame, r, cv::Scalar(255,0,0), 1 );
        }
        */

        dtrack.process( curFrame );

        cv::Mat output = curFrame.clone();
        dtrack.drawCurrentTracks( output );

        std::vector< cv::RotatedRect > res = dtrack.getTrackedRegions();
        for( cv::RotatedRect rr : res )
        {
            cv::Mat crop( output, readjustRect( rr.boundingRect(), cv::Size( curFrame.cols, curFrame.rows ) ) );
            cv::blur( crop, crop, cv::Size(35,35) );
        }

        imshow( "frame", output );
        waitKey(5);

    }

    return 0;

}
