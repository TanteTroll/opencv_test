#include "objectrecog.h"
#include <stdio.h>
#include <iostream>
#include "opencv2/opencv.hpp"
#include "opencv2/xfeatures2d/nonfree.hpp"
#include "convert.h"


ObjectRecog::ObjectRecog()
{

}
void ObjectRecog::setPicture(QImage img)
{
    pic_video = img;
    cv_video = convert::qimg2mat(img);
}
void ObjectRecog::setPicture(cv::Mat img)
{
    cv_video = img;
    pic_video = convert::mat2qimg(img);
}
void ObjectRecog::setFeatureMode(FeatureMode Mode)
{
    FMode = Mode;
}
void ObjectRecog::setPaintMode(PaintMode Mode)
{
    PMode = Mode;
}
void ObjectRecog::setMinHessian(int inputMinHessian)
{
    if (inputMinHessian > 0)
    {
        qWarning("Invalid MinHessian");
        return;
    }
    minHessian = inputMinHessian;
}
QImage ObjectRecog::getPic_feature()
{
    return pic_feature;
}
QImage ObjectRecog::getPic_video()
{
    return pic_video;
}
std::vector<cv::KeyPoint> ObjectRecog::getKeypoints()
{
    return keypoints_scene;
}
cv::Mat ObjectRecog::getDescriptor()
{
    return descriptors_scene;
}

void ObjectRecog::calcGrayscale()
{
    cv::cvtColor(cv_video,cv_grayscale,CV_BGR2GRAY);
}

cv::Mat ObjectRecog::calcGrayscale(cv::Mat img)
{
    cv::Mat grayscale;
    cv::cvtColor(img,grayscale,CV_BGR2GRAY);
    return grayscale;
}

void ObjectRecog::calcFeature()
{
    /// Input-Test
    if (pic_video.isNull())
    {   qWarning("Input Picture not set!");
        return;
    }


    switch (FMode)
        case SURF:
    {
        using namespace cv::xfeatures2d;
        calcGrayscale();
        cv::Ptr<cv::FeatureDetector> detector = SURF::create(minHessian);
        detector->detect(cv_grayscale,keypoints_scene);
        if (PMode == VIDEO || PMode == ALL)
        {
            cv::Mat draw;
            drawKeypoints(cv_grayscale, keypoints_scene, draw , 1, 4 );
            pic_feature = convert::mat2qimg(draw);
        }
        detector->compute(cv_grayscale,keypoints_scene,descriptors_scene);
    }
}
