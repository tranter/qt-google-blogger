#-------------------------------------------------
#
# Project created by QtCreator 2012-01-16T17:27:13
#
#-------------------------------------------------

QT       += core gui webkit network

TARGET = google_api_blogger_clent
TEMPLATE = app

linux* {
LIBS += ../qjson/build/lib/libqjson.so
INCLUDEPATH += ../qjson/include
}

win* {
LIBS += ../qjson/build/lib/qjson0.dll
INCLUDEPATH += ../qjson/include
}

macx* {
LIBS += -F../qjson/build/lib -framework qjson
INCLUDEPATH += ../qjson/include
}

SOURCES += main.cpp\
        mainwindow.cpp \
    oauth2.cpp \
    logindialog.cpp \
    form.cpp \
    blogger_data_manager.cpp

HEADERS  += mainwindow.h \
    oauth2.h \
    logindialog.h \
    form.h \
    blogger_data_manager.h

FORMS    += mainwindow.ui \
    logindialog.ui \
    form.ui

RESOURCES += \
    resource.qrc










