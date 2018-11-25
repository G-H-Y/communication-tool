#-------------------------------------------------
#
# Project created by QtCreator 2017-11-01T21:23:23
#
#-------------------------------------------------

QT       += core gui network

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = SERVER
TEMPLATE = app
DEFINES += QT_DEPRECATED_WARNING

SOURCES += main.cpp\
        server.cpp \
    dealuserinfo.cpp\
    communicate.cpp \
    encryption/qaeswrap.cpp \
    encryption/aes.c \
    sockets.cpp

HEADERS  += server.h \
    dealuserinfo.h\
    communicate.h \
    encryption/aes.h \
    encryption/qaeswrap.h \
    sockets.h

FORMS    += server.ui
