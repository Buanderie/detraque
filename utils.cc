
#include "utils.h"

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

RotatedRect rectToRotatedRect(Rect r)
{
    return RotatedRect( cv::Point2f( r.tl().x + r.width / 2,
                                     r.tl().y + r.height / 2),
                        cv::Size2f(r.width, r.height),
                        0 );
}
