#-------------------------------------------------
#
# Project created by QtCreator 2019-11-22T01:14:55
#
#-------------------------------------------------

QT += core gui multimedia sql

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = music-player
TEMPLATE = app

# The following define makes your compiler emit warnings if you use
# any feature of Qt which has been marked as deprecated (the exact warnings
# depend on your compiler). Please consult the documentation of the
# deprecated API in order to know how to port your code away from it.
DEFINES += QT_DEPRECATED_WARNINGS TAGLIB_STATIC

# You can also make your code fail to compile if you use deprecated APIs.
# In order to do so, uncomment the following line.
# You can also select to disable deprecated APIs only up to a certain version of Qt.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

CONFIG += c++11

SOURCES += \
    src/core/libraryproxymodel.cpp \
    src/core/main.cpp \
    src/core/playercontrols.cpp \
    src/core/playercontrolsslider.cpp \
    src/core/playerwindow.cpp \
    src/core/taglibfilerefwrapper.cpp \
    src/core/trackdurationdelegate.cpp

HEADERS += \
    src/core/libraryproxymodel.h \
    src/core/playercontrols.h \
    src/core/playercontrolsslider.h \
    src/core/playerwindow.h \
    src/core/taglibfilerefwrapper.h \
    src/core/trackdurationdelegate.h

FORMS += \
    src/ui/playercontrols.ui \
    src/ui/playerwindow.ui

DESTDIR     = bin
OBJECTS_DIR = build/obj
UI_DIR      = build/uic
MOC_DIR     = build/moc

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target

unix|win32: LIBS += -L$$PWD/libs/taglib/lib/ -ltag

INCLUDEPATH += $$PWD/libs/taglib/include $$PWD/src/core
DEPENDPATH += $$PWD/libs/taglib/include $$PWD/src/core

win32:!win32-g++: PRE_TARGETDEPS += $$PWD/libs/taglib/lib/tag.lib
else:unix|win32-g++: PRE_TARGETDEPS += $$PWD/libs/taglib/lib/libtag.a
