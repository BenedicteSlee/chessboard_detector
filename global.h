#ifndef GLOBAL_H
#define GLOBAL_H

#include <opencv2/opencv.hpp>

namespace global{

extern bool doDraw;
extern cv::Mat image_rgb;
extern cv::Mat image_gray;
extern cv::Mat image_norm;
extern cv::Mat image;
extern cv::Mat image_canny;
extern cv::Mat image_hough;
extern cv::Mat image_hough_mod;
extern cv::Mat image_rgb_resized;
extern cv::Mat image_r, image_g, image_b;
extern std::vector<cv::Mat> channels;
extern cv::Mat image_pieces;

} // end namespace global
#endif // GLOBAL_H
