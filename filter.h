#ifndef FILTER_H
#define FILTER_H

#include <opencv2/opencv.hpp>
#include <QImage>

class Filter
{
public:
    Filter(QImage img);
    Filter(cv::Mat img);


    QImage Filter::edgeDetection(int lowThreshold = 20, int ratio = 3, int kernel_size =3);
    QImage Filter::harrisCorner(int blockSize=3, double k=0.04,int tresh = 200, int displayedPoints = 0);

    cv::Mat edgeDetect;
    cv::Mat output_cv;
    QImage output_qt;

    void setImg(QImage img);

 private:
    cv::Mat input_cv;
    QImage input_qt;
    cv::Mat greyscale;

    void Filter::makeGreyscale();
};

#endif // FILTER_H
