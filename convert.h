#ifndef CONVERT_H
#define CONVERT_H

#include <opencv2/opencv.hpp>
#include <QImage>

class convert
{
public:
    convert();
    static QImage mat2qimg( const cv::Mat &inMat );
    static cv::Mat rotate_cw( cv::Mat image, int degrees);
    static cv::Mat qimg2mat( const QImage &inImage, bool inCloneImageData = true );
    static QImage rotate_cw(QImage image, int degrees);
};

#endif // CONVERT_H
