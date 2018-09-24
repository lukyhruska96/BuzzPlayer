#-------------------------------------------------
#
# Project created by QtCreator 2018-07-18T15:20:08
#
#-------------------------------------------------

QT       += core

QT       -= gui

LIBS += -lzmq

TARGET = buzzplayer_server
CONFIG   += console
CONFIG   -= app_bundle

TEMPLATE = app

QMAKE_CXXFLAGS += -std=c++14

SOURCES += main.cpp \
    fileparser.cpp \
    networking.cpp \
    clients.cpp

HEADERS += \
    fileparser.h \
    networking.h \
    clients.h
