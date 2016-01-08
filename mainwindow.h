#ifndef MAINWINDOW_H
#define MAINWINDOW_H
#include "database.h"

#include <thread>

#include <QMainWindow>
#include <QtMultimedia>
#include <QCameraViewfinder>

#include <opencv2/opencv.hpp>
#include <debugform.h>

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
    Database *db = new Database();
    ObjectRecog* objre = new ObjectRecog();
    DebugForm debForm;
protected:
    void MainWindow::paintEvent(QPaintEvent *event) Q_DECL_OVERRIDE;

public slots:
    void setActivePic(QImage img);
    void getPicture_Intervall();
    void changeLeftWidget(const int &num = 0);

    void Debug_showWindow();

    void Page4_DB_getFilepath();
    void Page4_BD_Name();
    void Page4_BD_Desc();
    void Page4_BD_Accept();
    void Page4_DB_drawEntry2(int row, int col);
    void Page4_DB_drawEntry(Entry e);
    void Page4_DB_deleteDBEntry();
    void Page4_DB_saveToFile();
    void Page4_DB_loadFromFile();
    void Page4_DB_switchToNewEntry(bool b);

};

#endif // MAINWINDOW_H
