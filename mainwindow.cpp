#include<thread>

#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "convert.h"
#include "filter.h"

#include <opencv2/opencv.hpp>
#include <QImage>
#include <QTimer>
#include <QtMultimedia>
#include <QCamera>
#include <QList>
#include <QCameraViewfinder>
#include <QVideoWidget>

MainWindow::MainWindow(QWidget *parent) : QMainWindow(parent), ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    ui->edges_hide->setVisible(false);
    ui->edges_hide_2->setVisible(false);
    // Kamerabild
    if (QCameraInfo::availableCameras().count() > 0)
    {
        //Kamera auswählen
         QList<QCameraInfo> cameras = QCameraInfo::availableCameras();
         camera= new QCamera(cameras[0]);
         //thumbnail
         viewfinder= new QCameraViewfinder(ui->page);
         viewfinder->show();
         //camera->setViewfinder(viewfinder);
         //großes Bild über regelmäßigen VideoCapture
         imageCapture = new QCameraImageCapture(camera);
         connect(imageCapture, SIGNAL(imageCaptured(int,QImage)), this, SLOT(setaktivesBild(int,QImage)));


         videoWidget = new QVideoWidget();
         videoWidget->setFixedSize(640,480);
         camera->setViewfinder(videoWidget);

        QTimer::singleShot( 200, this, SLOT( mytimerslot(  ) ) );
        camera->start();
    }
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::paintEvent(QPaintEvent *)
{
    //Periodische aufnahme des Bildes
    if (ui->box_Screenrefresh->isChecked())
        setaktivesBild( videoWidget->grab().toImage() );
}
void MainWindow::mytimerslot( )
{
    if (ui->box_Screenrefresh->isChecked() == false )
        setaktivesBild( videoWidget->grab().toImage() );
    QTimer::singleShot(ui->fspAuswahl->value(), this, SLOT(mytimerslot( )));
}

void MainWindow::setaktivesBild(QImage img)
{
    //Bild bearbeiten
    ui->Label_Rotation_display->setText(QString::number(ui->Rotate_image->value()*90)+"°");
    img = convert::rotate_cw(img, ui->Rotate_image->value()*90);

    //Bildanalyse
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

    //Bildausgabe
    MainWindow::aktivesBild = img;
    ui->label_image->setPixmap(QPixmap::fromImage(img));

}


