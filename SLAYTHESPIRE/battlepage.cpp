#include "battlepage.h"
#include "ui_battlepage.h"

#include "outlinedlabel.h"


BattlePage::BattlePage(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::BattlePage)
{
    ui->setupUi(this);

    // -- Main vertical layout --
    QVBoxLayout *mainLayout = new QVBoxLayout(this);
    mainLayout->setContentsMargins(0, 0, 0, 0);
    mainLayout->setSpacing(0);

    // -- Background image (stretched to fit 1280x720) --
    QLabel *bg = new QLabel(this);
    bg->setGeometry(0, 0, 1280, 720);
    bg->setPixmap(QPixmap(":/battlebackground.png").scaled(
        1280, 720,
        Qt::IgnoreAspectRatio,
        Qt::SmoothTransformation
        ));
    bg->lower(); // push behind all other widgets

    // -- Create the three main sections --
    topBar      = new QWidget(this);
    battleField = new QWidget(this);
    bottomBar   = new QWidget(this);

    // -- Temporary semi-transparent backgrounds to visualize sections --
    topBar     ->setStyleSheet("background-color: rgba(80,80,80,180);");
    battleField->setStyleSheet("background-color: rgba(0,0,0,0);");
    bottomBar->setStyleSheet("background: transparent;");

    // -- Fixed heights (total = 720px) --
    topBar     ->setFixedHeight(50);    // thin top bar
    battleField->setFixedHeight(430);   // main battle area
    bottomBar  ->setFixedHeight(240);   // cards + energy

    mainLayout->addWidget(topBar);
    mainLayout->addWidget(battleField);
    mainLayout->addWidget(bottomBar);

    setupTopBar();
    setupBattleField();
    setupBottomBar();
}

// ─────────────────────────────────────────
void BattlePage::setupTopBar()
{
    QHBoxLayout *layout = new QHBoxLayout(topBar);
    layout->setContentsMargins(20, 6, 20, 6);
    layout->setSpacing(0);

    // ===== LEFT GROUP: name + HP + gold + potions =====
    QHBoxLayout *leftGroup = new QHBoxLayout();
    leftGroup->setSpacing(10);

    // Character name
    QLabel *nameLabel = new QLabel("anaconda", topBar);
    nameLabel->setStyleSheet("color: white; font-size: 14px; font-weight: bold; background: transparent;");

    QLabel *classLabel = new QLabel("the Ironclad", topBar);
    classLabel->setStyleSheet("color: #cfcfcf; font-size: 13px; background: transparent;");

    // HP icon + value

    QLabel *heartIcon = new QLabel(topBar);
    heartIcon->setFixedSize(45, 45);
    heartIcon->setStyleSheet("background: transparent;");
    heartIcon->setPixmap(QPixmap(":/heartIcon.png").scaled(
        45, 45,
        Qt::KeepAspectRatio,
        Qt::SmoothTransformation
        ));
    heartIcon->setAlignment(Qt::AlignCenter);


    QLabel *hpValueLabel = new QLabel("80/80", topBar);
    hpValueLabel->setStyleSheet("color: #e63946; font-size: 14px; font-weight: bold; background: transparent;");

    // Gold icon + value
    QLabel *goldIcon = new QLabel(topBar);
    goldIcon->setFixedSize(45, 45);
    goldIcon->setStyleSheet("background: transparent;");
    goldIcon->setPixmap(QPixmap(":/moneyPouch.png").scaled(
             45,45,
             Qt::KeepAspectRatio,
             Qt::SmoothTransformation
             ));
    goldIcon->setAlignment(Qt::AlignCenter);

    QLabel *goldValueLabel = new QLabel("149", topBar);
    goldValueLabel->setStyleSheet("color: #f5c518; font-size: 14px; font-weight: bold; background: transparent;");

    leftGroup->addWidget(nameLabel);
    leftGroup->addWidget(classLabel);
    leftGroup->addSpacing(15);
    leftGroup->addWidget(heartIcon);
    leftGroup->addWidget(hpValueLabel);
    leftGroup->addSpacing(10);
    leftGroup->addWidget(goldIcon);
    leftGroup->addWidget(goldValueLabel);
    leftGroup->addSpacing(15);

    // Potion slots (placeholders, 3 empty by default)
    for (int i = 0; i < 3; i++) {
        QLabel *potionSlot = new QLabel(topBar);
        potionSlot->setFixedSize(24, 24);
        potionSlot->setStyleSheet("background: rgba(255,255,255,20); border: 1px solid #666; border-radius: 4px;");
        // later: potionSlot->setPixmap(QPixmap(":/icons/potion_X.png")...);
        leftGroup->addWidget(potionSlot);
        leftGroup->addSpacing(4);
    }

    // ===== CENTER: floor icon + count =====
    QHBoxLayout *centerGroup = new QHBoxLayout();
    centerGroup->setSpacing(6);

    QLabel *floorIcon = new QLabel(topBar);
    floorIcon->setFixedSize(45, 45);
    floorIcon->setStyleSheet("background: transparent;");
    floorIcon->setPixmap(QPixmap(":/floor.png").scaled(
        45,45,
        Qt::KeepAspectRatio,
        Qt::SmoothTransformation
        ));
    floorIcon->setAlignment(Qt::AlignCenter);

    QLabel *floorCountLabel = new QLabel("10", topBar);
    floorCountLabel->setStyleSheet("color: white; font-size: 14px; font-weight: bold;  background: transparent;");

    centerGroup->addWidget(floorIcon);
    centerGroup->addWidget(floorCountLabel);

    // ===== RIGHT GROUP: notes/scroll, map, settings =====
    QHBoxLayout *rightGroup = new QHBoxLayout();
    rightGroup->setSpacing(14);

    QLabel *mapIcon = new QLabel(topBar);
    mapIcon->setFixedSize(60, 60);
    mapIcon->setStyleSheet("background: transparent; margin-top: -20px;");

    mapIcon->setPixmap(QPixmap(":/mapIcon.png").scaled(
        55,55,
        Qt::KeepAspectRatio,
        Qt::SmoothTransformation
        ));
    mapIcon->setAlignment(Qt::AlignCenter);


    QLabel *deckIcon = new QLabel(topBar);
    deckIcon->setFixedSize(45, 45);
    deckIcon->setStyleSheet("background: transparent;");
    deckIcon->setPixmap(QPixmap(":/deckIcon.png").scaled(
        45,45,
        Qt::KeepAspectRatio,
        Qt::SmoothTransformation
        ));
    deckIcon->setAlignment(Qt::AlignCenter);

    QLabel *settingsIcon = new QLabel(topBar);
    settingsIcon->setFixedSize(45, 45);
    settingsIcon->setStyleSheet("background: transparent;");
    settingsIcon->setPixmap(QPixmap(":/settingicon.png").scaled(
        45,45,
        Qt::KeepAspectRatio,
        Qt::SmoothTransformation
        ));
    settingsIcon->setAlignment(Qt::AlignCenter);

    rightGroup->addWidget(mapIcon);
    rightGroup->addWidget(deckIcon);
    rightGroup->addWidget(settingsIcon);

    // ===== Assemble =====
    layout->addLayout(leftGroup);
    layout->addStretch();
    layout->addLayout(centerGroup);
    layout->addStretch();
    layout->addLayout(rightGroup);
}


// ─────────────────────────────────────────
void BattlePage::setupBattleField()
{
    QHBoxLayout *layout = new QHBoxLayout(battleField);
    layout->setContentsMargins(140, 0, 140, 0);

    // -- Player widget (left side) --
    QWidget *playerWidget = new QWidget(battleField);
    playerWidget->setFixedSize(250, 330);
    playerWidget->setStyleSheet("background: transparent;");
    QVBoxLayout *playerLayout = new QVBoxLayout(playerWidget);

    QLabel *playerImg = new QLabel(playerWidget);
    playerImg->setPixmap(QPixmap(":/ironclad.png").scaled(
        230, 290,
        Qt::KeepAspectRatio,
        Qt::SmoothTransformation
        ));
    playerImg->setAlignment(Qt::AlignCenter);

    // Player HP bar
    QProgressBar *playerHP = new QProgressBar(playerWidget);
    playerHP->setRange(0, 80);
    playerHP->setValue(80);
    playerHP->setFixedSize(170, 16);
    playerHP->setTextVisible(true);
    playerHP->setFormat("%v / %m");
    playerHP->setStyleSheet(
        "QProgressBar { background: #1a1a1a; border: 2px solid #333;"
        "border-radius: 6px; color: white; font-size: 12px; text-align: center; }"
        "QProgressBar::chunk { background: #e63946; border-radius: 4px; }"
        );

    playerLayout->addStretch();
    playerLayout->addWidget(playerImg);
    playerLayout->addWidget(playerHP, 0, Qt::AlignHCenter);

    // -- Enemy container (right side) --
    QWidget *enemyContainer = new QWidget(battleField);
    QHBoxLayout *enemyLayout = new QHBoxLayout(enemyContainer);
    enemyLayout->setAlignment(Qt::AlignCenter | Qt::AlignBottom);
    enemyLayout->setSpacing(40);

    // Single placeholder enemy widget
    QWidget *enemyWidget = new QWidget(enemyContainer);
    enemyWidget->setFixedSize(220, 300);
    enemyWidget->setStyleSheet("background: transparent;");
    QVBoxLayout *enemyInnerLayout = new QVBoxLayout(enemyWidget);
    enemyInnerLayout->setContentsMargins(0, 0, 0, 10);
    enemyInnerLayout->setSpacing(5);

    // Enemy intent (attack/defend indicator)
    QLabel *intentLabel = new QLabel("⚔ 12", enemyWidget);
    intentLabel->setAlignment(Qt::AlignCenter);
    intentLabel->setFixedHeight(30);
    intentLabel->setStyleSheet(
        "color: #f87171; font-size: 16px; font-weight: bold;"
        "background: rgba(0,0,0,120); border-radius: 8px; padding: 4px;"
        );

    QLabel *enemyImg = new QLabel(enemyWidget);
    enemyImg->setFixedSize(190, 210);
    enemyImg->setAlignment(Qt::AlignCenter);
    enemyImg->setStyleSheet(
        "background: qlineargradient(x1:0, y1:0, x2:1, y2:1,"
        "stop:0 #3b82f6, stop:1 #1e40af);"
        "border: 3px dashed #60a5fa; border-radius: 12px;"
        );

    enemyImg->setText("Enemy Sprite");
    enemyImg->setStyleSheet(enemyImg->styleSheet() + "color: white; font-size: 14px;");


    // Enemy HP bar
    QProgressBar *enemyHP = new QProgressBar(enemyWidget);
    enemyHP->setRange(0, 50);
    enemyHP->setValue(50);
    enemyHP->setFixedSize(170, 16);
    enemyHP->setTextVisible(true);
    enemyHP->setFormat("%v / %m");
    enemyHP->setStyleSheet(
        "QProgressBar { background: #1a1a1a; border: 2px solid #333;"
        "border-radius: 6px; color: white; font-size: 12px; text-align: center; }"
        "QProgressBar::chunk { background: #e63946; border-radius: 4px; }"
        );
    enemyInnerLayout->addStretch();
    enemyInnerLayout->addWidget(intentLabel);
    enemyInnerLayout->addStretch();
    enemyInnerLayout->addWidget(enemyImg);
    enemyInnerLayout->addWidget(enemyHP, 0, Qt::AlignHCenter);

    enemyLayout->addWidget(enemyWidget);

    layout->addWidget(playerWidget, 0, Qt::AlignBottom);
    layout->addStretch(2);
    layout->addWidget(enemyContainer, 0, Qt::AlignBottom);
}

// ─────────────────────────────────────────
void BattlePage::setupBottomBar()
{
    QVBoxLayout *layout = new QVBoxLayout(bottomBar);
    layout->setContentsMargins(50, 10, 50, 15);
    layout->setSpacing(10);

    // -- Top row: energy orb (left) + End Turn button (right) --
    QHBoxLayout *controlRow = new QHBoxLayout();

    QLabel *energyLabel = new QLabel(bottomBar);
    energyLabel->setFixedSize(130, 130);
    energyLabel->setStyleSheet("background: transparent;");
    energyLabel->setPixmap(QPixmap(":/energybutton.png").scaled(
        130, 130,
        Qt::KeepAspectRatio,
        Qt::SmoothTransformation
        ));
    energyLabel->setAlignment(Qt::AlignCenter);

    OutlinedLabel *energyValueLabel = new OutlinedLabel("3/3", energyLabel);
    energyValueLabel->setFixedSize(130, 130);
    energyValueLabel->move(0, 0);
    energyValueLabel->raise();


    QPushButton *endTurnBtn = new QPushButton("End Turn", bottomBar);
    endTurnBtn->setFixedSize(140, 50);
    endTurnBtn->setStyleSheet(
        "QPushButton { background-color: #b91c1c; color: white; font-size: 16px;"
        "border: 3px solid #7f1d1d; border-radius: 10px; font-weight: bold; }"
        "QPushButton:hover { background-color: #dc2626; }"
        "QPushButton:pressed { background-color: #450a0a; }"
        );

    controlRow->addWidget(energyLabel);
    controlRow->addStretch();
    controlRow->addWidget(endTurnBtn);

    // -- Bottom row: placeholder hand cards --
    QHBoxLayout *handRow = new QHBoxLayout();
    handRow->setAlignment(Qt::AlignCenter);
    handRow->setSpacing(12);

    // 5 placeholder cards
    for (int i = 0; i < 5; i++) {
        QPushButton *card = new QPushButton(QString("Card %1").arg(i + 1), bottomBar);
        card->setFixedSize(110, 160);
        card->setStyleSheet(
            "QPushButton {"
            "  background: qlineargradient(x1:0, y1:0, x2:0, y2:1,"
            "  stop:0 #1e3a5f, stop:1 #0f1c2e);"
            "  color: white; font-size: 13px; font-weight: bold;"
            "  border: 3px solid #3b82f6; border-radius: 10px;"
            "}"
            "QPushButton:hover {"
            "  background: qlineargradient(x1:0, y1:0, x2:0, y2:1,"
            "  stop:0 #2d5a8e, stop:1 #1e3a5f);"
            "  margin-bottom: 15px;"
            "  border-color: #60a5fa;"
            "}"
            "QPushButton:pressed {"
            "  background: #1e40af;"
            "  margin-bottom: 0px;"
            "}"
            );
        handRow->addWidget(card);
    }

    layout->addLayout(controlRow);
    layout->addLayout(handRow);
}

BattlePage::~BattlePage()
{
    delete ui;
}