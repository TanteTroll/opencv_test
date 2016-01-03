#ifndef DATABASE_H
#define DATABASE_H
#include "opencv2/opencv.hpp"
#include "objectrecog.h"

#include <QString>
#include <QImage>
#include <QTableWidget>

struct  SURFparameter {
    std::vector<cv::KeyPoint> featurePoints;
    cv::Mat descriptor;
};

struct Entry{
    cv::Mat image;
    QImage icon;
    QString name = "EntryName";
    QString data = "Beschreibung";
    SURFparameter paraSURF;

};

class Database
{

public:
    Database();
    void loadDB();
    void addEntry();
    void prepareEntry(QString filename);
    void prepareEntrySetName(QString Name);
    void prepareEntrySetDescription(QString Desc);

    bool deleteDbEntry(int index);

    void paintDatabase(QTableWidget *tableWidget);

    Entry getCurrentEntry();
    Entry getEntry(int index);

    void saveToFile();

private:
    bool entryPrepared;

    std::vector < Entry > database;

    Entry curEnt;
    ObjectRecog* ore = new ObjectRecog();

};

#endif // DATABASE_H
