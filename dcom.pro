QT += core
QT -= gui

TARGET = dcom
CONFIG += console
CONFIG -= app_bundle

TEMPLATE = app

SOURCES += main_dcom.cpp \
    daq.cpp \
    dpp.cpp

HEADERS += \
    dpp.h \
    daq.h


#---------------------CAEN Digitizer + Readline library----------------------------

unix:!macx: LIBS += -L/usr/lib/ -lCAENVME -lCAENDigitizer -lreadline

INCLUDEPATH += $$/usr/lib/
DEPENDPATH += $$/usr/lib/


#---------------------ROOT include----------------------------
unix:!macx:  LIBS += $$system(root-config --glibs) -lGpad -lCint -lCore -lGraf -lGraf3d -lHist -lGui -lTree -lRint -lPostscript -lMatrix -lPhysics -lMathCore -lRIO -lNet -lThread

INCLUDEPATH += "$(ROOTSYS)/include/root"
DEPENDPATH += "$(ROOTSYS)/include/root"


