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
    layout->setContentsMargins(30, 5, 30, 5);

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

    QLabel *energyLabel = new QLabel("3 / 3", bottomBar);
    energyLabel->setFixedSize(70, 70);
    energyLabel->setAlignment(Qt::AlignCenter);
    energyLabel->setStyleSheet(
        "background: qradialgradient(cx:0.5, cy:0.5, radius:0.8,"
        "stop:0 #fbbf24, stop:0.7 #f59e0b, stop:1 #b45309);"
        "border: 4px solid #78350f; border-radius: 35px;"
        "color: white; font-size: 20px; font-weight: bold;"
        );

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