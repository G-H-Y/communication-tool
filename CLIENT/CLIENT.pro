#-------------------------------------------------
#
# Project created by QtCreator 2017-11-04T21:37:47
#
#-------------------------------------------------

QT       += core gui network concurrent axcontainer

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = CLIENT
TEMPLATE = app


SOURCES += main.cpp\
        mainwindow.cpp \
    showuserlist.cpp \
    progressbarview.cpp \
    timetrack.cpp \
    chat_record.cpp \
    communicate.cpp \
    signinview.cpp \
    signupview.cpp \
    findpsdview.cpp \
    linkserverview.cpp

HEADERS  += mainwindow.h \
    showuserlist.h \
    useritem.h \
    progressbarview.h \
    timetrack.h \
    chat_record.h \
    communicate.h \
    signinview.h \
    signupview.h \
    findpsdview.h \
    linkserverview.h

FORMS    += mainwindow.ui \
    progressbarview.ui \
    signinview.ui \
    signupview.ui \
    findpsdview.ui \
    linkserverview.ui

RESOURCES += icon.qrc \
    icon.qrc

DISTFILES += style/login.qss
