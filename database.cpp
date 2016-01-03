#include "database.h"
#include "convert.h"
#include "objectrecog.h"

#include <QImageReader>
#include <QDebug>
#include <QTableWidget>
#include <QStringList>
#include <QString>
#include <QXmlStreamWriter>
#include <QFile>

#include <opencv2/opencv.hpp>


Database::Database()
{

}
Entry Database::getCurrentEntry()
{
    return curEnt;
}
void Database::prepareEntrySetName(QString Name)
{
    curEnt.name = Name;
}
void Database::prepareEntrySetDescription(QString Desc)
{
    curEnt.data = Desc;
}
Entry Database::getEntry(int index)
{
    return database.at(index);
}
bool Database::deleteDbEntry(int index)
{
    if (index>database.size()) return false;
    database.erase(database.begin() + index);
    return true;
}

void Database::prepareEntry(QString filename)
{
    QImageReader reader(filename);
    QImage img;
    reader.autoDetectImageFormat();
    if (reader.read(&img))
    {
        if ( img.width() > img.height() ) curEnt.icon = img.scaledToWidth(200);
        else curEnt.icon = img.scaledToHeight(200);
    }
    else
    {
        qWarning("Invalid Filename !");
        return;
    }
    curEnt.image = convert::qimg2mat(img);

    ore->setPicture(img);
    ore->setPaintMode(NONE);
    ore->setFeatureMode(SURF);
    ore->calcGrayscale();
    ore->calcFeature();

    curEnt.paraSURF.featurePoints = ore->getKeypoints();
    curEnt.paraSURF.descriptor = ore->getDescriptor();
    entryPrepared = true;
}
void Database::addEntry()
{
    if (!entryPrepared)
    {
        qWarning ("Something went wrong !");
        return;
    }
    entryPrepared = false;
    database.push_back(curEnt);
}
void Database::paintDatabase(QTableWidget *tableWidget)
{
    tableWidget->setRowCount(database.size());
    tableWidget->setColumnCount(4);

    QStringList Headers;
    Headers << "Name" << "Beschreibung" << "Featurepunkt" << "Descriptor" ;
    tableWidget->setHorizontalHeaderLabels(Headers);

    for (int column =0 ; column < 4 ; column++){
        for (int row = 0 ; row < database.size() ; row++ )
        {
            Entry ent_tmp=database.at(row);
            QTableWidgetItem *newItem = new QTableWidgetItem();
            newItem->setFlags(Qt::ItemIsSelectable|Qt::ItemIsEnabled);

            switch (column)
            {
            case 0:newItem->setText(ent_tmp.name);break;
            case 1:newItem->setText(ent_tmp.data);break;
            case 2:newItem->setText("Anzahl Punkte: " +
                                    QString::number(ent_tmp.paraSURF.featurePoints.size()));
            break;
            case 3:newItem->setText("Aufloesung: " +
                                    QString::number(ent_tmp.paraSURF.descriptor.rows) +
                                    " x " +
                                    QString::number(ent_tmp.paraSURF.descriptor.cols) );
            break;
            default:;
            };


            tableWidget->setItem(row, column, newItem);
        }
    }
    tableWidget->resizeColumnsToContents();
}
void Database::saveToFile()
{
    cv::FileStorage fs("database.xml", cv::FileStorage::WRITE);
    Entry entTmp;
    cv::Mat cvMatTmp;
    int tmp = database.size();
    //fs << "Database" << "{";
    fs << "databaseSize" << tmp; //Warum geht das nicht direkt?
    for ( int i = 0 ; i < database.size() ; i++)
    {
        entTmp = database.at( i );
        cvMatTmp = convert::qimg2mat(entTmp.icon);

        std::string nodename = "Entry_"+std::to_string(i);
        fs << nodename << "{";
            fs << "Name" << entTmp.name.toLocal8Bit().constData();
            fs << "Description" << entTmp.data.toLocal8Bit().constData();

            fs << "Bilder" << "{";
                fs << "Bild" << entTmp.image;
                fs << "Icon" << cvMatTmp;
            fs << "}";

            fs << "SURFparameter" << "{";
                fs << "FeaturePoints" << entTmp.paraSURF.featurePoints;
                fs << "Descriptor" << entTmp.paraSURF.descriptor;
            fs << "}";
        fs << "}";
    }
    //fs << "}";
    fs.release();
}
void Database::loadDB()
{
    try{
    cv::FileStorage fs;
    fs.open("database.xml", cv::FileStorage::READ);

    database.clear();

    int dbSize;
    cv::Mat iconAsCv;
    Entry entTmp;
    QImage img;

    dbSize = (int) fs["databaseSize"];
    for ( int i = 0 ; i < dbSize ; i++)
    {
        std::string nodename = "Entry_"+std::to_string(i);

        cv::FileNode entryNode = fs[nodename];
        cv::FileNode bilderNode = entryNode["Bilder"];
        cv::FileNode surfNode = entryNode["SURFparameter"];

        entTmp.name = QString::fromStdString(entryNode["Name"]);
        entTmp.data = QString::fromStdString(entryNode["Description"]);

        bilderNode["Bild"] >> entTmp.image;

        img = convert::mat2qimg(entTmp.image);

        if ( img.width() > img.height() ) entTmp.icon = img.scaledToWidth(200);
        else entTmp.icon = img.scaledToHeight(200);

        surfNode["FeaturePoints"] >> entTmp.paraSURF.featurePoints;
        surfNode["Descriptor"] >> entTmp.paraSURF.descriptor;

        database.push_back(entTmp);
    }}
    catch(...)
    {
        qDebug("Etwas stimmt mit der xml nicht!");
    }
}
