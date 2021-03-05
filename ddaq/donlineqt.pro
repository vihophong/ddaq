#-------------------------------------------------
#
# Project created by QtCreator 2015-11-25T20:41:28
#
#-------------------------------------------------

QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = donlineqt
TEMPLATE = app


SOURCES +=\
        mainwindow.cpp \
    daq.cpp \
    dpp.cpp \
    main_donlineqt.cpp

HEADERS  += \
    daq.h \
    dpp.h \
mainwindow.h

FORMS    += mainwindow.ui

#---------------------CAEN Digitizer----------------------------

unix:!macx: LIBS += -L/usr/lib/ -lCAENVME -lCAENDigitizer

INCLUDEPATH += $$/usr/lib/
DEPENDPATH += $$/usr/lib/


#---------------------ROOT include----------------------------
unix:!macx:  LIBS += $$system(root-config --glibs) -lGpad -lCint -lCore -lGraf -lGraf3d -lHist -lGui -lTree -lRint -lPostscript -lMatrix -lPhysics -lMathCore -lRIO -lNet -lThread

INCLUDEPATH += "$(ROOTSYS)/include"
DEPENDPATH += "$(ROOTSYS)/include"
include("$(ROOTSYS)/include/rootcint.pri")
