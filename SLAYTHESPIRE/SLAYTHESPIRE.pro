QT += widgets multimedia

CONFIG += c++17

# You can make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

SOURCES += \
    attackcards.cpp \
    bossenemies.cpp \
    bossrelics.cpp \
    campfire.cpp \
    card.cpp \
    combatcalculator.cpp \
    combatdeck.cpp \
    cursecards.cpp \
    effect.cpp \
    eliteenemy.cpp \
    enemy.cpp \
    eventrelics.cpp \
    loginpage.cpp \
    main.cpp \
    mainpage.cpp \
    mainwindow.cpp \
    map.cpp \
    mapfloor.cpp \
    mapnode.cpp \
    masterdeck.cpp \
    normalenemies.cpp \
    normalrelics.cpp \
    player.cpp \
    potion.cpp \
    powercard.cpp \
    relic.cpp \
    relicsystem.cpp \
    skillcards.cpp \
    splashpage.cpp \
    character.cpp \
    statuscards.cpp

HEADERS += \
    attackcards.h \
    bossenemies.h \
    bossrelics.h \
    campfire.h \
    card.h \
    combatcalculator.h \
    combatdeck.h \
    cursecards.h \
    effect.h \
    eliteenemy.h \
    enemy.h \
    eventrelics.h \
    loginpage.h \
    mainpage.h \
    mainwindow.h \
    map.h \
    mapfloor.h \
    mapnode.h \
    masterdeck.h \
    normalenemies.h \
    normalrelics.h \
    player.h \
    potion.h \
    powercard.h \
    relic.h \
    relicsystem.h \
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
