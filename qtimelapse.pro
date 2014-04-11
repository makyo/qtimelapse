#-------------------------------------------------
#
# Project created by QtCreator 2014-04-11T10:20:00
#
#-------------------------------------------------

QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = qtimelapse
TEMPLATE = app


SOURCES += main.cpp\
        qtimelapse.cpp \
    camerahandler.cpp \
    timelapse.cpp

HEADERS  += qtimelapse.h \
    camerahandler.h \
    timelapse.h

FORMS    += qtimelapse.ui

unix|win32: LIBS += -lgphoto2

unix|win32: LIBS += -lgphoto2_port
