#-------------------------------------------------
#
# Project created by QtCreator 2015-12-22T23:21:41
#
#-------------------------------------------------

QT       += core gui \
            multimedia \
            multimediawidgets \


greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = opencv_test
TEMPLATE = app


SOURCES += main.cpp\
        mainwindow.cpp \
    convert.cpp \
    filter.cpp \
    objectrecog.cpp \
    database.cpp \
    debugform.cpp

HEADERS  += mainwindow.h \
    convert.h \
    filter.h \
    objectrecog.h \
    database.h \
    debugform.h

FORMS    += mainwindow.ui \
    debugform.ui



RESOURCES += \
    ressource.qrc



win32:CONFIG(release, debug|release): LIBS += -L$$PWD/../../../../opencv/build_new/lib/ -lopencv_core310 \
-lopencv_features2d310 -lopencv_highgui310 -lopencv_xfeatures2d310 -lopencv_imgproc310 -lopencv_flann310 \
-lopencv_cudafeatures2d310

else:win32:CONFIG(debug, debug|release): LIBS += -L$$PWD/../../../../opencv/build_new/lib/ -lopencv_core310d \
-lopencv_features2d310d -lopencv_highgui310d -lopencv_xfeatures2d310d -lopencv_imgproc310d -lopencv_flann310d \
-lopencv_cudafeatures2d310d
else:unix: LIBS += -L$$PWD/../../../../opencv/build_new/lib/ -lopencv_core310

INCLUDEPATH += $$PWD/../../../../opencv/build_new/install/include
DEPENDPATH += $$PWD/../../../../opencv/build_new/install/include



