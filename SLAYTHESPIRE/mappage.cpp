#include "mappage.h"
#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QScrollBar>
#include "player.h"
#include "potion.h"

MapPage::MapPage(Map* gameMap, Player* playerPtr, QWidget *parent)
    : QWidget(parent), map(gameMap), player(playerPtr)
{
    setupUI();
    updateTopBarData();
}

void MapPage::setupUI() {

    setStyleSheet("MapPage { background-image: url(map/map.png); background-position: center; }");

    QVBoxLayout* mainLayout = new QVBoxLayout(this);
    mainLayout->setContentsMargins(0, 0, 0, 0);
    mainLayout->setSpacing(0);

    createTopBar(mainLayout);

    QScrollArea* scrollArea = new QScrollArea(this);
    scrollArea->setStyleSheet("QScrollArea { background: transparent; border: none; }");
    scrollArea->setWidgetResizable(true);
    scrollArea->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);

    mapWidget = new MapWidget(map, this);
    scrollArea->setWidget(mapWidget);

    QScrollBar *vScrollBar = scrollArea->verticalScrollBar();
    vScrollBar->setValue(vScrollBar->maximum());

    mainLayout->addWidget(scrollArea);

    connect(mapWidget, &MapWidget::nodeClicked, this, &MapPage::onNodeClicked);
}

void MapPage::createTopBar(QVBoxLayout* mainLayout) {
    QWidget* topBarWidget = new QWidget(this);
    topBarWidget->setFixedHeight(60);
    topBarWidget->setStyleSheet("background-color: rgba(20, 20, 20, 200); color: white;");

    QHBoxLayout* topBarLayout = new QHBoxLayout(topBarWidget);
    topBarLayout->setContentsMargins(20, 5, 20, 5);

    // --- قسمت چپ: اطلاعات بازیکن ---
    hpBar = new QProgressBar();
    hpBar->setFixedSize(150, 20);
    hpBar->setTextVisible(true);
    hpBar->setAlignment(Qt::AlignCenter);
    hpBar->setStyleSheet("QProgressBar { border: 1px solid #555; border-radius: 3px; background-color: #333; }"
                         "QProgressBar::chunk { background-color: #e74c3c; width: 1px; }");

    goldLabel = new QLabel("💰 99");
    goldLabel->setStyleSheet("font-size: 16px; font-weight: bold;");

    relicBtn = new QPushButton("💎 x0");
    relicBtn->setCursor(Qt::PointingHandCursor);
    relicBtn->setStyleSheet("QPushButton { background-color: transparent; font-size: 16px; font-weight: bold; color: white; border: none; }");
    connect(relicBtn, &QPushButton::clicked, this, &MapPage::onRelicButtonClicked);

    QHBoxLayout* potionsLayout = new QHBoxLayout();
    for (int i = 0; i < 3; ++i) {
        potionSlots[i] = new QLabel();
        potionSlots[i]->setFixedSize(32, 32);
        potionSlots[i]->setStyleSheet("background-image: url(assets/ui/icons/potion_empty.png); background-position: center; background-repeat: no-repeat;");
        potionsLayout->addWidget(potionSlots[i]);
    }

    topBarLayout->addWidget(hpBar);
    topBarLayout->addSpacing(15);
    topBarLayout->addWidget(goldLabel);
    topBarLayout->addSpacing(15);
    topBarLayout->addWidget(relicBtn);
    topBarLayout->addSpacing(15);
    topBarLayout->addLayout(potionsLayout);
    topBarLayout->addStretch(); // هل دادن به سمت چپ

    // --- قسمت وسط: اطلاعات نقشه ---
    QHBoxLayout* centerLayout = new QHBoxLayout();
    floorIconLabel = new QLabel();
    floorIconLabel->setFixedSize(24, 24);
    floorIconLabel->setStyleSheet("background-image: url(map/floorIcon.png); background-position: center; background-repeat: no-repeat;");

    floorInfoLabel = new QLabel("Act 1  Floor 0 / 15");
    floorInfoLabel->setStyleSheet("font-size: 18px; font-weight: bold;");

    centerLayout->addWidget(floorIconLabel);
    centerLayout->addWidget(floorInfoLabel);

    topBarLayout->addLayout(centerLayout);
    topBarLayout->addStretch(); // هل دادن به سمت راست

    // --- قسمت راست: Settings و Deck ---
    deckBtn = new QPushButton();
    deckBtn->setFixedSize(40, 40);
    deckBtn->setCursor(Qt::PointingHandCursor);
    deckBtn->setStyleSheet("QPushButton { background-image: url(map/cardsBtn.png); background-color: transparent; border: none; }");
    connect(deckBtn, &QPushButton::clicked, this, &MapPage::onDeckButtonClicked);

    settingsBtn = new QPushButton();
    settingsBtn->setFixedSize(40, 40);
    settingsBtn->setCursor(Qt::PointingHandCursor);
    settingsBtn->setStyleSheet("QPushButton { background-image: url(map/settingBtnMap.png); background-color: transparent; border: none; }");
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

    // Update Gold
    goldLabel->setText(QString("💰 %1").arg(player->getGold()));

    // Update Relic Count
    relicBtn->setText(QString("💎 x%1").arg(player->getAllRelics().size()));

    floorInfoLabel->setText(QString("Act %1  Floor %2 / 15")
                                .arg(map->getCurrentAct())
                                .arg(map->getCurrentFloorIndex()));

    for(int i = 0; i < 3; ++i) {
        if(i < player->getPotionCount() && player->getPotion(i) != nullptr) {
            // Converts "Health Potion" to "health_potion" for the file name
            QString potionName = player->getPotion(i)->getName().toLower().replace(" ", "_");
            QString iconPath = QString("map/%1.png").arg(potionName);
            potionSlots[i]->setStyleSheet(QString("background-image: url(%1); background-position: center; background-repeat: no-repeat;").arg(iconPath));
        } else {
            potionSlots[i]->setStyleSheet("background-image: url(map/potionEmpty.png); background-position: center; background-repeat: no-repeat;");
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
