
#include "dlibfacedetector.h"

using namespace dlib;
using namespace std;
using namespace cv;

std::vector<RotatedRect> DlibFaceDetector::detect(Mat &img)
{
    // Check if gray
    cv::Mat input;
    if( img.channels() != 1 )
    {
        cvtColor( img, input, CV_RGB2GRAY );
    }
    else
    {
        input = img.clone();
    }

    array2d<unsigned char> dlib_img;
    dlib::assign_image(dlib_img, dlib::cv_image<uchar>(input));

    std::vector< dlib::rectangle > dets;
    dets = _detector.operator ()( dlib_img, 0.01 );

    // cerr << "detected n=" << dets.size() << endl;
    std::vector< cv::RotatedRect > ret;
    for( dlib::rectangle r : dets )
    {
        cv::Point2f center( r.tl_corner().x() + r.width() / 2, r.tl_corner().y() + r.height() / 2 );
        cv::Size rsize( r.width(), r.height() );
        cv::RotatedRect rrect( center, rsize, 0 );
        ret.push_back( rrect );
    }

    return ret;
}
