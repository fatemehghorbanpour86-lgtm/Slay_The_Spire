QT += widgets

CONFIG += c++17

# You can make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

SOURCES += \
    attackcards.cpp \
    card.cpp \
    effect.cpp \
    loginpage.cpp \
    main.cpp \
    mainpage.cpp \
    mainwindow.cpp \
    player.cpp \
    skillcards.cpp \
    splashpage.cpp \
    character.cpp \
    statuscards.cpp

HEADERS += \
    attackcards.h \
    card.h \
    effect.h \
    loginpage.h \
    mainpage.h \
    mainwindow.h \
    player.h \
    skillcards.h \
    splashpage.h \
    character.h \
    statuscards.h

FORMS += \
    mainwindow.ui

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target

RESOURCES += \
    resources.qrc
