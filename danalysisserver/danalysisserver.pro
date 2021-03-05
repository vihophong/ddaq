QT += core
QT -= gui

TARGET = danalysisserver
CONFIG += console
CONFIG -= app_bundle

TEMPLATE = app

SOURCES += main.cpp \
    ../ddaq/dpp.cpp \
    ../ddaq/daq_class.cpp \
    ../ddaq/mysignalhandler.cpp \
    ../ddaq/keyb.cpp \
    randCircle.cpp

HEADERS += \
    ../ddaq/dpp.h \
    ../ddaq/daq_class.h \
    ../ddaq/mysignalhandler.h \
    ../ddaq/keyb.h \
    randCircle.h


#---------------------ROOT include----------------------------
LIBS += $$system(root-config --glibs --cflags) -lRHTTP -lGpad -lCint -lCore -lGraf -lGraf3d -lHist -lGui -lTree -lRint -lPostscript -lMatrix -lPhysics -lMathCore -lRIO -lNet -lThread

INCLUDEPATH += "$(ROOTSYS)/include"
DEPENDPATH += "$(ROOTSYS)/include"

