#-------------------------------------------------
#
# Project created by QtCreator 2015-08-20T20:06:47
#
#-------------------------------------------------

QT       += core gui widgets multimedia multimediawidgets
CONFIG += C++11

TARGET = QKidsPlayer
TEMPLATE = app

ios {
    ios_icon.files = $$files($$PWD/ios/AppIcon*.png)
    QMAKE_BUNDLE_DATA += ios_icon
    QMAKE_INFO_PLIST = $$PWD/ios/Info.plist
}

include(3rd/QtAwesome/QtAwesome/QtAwesome.pri)

SOURCES += main.cpp\
        wndmain.cpp \
    playlistmonitor.cpp \
    paappidledetect.cpp

HEADERS  += wndmain.h \
    playlistmonitor.h \
    paappidledetect.h

FORMS    += wndmain.ui

RESOURCES += \
    3rd/QDarkStyleSheet/qdarkstyle/style.qrc
