#include "treasurepage.h"
#include "campfirepage.h"   // TopBarWidget - reused, not redefined
#include "player.h"
#include "map.h"
#include "reward.h"
#include "relic.h"
#include "potion.h"
#include "audiomanager.h"
#include "relicviewer.h"
#include "deckviewer.h"
#include "relicviewer.h"
#include <QVBoxLayout>

TreasurePage::TreasurePage(Player* playerPtr, Map* mapPtr, QWidget* parent)
    : QWidget(parent), player(playerPtr), map(mapPtr), chestOpened(false)
{
    setupUI();
    topBar->updateData(player, map);
    treasureManager.generateReward(player);
}

void TreasurePage::setupUI()
{
    setObjectName("TreasurePage");
    setStyleSheet(
        "#TreasurePage {"
        "background-image: url(:/Treasure/TreasureBackground.png);"
        "background-repeat: no-repeat;"
        "background-position: center;"
        "background-color: #1a1410;"
        "}"
        "QToolTip { color: #facc15; background-color: #1f2937; border: 1px solid #b91c1c;"
        "border-radius: 4px; padding: 6px; font-weight: bold; font-family: Tahoma;"
        "}"
        );
    setFixedSize(1280, 720);

    QVBoxLayout* mainLayout = new QVBoxLayout(this);
    mainLayout->setContentsMargins(0, 0, 0, 0);
    mainLayout->setSpacing(0);

    // --- Top Bar: same widget CampfirePage uses, not a new one ---
    topBar = new TopBarWidget(this);
    connect(topBar, &TopBarWidget::relicClicked, this, &TreasurePage::onRelicButtonClicked);
    connect(topBar, &TopBarWidget::deckClicked, this, &TreasurePage::onDeckButtonClicked);
    connect(topBar, &TopBarWidget::settingsClicked, this, &TreasurePage::onSettingsButtonClicked);
    mainLayout->addWidget(topBar);

    mainLayout->addStretch();

    // --- Reward button: hidden until the chest is opened ---
    rewardBtn = new QPushButton(this);
    rewardBtn->setFixedSize(110, 110);
    rewardBtn->setCursor(Qt::PointingHandCursor);
    rewardBtn->hide();
    rewardBtn->move(560, 220);
    connect(rewardBtn, &QPushButton::pressed, this, []() {
        AudioManager::instance().play(AudioManager::Sound::ButtonClick);
    });
    connect(rewardBtn, &QPushButton::clicked, this, &TreasurePage::onRewardClicked);
    //mainLayout->addWidget(rewardBtn, 0, Qt::AlignHCenter);

    //mainLayout->addSpacing(20);

    // --- Chest ---
    chestBtn = new QPushButton(this);
    chestBtn->setFixedSize(730, 450);
    chestBtn->setCursor(Qt::PointingHandCursor);
    connect(chestBtn, &QPushButton::pressed, this, []() {
        AudioManager::instance().play(AudioManager::Sound::ButtonClick);
    });
    connect(chestBtn, &QPushButton::clicked, this, &TreasurePage::onChestClicked);
    //mainLayout->addWidget(chestBtn, 0, Qt::AlignHCenter);

   // mainLayout->addStretch();

    // --- Back ---
    backBtn = new QPushButton(this);
    backBtn->setFixedSize(180, 80);
    backBtn->setCursor(Qt::PointingHandCursor);
    backBtn->setStyleSheet(
        "QPushButton { border-image: url(:/Treasure/ProceedBtn.png); border: none; background: transparent; }"
        "QPushButton:pressed { margin: 5px 5px 5px 5px; }"
        );
    connect(backBtn, &QPushButton::pressed, this, []() {
        AudioManager::instance().play(AudioManager::Sound::ButtonClick);
    });
    connect(backBtn, &QPushButton::clicked, this, &TreasurePage::onBackClicked);
    backBtn->move(1050, 550);
    //mainLayout->addWidget(backBtn, 0, Qt::AlignHCenter);
    //mainLayout->addSpacing(30);

    refreshChestVisual();
}

void TreasurePage::refreshChestVisual()
{
    // Placeholder paths - swap in the real Closed/Opened art later,
    // nothing else in this method needs to change.
    QString imagePath = chestOpened ? ":/Treasure/ChestOpen.png" : ":/Treasure/ChestClose.png";

    chestBtn->setStyleSheet(
        QString("QPushButton { border-image: url(%1); border: none; background: transparent; }"
                "QPushButton:pressed { margin: 5px 5px 5px 5px; }").arg(imagePath)
        );

    if(chestOpened)
    {
        chestBtn->move(220, 70);
        chestBtn->setFixedSize(780, 600);

    }
    else
    {
        chestBtn->move(250, 170);
    }
}

void TreasurePage::refreshRewardVisual()
{
    Reward* reward = treasureManager.getReward();

    if (!reward)
    {
        rewardBtn->hide();
        return;
    }

    // Tooltip works today even without art; once icons are provided this
    // is also the place to pick the right image per RewardType.
    QString tip = "Reward";

    switch (reward->getType())
    {
    case RewardType::Gold:
        rewardBtn->setStyleSheet(
            "QPushButton { border-image: url(:/Treasure/Money.png); border: none; background: transparent; }"
            "QPushButton:pressed { margin: 5px 5px 5px 5px; }"
            );
        tip = QString("Gold  +%1").arg(reward->getGoldAmount());
        break;

    case RewardType::Potion:
        if (reward->getPotion())
        {
            QString potionName = reward->getPotion()->getName().toLower().replace(" ", "_");
            QString iconPath = QString(":/Potion/%1.png").arg(potionName);
            rewardBtn->setStyleSheet(
                QString("QPushButton { border-image: url(%1); border: none; background: transparent; }"
                        "QPushButton:pressed { margin: 5px 5px 5px 5px; }").arg(iconPath)
                );
            tip = reward->getPotion()->getName();
        }
        break;

    case RewardType::Relic:
        if (reward->getRelic())
        {
            rewardBtn->setStyleSheet(
                QString("QPushButton { border-image: url(%1); border: none; background: transparent; }"
                        "QPushButton:pressed { margin: 5px 5px 5px 5px; }").arg(RelicViewerDialog::relicIconPath(reward->getRelic()))
                );
            tip = reward->getRelic()->getName();
        }
        break;

    case RewardType::Card:
        break; // never happens for Treasure rewards
    }

    rewardBtn->setToolTip(tip);
    rewardBtn->raise();
    rewardBtn->show();
}

void TreasurePage::onChestClicked()
{
    if (chestOpened)
        return;

    chestOpened = true;
    chestBtn->setEnabled(false); // opens exactly once

    refreshChestVisual();
    refreshRewardVisual();
}

void TreasurePage::onRewardClicked()
{
    if (!treasureManager.claimReward(player))
        return;

    AudioManager::instance().play(AudioManager::Sound::Reward);

    rewardBtn->hide(); // can't be claimed a second time
    topBar->updateData(player, map); // reflect new HP/Gold/Potions/Relics immediately
}

void TreasurePage::onBackClicked()
{
    if (!treasureManager.isClaimed())
    {
        // Reward was never picked up - it's lost for good, nothing is
        // added to the player.
        treasureManager.discardReward();
    }

    emit treasureFinished();
}

void TreasurePage::onRelicButtonClicked()
{
    RelicViewerDialog dialog(player, this);
    dialog.exec();
}

void TreasurePage::onDeckButtonClicked()
{
    DeckViewerDialog dialog(player, this);
    dialog.exec();
}

void TreasurePage::onSettingsButtonClicked()
{
    // TODO: open the same Settings Dialog used by MapPage/CampfirePage.
    // emit settingsRequested();
}