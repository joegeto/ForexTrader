#-------------------------------------------------
#
# Project created by QtCreator 2013-02-11T15:04:29
#
#-------------------------------------------------

QT       += core gui

TARGET = ForexTrader
TEMPLATE = app

MOC_DIR=garbage
OBJECTS_DIR=garbage
UI_DIR=garbage

SOURCES += main.cpp\
        mainwindow.cpp \
    bid.cpp \
    outputwidget.cpp \
    eventfilter.cpp \
    tooltipwidget.cpp

HEADERS  += mainwindow.h \
    bid.h \
    outputwidget.h \
    eventfilter.h \
    tooltipwidget.h

FORMS    += mainwindow.ui
