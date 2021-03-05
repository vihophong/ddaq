QT += core
QT -= gui

TARGET = offlineanalysis
CONFIG += console
CONFIG -= app_bundle

TEMPLATE = app

SOURCES += main.cpp \
    ../ddaq/dpp.cpp \
    ../ddaq/daq.cpp


HEADERS += \
    ../ddaq/dpp.h \
    ../ddaq/daq.h

#---------------------CAEN Digitizer----------------------------

LIBS += -L/usr/lib/ -lCAENVME -lCAENDigitizer

INCLUDEPATH += $$/usr/lib/
DEPENDPATH += $$/usr/lib/

#---------------------ROOT include----------------------------
LIBS += $$system(root-config --glibs --cflags) -lRHTTP -lGpad -lCint -lCore -lGraf -lGraf3d -lHist -lGui -lTree -lRint -lPostscript -lMatrix -lPhysics -lMathCore -lRIO -lNet -lThread

INCLUDEPATH += "$(ROOTSYS)/include"
DEPENDPATH += "$(ROOTSYS)/include"
