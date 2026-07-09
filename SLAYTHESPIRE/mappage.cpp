#include "mappage.h"
#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QScrollBar>
#include "player.h"
#include "potion.h"

static const QString BONE_COLOR = "#E8DCC0";

MapPage::MapPage(Map* gameMap, Player* playerPtr, QWidget *parent)
    : QWidget(parent), map(gameMap), player(playerPtr)
{
    setupUI();
    updateTopBarData();
}

void MapPage::setupUI() {
    setObjectName("MapPage");
    setStyleSheet(
        "#MapPage {"
        "background-image: url(:/map/map.png);"
        "background-repeat: no-repeat;"
        "background-position: center;"
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
        "background-color: rgba(20,20,20,200);"
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

    relicBtn = new QPushButton("💎 x0");
    relicBtn->setCursor(Qt::PointingHandCursor);
    relicBtn->setStyleSheet(
        "QPushButton { background-color: transparent; font-size: 16px; "
        "font-weight: bold; color: " + BONE_COLOR + "; border: none; }");
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
        potionSlots[i]->setStyleSheet("border-image: url(:/map/potionEmpty.png); background: transparent;");
        potionsLayout->addWidget(potionSlots[i]);
    }

    topBarLayout->addWidget(hpBar);
    topBarLayout->addSpacing(15);
    topBarLayout->addWidget(goldLabel);
    topBarLayout->addSpacing(15);
    topBarLayout->addWidget(relicBtn);
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
    floorIconLabel->setStyleSheet("border-image: url(:/map/floorIcon.png); background: transparent;");

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
        "}");
    connect(deckBtn, &QPushButton::clicked, this, &MapPage::onDeckButtonClicked);

    settingsBtn = new QPushButton();
    settingsBtn->setFixedSize(40, 40);
    settingsBtn->setCursor(Qt::PointingHandCursor);
    settingsBtn->setStyleSheet("QPushButton { border-image: url(:/map/settingBtnMap.png); }");
    connect(settingsBtn, &QPushButton::clicked, this, &MapPage::onSettingsButtonClicked);

    topBarLayout->addWidget(deckBtn);
    topBarLayout->addSpacing(10);
    topBarLayout->addWidget(settingsBtn);

    mainLayout->addWidget(topBarWidget);
}

void MapPage::updateTopBarData() {
    hpBar->setMaximum(player->getMaxHealth());
    hpBar->setValue(player->getCurrentHealth());
    hpBar->setFormat(QString("%1 / %2").arg(player->getCurrentHealth()).arg(player->getMaxHealth()));

    goldLabel->setText(QString("💰 %1").arg(player->getGold()));

    relicBtn->setText(QString("💎 x%1").arg(player->getAllRelics().size()));

    actLabel->setText(QString("Act %1").arg(map->getCurrentAct()));
    floorNumberLabel->setText(QString::number(map->getCurrentFloorIndex()));

    for(int i = 0; i < 3; ++i) {
        if(i < player->getPotionCount() && player->getPotion(i) != nullptr) {
            QString potionName = player->getPotion(i)->getName().toLower().replace(" ", "_");
            QString iconPath = QString(":/map/%1.png").arg(potionName);
            potionSlots[i]->setStyleSheet(QString("border-image: url(%1); background: transparent;").arg(iconPath));
        } else {
            potionSlots[i]->setStyleSheet("border-image: url(:/map/potionEmpty.png); background: transparent;");
        }
    }
}

void MapPage::onNodeClicked(int nodeId) {
    bool moved = map->moveToNode(nodeId);
    if (moved) {
        updateTopBarData();
        mapWidget->refreshUI();
        //TODO (GameManager)
        //emit roomEntered(map->getCurrentNode()->getType());
    }
}

void MapPage::onRelicButtonClicked() {
    //TODO:
}

void MapPage::onDeckButtonClicked() {
    // TODO:
}

void MapPage::onSettingsButtonClicked() {
    // TODO:
}