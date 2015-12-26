#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <thread>
#include <QMainWindow>

#include <opencv2/opencv.hpp>
#include <QtMultimedia>
#include <QCameraViewfinder>

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT


public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();
    QImage aktivesBild;


private:
    Ui::MainWindow *ui;
    QCamera *camera;
    QCameraViewfinder *viewfinder;
    QCameraImageCapture *imageCapture;
    QVideoWidget *videoWidget;


protected:
    void MainWindow::paintEvent(QPaintEvent *event) Q_DECL_OVERRIDE;

public slots:
    void setaktivesBild(QImage img);
    void mytimerslot();

};

#endif // MAINWINDOW_H
