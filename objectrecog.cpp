#include "objectrecog.h"
#include <stdio.h>
#include <iostream>
#include "opencv2/opencv.hpp"
#include "opencv2/xfeatures2d/nonfree.hpp"
#include "opencv2/features2d/features2d.hpp"
#include "convert.h"

bool sortBySize(const std::vector< std::pair <int , cv::DMatch> > &lhs,
                const std::vector< std::pair <int , cv::DMatch> > &rhs)
{ return lhs.size() < rhs.size(); }

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
void ObjectRecog::setMinDist(double input)
{
    min_dist = input;
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

        cv::Ptr<cv::FeatureDetector> detector = SURF::create();
        detector->detectAndCompute(cv_grayscale, cv::noArray(), keypoints_scene, descriptors_scene);

        if (PMode == FPOINT || PMode == ALL)
        {
            cv::Mat draw;
            drawKeypoints(cv_grayscale, keypoints_scene, draw , 1, 4 );
            pic_feature = convert::mat2qimg(draw);
        }

    }
}
void ObjectRecog::searchInDB(std::vector < cv::Mat > object_descriptors)
{
    if ( object_descriptors.empty() )
    {
       qWarning("OBJECT descriptor empty");
       return;
    }
    if ( descriptors_scene.empty() )
    {
       qWarning("SZENE descriptor empty");
       return;
    }
    goodMatches.clear();

    cv::FlannBasedMatcher matcher;
    std::vector< std::vector< cv::DMatch > > matches;
    std::vector< std::pair <int , cv::DMatch> > goodMatchesTmp;

    for ( int i = 0; i < object_descriptors.size() ; i++)
    {
        cv::Mat object_descriptor = object_descriptors.at(i);
        if ( object_descriptors.empty() )
        {
           cvError(0,"MatchFinder","OBJECT descriptor empty",__FILE__,__LINE__);
           continue;
        }
        std::vector< cv::DMatch > match;

        matcher.match( object_descriptor, descriptors_scene, match );
        matches.push_back(match);

        for( int j = 0; j < object_descriptors.at(i).rows; j++ )
        {

            if( matches[i][j].distance < .03*min_dist )
            {
                std::pair <int , cv::DMatch> pairEntry;
                pairEntry.first = i;
                pairEntry.second = matches[i][j];
                goodMatchesTmp.push_back( pairEntry);
            }
        }
        if (!goodMatchesTmp.empty())
            goodMatches.push_back(goodMatchesTmp);
    }

    std::sort(goodMatches.begin(), goodMatches.end() , sortBySize );

    if (PMode == MATCHES || PMode == ALL)
    {
       //TODO
    }

}
int ObjectRecog::getPositionOfGoodMatch(int index, std::vector<cv::DMatch> *Hits)
{
    if (goodMatches.empty()) return -1;

    std::vector< std::pair <int , cv::DMatch> > match;
    match = goodMatches.at(index);

    Hits->empty();
    for ( int i = 0 ; i < match.size() ; i++)
    {
        cv::DMatch matchTmp = match.at(i).second;
        Hits->push_back(matchTmp);
    }

    return match.at(0).first;
}
int ObjectRecog::getPositionOfGoodMatch(int index)
{
    if (goodMatches.empty()) return -1;
    std::vector< std::pair <int , cv::DMatch> > match;

    match = goodMatches.at(index);

    return match.at(0).first;
}
