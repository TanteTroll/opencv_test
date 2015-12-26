#ifndef FILTER_H
#define FILTER_H

#include <opencv2/opencv.hpp>
#include <QImage>

class Filter
{
public:
    Filter(QImage img);
    Filter(cv::Mat img);

    cv::Mat edgeDetect;
    cv::Mat output_cv;
    QImage output_qt;

    void setImg(QImage img);
    QImage Filter::edgeDetection(int lowThreshold = 20, int ratio = 3, int kernel_size =3);
    QImage Filter::HarrisCorner(int blockSize=3, double k=0.04,int tresh = 200, int displayedPoints = 0);

 private:
    cv::Mat input_cv;
    QImage input_qt;
    cv::Mat greyscale;

    void Filter::makeGreyscale();
   // void Filter::setOutput(cv::Mat img);


};

#endif // FILTER_H
