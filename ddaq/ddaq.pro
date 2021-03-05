QT += core
QT -= gui

TARGET = ddaq
CONFIG += console
CONFIG -= app_bundle

TEMPLATE = app

HEADERS += \
    dpp.h \
    daq.h

SOURCES += main.cpp \
    dpp.cpp \
    daq.cpp


#---------------------CAEN Digitizer----------------------------

LIBS += -L/usr/lib/ -lCAENVME -lCAENDigitizer

INCLUDEPATH += $$/usr/lib/
DEPENDPATH += $$/usr/lib/


#---------------------ROOT include----------------------------
LIBS += $$system(root-config --glibs) -lGpad -lCint -lCore -lGraf -lGraf3d -lHist -lGui -lTree -lRint -lPostscript -lMatrix -lPhysics -lMathCore -lRIO -lNet -lThread

INCLUDEPATH += "$(ROOTSYS)/include"
DEPENDPATH += "$(ROOTSYS)/include"

