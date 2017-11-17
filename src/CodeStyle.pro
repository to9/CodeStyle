#-------------------------------------------------
#
# Project created by QtCreator 2017-11-15T11:24:03
#
#-------------------------------------------------

QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = CodeStyle
TEMPLATE = app

# The following define makes your compiler emit warnings if you use
# any feature of Qt which has been marked as deprecated (the exact warnings
# depend on your compiler). Please consult the documentation of the
# deprecated API in order to know how to port your code away from it.
DEFINES += QT_DEPRECATED_WARNINGS

# You can also make your code fail to compile if you use deprecated APIs.
# In order to do so, uncomment the following line.
# You can also select to disable deprecated APIs only up to a certain version of Qt.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0


VERSION = 1.0.0.1
RC_ICONS = CodeStyle.ico
QMAKE_TARGET_COMPANY = "ZCZX"
QMAKE_TARGET_PRODUCT = "CodeStyle"
QMAKE_TARGET_DESCRIPTION = "Qt Creator based on Qt 5.9.2 (MinGW 5.3.0, 32 bit)"
QMAKE_TARGET_COPYRIGHT = "https://github.com/to9"
RC_LANG = 0x0004




SOURCES += \
        main.cpp \
        widget.cpp \
    processthread.cpp

HEADERS += \
        widget.h \
    processthread.h \
    version.h

FORMS += \
        widget.ui
