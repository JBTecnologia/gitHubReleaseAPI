#-------------------------------------------------
#
# Project created by QtCreator 2015-08-24T13:07:03
#
#-------------------------------------------------

QT       += core gui network
QT       += xml

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = releaseEditor
TEMPLATE = app


SOURCES += main.cpp\
        mainwindow.cpp \
    createnewreleasedialog.cpp

HEADERS  += mainwindow.h \
    createnewreleasedialog.h

FORMS    += mainwindow.ui \
    createnewreleasedialog.ui

win32:CONFIG(release, debug|release): LIBS += -L$$OUT_PWD/../gitHubReleaseAPI/release/ -lgitHubReleaseAPI
else:win32:CONFIG(debug, debug|release): LIBS += -L$$OUT_PWD/../gitHubReleaseAPI/debug/ -lgitHubReleaseAPI
else:unix: LIBS += -L$$OUT_PWD/../gitHubReleaseAPI/ -lgitHubReleaseAPI

INCLUDEPATH += $$PWD/../gitHubReleaseAPI
DEPENDPATH += $$PWD/../gitHubReleaseAPI

RESOURCES += \
    resources.qrc
