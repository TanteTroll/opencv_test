#include "objectrecog.h"
#include <stdio.h>
#include <iostream>
#include "opencv2/opencv.hpp"
#include "opencv2/xfeatures2d/nonfree.hpp"
#include "opencv2/features2d/features2d.hpp"
#include "convert.h"
#include "opencv2/cudafeatures2d.hpp"
#include <QElapsedTimer>
#include <QDebug>

bool sortBySize(const std::vector< ObjectRecog::goodMatchEntry > &lhs,
                const std::vector< ObjectRecog::goodMatchEntry > &rhs)
{ return    ( static_cast<double>(lhs.size())/static_cast<double>(lhs.at(0).object_size) )
            > static_cast<double>(rhs.size()/static_cast<double>(rhs.at(0).object_size)); }

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
    QElapsedTimer timer;

    /// Input-Test
    if (pic_video.isNull())
    {   qWarning("Input Picture not set!");
        return;
    }

    switch (FMode)
    case SURF:
    {
        using namespace cv::xfeatures2d;


        calcGrayscale();//Umwadlung Bild in Grayscale

        cv::Ptr<cv::FeatureDetector> detector = SURF::create(1200);

        timer.start(); //Timer für Berechnungsdauer

        detector->detectAndCompute(cv_grayscale, cv::noArray(), keypoints_scene, descriptors_scene);

        timeDetectCompute = timer.elapsed(); // Ende timer ohne Bild

        if (PMode == FPOINT || PMode == ALL)//Ausgabe des Video
        {
            cv::Mat draw;
            drawKeypoints(cv_grayscale, keypoints_scene, draw , 1, 4 );
            pic_feature = convert::mat2qimg(draw);
        }

    }
    timeDetectCompute_draw = timer.elapsed(); // Ende timer mit Bild
}
void ObjectRecog::searchInDB(std::vector < cv::Mat > object_descriptors)
{
    //Input tests
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
    goodMatches.clear(); //Löschen der alten Werte


    std::vector< std::vector< cv::DMatch > > matches; //alle gefundenen Übereinstimmungen
    std::vector< ObjectRecog::goodMatchEntry > goodMatchesTmp; //alle Übereinstimmungen mit bestimmtem Abstand
    QElapsedTimer timer;
    cv::FlannBasedMatcher matcher;
    //cv::Ptr< cv::cuda::DescriptorMatcher > matcher = cv::cuda::DescriptorMatcher::createBFMatcher(cv::NORM_L2);;


    //matching point für alle Objekte berechnen
    timer.start(); //timer für dauer der Berechnung
    for ( int i = 0; i < object_descriptors.size() ; i++)
    {
        //Input
        cv::Mat object_descriptor = object_descriptors.at(i); //auswhal des aktuellen Objektes
        if ( object_descriptors.empty() ) //test der Auswahl
        {
           cvError(0,"MatchFinder","OBJECT descriptor empty",__FILE__,__LINE__);
           continue;
        }


        //matching
        std::vector< cv::DMatch > match;
        matcher.match( object_descriptor, descriptors_scene, match ); //vergleich mit aktueller Szene
        matches.push_back(match);


        // gute Punkte auswählen
        for( int j = 0; j < object_descriptor.rows; j++ ) //alle matches auf Abstand kontrollieren
        {
            if( match[j].distance < .01*min_dist )
            {
                ObjectRecog::goodMatchEntry gmEntry;
                gmEntry.macht_point = match[j];
                gmEntry.object_index = i;
                gmEntry.object_size = object_descriptor.rows;
                goodMatchesTmp.push_back( gmEntry );
            }
        }


        // werte speichern
        if (!goodMatchesTmp.empty())
        {
            goodMatches.push_back(goodMatchesTmp); //Speichern im Vektor
            goodMatchesTmp.clear();
        }
    }
    timeMatching = timer.elapsed(); //stop den Tiemr


    // matching points nach qualität sortieren
    std::sort(goodMatches.begin(), goodMatches.end() , sortBySize ); //sortieren nach Qualitär der Funde
    timeMatching_sort = timer.elapsed();
    emit sigGooMatches(goodMatches);


    //Ausabe video
    if (PMode == MATCHES || PMode == ALL)
    {
       //TODO
    }
    timeMatching_paint = timer.elapsed();

}
int ObjectRecog::getPositionOfGoodMatch(int index, std::vector<cv::DMatch> *Hits)
{
    if (goodMatches.empty()) return -1;

    std::vector< ObjectRecog::goodMatchEntry > match;
    match = goodMatches.at(index);

    Hits->empty();
    for ( int i = 0 ; i < match.size() ; i++)
    {
        cv::DMatch matchTmp = match.at(i).macht_point;
        Hits->push_back(matchTmp);
    }

    return match.at(0).object_index;
}
int ObjectRecog::getPositionOfGoodMatch(int index)
{
    if (goodMatches.empty()) return -1;
    std::vector< ObjectRecog::goodMatchEntry > match;

    match = goodMatches.at(index);

    return match.at(0).object_index;
}

