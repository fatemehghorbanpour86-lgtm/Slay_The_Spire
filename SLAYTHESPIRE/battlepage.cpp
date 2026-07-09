#include "battlepage.h"
#include "ui_battlepage.h"

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
    bottomBar  ->setStyleSheet("background-color: rgba(30,30,30,180);");

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
    layout->setContentsMargins(10, 5, 10, 5);

    // Left: player HP
    QLabel *hpLabel = new QLabel("HP: 80 / 80", topBar);
    hpLabel->setStyleSheet("color: white; font-size: 14px;");

    // Center: turn counter
    QLabel *turnLabel = new QLabel("Turn 1", topBar);
    turnLabel->setStyleSheet("color: #facc15; font-size: 14px;");
    turnLabel->setAlignment(Qt::AlignCenter);

    // Right: deck and discard pile counts
    QLabel *deckLabel = new QLabel("Deck: 10   Discard: 0", topBar);
    deckLabel->setStyleSheet("color: white; font-size: 14px;");
    deckLabel->setAlignment(Qt::AlignRight);

    layout->addWidget(hpLabel);
    layout->addStretch();
    layout->addWidget(turnLabel);
    layout->addStretch();
    layout->addWidget(deckLabel);
}

// ─────────────────────────────────────────
void BattlePage::setupBattleField()
{
    QHBoxLayout *layout = new QHBoxLayout(battleField);
    layout->setContentsMargins(40, 20, 40, 20);

    // -- Player widget (left side) --
    QWidget *playerWidget = new QWidget(battleField);
    playerWidget->setFixedSize(120, 200);
    playerWidget->setStyleSheet("background-color: rgba(0,100,200,120); border-radius: 8px;");

    QVBoxLayout *playerLayout = new QVBoxLayout(playerWidget);

    QLabel *playerImg = new QLabel("Player", playerWidget);
    playerImg->setAlignment(Qt::AlignCenter);
    playerImg->setStyleSheet("color: white; font-size: 13px;");

    // Player HP bar
    QProgressBar *playerHP = new QProgressBar(playerWidget);
    playerHP->setRange(0, 80);
    playerHP->setValue(80);
    playerHP->setFixedHeight(14);
    playerHP->setStyleSheet(
        "QProgressBar { background: #333; border-radius: 4px; }"
        "QProgressBar::chunk { background: #e63946; border-radius: 4px; }"
        );

    playerLayout->addStretch();
    playerLayout->addWidget(playerImg);
    playerLayout->addWidget(playerHP);

    // -- Enemy container (right side) --
    QWidget *enemyContainer = new QWidget(battleField);
    QHBoxLayout *enemyLayout = new QHBoxLayout(enemyContainer);
    enemyLayout->setAlignment(Qt::AlignCenter);
    enemyLayout->setSpacing(30);

    // Single placeholder enemy widget
    QWidget *enemyWidget = new QWidget(enemyContainer);
    enemyWidget->setFixedSize(120, 200);
    enemyWidget->setStyleSheet("background-color: rgba(200,0,0,120); border-radius: 8px;");

    QVBoxLayout *enemyInnerLayout = new QVBoxLayout(enemyWidget);

    // Enemy intent (attack/defend indicator)
    QLabel *intentLabel = new QLabel("Attack: 12", enemyWidget);
    intentLabel->setAlignment(Qt::AlignCenter);
    intentLabel->setStyleSheet("color: #facc15; font-size: 13px;");

    QLabel *enemyImg = new QLabel("Enemy", enemyWidget);
    enemyImg->setAlignment(Qt::AlignCenter);
    enemyImg->setStyleSheet("color: white; font-size: 13px;");

    // Enemy HP bar
    QProgressBar *enemyHP = new QProgressBar(enemyWidget);
    enemyHP->setRange(0, 50);
    enemyHP->setValue(50);
    enemyHP->setFixedHeight(14);
    enemyHP->setStyleSheet(
        "QProgressBar { background: #333; border-radius: 4px; }"
        "QProgressBar::chunk { background: #e63946; border-radius: 4px; }"
        );

    enemyInnerLayout->addWidget(intentLabel);
    enemyInnerLayout->addStretch();
    enemyInnerLayout->addWidget(enemyImg);
    enemyInnerLayout->addWidget(enemyHP);

    enemyLayout->addWidget(enemyWidget);

    layout->addWidget(playerWidget);
    layout->addStretch();
    layout->addWidget(enemyContainer);
}

// ─────────────────────────────────────────
void BattlePage::setupBottomBar()
{
    QVBoxLayout *layout = new QVBoxLayout(bottomBar);
    layout->setContentsMargins(10, 5, 10, 10);
    layout->setSpacing(5);

    // -- Top row: energy label (left) + End Turn button (right) --
    QHBoxLayout *controlRow = new QHBoxLayout();

    QLabel *energyLabel = new QLabel("Energy: 3 / 3", bottomBar);
    energyLabel->setStyleSheet("color: #a78bfa; font-size: 18px; font-weight: bold;");

    QPushButton *endTurnBtn = new QPushButton("End Turn", bottomBar);
    endTurnBtn->setFixedSize(120, 40);
    endTurnBtn->setStyleSheet(
        "QPushButton { background-color: #b91c1c; color: white; font-size: 14px;"
        "border-radius: 6px; font-weight: bold; }"
        "QPushButton:hover { background-color: #dc2626; }"
        "QPushButton:pressed { background-color: #7f1d1d; }"
        );

    controlRow->addWidget(energyLabel);
    controlRow->addStretch();
    controlRow->addWidget(endTurnBtn);

    // -- Bottom row: placeholder hand cards --
    QHBoxLayout *handRow = new QHBoxLayout();
    handRow->setAlignment(Qt::AlignCenter);
    handRow->setSpacing(10);

    // 5 placeholder cards
    for (int i = 0; i < 5; i++) {
        QPushButton *card = new QPushButton(QString("Card %1").arg(i + 1), bottomBar);
        card->setFixedSize(90, 130);
        card->setStyleSheet(
            "QPushButton { background-color: #1e3a5f; color: white; font-size: 12px;"
            "border: 2px solid #60a5fa; border-radius: 8px; }"
            "QPushButton:hover { background-color: #2d5a8e; margin-bottom: 10px; }"
            "QPushButton:pressed { background-color: #1e40af; }"
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