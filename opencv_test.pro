#-------------------------------------------------
#
# Project created by QtCreator 2015-12-22T23:21:41
#
#-------------------------------------------------

QT       += core gui \
            multimedia \
            multimediawidgets

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = opencv_test
TEMPLATE = app


SOURCES += main.cpp\
        mainwindow.cpp \
    convert.cpp \
    filter.cpp

HEADERS  += mainwindow.h \
    convert.h \
    filter.h

FORMS    += mainwindow.ui



win32:CONFIG(release, debug|release): LIBS += -L$$PWD/../../../../opencv/build/x64/vc12/lib/ -lopencv_world300
else:win32:CONFIG(debug, debug|release): LIBS += -L$$PWD/../../../../opencv/build/x64/vc12/lib/ -lopencv_world300d
else:unix: LIBS += -L$$PWD/../../../../opencv/build/x64/vc12/lib/ -lopencv_world300

INCLUDEPATH += $$PWD/../../../../opencv/include
DEPENDPATH += $$PWD/../../../../opencv/include


win32:CONFIG(release, debug|release): LIBS += -L$$PWD/../../../../opencv/build/x64/vc12/staticlibx64/ -lopencv_core300
else:win32:CONFIG(debug, debug|release): LIBS += -L$$PWD/../../../../opencv/build/x64/vc12/staticlibx64/ -lopencv_core300d

INCLUDEPATH += $$PWD/../../../../opencv/include
DEPENDPATH += $$PWD/../../../../opencv/include

win32-g++:CONFIG(release, debug|release): PRE_TARGETDEPS += $$PWD/../../../../opencv/build/x64/vc12/staticlibx64/libopencv_core300.a
else:win32-g++:CONFIG(debug, debug|release): PRE_TARGETDEPS += $$PWD/../../../../opencv/build/x64/vc12/staticlibx64/libopencv_core300d.a
else:win32:!win32-g++:CONFIG(release, debug|release): PRE_TARGETDEPS += $$PWD/../../../../opencv/build/x64/vc12/staticlibx64/opencv_core300.lib
else:win32:!win32-g++:CONFIG(debug, debug|release): PRE_TARGETDEPS += $$PWD/../../../../opencv/build/x64/vc12/staticlibx64/opencv_core300d.lib

RESOURCES += \
    ressource.qrc

