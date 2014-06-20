#-------------------------------------------------
#
# Project created by QtCreator 2014-05-31T15:50:36
#
#-------------------------------------------------

QT       += core gui \
            testlib

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = CVGui
TEMPLATE = app

SOURCES += main.cpp\
        mainwindow.cpp \
    opencvbook.cpp \
    cvutils.cpp \
    Line.cpp \
    Chessboard.cpp \
    preprocess.cpp \
    boarddetector.cpp \
    corner.cpp

HEADERS  += mainwindow.h \
    Line.h \
    Chessboard.h \
    preprocess.h \
    boarddetector.h \
    cvutils.h \
    typedefs.h \
    corner.h

FORMS    += mainwindow.ui

INCLUDEPATH += /usr/local/Cellar/opencv/2.4.8.2/include/ \
    /usr/local/include/
LIBS += -L/usr/local/lib \
     -lopencv_core \
     -lopencv_imgproc \
     -lopencv_features2d\
     -lopencv_highgui \
     -lboost_math_c99 \


SUBDIRS += \
    tests/tests.pro \

