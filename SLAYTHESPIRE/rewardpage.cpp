#include "rewardpage.h"
#include "reward.h"
#include "player.h"
#include "relic.h"
#include "card.h"
#include "potion.h"
#include "cardselection.h"
#include "relicselection.h"
#include "audiomanager.h"
#include "relicviewer.h"
#include "mappage.h"
#include <QVBoxLayout>
#include <QLabel>

RewardPage::RewardPage(Player* playerPtr, RewardSystem* rewardSystemPtr, QWidget* parent)
    : QWidget(parent), player(playerPtr), rewardSystem(rewardSystemPtr)
{
    setupUI();
    rebuildRewardButtons();
}

void RewardPage::setupUI()
{
    setWindowTitle("Reward");
    setObjectName("RewardPage");
    setStyleSheet(
        "#RewardPage { border-image: url(:/Reward/RewardViewer.png); }"
        "QToolTip { color: #facc15; background-color: #1f2937; border: 1px solid #b91c1c;"
        "border-radius: 4px; padding: 6px; font-weight: bold; }"
        );
    setFixedSize(400, 550);

    QVBoxLayout* mainLayout = new QVBoxLayout(this);
    mainLayout->setContentsMargins(40, 40, 40, 30);
    mainLayout->setSpacing(20);

    QLabel* title = new QLabel("Rewards", this);
    title->setAlignment(Qt::AlignCenter);
    title->setStyleSheet("color: #2C444F; font-size: 30px; font-weight: bold; background: transparent;");
    mainLayout->addWidget(title);

    mainLayout->addStretch(1);

    rewardsLayout = new QVBoxLayout();
    rewardsLayout->setSpacing(16);
    rewardsLayout->setAlignment(Qt::AlignCenter);
    mainLayout->addLayout(rewardsLayout);

    mainLayout->addStretch(4);

    continueBtn = new QPushButton(this);
    continueBtn->setFixedSize(110, 50);
    continueBtn->setCursor(Qt::PointingHandCursor);
    continueBtn->setStyleSheet(
        "QPushButton { border-image: url(:/RestSite/BackBtn.png); background: transparent; border: none; }"
        "QPushButton:pressed { margin: 5px 5px 5px 5px; }"
        );

    continueBtn->move(0, 450);

    connect(continueBtn, &QPushButton::pressed, this, []() {
        AudioManager::instance().play(AudioManager::Sound::ButtonClick);
    });
    connect(continueBtn, &QPushButton::clicked, this, &RewardPage::onContinueClicked);
}

void RewardPage::rebuildRewardButtons()
{
    QLayoutItem* item;
    while ((item = rewardsLayout->takeAt(0)) != nullptr)
    {
        if (item->widget())
            item->widget()->deleteLater();

        delete item;
    }

    if (!rewardSystem)
        return;

    for (Reward* reward : rewardSystem->getRewards())
    {
        QPushButton* btn = createRewardButton(reward);

        if (btn)
            rewardsLayout->addWidget(btn, 0, Qt::AlignHCenter);
    }
}

QPushButton* RewardPage::createRewardButton(Reward* reward)
{
    if (!reward)
        return nullptr;

    QPushButton* btn = new QPushButton(this);
    btn->setFixedSize(220, 60);
    btn->setCursor(Qt::PointingHandCursor);

    switch (reward->getType())
    {
    case RewardType::Gold:
        btn->setStyleSheet(
            "QPushButton { border-image: url(:/Reward/GoldReward.png); font-size: 15px;"
            "font-weight: bold; border: 1px solid rgba(255,255,255,40); }"
            "QPushButton:pressed { margin: 5px 5px 5px 5px; }"
            );
        btn->setText(QString("Gold  +%1").arg(reward->getGoldAmount()));
        connect(btn, &QPushButton::clicked, this, [this, reward, btn]() {
            onGoldClicked(reward, btn);
        });
        break;

    case RewardType::Potion:
    {
        Potion* potion = reward->getPotion();
        btn->setStyleSheet(
            "QPushButton { border-image: url(:/Reward/PotionReward.png); font-size: 15px;"
            "font-weight: bold; border: 1px solid rgba(255,255,255,40); }"
            "QPushButton:pressed { margin: 5px 5px 5px 5px; }"
            );

        btn->setText(potion ? QString("%1").arg(potion->getName()) : "Potion");

        bool full = player && player->getPotionCount() >= RewardSystem::MAX_POTIONS;
        btn->setEnabled(!full);
        btn->setToolTip(full ? "Potion slots are full."
                             : (potion ? potion->getDescription() : QString()));

        connect(btn, &QPushButton::clicked, this, [this, reward, btn]() {
            onPotionClicked(reward, btn);
        });
        break;
    }

    case RewardType::Relic:
        btn->setStyleSheet(
            "QPushButton { border-image: url(:/Reward/RelicReward.png); font-size: 15px;"
            "font-weight: bold; border: 1px solid rgba(255,255,255,40); }"
            "QPushButton:pressed { margin: 5px 5px 5px 5px; }"
            );
        if (reward->isChoice())
        {
            btn->setText("Choose a Relic");
            connect(btn, &QPushButton::clicked, this, [this, reward, btn]() {
                onBossRelicClicked(reward, btn);
            });
        }
        else
        {
            Relic* relic = reward->getRelic();
            btn->setText(relic ? relic->getName() : "Relic");

            if (relic)
            {
                btn->setToolTip(relic->getDescription());
                // btn->setStyleSheet(btn->styleSheet() +
                //                    QString("QPushButton { border-image: url(%1); }")
                //                        .arg(RelicViewerDialog::relicIconPath(relic)));
            }

            connect(btn, &QPushButton::clicked, this, [this, reward, btn]() {
                onRelicClicked(reward, btn);
            });
        }
        break;

    case RewardType::Card:
        btn->setStyleSheet(
            "QPushButton { border-image: url(:/Reward/CardReward.png); font-size: 15px;"
            "font-weight: bold; border: 1px solid rgba(255,255,255,40); }"
            "QPushButton:pressed { margin: 5px 5px 5px 5px; }"
            );
        btn->setText("Card Reward");
        connect(btn, &QPushButton::clicked, this, [this, reward, btn]() {
            onCardClicked(reward, btn);
        });
        break;
    }

    return btn;
}

void RewardPage::onGoldClicked(Reward* reward, QPushButton* button)
{
    if (!rewardSystem || !rewardSystem->claimGoldReward(player, reward))
        return;

    AudioManager::instance().play(AudioManager::Sound::Reward);

    rewardsLayout->removeWidget(button);
    button->deleteLater();
    MapPage::updateTopBarData();
}

void RewardPage::onPotionClicked(Reward* reward, QPushButton* button)
{
    if (!rewardSystem || !rewardSystem->claimPotionReward(player, reward))
        return;

    AudioManager::instance().play(AudioManager::Sound::Drink);

    rewardsLayout->removeWidget(button);
    button->deleteLater();
    MapPage::updateTopBarData();
}

void RewardPage::onRelicClicked(Reward* reward, QPushButton* button)
{
    if (!rewardSystem || !rewardSystem->claimRelicReward(player, reward))
        return;

    AudioManager::instance().play(AudioManager::Sound::Reward);

    rewardsLayout->removeWidget(button);
    button->deleteLater();
    MapPage::updateTopBarData();
}

void RewardPage::onCardClicked(Reward* reward, QPushButton* button)
{
    Q_UNUSED(reward)
    Q_UNUSED(button)

    if (!reward)
        return;

    CardSelectionDialog dialog(reward->getCardChoices(), this);

    if (dialog.exec() != QDialog::Accepted)
        return; // player closed the dialog: the reward stays pending

    Card* chosen = dialog.getChosenCard();

    if (!rewardSystem || !rewardSystem->claimCardReward(player, reward, chosen))
        return;

    rewardsLayout->removeWidget(button);
    button->deleteLater();
    MapPage::updateTopBarData();
}

void RewardPage::onBossRelicClicked(Reward* reward, QPushButton* button)
{

    Q_UNUSED(reward)
    Q_UNUSED(button)

    if (!reward)
        return;

    RelicSelectionDialog dialog(reward->getRelicChoices(), this);

    if (dialog.exec() != QDialog::Accepted)
        return; // player closed the dialog: the reward stays pending

    Relic* chosen = dialog.getChosenRelic();

    if (!rewardSystem || !rewardSystem->claimBossRelicReward(player, reward, chosen))
        return;

    AudioManager::instance().play(AudioManager::Sound::Reward);

    rewardsLayout->removeWidget(button);
    button->deleteLater();
    MapPage::updateTopBarData();
}

void RewardPage::onContinueClicked()
{
    // Anything the player never clicked on is discarded here, per the
    // design doc: "فقط Reward هایی که بازیکن روی آن‌ها کلیک کرده به
    // دارایی‌های Player اضافه شوند".
    if (rewardSystem)
        rewardSystem->discardRemaining();

    emit continueClicked();
}
