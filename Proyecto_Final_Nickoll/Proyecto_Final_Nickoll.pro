QT += core gui
greaterThan(QT_MAJOR_VERSION, 4): QT += widgets multimedia multimediawidgets

CONFIG += c++17

SOURCES += \
    bala.cpp \
    indicadorvida.cpp \
    main.cpp \
    mainwindow.cpp \
    personaje.cpp

HEADERS += \
    bala.h \
    indicadorvida.h \
    mainwindow.h \
    personaje.h

FORMS += \
    mainwindow.ui

RESOURCES += \
    rcs.qrc

# Default rules for deployment
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target
