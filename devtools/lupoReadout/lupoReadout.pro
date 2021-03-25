#-------------------------------------------------
#
#
#
#-------------------------------------------------

QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = lupoReadout
TEMPLATE = app


SOURCES += main.cpp\
        lupoReadout.cpp \
    caenvmeA32.cpp \
    lupoLib.cpp

HEADERS  += lupoReadout.h \
    caenvmeA32.h \
    lupoLib.h

FORMS    += luporeadout.ui


LIBS += -L/usr/lib/ -lCAENVME

INCLUDEPATH += $$/usr/lib/
DEPENDPATH += $$/usr/lib/
