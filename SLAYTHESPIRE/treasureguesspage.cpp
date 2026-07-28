#include "treasureguesspage.h"
#include "campfirepage.h" // TopBarWidget (shared with Campfire/Map/Event/Treasure)
#include "potion.h"
#include "player.h"
#include "audiomanager.h"
#include "relicviewer.h"
#include "deckviewer.h"
#include "rewardsystem.h"

#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QDir>
#include <QCoreApplication>

TreasureGuessPage::TreasureGuessPage(Player* playerPtr, Map* mapPtr, QWidget* parent)
    : QWidget(parent), player(playerPtr), map(mapPtr)
{
    setupUI();

    if (topBar)
        topBar->updateData(player, map);

    setFixedSize(1280, 720);

    model.initializeGame();
    updateUI();
}

void TreasureGuessPage::setupUI()
{
    setObjectName("TreasureGuessPage");
    setStyleSheet(
        "#TreasureGuessPage {"
        "background-image: url(:/Treasure/TreasureBackground.png);"
        "background-repeat: no-repeat;"
        "background-position: center;"
        "background-color: #1a1410;"
        "}"
        "QToolTip { color: #facc15; background-color: #1f2937; border: 1px solid #b91c1c;"
        "border-radius: 4px; padding: 6px; font-weight: bold; font-family: Tahoma;"
        "}"
        );

    QPixmap pixmap(":/cursor.png");
    QPixmap scaledPixmap = pixmap.scaled(30, 40, Qt::KeepAspectRatio, Qt::SmoothTransformation);
    QCursor customCursor(scaledPixmap, 0, 0);
    this->setCursor(customCursor);

    QString baseDir = QCoreApplication::applicationDirPath();
    QString BtnPath = QDir(baseDir).filePath("assets/image/cursorBtn.png");
    QPixmap buttonHoverPixmap(BtnPath);
    QPixmap scaledHover = buttonHoverPixmap.scaled(40, 61, Qt::KeepAspectRatio, Qt::SmoothTransformation);
    QCursor buttonHoverCursor(scaledHover, scaledHover.width() / 2, 10);

    QVBoxLayout* mainLayout = new QVBoxLayout(this);
    mainLayout->setContentsMargins(0, 0, 0, 0);
    mainLayout->setSpacing(0);

    // --- Shared TopBar (same widget as Map/Campfire/Treasure/Event) ---
    topBar = new TopBarWidget(this);
    connect(topBar, &TopBarWidget::relicClicked, this, &TreasureGuessPage::onRelicButtonClicked);
    connect(topBar, &TopBarWidget::deckClicked, this, &TreasureGuessPage::onDeckButtonClicked);
    connect(topBar, &TopBarWidget::settingsClicked, this, &TreasureGuessPage::onSettingsButtonClicked);
    mainLayout->addWidget(topBar);

    // --- Title ---
    titleLabel = new QLabel("Guess which chest contains the reward!", this);
    titleLabel->setAlignment(Qt::AlignCenter);
    titleLabel->setStyleSheet(
        "color: #facc15; font-family: Tahoma; font-weight: bold; font-size: 24px; background: transparent;");
    titleLabel->setFixedHeight(100);
    mainLayout->addWidget(titleLabel);

    // --- Result message (empty until a wrong chest is opened) ---
    messageLabel = new QLabel(QString(), this);
    messageLabel->setAlignment(Qt::AlignCenter);
    messageLabel->setStyleSheet(
        "color: #e63946; font-family: Tahoma; font-weight: bold; font-size: 20px; background: transparent;");
    messageLabel->setFixedHeight(30);
    mainLayout->addWidget(messageLabel);

    mainLayout->addStretch(1);

    // --- Three chests, centered ---
    QHBoxLayout* chestsLayout = new QHBoxLayout();
    chestsLayout->setSpacing(60);
    chestsLayout->setAlignment(Qt::AlignCenter);

    for (int i = 0; i < TreasureGuessModel::CHEST_COUNT; ++i)
    {
        QPushButton* chestBtn = new QPushButton(this);
        chestBtn->setFixedSize(350, 250);
        chestBtn->setCursor(buttonHoverCursor);
        chestBtn->setStyleSheet(
            "QPushButton { border-image: url(:/Treasure/ChestClose.png); border: none; background: transparent; }"
            "QPushButton:pressed { margin: 5px 5px 5px 5px; }"
            );

        connect(chestBtn, &QPushButton::pressed, this, []() {
            AudioManager::instance().play(AudioManager::Sound::ButtonClick);
        });

        connect(chestBtn, &QPushButton::clicked, this, [this, i]() {
            onChestClicked(i);
        });

        chestButtons.append(chestBtn);
        chestsLayout->addWidget(chestBtn);
    }

    mainLayout->addLayout(chestsLayout);
    mainLayout->addStretch(2);

    // --- Reward button: hidden until the correct chest is opened.
    //     Same visual role/pattern as TreasurePage's rewardBtn. ---
    rewardBtn = new QPushButton(this);
    rewardBtn->setFixedSize(110, 110);
    rewardBtn->setCursor(buttonHoverCursor);
    rewardBtn->hide();
    connect(rewardBtn, &QPushButton::pressed, this, []() {
        AudioManager::instance().play(AudioManager::Sound::ButtonClick);
    });
    connect(rewardBtn, &QPushButton::clicked, this, &TreasureGuessPage::onRewardClicked);

    // --- Proceed: bottom-right, hidden until the round is resolved ---
    proceedBtn = new QPushButton(this);
    proceedBtn->setFixedSize(180, 80);
    proceedBtn->setCursor(buttonHoverCursor);
    proceedBtn->hide();
    proceedBtn->setStyleSheet(
        "QPushButton { border-image: url(:/Treasure/ProceedBtn.png); border: none; background: transparent; }"
        "QPushButton:pressed { margin: 5px 5px 5px 5px; }"
        );
    connect(proceedBtn, &QPushButton::pressed, this, []() {
        AudioManager::instance().play(AudioManager::Sound::ButtonClick);
    });
    connect(proceedBtn, &QPushButton::clicked, this, &TreasureGuessPage::onProceedClicked);
    proceedBtn->move(1050, 570);
}

void TreasureGuessPage::onChestClicked(int index)
{
    if (!model.selectChest(index))
        return; // already selected this round, or invalid index

    refreshChestVisual(index);

    // Only one pick allowed: lock every chest immediately.
    for (QPushButton* btn : std::as_const(chestButtons))
    {
        btn->setEnabled(false);
    }

    if (model.isCorrectChest(index))
    {
        showReward();
    }
    else
    {
        showFailureMessage();
        showProceedButton();
    }
}

void TreasureGuessPage::refreshChestVisual(int index)
{
    if (index < 0 || index >= chestButtons.size())
        return;

    chestButtons[index]->setStyleSheet(
        "QPushButton { border-image: url(:/Treasure/ChestOpen.png); border: none; background: transparent; }"
        "QPushButton:pressed { margin: 5px 5px 5px 5px; }"
        );
}

void TreasureGuessPage::showReward()
{
    Potion* reward = model.getReward();

    QString iconPath = ":/Potion/potionEmpty.png";
    QString tip = "Reward";

    if (reward)
    {
        QString potionName = reward->getName().toLower().replace(" ", "_");
        iconPath = QString(":/Potion/%1.png").arg(potionName);
        tip = reward->getName();
    }

    rewardBtn->setStyleSheet(
        QString("QPushButton { border-image: url(%1); border: none; background: transparent; }"
                "QPushButton:pressed { margin: 5px 5px 5px 5px; }").arg(iconPath)
        );
    rewardBtn->setToolTip(tip);

    const int selectedIndex = model.getSelectedChestIndex();

    if (selectedIndex >= 0 && selectedIndex < chestButtons.size())
    {
        const QRect chestGeo = chestButtons[selectedIndex]->geometry();
        rewardBtn->move(chestGeo.center().x() - rewardBtn->width() / 2,
                        chestGeo.top() - rewardBtn->height() - 10);
    }

    rewardBtn->raise();
    rewardBtn->show();
}

void TreasureGuessPage::showFailureMessage()
{
    messageLabel->setText("Wrong chest! Better luck next time.");
}

void TreasureGuessPage::showProceedButton()
{
    proceedBtn->raise();
    proceedBtn->show();
}

void TreasureGuessPage::onRewardClicked()
{
    if (player->getPotionCount() >= RewardSystem::MAX_POTIONS)
        return; // slots full: reward stays pending, not claimable

    Potion* reward = model.claimReward();

    if (!player || !reward)
        return;

    if (player)
        player->addPotion(reward);
    else
        delete reward;

    AudioManager::instance().play(AudioManager::Sound::Reward);

    rewardBtn->hide();
    topBar->updateData(player, map);

    showProceedButton();
}

void TreasureGuessPage::onProceedClicked()
{
    emit proceedRequested();
}

void TreasureGuessPage::updateUI()
{
    messageLabel->clear();

    for (QPushButton* btn : std::as_const(chestButtons))
    {
        btn->setEnabled(true);
        btn->setStyleSheet(
            "QPushButton { border-image: url(:/Treasure/ChestClose.png); border: none; background: transparent; }"
            "QPushButton:pressed { margin: 5px 5px 5px 5px; }"
            );
    }

    rewardBtn->hide();
    proceedBtn->hide();
}

void TreasureGuessPage::onRelicButtonClicked()
{
    RelicViewerDialog dialog(player, this);
    dialog.exec();
}

void TreasureGuessPage::onDeckButtonClicked()
{
    DeckViewerDialog dialog(player, this);
    dialog.exec();
}

void TreasureGuessPage::onSettingsButtonClicked()
{
    emit settingsRequested();
}
