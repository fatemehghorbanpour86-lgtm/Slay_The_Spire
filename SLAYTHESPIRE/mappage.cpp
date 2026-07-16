#include "mappage.h"
#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QScrollBar>
#include "player.h"
#include "potion.h"
#include "audiomanager.h"
#include "relicviewer.h"
#include "deckviewer.h"
#include "rewardpage.h"
#include "rewardsystem.h"

MapPage* MapPage::instance = nullptr;

static const QString BONE_COLOR = "#E8DCC0";

MapPage::MapPage(Map* gameMap, Player* playerPtr, QWidget *parent)
    : QWidget(parent), map(gameMap), player(playerPtr)
{
    instance = this;
    setupUI();
    updateTopBarData();
}

MapPage::~MapPage() {
    if (instance == this) {
        instance = nullptr;
    }
}

void MapPage::setupUI() {
    setObjectName("MapPage");
    setStyleSheet(
        "#MapPage {"
        "background-image: url(:/map/map.png);"
        "background-repeat: no-repeat;"
        "background-position: center;"
        "}"
        "QToolTip { color: #facc15; background-color: #1f2937; border: 1px solid #b91c1c;"
        "border-radius: 4px; padding: 6px; font-weight: bold; font-family: Tahoma;"
        "}"
        );

    QVBoxLayout* mainLayout = new QVBoxLayout(this);
    mainLayout->setContentsMargins(0, 0, 0, 0);
    mainLayout->setSpacing(0);

    createTopBar(mainLayout);

    QScrollArea* scrollArea = new QScrollArea(this);
    scrollArea->setFrameShape(QFrame::NoFrame);
    scrollArea->setStyleSheet("QScrollArea { background: transparent; border: none; }");
    scrollArea->setWidgetResizable(true);
    scrollArea->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);

    // viewport به‌صورت پیش‌فرض bg خودش رو پر (opaque) می‌کنه و
    // جلوی دیده‌شدن عکس مپِ پشت MapPage رو می‌گیره - خاموشش می‌کنیم
    scrollArea->viewport()->setAutoFillBackground(false);
    scrollArea->viewport()->setStyleSheet("background: transparent;");

    mapWidget = new MapWidget(map, this);
    scrollArea->setWidget(mapWidget);

    QScrollBar *vScrollBar = scrollArea->verticalScrollBar();
    vScrollBar->setValue(vScrollBar->maximum());

    mainLayout->addWidget(scrollArea);

    connect(mapWidget, &MapWidget::nodeClicked, this, &MapPage::onNodeClicked);
}

void MapPage::createTopBar(QVBoxLayout* mainLayout) {
    topBarWidget = new QWidget(this);
    topBarWidget->setFixedHeight(60);
    topBarWidget->setObjectName("TopBar");
    topBarWidget->setStyleSheet(
        "#TopBar {"
        "background-color: #2C444F ;"
        "}"
        );
    // QWidget خام بدون این attribute، background-color رو رندر نمی‌کنه
    // (برخلاف QFrame/QLabel که خودشون استایل‌شیت رو پشتیبانی می‌کنن)
    topBarWidget->setAttribute(Qt::WA_StyledBackground, true);

    QHBoxLayout* topBarLayout = new QHBoxLayout(topBarWidget);
    topBarLayout->setContentsMargins(20, 5, 20, 5);

    // --- قسمت چپ: اطلاعات بازیکن ---
    hpBar = new QProgressBar();
    hpBar->setFixedSize(150, 20);
    hpBar->setTextVisible(true);
    hpBar->setAlignment(Qt::AlignCenter);
    hpBar->setStyleSheet(
        "QProgressBar { border: 1px solid #555; border-radius: 3px; "
        "background-color: #333; color: " + BONE_COLOR + "; font-weight: bold; }"
                       "QProgressBar::chunk { background-color: #e74c3c; width: 1px; }");

    goldLabel = new QLabel("💰 99");
    goldLabel->setStyleSheet("font-size: 16px; font-weight: bold; color: " + BONE_COLOR + ";");

    relicBtn = new QPushButton();
    relicBtn->setFixedSize(30, 25);
    relicBtn->setCursor(Qt::PointingHandCursor);
    relicBtn->setStyleSheet(
        "QPushButton {"
        "border: none;"
        "border-image: url(:/map/relicIcon.png);"
        "}"
        "QPushButton:pressed { "
        "   margin: 3px 3px 3px 3px; "
        "}"
        );

    relicLabel = new QLabel("x0");
    relicLabel->setStyleSheet("font-size: 16px; font-weight: bold; color: " + BONE_COLOR + ";");

    connect(relicBtn, &QPushButton::pressed,
            this, []()
            {
                AudioManager::instance().play(AudioManager::Sound::ButtonClick);
            });
    connect(relicBtn, &QPushButton::clicked, this, &MapPage::onRelicButtonClicked);

    // پنل تیره با گوشه‌های گرد پشت پوشن‌ها (QFrame خودش QSS رو رندر می‌کنه)
    potionsPanel = new QFrame();
    potionsPanel->setObjectName("PotionsPanel");
    potionsPanel->setStyleSheet(
        "#PotionsPanel {"
        "background-color: rgba(0,0,0,140);"
        "border: 1px solid rgba(255,255,255,40);"
        "border-radius: 10px;"
        "}"
        );
    QHBoxLayout* potionsLayout = new QHBoxLayout(potionsPanel);
    potionsLayout->setContentsMargins(8, 4, 8, 4);
    potionsLayout->setSpacing(6);

    for (int i = 0; i < 3; ++i) {
        potionSlots[i] = new QLabel();
        potionSlots[i]->setFixedSize(32, 32);
        potionSlots[i]->setStyleSheet("border-image: url(:/Potion/potionEmpty.png); background: transparent;");
        potionsLayout->addWidget(potionSlots[i]);
    }

    topBarLayout->addWidget(hpBar);
    topBarLayout->addSpacing(15);
    topBarLayout->addWidget(goldLabel);
    topBarLayout->addSpacing(15);
    topBarLayout->addWidget(relicBtn);
    topBarLayout->addSpacing(0);
    topBarLayout->addWidget(relicLabel);
    topBarLayout->addSpacing(15);
    topBarLayout->addWidget(potionsPanel);
    topBarLayout->addStretch();

    // --- قسمت وسط: Act X | آیکون فلور | شماره فلور ---
    QHBoxLayout* centerLayout = new QHBoxLayout();
    centerLayout->setSpacing(8);

    actLabel = new QLabel("Act 1");
    actLabel->setStyleSheet("font-size: 18px; font-weight: bold; color: " + BONE_COLOR + ";");

    floorIconLabel = new QLabel();
    floorIconLabel->setFixedSize(24, 24);
    floorIconLabel->setStyleSheet(
        "border-image: url(:/map/floorIcon.png); background: transparent;");

    floorNumberLabel = new QLabel("0");
    floorNumberLabel->setStyleSheet("font-size: 18px; font-weight: bold; color: " + BONE_COLOR + ";");

    centerLayout->addWidget(actLabel);
    centerLayout->addSpacing(6);
    centerLayout->addWidget(floorIconLabel);
    centerLayout->addWidget(floorNumberLabel);

    topBarLayout->addLayout(centerLayout);
    topBarLayout->addStretch();

    // --- قسمت راست: Deck و Settings ---
    deckBtn = new QPushButton();
    deckBtn->setFixedSize(40, 40);
    deckBtn->setCursor(Qt::PointingHandCursor);
    deckBtn->setStyleSheet(
        "QPushButton {"
        "border: none;"
        "border-image: url(:/map/cardsBtn.png);"
        "}"
        "QPushButton:pressed { "
        "   margin: 5px 5px 5px 5px; "
        "}"
        );

    connect(deckBtn, &QPushButton::pressed,
            this, []()
            {
                AudioManager::instance().play(AudioManager::Sound::ButtonClick);
            });
    connect(deckBtn, &QPushButton::clicked, this, &MapPage::onDeckButtonClicked);
    
    cardLabel = new QLabel("0", this);
    cardLabel->setStyleSheet("font-size: 18px; font-weight: bold; color: " + BONE_COLOR + ";");
    cardLabel->move(1185, 26);

    settingsBtn = new QPushButton();
    settingsBtn->setFixedSize(45, 45);
    settingsBtn->setCursor(Qt::PointingHandCursor);
    settingsBtn->setStyleSheet(
        "QPushButton { border-image: url(:/map/settingBtnMap.png); }"
        "QPushButton:pressed { "
        "   margin: 5px 5px 5px 5px; "
        "}"
        );
    connect(settingsBtn, &QPushButton::pressed,
            this, []()
            {
                AudioManager::instance().play(AudioManager::Sound::ButtonClick);
            });
    connect(settingsBtn, &QPushButton::clicked, this, &MapPage::onSettingsButtonClicked);

    topBarLayout->addWidget(deckBtn);
    topBarLayout->addSpacing(10);
    topBarLayout->addWidget(settingsBtn);

    mainLayout->addWidget(topBarWidget);
}

void MapPage::updateTopBarData() {

    if (!instance) return;

    instance->hpBar->setMaximum(instance->player->getMaxHealth());
    instance->hpBar->setValue(instance->player->getCurrentHealth());
    instance->hpBar->setFormat(QString("%1 / %2").arg(instance->player->getCurrentHealth()).arg(instance->player->getMaxHealth()));

    double hpPercent = static_cast<double>(instance->player->getCurrentHealth()) /
                       instance->player->getMaxHealth();

    QString chunkColor;
    QString textColor;

    if (hpPercent > 0.5) {
        chunkColor = "#2ecc71";
        textColor = BONE_COLOR;
    }
    else if (hpPercent > 0.25) {
        chunkColor = "#f1c40f";
        textColor = "#000000";
    }
    else {
        chunkColor = "#e74c3c";
        textColor = BONE_COLOR;
    }

    instance->hpBar->setStyleSheet(QString(
                             "QProgressBar { border: 1px solid #555; border-radius: 10px;"
                             "background-color: #222; color: %1;"
                             "font-weight: bold; text-align: center;"
                             "}"
                             "QProgressBar::chunk {"
                             "background-color: %2;"
                             "border-radius: 8px;"
                             "margin: 1px;"
                             "}"
                             ).arg(textColor, chunkColor));

    instance->goldLabel->setText(QString("💰 %1").arg(instance->player->getGold()));

    instance->relicLabel->setText(QString("x%1").arg(instance->player->getAllRelics().size()));

    instance->cardLabel->setText(QString("%1").arg(instance->player->getMasterDeck()->getCardCount()));

    instance->actLabel->setText(QString("Act %1").arg(instance->map->getCurrentAct()));
    instance->floorNumberLabel->setText(QString::number(instance->map->getCurrentFloorIndex()));

    for(int i = 0; i < 3; ++i) {
        if(i < instance->player->getPotionCount() && instance->player->getPotion(i) != nullptr) {
            QString potionName = instance->player->getPotion(i)->getName().toLower().replace(" ", "_");
            QString iconPath = QString(":/Potion/%1.png").arg(potionName);
            instance->potionSlots[i]->setStyleSheet(QString("border-image: url(%1); background: transparent;").arg(iconPath));
            instance->potionSlots[i]->setFixedSize(42, 42);
        } else {
            instance->potionSlots[i]->setStyleSheet("border-image: url(:/Potion/potionEmpty.png); background: transparent;");
        }
    }
}

Player* MapPage::getPlayer() const
{
    return player;
}

Map* MapPage::getMap() const
{
    return map;
}


void MapPage::onNodeClicked(int nodeId) {
    bool moved = map->moveToNode(nodeId);
    if (moved) {
        updateTopBarData();
        mapWidget->refreshUI();
        //TODO (GameManager)
        //emit roomEntered(map->getCurrentNode()->getType());

        // const MapNode* node = map->getCurrentNode();
        // if (node && node->getType() == NodeType::Monster)
        // {
        //     emit battleRequested();
        // }

        // if (map->getCurrentNodeType() == NodeType::Monster)
        // {
        //     RewardSystem* rewardSystem = new RewardSystem();
        //     rewardSystem->generateNormalReward(player);

        //     RewardPage* rewardPage = new RewardPage(player, rewardSystem);

        //     rewardPage->setWindowTitle("Reward");
        //     rewardPage->setAttribute(Qt::WA_DeleteOnClose);

        //     connect(rewardPage, &RewardPage::continueClicked, [rewardPage, rewardSystem]() {
        //         rewardPage->close();
        //         rewardPage->deleteLater();
        //         delete rewardSystem;
        //     });

        //     rewardPage->show();
        // }

        // else if (map->getCurrentNodeType() == NodeType::Elite)
        // {
        //     RewardSystem* rewardSystem = new RewardSystem();
        //     rewardSystem->generateEliteReward(player);

        //     RewardPage* rewardPage = new RewardPage(player, rewardSystem);

        //     rewardPage->setWindowTitle("Reward");
        //     rewardPage->setAttribute(Qt::WA_DeleteOnClose);

        //     connect(rewardPage, &RewardPage::continueClicked, [rewardPage, rewardSystem]() {
        //         rewardPage->close();
        //         rewardPage->deleteLater();
        //         delete rewardSystem;
        //     });

        //     rewardPage->show();
        // }

        // if (map->getCurrentNodeType() == NodeType::Boss)
        // {
        //     RewardSystem* rewardSystem = new RewardSystem();
        //     rewardSystem->generateBossReward(player);

        //     RewardPage* rewardPage = new RewardPage(player, rewardSystem);

        //     rewardPage->setWindowTitle("Reward");
        //     rewardPage->setAttribute(Qt::WA_DeleteOnClose);

        //     connect(rewardPage, &RewardPage::continueClicked, [rewardPage, rewardSystem]() {
        //         rewardPage->close();
        //         rewardPage->deleteLater();
        //         delete rewardSystem;
        //     });

        //     rewardPage->show();
        // }

        // else if (map->getCurrentNodeType() == NodeType::Campfire)
        // {
        //     emit campfireEntered();
        // }
    }
}

void MapPage::onRelicButtonClicked() {
    RelicViewerDialog dialog(player, this);
    dialog.exec();
}

void MapPage::onDeckButtonClicked() {
    DeckViewerDialog dialog(player, this);
    dialog.exec();
}

void MapPage::onSettingsButtonClicked() {
    // TODO:
}