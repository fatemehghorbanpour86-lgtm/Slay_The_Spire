#include "battlepage.h"
#include "combatdeck.h"
#include "enemy.h"
#include "normalenemies.h"
#include "player.h"

#include "outlinedlabel.h"

#include <QPropertyAnimation>
#include <QGraphicsProxyWidget>
#include <QGraphicsView>
#include <QGraphicsScene>
#include <QGraphicsDropShadowEffect>
#include <QEasingCurve>




BattlePage::BattlePage(Player* player, QVector<Enemy*> enemies, QWidget* parent)
    : QWidget(parent), player(player), enemies(enemies)
{
    if (this->player == nullptr)
        this->player = new Player("Ironclad", 80);

    if (this->enemies.isEmpty())
        this->enemies.append (new Cultist);


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

    combatManager = new CombatManager(player, enemies, this);

    connect(combatManager, &CombatManager::statsUpdated, this, &BattlePage::updateStats);
    connect(combatManager, &CombatManager::battleWon,    this, &BattlePage::onBattleWon);
    connect(combatManager, &CombatManager::battleLost,   this, &BattlePage::onBattleLost);
    connect(endTurnBtn,    &QPushButton::clicked,        combatManager, &CombatManager::endTurn);
    connect(combatManager, &CombatManager::enemyIntentUpdated,this, &BattlePage::updateEnemyIntent);

    combatManager->startCombat();
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


    playerHpLabel = new QLabel("80/80", topBar);
    playerHpLabel->setStyleSheet("color: #e63946; font-size: 14px; font-weight: bold; background: transparent;");

    playerBlockLabel = new QLabel("Block: 0", topBar);
    playerBlockLabel->setStyleSheet("color: #60a5fa; font-size: 13px; font-weight: bold; background: transparent;");
    leftGroup->addWidget(playerBlockLabel);

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

    goldValueLabel = new QLabel(QString::number(player->getGold()), topBar);
    goldValueLabel->setStyleSheet("color: #f5c518; font-size: 14px; font-weight: bold; background: transparent;");

    leftGroup->addWidget(nameLabel);
    leftGroup->addWidget(classLabel);
    leftGroup->addSpacing(15);
    leftGroup->addWidget(heartIcon);
    leftGroup->addWidget(playerHpLabel);
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
    playerHPBar = new QProgressBar(playerWidget);
    playerHPBar->setRange(0, player->getMaxHealth());
    playerHPBar->setValue(player->getCurrentHealth());
    playerHPBar->setFixedSize(170, 16);
    playerHPBar->setTextVisible(true);
    playerHPBar->setFormat("%v / %m");
    playerHPBar->setStyleSheet(
        "QProgressBar { background: #1a1a1a; border: 2px solid #333;"
        "border-radius: 6px; color: white; font-size: 12px; text-align: center; }"
        "QProgressBar::chunk { background: #e63946; border-radius: 4px; }"
        );

    playerLayout->addStretch();
    playerLayout->addWidget(playerImg);
    playerLayout->addWidget(playerHPBar, 0, Qt::AlignHCenter);

    // -- Enemy container (right side) --
    QWidget *enemyContainer = new QWidget(battleField);
    QHBoxLayout *enemyLayout = new QHBoxLayout(enemyContainer);
    enemyLayout->setAlignment(Qt::AlignCenter | Qt::AlignBottom);
    enemyLayout->setSpacing(40);

    // Single placeholder enemy widget
    QWidget *enemyWidget = new QWidget(enemyContainer);
    enemyWidget->setFixedSize(250, 400);
    enemyWidget->setStyleSheet("background: transparent;");
    QVBoxLayout *enemyInnerLayout = new QVBoxLayout(enemyWidget);
    enemyInnerLayout->setContentsMargins(0, 0, 0, 10);
    enemyInnerLayout->setSpacing(5);

    // Enemy intent (attack/defend indicator)
    intentLabel = new QLabel(getIntentText(enemies[0]), enemyWidget);
    intentLabel->setAlignment(Qt::AlignCenter);
    intentLabel->setFixedHeight(30);
    intentLabel->setStyleSheet(
        "color: #f87171; font-size: 16px; font-weight: bold;"
        "background: rgba(0,0,0,120); border-radius: 8px; padding: 4px;"
        );

    QLabel *enemyImg = new QLabel(enemyWidget);
    enemyImg->setFixedSize(190, 210);
    enemyImg->setAlignment(Qt::AlignCenter);
    enemyImg->setStyleSheet("background: transparent;");
    enemyImg->setPixmap(QPixmap(enemyImagePath(enemies[0])).scaled(
        190, 210,
        Qt::KeepAspectRatio,
        Qt::SmoothTransformation
        ));



    // Enemy HP bar
    enemyHPBar  = new QProgressBar(enemyWidget);
    enemyHPBar->setRange(0, enemies[0]->getMaxHealth());
    enemyHPBar->setValue(enemies[0]->getCurrentHealth());
    enemyHPBar->setFixedSize(170, 16);
    enemyHPBar->setTextVisible(true);
    enemyHPBar->setFormat("%v / %m");
    enemyHPBar->setStyleSheet(
        "QProgressBar { background: #1a1a1a; border: 2px solid #333;"
        "border-radius: 6px; color: white; font-size: 12px; text-align: center; }"
        "QProgressBar::chunk { background: #e63946; border-radius: 4px; }"
        );
    enemyInnerLayout->addStretch();
    enemyInnerLayout->addWidget(intentLabel);
    enemyInnerLayout->addStretch();
    enemyInnerLayout->addWidget(enemyImg);
    enemyInnerLayout->addWidget(enemyHPBar, 0, Qt::AlignHCenter);

    enemyLayout->addWidget(enemyWidget);

    layout->addWidget(playerWidget, 0, Qt::AlignBottom);
    layout->addStretch(2);
    layout->addWidget(enemyContainer, 0, Qt::AlignBottom);
}



// ─────────────────────────────────────────
void BattlePage::setupBottomBar()
{
    QVBoxLayout *layout = new QVBoxLayout(bottomBar);
    layout->setContentsMargins(50, 0, 50,0);
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

    energyValueLabel = new OutlinedLabel("3/3", energyLabel);
    energyValueLabel->setFixedSize(130, 130);
    energyValueLabel->move(0, 0);
    energyValueLabel->raise();


    endTurnBtn = new QPushButton("End Turn", bottomBar);
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


    // ===== Card hand with arc layout =====

    const int handViewWidth = 900;
    const int handViewHeight = 400;

    // Create the scene/view BEFORE computing layout, since we now size
    // everything against handView's fixed dimensions instead of the
    // undefined "handContainer" that used to be referenced here.
    handScene = new QGraphicsScene(bottomBar);

    //ch
    handScene->setSceneRect(0, 0, handViewWidth, handViewHeight);

    handView = new QGraphicsView(handScene, bottomBar);
    handView->setFixedSize(handViewWidth, handViewHeight);
    handView->setStyleSheet("background: transparent; border: none;");
    handView->setFrameShape(QFrame::NoFrame);
    handView->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    handView->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    handView->setRenderHint(QPainter::Antialiasing);
    //ch
    handView->setResizeAnchor(QGraphicsView::NoAnchor);
    handView->setTransformationAnchor(QGraphicsView::NoAnchor);
    //2
    handView->setViewportUpdateMode(QGraphicsView::FullViewportUpdate);
    handView->viewport()->setAutoFillBackground(false);

    layout->addLayout(controlRow);

    handView->setParent(this);
    handView->setGeometry(
        (1280 - 900) / 2,   // x: centered = 190
        720  - 400,          // y: anchored to bottom = 320
        900,
        400
        );


    // ===== Draw Pile button (bottom-left) =====
    drawPileBtn = new QPushButton(this);
    drawPileBtn->setFixedSize(70, 90);
    drawPileBtn->setCursor(Qt::PointingHandCursor);
    drawPileBtn->setIcon(QIcon(QPixmap(":/drawPile.png").scaled(
        80, 100, Qt::KeepAspectRatio, Qt::SmoothTransformation)));
    drawPileBtn->setIconSize(QSize(80, 100));
    drawPileBtn->setStyleSheet(
        "QPushButton { border: none; background: transparent; }"
        );
    drawPileBtn->setGeometry(30, 720 - 110, 70, 90);
    drawPileBtn->raise();

    drawPileCountLabel = new QLabel(drawPileBtn);
    drawPileCountLabel->setAlignment(Qt::AlignCenter);
    drawPileCountLabel->setStyleSheet(
        "color: white; font-size: 16px; font-weight: bold; background: transparent;"
        );
    drawPileCountLabel->setGeometry(0, 0, 110, 110);
    drawPileCountLabel->setText("0");
    drawPileCountLabel->raise();

    connect(drawPileBtn, &QPushButton::clicked, this, &BattlePage::onDrawPileClicked);

    // ===== Discard Pile button (bottom-right) =====
    discardPileBtn = new QPushButton(this);
    discardPileBtn->setFixedSize(70, 90);
    discardPileBtn->setCursor(Qt::PointingHandCursor);
    discardPileBtn->setIcon(QIcon(QPixmap(":/discardPile.png").scaled(
        80, 100, Qt::KeepAspectRatio, Qt::SmoothTransformation)));
    discardPileBtn->setIconSize(QSize(80, 100));
    discardPileBtn->setStyleSheet(
        "QPushButton { border: none; background: transparent; }"
        );
    discardPileBtn->setGeometry(1280 - 100, 720 - 110, 70, 90);
    discardPileBtn->raise();

    discardPileCountLabel = new QLabel(discardPileBtn);
    discardPileCountLabel->setAlignment(Qt::AlignCenter);
    discardPileCountLabel->setStyleSheet(
        "color: white; font-size: 16px; font-weight: bold; background: transparent;"
        );
    discardPileCountLabel->setGeometry(0, 0, 25, 110);
    discardPileCountLabel->setText("0");
    discardPileCountLabel->raise();

    connect(discardPileBtn, &QPushButton::clicked, this, &BattlePage::onDiscardPileClicked);


}


bool BattlePage::eventFilter(QObject *obj, QEvent *event)
{
    QPushButton* card = qobject_cast<QPushButton*>(obj);
    if (!card) return QWidget::eventFilter(obj, event);

    QGraphicsProxyWidget* proxy = card->property("proxy").value<QGraphicsProxyWidget*>();
    if (!proxy) return QWidget::eventFilter(obj, event);

    if (event->type() == QEvent::Enter)
    {
        QPropertyAnimation* moveUp = new QPropertyAnimation(proxy, "pos");
        moveUp->setDuration(200);
        moveUp->setStartValue(proxy->pos());
        moveUp->setEndValue(QPointF(proxy->pos().x(), card->property("defaultY").toInt() - 120));
        moveUp->setEasingCurve(QEasingCurve::OutCubic);
        moveUp->start(QAbstractAnimation::DeleteWhenStopped);

        QPropertyAnimation* rotateFlat = new QPropertyAnimation(proxy, "rotation");
        rotateFlat->setDuration(200);
        rotateFlat->setStartValue(proxy->rotation());
        rotateFlat->setEndValue(0.0);
        rotateFlat->setEasingCurve(QEasingCurve::OutCubic);
        rotateFlat->start(QAbstractAnimation::DeleteWhenStopped);

        QPropertyAnimation* scaleUp = new QPropertyAnimation(proxy, "scale");
        scaleUp->setDuration(200);
        scaleUp->setStartValue(proxy->scale());
        scaleUp->setEndValue(1.2);
        scaleUp->setEasingCurve(QEasingCurve::OutCubic);
        scaleUp->start(QAbstractAnimation::DeleteWhenStopped);

        proxy->setZValue(100);

        QGraphicsDropShadowEffect* glow = new QGraphicsDropShadowEffect();
        glow->setColor(QColor(0, 150, 255, 255));
        glow->setBlurRadius(80);
        glow->setOffset(0, 0);
        proxy->setGraphicsEffect(glow);
    }
    else if (event->type() == QEvent::Leave)
    {
        QPropertyAnimation* moveDown = new QPropertyAnimation(proxy, "pos");
        moveDown->setDuration(200);
        moveDown->setStartValue(proxy->pos());
        moveDown->setEndValue(QPointF(proxy->pos().x(), card->property("defaultY").toInt()));
        moveDown->setEasingCurve(QEasingCurve::OutCubic);
        moveDown->start(QAbstractAnimation::DeleteWhenStopped);

        QPropertyAnimation* rotateBack = new QPropertyAnimation(proxy, "rotation");
        rotateBack->setDuration(200);
        rotateBack->setStartValue(proxy->rotation());
        rotateBack->setEndValue(card->property("defaultRotation").toDouble());
        rotateBack->setEasingCurve(QEasingCurve::OutCubic);
        rotateBack->start(QAbstractAnimation::DeleteWhenStopped);

        QPropertyAnimation* scaleDown = new QPropertyAnimation(proxy, "scale");
        scaleDown->setDuration(200);
        scaleDown->setStartValue(proxy->scale());
        scaleDown->setEndValue(1.0);
        scaleDown->setEasingCurve(QEasingCurve::OutCubic);
        scaleDown->start(QAbstractAnimation::DeleteWhenStopped);

        proxy->setZValue(card->property("index").toInt());
        proxy->setGraphicsEffect(nullptr);
    }

    return QWidget::eventFilter(obj, event);
}


void BattlePage::updateStats()
{
    CombatDeck* deck = player->getCombatDeck();
    playerHPBar->setMaximum(player->getMaxHealth());
    playerHPBar->setValue(player->getCurrentHealth());

    playerHpLabel->setText(QString("%1 / %2")
                               .arg(player->getCurrentHealth())
                               .arg(player->getMaxHealth()));

    playerBlockLabel->setText(QString::number(player->getBlock()));

    energyValueLabel->setText(QString("%1/%2")
                                  .arg(player->getCurrentEnergy())
                                  .arg(player->getMaxEnergy()));

    if (!enemies.isEmpty() && !enemies[0]->isDead()) {
        enemyHPBar->setMaximum(enemies[0]->getMaxHealth());
        enemyHPBar->setValue(enemies[0]->getCurrentHealth());
        intentLabel->setText(getIntentText(enemies[0]));
    }

    refreshHand();

    if (deck)
    {
        drawPileCountLabel->setText(QString::number(deck->getDrawPile().size()));
        discardPileCountLabel->setText(QString::number(deck->getDiscardPile().size()));
    }

}


void BattlePage::refreshHand()
{
    handScene->clear();

    CombatDeck* deck = player->getCombatDeck();
    if (!deck) return;
    const auto& hand = deck->getHand();
    if (hand.isEmpty()) return;

    const int cardW      = 150;
    const int cardH      = 220;
    const int spacing    = 105;
    const int viewW      = 900;
    const float maxRot   = 12.0f;
    const int arcH       = 20;
    const int baseY = 400 - cardH + 120;

    const int totalW = (hand.size() - 1) * spacing + cardW;
    const int startX = (viewW - totalW) / 2;

    for (int i = 0; i < hand.size(); ++i) {
        Card* card = hand[i];
        if (!card) continue;

        float norm = hand.size() > 1
                         ? (i - (hand.size()-1) / 2.0f) / ((hand.size()-1) / 2.0f)
                         : 0.0f;

        int arcOffset  = static_cast<int>(arcH * (1.0f - norm * norm));
        float rotation = maxRot * norm;
        int x = startX + i * spacing;
        int y = baseY - arcOffset;

        // Build the card as a single QPushButton styled with the card image
        // This is exactly the old approach — one widget, no children, clean hover events
        QPushButton* btn = new QPushButton();
        btn->setFixedSize(cardW, cardH);
        btn->setCursor(Qt::PointingHandCursor);

        QPixmap px(cardImagePath(card));
        if (!px.isNull()) {
            // Use the card image as the button background
            QPixmap scaled = px.scaled(cardW, cardH,
                                       Qt::IgnoreAspectRatio,
                                       Qt::SmoothTransformation);
            btn->setIcon(QIcon(scaled));
            btn->setIconSize(QSize(cardW, cardH));
            btn->setStyleSheet(
                "QPushButton { border: none; background: transparent; padding: 0; }"
                );
        } else {
            // Fallback styled card (no image)
            QString typeColor = (card->getType() == CardType::Attack) ? "#7f1d1d" : "#1e3a5f";
            btn->setStyleSheet(QString(
                                   "QPushButton {"
                                   "  background: qlineargradient(x1:0,y1:0,x2:0,y2:1,"
                                   "    stop:0 %1, stop:1 #0f172a);"
                                   "  border: 2px solid #475569;"
                                   "  border-radius: 8px;"
                                   "  color: white;"
                                   "  font-size: 10px;"
                                   "  font-weight: bold;"
                                   "  padding: 4px;"
                                   "}"
                                   ).arg(typeColor));
            btn->setText(QString("%1\nCost: %2")
                             .arg(card->getName())
                             .arg(card->getEnergyCost()));
        }

        QGraphicsProxyWidget* proxy = handScene->addWidget(btn);
        proxy->setFlag(QGraphicsItem::ItemClipsChildrenToShape, false);
        proxy->setFlag(QGraphicsItem::ItemClipsToShape, false);
        proxy->setCacheMode(QGraphicsItem::DeviceCoordinateCache);

        proxy->setPos(x, y);
        proxy->setRotation(rotation);
        proxy->setTransformOriginPoint(cardW / 2.0, cardH / 2.0);
        proxy->setZValue(i);

        // Store properties — exactly the same as the old working code
        btn->setProperty("proxy",           QVariant::fromValue(proxy));
        btn->setProperty("defaultY",        y);
        btn->setProperty("defaultRotation", rotation);
        btn->setProperty("index",           i);
        btn->installEventFilter(this);

        connect(btn, &QPushButton::clicked, [this, card]() {
            onCardClicked(card);
        });
    }
}


void BattlePage::onCardClicked(Card* card)
{
    Enemy* target = enemies.isEmpty() ? nullptr : enemies[0];
    combatManager->playCard(card, target);
}


void BattlePage::onBattleWon()
{
    emit battleEnded();
}
void BattlePage::onBattleLost()
{
    emit battleEnded();
}
QString BattlePage::cardImagePath(const Card* card)
{
    if (!card) return QString();

    QString cleanName = card->getName();
    cleanName.remove(' ');
    cleanName.remove('\'');
    cleanName.remove('.');
    if (cleanName.endsWith('+'))
        cleanName.chop(1);
    if (card->getIsUpgraded())
        cleanName += "Plus";

    return QString(":/card/%1.png").arg(cleanName);
}

void BattlePage::onDrawPileClicked()
{
    // TODO: نمایش کارت‌های draw pile
}

void BattlePage::onDiscardPileClicked()
{
    // TODO: نمایش کارت‌های discard pile
}


QString BattlePage::getIntentText(Enemy* enemy)
{
    if (!enemy) return "❓ Unknown";

    QString emoji;
    switch (enemy->getIntent())
    {
    case Intent::Attack:       emoji = "⚔️";      break;
    case Intent::Defend:       emoji = "🛡️";      break;
    case Intent::Buff:         emoji = "✨";       break;
    case Intent::Debuff:       emoji = "💀";       break;
    case Intent::AttackDefend: emoji = "⚔️🛡️";   break;
    case Intent::AttackBuff:   emoji = "⚔️✨";    break;
    case Intent::AttackDebuff: emoji = "⚔️💀";   break;
    case Intent::DefendBuff:   emoji = "🛡️✨";    break;
    case Intent::Escape:       emoji = "💨";       break;
    case Intent::Unknown:      emoji = "❓";       break;
    }

    QString text = emoji;
    if (enemy->getIntentDamage() > 0)
    {
        text += QString(" %1").arg(enemy->getIntentDamage());
        if (enemy->getIntentHits() > 1)
            text += QString(" x%1").arg(enemy->getIntentHits());
    }
    return text;
}


void BattlePage::updateEnemyIntent(Enemy* enemy)
{
    if (!enemy || !intentLabel) return;
    intentLabel->setText(getIntentText(enemy));
}


QString BattlePage::enemyImagePath(Enemy* enemy)
{
    if (!enemy) return QString();

    QString cleanName = enemy->getName();
    cleanName.remove(' ');

    return QString(":/Enemy/%1.png").arg(cleanName);
}
