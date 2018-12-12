#pragma once

#include <opencv2/opencv.hpp>

cv::Mat aspectResize(cv::Mat &img, cv::Size newSize, float &aspectRatio);
cv::RotatedRect rectToRotatedRect( cv::Rect r );
