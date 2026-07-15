QT += widgets multimedia
QT += core gui widgets

CONFIG += c++17

# You can make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

SOURCES += \
    attackcards.cpp \
    audiomanager.cpp \
    battlepage.cpp \
    bossenemies.cpp \
    bossrelics.cpp \
    campfire.cpp \
    campfirepage.cpp \
    card.cpp \
    combatcalculator.cpp \
    combatdeck.cpp \
    combatmanager.cpp \
    cursecards.cpp \
    deckviewer.cpp \
    effect.cpp \
    eliteenemy.cpp \
    enemy.cpp \
    event.cpp \
    eventrelics.cpp \
    eventsystem.cpp \
    loginpage.cpp \
    main.cpp \
    mainpage.cpp \
    mainwindow.cpp \
    map.cpp \
    mapfloor.cpp \
    mapnode.cpp \
    mappage.cpp \
    mapwidget.cpp \
    masterdeck.cpp \
    normalenemies.cpp \
    normalrelics.cpp \
    player.cpp \
    potion.cpp \
    powercard.cpp \
    relic.cpp \
    relicsystem.cpp \
    relicviewer.cpp \
    reward.cpp \
    rewardsystem.cpp \
    savemanager.cpp \
    skillcards.cpp \
    splashpage.cpp \
    character.cpp \
    statuscards.cpp \
    upgradecards.cpp

HEADERS += \
    attackcards.h \
    audiomanager.h \
    battlepage.h \
    bossenemies.h \
    bossrelics.h \
    campfire.h \
    campfirepage.h \
    card.h \
    combatcalculator.h \
    combatdeck.h \
    combatmanager.h \
    cursecards.h \
    deckviewer.h \
    effect.h \
    eliteenemy.h \
    enemy.h \
    event.h \
    eventrelics.h \
    eventsystem.h \
    loginpage.h \
    mainpage.h \
    mainwindow.h \
    map.h \
    mapfloor.h \
    mapnode.h \
    mappage.h \
    mapwidget.h \
    masterdeck.h \
    normalenemies.h \
    normalrelics.h \
    outlinedlabel.h \
    player.h \
    potion.h \
    powercard.h \
    relic.h \
    relicsystem.h \
    relicviewer.h \
    reward.h \
    rewardsystem.h \
    savemanager.h \
    skillcards.h \
    splashpage.h \
    character.h \
    statuscards.h \
    upgradecards.h

FORMS += \
    battlepage.ui \
    mainwindow.ui

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target

RESOURCES += \
    resources.qrc
