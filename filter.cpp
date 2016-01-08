#include "filter.h"
#include "convert.h"
#include <opencv2/opencv.hpp>
#include <QImage>
#include <QDebug>

Filter::Filter(cv::Mat img)
{
    this->input_cv = img;
    this->input_qt = convert::mat2qimg(img);
}
Filter::Filter(QImage img)
{
    this->input_qt = img;
    this->input_cv = convert::qimg2mat(img);
}


void Filter::setImg(QImage img)
{
    this->input_qt = img;
    this->input_cv = convert::qimg2mat(img);
}
void Filter::makeGreyscale()
{
    cv::cvtColor(this->input_cv,this->greyscale,CV_BGR2GRAY);
}

QImage Filter::edgeDetection(int lowThreshold, int ratio, int kernel_size)
{

    //Input Control
    if (kernel_size>ratio)kernel_size=ratio;


    //greyscale
    makeGreyscale();


    //detection
    cv::Mat detected_edges;
    cv::blur( greyscale, detected_edges, cv::Size(3,3) );
    cv::Canny( detected_edges, detected_edges, lowThreshold, lowThreshold*ratio, kernel_size );
    detected_edges.copyTo(edgeDetect);


    //convert back
    cv::cvtColor(detected_edges,detected_edges,CV_GRAY2RGB);
    QImage filtered_image;
    filtered_image = convert::mat2qimg(detected_edges);


    //output
    return filtered_image;
}
QImage Filter::harrisCorner(int blockSize, double k, int tresh, int displayedPoints)
{
    makeGreyscale();
    cv::Mat dst,dst_norm,dst_norm_scaled;
    int apertureSize=3;

    cv::cornerHarris( greyscale, dst, blockSize, apertureSize, k, cv::BORDER_DEFAULT );

    cv::normalize( dst, dst_norm, 0, 255, cv::NORM_MINMAX, CV_32FC1, cv::Mat() );

    cv::convertScaleAbs( dst_norm, dst_norm_scaled );

    //Draw circles
    int drawenCircles = 0;
    for( int j = 0; j < dst_norm.rows ; j++ )
    { for( int i = 0; i < dst_norm.cols; i++ )
        {
        if( (int) dst_norm.at<float>(j,i) > tresh )//tresh
            {
                cv::circle( dst_norm_scaled, cv::Point( i, j ), 5,  cv::Scalar(0), 2, 8, 0 );
                drawenCircles++;
                if (drawenCircles>=displayedPoints)
                {
                    cv::cvtColor(dst_norm_scaled,dst_norm_scaled,CV_GRAY2RGB);
                    return  convert::mat2qimg(dst_norm_scaled);
                }
            }
        }
    }
    cv::cvtColor(dst_norm_scaled,dst_norm_scaled,CV_GRAY2RGB);
    return  convert::mat2qimg(dst_norm_scaled);

}
