#include<thread>

#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "convert.h"
#include "filter.h"
#include "objectrecog.h"
#include "database.h"

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

MainWindow::MainWindow(QWidget *parent) : QMainWindow(parent), ui(new Ui::MainWindow)
{
    //Init Visibility
    ui->setupUi(this);
    ui->edges_hide_1->setVisible(false);
    ui->edges_hide_2->setVisible(false);
    ui->surf_hide->setVisible(false);
    ui->widget_13->setVisible(false);

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


    // get Camera Picture
    if (QCameraInfo::availableCameras().count() > 0)
    {
        //Kamera auswählen
        QList<QCameraInfo> cameras = QCameraInfo::availableCameras();
        camera= new QCamera(cameras[0]);
        videoWidget = new QVideoWidget();
        videoWidget->setFixedSize(640,480);
        camera->setViewfinder(videoWidget);
        camera->start();
        QTimer::singleShot( 200, this, SLOT( getPicture_Intervall(  ) ) );
    }
}
MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::Page4_BD_Accept()
{
    db->addEntry();
    db->paintDatabase(ui->tableWidget);
}
void MainWindow::paintEvent(QPaintEvent *)
{
    //für Bildaufnahme so bald wie möglich
    if (ui->box_Screenrefresh->isChecked())
        setActivePic( videoWidget->grab().toImage() );
}
void MainWindow::getPicture_Intervall( )
{
    //für Bildaufnahme mit vorgegebenem Intervall
    if (ui->box_Screenrefresh->isChecked() == false )
        setActivePic( videoWidget->grab().toImage() );
    QTimer::singleShot(ui->fspAuswahl->value(), this, SLOT(getPicture_Intervall( )));
}
void MainWindow::changeLeftWidget(const int &num )
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
        break;
        default:
            ui->ContentWidget_Data->setVisible(false);
            ui->ContentWidget_Video->setVisible(true);
    }
}
void MainWindow::Page4_DB_getFilepath()
{
    QString fileName = QFileDialog::getOpenFileName(this,
        tr("Open Image"), "C:\Users\Konstantin\Pictures", tr("Image Files (*.png *.jpg *.bmp)"));

    ui->LineDB_pathToFile->setText(fileName);
    db->prepareEntry(fileName);
    db->prepareEntrySetName(ui->LineDB_EntryName->text());
    db->prepareEntrySetDescription(ui->LineDB_EntryDescription->toPlainText() );

    Page4_DB_drawEntry (db->getCurrentEntry());

}
void MainWindow::Page4_BD_Desc()
{
    db->prepareEntrySetDescription(ui->LineDB_EntryDescription->toPlainText() );
    ui->DBView_SingleEntry_Data_Text->setText(ui->LineDB_EntryDescription->toPlainText());

}
void MainWindow::Page4_BD_Name()
{
    db->prepareEntrySetName(ui->LineDB_EntryName->text());
    ui->DBView_SingleEntry_Data_Name->setText(ui->LineDB_EntryName->text());
}
void MainWindow::Page4_DB_drawEntry(Entry e)
{
    ui->DBView_SingleEntry_Picture->setPixmap(QPixmap::fromImage(e.icon));

    QString strTmp = QString::number(e.paraSURF.featurePoints.size() ) ;
    ui->DBView_SingleEntry_Data_SURFFP->setText("Anzahl Punkte: " + strTmp);

    strTmp = QString::number(e.paraSURF.descriptor.rows) +
            " x " + QString::number(e.paraSURF.descriptor.cols) ;
    ui->DBView_SingleEntry_Data_SURFDESC->setText("Matrixgroeße: " + strTmp);

    ui->DBView_SingleEntry_Data_Name->setText(e.name);

    ui->DBView_SingleEntry_Data_Text->setText(e.data);
}
void MainWindow::Page4_DB_drawEntry2(int row, int col)
{
    if (ui->Radio_Database->isChecked())
    {
        Page4_DB_drawEntry(db->getEntry(row));
        ui->deleteFileNumber->setText(QString::number(row));
    }
}
void MainWindow::Page4_DB_deleteDBEntry()
{
    int index = ui->deleteFileNumber->text().toInt();
    if (db->deleteDbEntry(index))
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

//"Main"
void MainWindow::setActivePic(QImage img)
{
    //Bildaufnahme
    ui->Label_Rotation_display->setText(QString::number(ui->Rotate_image->value()*90)+"°");
    img = convert::rotate_cw(img, ui->Rotate_image->value()*90);

    //Bildverarbeitung
    ///Filter
    /// Filter über das Bild gelegt
    /// ohne weitere Intragktion
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
            img = fil.HarrisCorner(
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
        static ObjectRecog* ore = new ObjectRecog();
        ore->setPicture(img);
        ore->calcGrayscale();
        ore->calcFeature();
        img = ore->getPic_feature();
    }
    //Bildausgabe
    MainWindow::aktivesBild = img;
    ui->label_image->setPixmap(QPixmap::fromImage(img));
}


