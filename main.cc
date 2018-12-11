
#include <iostream>
#include <unistd.h>

#include <opencv2/opencv.hpp>

#include "dlibfacedetector.h"

using namespace std;
using namespace cv;

Mat aspectResize(Mat &img, Size newSize, float &aspectRatio)
{
    float ar = (float)(img.cols) / (float)(img.rows);
    if( ar > 1.0f )
    {
        // image plus large que haute
        aspectRatio = (float)(newSize.width) / (float)(img.cols);
    }
    else
    {
        aspectRatio = (float)(newSize.height) / (float)(img.rows);
    }
    cv::Mat ret;
    cv::resize( img, ret, cv::Size(), aspectRatio, aspectRatio );
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

    DlibFaceDetector dlibd;

    for(;;)
    {
        cv::Mat frame;

        if( isPlaying )
        {
            cap >> frame;
            if( frame.cols <= 0 || frame.rows <= 0 )
                continue;
            // curFrame = aspectResize( frame, cv::Size(800,600), ar );
            curFrame = frame.clone();
        }

        dlibd.detect( curFrame );

        imshow( "frame", curFrame );
        waitKey(5);

    }

    return 0;

}
