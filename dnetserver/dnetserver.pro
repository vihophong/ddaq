QT += core
QT -= gui

TARGET = dnetserver
CONFIG += console
CONFIG -= app_bundle

TEMPLATE = app

SOURCES += main.cpp \
    ../ddaq/daq.cpp \
    ../ddaq/dpp.cpp

HEADERS += \
    ../ddaq/daq.h \
    ../ddaq/dpp.h

#---------------------CAEN Digitizer + Readline library----------------------------

LIBS += -L/usr/lib/ -lCAENVME -lCAENDigitizer -lreadline

INCLUDEPATH += $$/usr/lib/
DEPENDPATH += $$/usr/lib/


#---------------------ROOT include----------------------------
LIBS += $$system(root-config --glibs) -lGpad -lCint -lCore -lGraf -lGraf3d -lHist -lGui -lTree -lRint -lPostscript -lMatrix -lPhysics -lMathCore -lRIO -lNet -lThread

INCLUDEPATH += "$(ROOTSYS)/include"
DEPENDPATH += "$(ROOTSYS)/include"
