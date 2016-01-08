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

    void prepareEntry(QString filename);
    void prepareEntrySetName(QString Name);
    void prepareEntrySetDescription(QString Desc);

    void addEntry();
    bool deleteEntry(int index);

    void paintDatabase(QTableWidget *tableWidget);

    void saveToFile();

    std::vector < cv::Mat > getDescriptor();
    Entry getCurrentEntry();
    int getDbSize();
    Entry getEntry(int index);

private:
    bool entryPrepared_;

    std::vector < Entry > database_;

    Entry curEnt_;
    ObjectRecog *ore_ = new ObjectRecog();

};

#endif // DATABASE_H
