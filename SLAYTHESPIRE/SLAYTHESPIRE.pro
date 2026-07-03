QT += widgets

CONFIG += c++17

# You can make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

SOURCES += \
    attackcards.cpp \
    bossenemies.cpp \
    card.cpp \
    cursecards.cpp \
    effect.cpp \
    eliteenemy.cpp \
    enemy.cpp \
    loginpage.cpp \
    main.cpp \
    mainpage.cpp \
    mainwindow.cpp \
    normalenemies.cpp \
    normalrelics.cpp \
    player.cpp \
    powercard.cpp \
    relic.cpp \
    skillcards.cpp \
    splashpage.cpp \
    character.cpp \
    statuscards.cpp

HEADERS += \
    attackcards.h \
    bossenemies.h \
    card.h \
    cursecards.h \
    effect.h \
    eliteenemy.h \
    enemy.h \
    loginpage.h \
    mainpage.h \
    mainwindow.h \
    normalenemies.h \
    normalrelics.h \
    player.h \
    powercard.h \
    relic.h \
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
