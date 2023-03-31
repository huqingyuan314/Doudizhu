#-------------------------------------------------
#
# Project created by QtCreator 2022-05-27 T19:18:00
#
#-------------------------------------------------

QT       += core gui multimedia
UI_DIR    =./UI

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = LandlordCardGame
TEMPLATE = app

RC_FILE = LandlordCardGame.rc

SOURCES += main.cpp\
        MainFrame.cpp \
    GamePanel.cpp \
    GameControl.cpp \
    MarkBoard.cpp \
    Player.cpp \
    Robot.cpp \
    UserPlayer.cpp \
    Cards.cpp \
    CardPic.cpp \
    Strategy.cpp \
    CallLordThread.cpp \
    PlayHandThread.cpp \
    Hand.cpp \
    bgmcontrol.cpp

HEADERS  += MainFrame.h \
    GamePanel.h \
    GameControl.h \
    MarkBoard.h \
    Player.h \
    Robot.h \
    UserPlayer.h \
    Cards.h \
    CardPic.h \
    Strategy.h \
    CallLordThread.h \
    PlayHandThread.h \
    Hand.h \
    bgmcontrol.h

FORMS    += MainFrame.ui \
    MarkBoard.ui

TRANSLATIONS += zh.ts #中文语言包

RESOURCES += \
    LandlordCardGame.qrc

OTHER_FILES += \
    zh.ts \
    LandlordCardGame.rc \
    stylesheet.qss

