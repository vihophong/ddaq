QT += core
QT -= gui

TARGET = donline
CONFIG += console
CONFIG -= app_bundle

TEMPLATE = app

SOURCES += main_donline.cpp \
    daq.cpp \
    keyb.cpp \
    mysignalhandler.cpp \
    dpp.cpp

HEADERS += \
    daq.h \
    keyb.h \
    mysignalhandler.h



#---------------------CAEN Digitizer----------------------------

unix:!macx: LIBS += -L/usr/lib/ -lCAENVME -lCAENDigitizer

INCLUDEPATH += $$/usr/lib/
DEPENDPATH += $$/usr/lib/


#---------------------ROOT include----------------------------
unix:!macx:  LIBS += $$system(root-config --glibs) -lGpad -lCint -lCore -lGraf -lGraf3d -lHist -lGui -lTree -lRint -lPostscript -lMatrix -lPhysics -lMathCore -lRIO -lNet -lThread

INCLUDEPATH += "$(ROOTSYS)/include"
DEPENDPATH += "$(ROOTSYS)/include"

HEADERS += \
    dpp.h \
    daq.h
