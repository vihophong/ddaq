QT += core
QT -= gui

TARGET = doffline
CONFIG += console
CONFIG -= app_bundle

TEMPLATE = app

SOURCES += \    
    daq.cpp \
    dpp.cpp \
    main_offline.cpp

HEADERS += \
    daq.h \
    dpp.h




#---------------------CAEN Digitizer----------------------------

unix:!macx: LIBS += -L/usr/lib/ -lCAENVME -lCAENDigitizer

INCLUDEPATH += $$/usr/lib/
DEPENDPATH += $$/usr/lib/


#---------------------ROOT include----------------------------
unix:!macx:  LIBS += $$system(root-config --glibs) -lGpad -lCint -lCore -lGraf -lGraf3d -lHist -lGui -lTree -lRint -lPostscript -lMatrix -lPhysics -lMathCore -lRIO -lNet -lThread

INCLUDEPATH += "$(ROOTSYS)/include/root"
DEPENDPATH += "$(ROOTSYS)/include/root"
