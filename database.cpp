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

void Database::loadDB()
{
    //load Database to gs
    cv::FileStorage fs;
    try {fs.open("database.xml", cv::FileStorage::READ);}
    catch(...) {qDebug("Etwas stimmt mit der xml nicht!");}


    //delete old Database
    database_.clear();


    // read Number of Entries
    int dbSize = (int) fs["databaseSize"];


    //read the rest of the database
    for ( int i = 0 ; i < dbSize ; i++)
    {
        //Initiate Nodes
        std::string nodename = "Entry_"+std::to_string(i);
        cv::FileNode entryNode = fs[nodename];
        cv::FileNode bilderNode = entryNode["Bilder"];
        cv::FileNode surfNode = entryNode["SURFparameter"];

        Entry entTmp;
        //read Strings
        entTmp.name = QString::fromStdString(entryNode["Name"]);
        entTmp.data = QString::fromStdString(entryNode["Description"]);

        //read vectors
        surfNode["FeaturePoints"] >> entTmp.paraSURF.featurePoints;
        surfNode["Descriptor"] >> entTmp.paraSURF.descriptor;

        //read Pictures
        bilderNode["Bild"] >> entTmp.image;

        //generate the Icon-Picture from full picture
        // TODO read Icon directly
        // - convert not working properly
        QImage img = convert::mat2qimg(entTmp.image);
        if ( img.width() > img.height() ) entTmp.icon = img.scaledToWidth(200);
        else entTmp.icon = img.scaledToHeight(200);


        //save Entry in database
        database_.push_back(entTmp);
    }
}

void Database::prepareEntrySetName(QString Name)
{
    curEnt_.name = Name;
}
void Database::prepareEntrySetDescription(QString Desc)
{
    curEnt_.data = Desc;
}
void Database::prepareEntry(QString filename)
{
    // read Image from File
    QImageReader reader(filename);
    QImage img;
    reader.autoDetectImageFormat();
    if (!reader.read(&img))
    {
        qWarning("Invalid Filename !");
        return;
    }

    //set Picture
    curEnt_.image = convert::qimg2mat(img);


    // generate Icon from Picture
    if ( img.width() > img.height() ) curEnt_.icon = img.scaledToWidth(200);
    else curEnt_.icon = img.scaledToHeight(200);


    // calculate Feature Points and Descriptors
    ore_->setPicture(img);
    ore_->setPaintMode(NONE);
    ore_->setFeatureMode(SURF);
    ore_->calcGrayscale();
    ore_->calcFeature();


    //set Feature Points and Descriptors
    curEnt_.paraSURF.featurePoints = ore_->getKeypoints();
    curEnt_.paraSURF.descriptor = ore_->getDescriptor();


    //allow submit Button
    entryPrepared_ = true;
}

void Database::addEntry()
{
    if (!entryPrepared_)
    {
        qWarning ("Entry not complete yet! ");
        return;
    }
    entryPrepared_ = false;
    database_.push_back(curEnt_);
}
bool Database::deleteEntry(int index)
{
    if (index>database_.size()) return false;
    database_.erase(database_.begin() + index);
    return true;
}

void Database::paintDatabase(QTableWidget *tableWidget)
{
    //Init Table
    tableWidget->setRowCount(database_.size());
    tableWidget->setColumnCount(4);
    QStringList Headers;
    Headers << "Name" << "Beschreibung" << "Featurepunkt" << "Descriptor" ;
    tableWidget->setHorizontalHeaderLabels(Headers);


    //print Entries
    for (int column =0 ; column < 4 ; column++){
        for (int row = 0 ; row < database_.size() ; row++ )
        {
            //read Data
            Entry ent_tmp=database_.at(row);


            // Init Field
            QTableWidgetItem *newItem = new QTableWidgetItem();
            newItem->setFlags(Qt::ItemIsSelectable|Qt::ItemIsEnabled);


            //Field content
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


            // Add Field to Table
            tableWidget->setItem(row, column, newItem);
        }
    }
    tableWidget->resizeColumnsToContents();
}

void Database::saveToFile()
{
    // open File
    cv::FileStorage fs("database.xml", cv::FileStorage::WRITE);


    // write Header
    int tmp = database_.size();
    fs << "databaseSize" << tmp; //Warum geht das nicht direkt und ohne tmp?


    //write Data
    for ( int i = 0 ; i < database_.size() ; i++)
    {
        // Read Data to write
        Entry entTmp = database_.at( i );
        cv::Mat cvMatTmp = convert::qimg2mat(entTmp.icon);


        // Write Data
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
    fs.release();
}

Entry Database::getCurrentEntry()
{
    return curEnt_;
}
int Database::getDbSize()
{
    return database_.size();
}
std::vector<cv::Mat> Database::getDescriptor()
{
    std::vector <cv::Mat> vec;
    if ( database_.size() <= 0)
    {
        qWarning("keine Einträge in der Datenbank!");
        throw 20;
    }
    else
    {
        Entry a;
        for ( int i = 0 ; i < database_.size() ; i++)
        {

            a = database_.at(i);
            vec.push_back( a.paraSURF.descriptor );
        }
    }
    return vec;
}
Entry Database::getEntry(int index)
{
    return database_.at(index);
}


