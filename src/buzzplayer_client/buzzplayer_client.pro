#-------------------------------------------------
#
# Project created by QtCreator 2018-07-08T10:53:52
#
#-------------------------------------------------

QT       += core

QT       -= gui

LIBS += -lasound
LIBS += -lzmq

TARGET = buzzplayer_client
CONFIG   += console
CONFIG   -= app_bundle

TEMPLATE = app

QMAKE_CXXFLAGS += -std=c++14

SOURCES += main.cpp \
    networking.cpp \
    beep.cpp

HEADERS += \
    networking.h \
    beep.h
