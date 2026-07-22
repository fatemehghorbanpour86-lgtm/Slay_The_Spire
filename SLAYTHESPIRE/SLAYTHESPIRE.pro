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
    cardremovaldialog.cpp \
    cardselection.cpp \
    combatcalculator.cpp \
    combatdeck.cpp \
    combatmanager.cpp \
    cursecards.cpp \
    deckviewer.cpp \
    effect.cpp \
    eliteenemy.cpp \
    enemy.cpp \
    enemyfactory.cpp \
    event.cpp \
    eventmanager.cpp \
    eventpage.cpp \
    eventrelics.cpp \
    eventsystem.cpp \
    gamemanager.cpp \
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
    memorygame.cpp \
    memorygamecard.cpp \
    memorygamepage.cpp \
    normalenemies.cpp \
    normalrelics.cpp \
    pauseviewer.cpp \
    pileviewerdialog.cpp \
    player.cpp \
    potion.cpp \
    powercard.cpp \
    relic.cpp \
    relicselection.cpp \
    relicsystem.cpp \
    relicviewer.cpp \
    reward.cpp \
    rewardpage.cpp \
    rewardsystem.cpp \
    savemanager.cpp \
    settingpage.cpp \
    shop.cpp \
    shoppage.cpp \
    skillcards.cpp \
    splashpage.cpp \
    character.cpp \
    statuscards.cpp \
    treasure.cpp \
    treasurepage.cpp \
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
    cardremovaldialog.h \
    cardselection.h \
    combatcalculator.h \
    combatdeck.h \
    combatmanager.h \
    cursecards.h \
    deckviewer.h \
    effect.h \
    eliteenemy.h \
    enemy.h \
    enemyfactory.h \
    event.h \
    eventmanager.h \
    eventpage.h \
    eventrelics.h \
    eventsystem.h \
    gamemanager.h \
    loginpage.h \
    mainpage.h \
    mainwindow.h \
    map.h \
    mapfloor.h \
    mapnode.h \
    mappage.h \
    mapwidget.h \
    masterdeck.h \
    memorygame.h \
    memorygamecard.h \
    memorygamepage.h \
    normalenemies.h \
    normalrelics.h \
    outlinedlabel.h \
    pauseviewer.h \
    pileviewerdialog.h \
    player.h \
    potion.h \
    powercard.h \
    relic.h \
    relicselection.h \
    relicsystem.h \
    relicviewer.h \
    reward.h \
    rewardpage.h \
    rewardsystem.h \
    savemanager.h \
    settingpage.h \
    shop.h \
    shoppage.h \
    skillcards.h \
    splashpage.h \
    character.h \
    statuscards.h \
    treasure.h \
    treasurepage.h \
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
