#ifndef OBJECTRECOG_H
#define OBJECTRECOG_H

#include <QObject>
#include <QImage>
#include <vector>
#include <stdio.h>
#include <iostream>
#include "opencv2/opencv.hpp"

enum FeatureMode {SURF};
enum PaintMode {ALL, NONE, FPOINT, MATCHES};


class ObjectRecog: public QObject
{
    Q_OBJECT

public:
    struct goodMatchEntry
    {
        int object_index;
        cv::DMatch macht_point;
        int object_size;
    };

    double timeDetectCompute;
    double timeDetectCompute_draw;
    double timeMatching;
    double timeMatching_sort;
    double timeMatching_paint;

    explicit ObjectRecog();

    void setPicture(QImage pic);
    void setPicture(cv::Mat img);
    void setPaintMode(PaintMode Mode);
    void setFeatureMode(FeatureMode Mode);
    void setMinDist(double input);

    int getPositionOfGoodMatch(int index, std::vector<cv::DMatch> *Hits);
    int getPositionOfGoodMatch(int index);
    QImage getPic_video();
    QImage getPic_feature();


    std::vector<cv::KeyPoint> getKeypoints();
    cv::Mat getDescriptor();

    void calcGrayscale();
    cv::Mat calcGrayscale(cv::Mat img);
    void calcFeature();
    void searchInDB(std::vector < cv::Mat > object_descriptors );
    std::vector< std::vector< ObjectRecog::goodMatchEntry > > goodMatches;

private:
    FeatureMode FMode = SURF;
    PaintMode PMode = ALL;

    cv::Mat cv_video;
    cv::Mat cv_grayscale;
    cv::Mat cv_feature;
    QImage pic_video;
    QImage pic_feature;
    double min_dist = 10;

    std::vector<cv::KeyPoint> keypoints_scene;
    cv::Mat descriptors_scene;


signals:
    void sigGooMatches(std::vector< std::vector< ObjectRecog::goodMatchEntry > > vec);


public slots:

};

#endif // OBJECTRECOG_H
