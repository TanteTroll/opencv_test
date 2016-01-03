#ifndef OBJECTRECOG_H
#define OBJECTRECOG_H

#include <QObject>
#include <QImage>
#include <vector>
#include <stdio.h>
#include <iostream>
#include "opencv2/opencv.hpp"

enum FeatureMode {SURF};
enum PaintMode {ALL, NONE, VIDEO, FPOINT};



class ObjectRecog
{
    //Q_OBJECT

public:
    explicit ObjectRecog();

    void setPicture(QImage pic);
    void setPicture(cv::Mat img);
    void setPaintMode(PaintMode Mode);
    void setFeatureMode(FeatureMode Mode);
    void setMinHessian(int minH);

    QImage getPic_video();
    QImage getPic_feature();
    std::vector<cv::KeyPoint> getKeypoints();
    cv::Mat getDescriptor();

    void calcGrayscale();
    cv::Mat calcGrayscale(cv::Mat img);
    void calcFeature();



private:
    int minHessian = 400;
    FeatureMode FMode = SURF;
    PaintMode PMode = ALL;

    cv::Mat cv_video;
    cv::Mat cv_grayscale;
    cv::Mat cv_feature;
    QImage pic_video;
    QImage pic_feature;

    std::vector<cv::KeyPoint> keypoints_object;
    std::vector<cv::KeyPoint> keypoints_scene;

    cv::Mat descriptors_object, descriptors_scene;




signals:

public slots:

};

#endif // OBJECTRECOG_H
