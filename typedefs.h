#ifndef TYPEDEFS_H
#define TYPEDEFS_H

#include <opencv2/opencv.hpp>

class Line;
class Square;
class Corner;
class Layer;

typedef std::vector<cv::Point> Points;
typedef std::vector<Line> Lines;
typedef std::vector<Square> Squares;
typedef std::vector<Corner> Corners;
typedef std::vector<Layer> Layers;

#endif // TYPEDEFS_H
