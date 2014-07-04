#-------------------------------------------------
#
# Project created by QtCreator 2014-05-31T15:50:36
#
#-------------------------------------------------

QT       += core gui \
            testlib

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

CONFIG += c++11

TARGET = CVGui
TEMPLATE = app

SOURCES += main.cpp\
        mainwindow.cpp \
    opencvbook.cpp \
    cvutils.cpp \
    Line.cpp \
    preprocess.cpp \
    boarddetector.cpp \
    corner.cpp \
    square.cpp \
    oldboard.cpp \
    board.cpp

HEADERS  += mainwindow.h \
    Line.h \
    preprocess.h \
    boarddetector.h \
    cvutils.h \
    typedefs.h \
    corner.h \
    square.h \
    matrix.h \
    oldboard.h \
    board.h

FORMS    += mainwindow.ui

INCLUDEPATH += /usr/local/Cellar/opencv/2.4.8.2/include/ \
    /usr/local/include/ \
    /usr/local/Cellar/armadillo/4.100.2/include/

LIBS += -L/usr/local/lib \
     -lopencv_core \
     -lopencv_imgproc \
     -lopencv_features2d\
     -lopencv_highgui \
     -lopencv_calib3d \
     -lboost_math_c99 \
     -larmadillo \
     -llapack \
     -lblas


SUBDIRS += \
    tests/tests.pro \

