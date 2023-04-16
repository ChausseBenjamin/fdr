QT       += core gui multimedia

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

CONFIG += c++17

# You can make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

SOURCES += \
    chord.cpp \
    common.cpp \
    difficultymenu.cpp \
    endmenu.cpp \
    fret.cpp \
    gamescene.cpp \
    infomenu.cpp \
    leftbar.cpp \
    mainmenu.cpp \
    note.cpp \
    remote.cpp \
    rightbar.cpp \
    rulesmenu.cpp \
    song.cpp \
    songmenu.cpp \
    timestamp.cpp \
    ui.cpp \
    main.cpp

HEADERS += \
    chord.h \
    common.h \
    difficultymenu.h \
    endmenu.h \
    fret.h \
    gamescene.h \
    infomenu.h \
    leftbar.h \
    mainmenu.h \
    note.h \
    remote.h \
    rightbar.h \
    rulesmenu.h \
    song.h \
    songmenu.h \
    timestamp.h \
    ui.h

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target

RESOURCES +=
