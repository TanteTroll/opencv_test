#include<thread>

#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "convert.h"
#include "filter.h"
#include "objectrecog.h"
#include "database.h"
#include "ui_debugform.h"
#include "debugform.h"

#include <opencv2/opencv.hpp>
#include <QImage>
#include <QTimer>
#include <QtMultimedia>
#include <QCamera>
#include <QList>
#include <QCameraViewfinder>
#include <QVideoWidget>
#include <QFileDialog>
#include <QString>
#include <QElapsedTimer>
#include <QDebug>

/// <summary>
/// Initialisiert das GUI
/// </summary>
MainWindow::MainWindow(QWidget *parent) : QMainWindow(parent), ui(new Ui::MainWindow)
{
    //Init Database
    db->loadDB();
    //Init Visibility
    ui->setupUi(this);
    ui->edges_hide_1->setVisible(false);
    ui->edges_hide_2->setVisible(false);
    ui->groupBox_12->setVisible(false);

    this->changeLeftWidget(ui->toolBox->currentIndex());

    //Connect Buttons
    connect(ui->toolBox,&QToolBox::currentChanged,
            this, &MainWindow::changeLeftWidget );
    connect(ui->ButtonDB_FileDialog,&QPushButton::clicked,
            this, &MainWindow::Page4_DB_getFilepath );
    connect(ui->LineDB_EntryName,&QLineEdit::textChanged,
            this, &MainWindow::Page4_BD_Name );
    connect(ui->LineDB_EntryDescription,&QPlainTextEdit::textChanged,
            this, &MainWindow::Page4_BD_Desc );
    connect(ui->LineDB_accept, &QPushButton::clicked,
            this, &MainWindow::Page4_BD_Accept );
    connect(ui->tableWidget, &QTableWidget::cellClicked,
            this, &MainWindow::Page4_DB_drawEntry2 );
    connect(ui->ButtonDB_Delete,&QPushButton::clicked,
            this, &MainWindow::Page4_DB_deleteDBEntry );
    connect(ui->ButtonDB_SaveToFile,&QPushButton::clicked,
            this, &MainWindow::Page4_DB_saveToFile);
    connect(ui->ButtonDB_LoadFromFile,&QPushButton::clicked,
            this, &MainWindow::Page4_DB_loadFromFile);
    connect(ui->radio_AddEntry, &QRadioButton::toggled,
            this, &MainWindow::Page4_DB_switchToNewEntry);
    connect(ui->pushButton_showDebugUi, &QRadioButton::clicked,
            this, &MainWindow::Debug_showWindow);




    // Init Kamera
    if (QCameraInfo::availableCameras().count() > 0)
    {
        //Kamera auswählen
        QList<QCameraInfo> cameras = QCameraInfo::availableCameras();
        camera= new QCamera(cameras[0]);
        videoWidget = new QVideoWidget();
        videoWidget->setFixedSize(640,480);
        camera->setViewfinder(videoWidget);
        camera->start();

        //Timer für Kameraaufnahme
        QTimer::singleShot( 200, this, SLOT( getPicture_Intervall(  ) ) );
    }


}

MainWindow::~MainWindow()
{
    delete ui;
}

/// <summary>
/// für Bildaufnahme so bald wie möglich
/// </summary>
void MainWindow::paintEvent(QPaintEvent *)
{
    if (ui->box_Screenrefresh->isChecked())
        setActivePic( videoWidget->grab().toImage() );
}

void MainWindow::getPicture_Intervall( )
//für Bildaufnahme mit vorgegebenem Intervall
{
    if (ui->box_Screenrefresh->isChecked() == false )
        setActivePic( videoWidget->grab().toImage() );
    QTimer::singleShot(ui->fspAuswahl->value(), this, SLOT(getPicture_Intervall( )));
}
void MainWindow::Page4_DB_switchToNewEntry(bool b)
{

    if(b)
    {
        this->Page4_DB_drawEntry2(-1,1);
    }
}

void MainWindow::changeLeftWidget(const int &num )
// für Hauptfenster abhängig von ausgewählter Seite
{
    switch (num)
    {
        case 0:
        case 1:
        case 2:
            ui->ContentWidget_Data->setVisible(false);
            ui->ContentWidget_Video->setVisible(true);
        break;
        case 3:
            ui->ContentWidget_Data->setVisible(true);
            ui->ContentWidget_Video->setVisible(false);
            db->paintDatabase(ui->tableWidget);
        break;
        default:
            ui->ContentWidget_Data->setVisible(false);
            ui->ContentWidget_Video->setVisible(true);
    }
}

void MainWindow::Page4_DB_getFilepath()
//Bild für DB auswählen und an datenbank.h schicken
{
    QString fileName = QFileDialog::getOpenFileName(this,
        tr("Open Image"), "C:\\Users\\Konstantin\\Pictures", tr("Image Files (*.png *.jpg *.bmp)"));

    //Ui anpassen
    ui->LineDB_pathToFile->setText(fileName);
    db->prepareEntry(fileName);
    db->prepareEntrySetName(ui->LineDB_EntryName->text());
    db->prepareEntrySetDescription(ui->LineDB_EntryDescription->toPlainText() );
    Page4_DB_drawEntry (db->getCurrentEntry());
}

void MainWindow::Page4_BD_Accept()
//Datenbankeintrag bestätigen
{

    db->addEntry();
    db->paintDatabase(ui->tableWidget);
}

void MainWindow::Page4_BD_Desc()
//Text: "Beschreibung" (des Eintrages)
{
    db->prepareEntrySetDescription(ui->LineDB_EntryDescription->toPlainText() );
    ui->DBView_SingleEntry_Data_Text->setText(ui->LineDB_EntryDescription->toPlainText());
}

void MainWindow::Page4_BD_Name()
//Text: "Name" (des Eintrages)
{
    db->prepareEntrySetName(ui->LineDB_EntryName->text());
    ui->DBView_SingleEntry_Data_Name->setText(ui->LineDB_EntryName->text());
}

void MainWindow::Page4_DB_drawEntry(Entry e)
//einzelnen Eintrag über der Datenbank anzeigen
{
    //Bild
    ui->DBView_SingleEntry_Picture->setPixmap(QPixmap::fromImage(e.icon));
    // feature Point
    QString strTmp = QString::number(e.paraSURF.featurePoints.size() ) ;
    ui->DBView_SingleEntry_Data_SURFFP->setText("Anzahl Punkte: " + strTmp);
    //descriptor
    strTmp = QString::number(e.paraSURF.descriptor.rows) +
            " x " + QString::number(e.paraSURF.descriptor.cols) ;
    ui->DBView_SingleEntry_Data_SURFDESC->setText("Matrixgroeße: " + strTmp);
    //Name
    ui->DBView_SingleEntry_Data_Name->setText(e.name);
    //Beschreibung
    ui->DBView_SingleEntry_Data_Text->setText(e.data);
}

void MainWindow::Page4_DB_drawEntry2(int row, int col)
//einzelnen Eintrag aus Datenbank anzeigen
{
    if (row < 0)
    {
        //draw empty Entry
        Entry a;
        this->Page4_DB_drawEntry(a);
    }
    else if (ui->Radio_Database->isChecked())
    {
        //draw selected Entry
        Page4_DB_drawEntry(db->getEntry(row));
        ui->deleteFileNumber->setText(QString::number(row));
    }
}

void MainWindow::Page4_DB_deleteDBEntry()
// Datenbankeintrag löschen, (atm. :index aus ui)
{
    int index = ui->deleteFileNumber->text().toInt();
    if ( index >= 0 && index < db->getDbSize())
        if (db->deleteEntry(index))
            ui->tableWidget->removeRow(index);
}

void MainWindow::Page4_DB_saveToFile()
{
    db->saveToFile();
    db->paintDatabase(ui->tableWidget);
}
void MainWindow::Page4_DB_loadFromFile()
{
    db->loadDB();
    db->paintDatabase(ui->tableWidget);
}
void MainWindow::setActivePic(QImage img)
{
    QElapsedTimer timer;
    timer.start();
    //Bildaufnahme
    ui->Label_Rotation_display->setText(QString::number(ui->Rotate_image->value()*90)+"°");
    img = convert::rotate_cw(img, ui->Rotate_image->value()*90);

    //Bildverarbeitung

    if (ui->page_filter->isVisible()  )
    {
        static Filter fil(img);
        if(ui->radio_edegs->isChecked())
        {
            fil.setImg(img);
            img = fil.edgeDetection(ui->Edges_Threshold->value(),ui->edges_ratio->value());
        }

        if(ui->radio_harris->isChecked())
        {
            fil.setImg(img);
            img = fil.harrisCorner(
                        ui->Harris_blovkSize->value(),
                        (double)ui->Harris_k->value()/100.0,
                        ui->Harris_treshhold->value(),
                        ui->Harris_NumberPoints->value()
                        );
        }
    }
    ///Objekterkennung
    /// mit Datenbankabgleich

    if (ui->page_objektrec->isVisible()  )
    {
        objre->setPicture(img);
        objre->setMinDist(ui->ObjectRec_minDist->value());
        objre->calcGrayscale();

        objre->calcFeature();
        objre->searchInDB(db->getDescriptor());
        img = objre->getPic_feature();

        std::vector<cv::DMatch> *Hits = new std::vector<cv::DMatch>;
        int posOfMatch = objre->getPositionOfGoodMatch(0, Hits);
        if ( posOfMatch < 0 )
        {
            qDebug ("nothing found");
            ui->ObjectRec_Disc->setText("");
            ui->ObjectRec_Name->setText("");
            ui->ObjectRec_NumMatch->setText("Anzahl Übereinstimmungen: 0");
            ui->ObjectRec_pic->setVisible(false);
        }
        else
        {

            Entry ent;
            ent = db->getEntry(posOfMatch);
            ui->ObjectRec_Name->setText(ent.name);
            ui->ObjectRec_Disc->setText(ent.data);
            ui->ObjectRec_pic->setVisible(true);
            ui->ObjectRec_pic->setPixmap(QPixmap::fromImage(ent.icon).scaledToWidth(100));
            ui->ObjectRec_NumMatch->setText("Anzahl Übereinstimmungen: " + QString::number(Hits->size()));
            ui->ObjectRec_timeFP_label->setText(QString::number(objre->timeDetectCompute));
            ui->ObjectRec_timeFPPaint_label->setText(QString::number(objre->timeDetectCompute_draw));
            ui->ObjectRec_timeMatch->setText(QString::number(objre->timeMatching));
            ui->ObjectRec_timeMatchSort->setText(QString::number(objre->timeMatching_sort));
        }

    }
    //Bildausgabe
    MainWindow::aktivesBild = img;
    ui->label_image->setPixmap(QPixmap::fromImage(img));
}
void MainWindow::Debug_showWindow()
{
    debForm.show();
    connect(objre, &ObjectRecog::sigGooMatches,
            &debForm, &DebugForm::printGoodMatches);
}


