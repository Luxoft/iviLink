#-------------------------------------------------
#
# Project created by QtCreator 2012-05-21T22:05:05
#
#-------------------------------------------------

QT       += core

QT       -= gui

TARGET = copyrightcons
CONFIG   += console
CONFIG   -= app_bundle

TEMPLATE = app


SOURCES += main.cpp

contains(MEEGO_EDITION,harmattan) {
    target.path = /opt/copyrightcons/bin
    INSTALLS += target
}
