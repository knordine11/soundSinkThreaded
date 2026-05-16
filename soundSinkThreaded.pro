QT += widgets multimedia

CONFIG += c++17

# You can make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

SOURCES += \
    fileloader.cpp \
    main.cpp \
    widget.cpp

HEADERS += \
    fileloader.h \
    widget.h

FORMS += \
    widget.ui

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target

DISTFILES += \
    config.txt \
    lessons.txt \
    v_sounds/v43.wav \
    v_sounds/v45.wav \
    v_sounds/v47.wav \
    v_sounds/v48.wav \
    v_sounds/v50.wav \
    v_sounds/v52.wav \
    v_sounds/v54.wav \
    v_sounds/v55.wav

RESOURCES += \
    Resources.qrc
