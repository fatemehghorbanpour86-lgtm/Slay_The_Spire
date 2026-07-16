#include "battlepage.h"
#include "combatdeck.h"
#include "enemy.h"
#include "normalenemies.h"
#include "player.h"

#include "outlinedlabel.h"
#include "qtimer.h"

#include <QPropertyAnimation>
#include <QGraphicsProxyWidget>
#include <QGraphicsView>
#include <QGraphicsScene>
#include <QGraphicsDropShadowEffect>
#include <QEasingCurve>
#include <QPointer>

#include <utility>




BattlePage::BattlePage(Player* player, QVector<Enemy*> enemies, QWidget* parent)
    : QWidget(parent), player(player), enemies(enemies)
{
    if (this->player == nullptr)
        this->player = new Player("َAna", 80);

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
    setupClickOverlays();
    QTimer::singleShot(0, this, &BattlePage::repositionOverlays);


    combatManager = new CombatManager(player, enemies, this);

    connect(combatManager, &CombatManager::statsUpdated, this, &BattlePage::updateStats);
    connect(combatManager, &CombatManager::battleWon,    this, &BattlePage::onBattleWon);
    connect(combatManager, &CombatManager::battleLost,   this, &BattlePage::onBattleLost);
    connect(endTurnBtn,    &QPushButton::clicked,        combatManager, &CombatManager::endTurn);
    connect(combatManager, &CombatManager::enemyIntentUpdated,this, &BattlePage::updateEnemyIntent);

    combatManager->startCombat();

    connect(combatManager, &CombatManager::enemyAttacking, this, [this](Enemy* e)
    {
        Q_UNUSED(e);
        QWidget* enemyW = enemyWidgets.isEmpty() ? nullptr : enemyWidgets[0];
        animateAttack(enemyW, playerWidget);
    });

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
    QLabel *nameLabel = new QLabel(player->getName(), topBar);
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
    this->playerWidget = playerWidget;
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


    // Shield icon + block count
    playerBlockIconLabel = new QLabel(playerWidget);
    playerBlockIconLabel->setFixedSize(36, 36);
    playerBlockIconLabel->setStyleSheet("background: transparent;");
    playerBlockIconLabel->setPixmap(QPixmap(":/defendIcon.png").scaled(
        36, 36, Qt::KeepAspectRatio, Qt::SmoothTransformation));

    playerBlockLabel = new QLabel(playerBlockIconLabel);
    playerBlockLabel->setAlignment(Qt::AlignCenter);
    playerBlockLabel->setGeometry(0, 0, 36, 36);
    playerBlockLabel->setStyleSheet("color: black; font-size: 13px; font-weight: bold; background: transparent;");

    playerBlockIconLabel->hide();


    // -- Enemy container (right side) --
    QWidget *enemyContainer = new QWidget(battleField);
    QHBoxLayout *enemyLayout = new QHBoxLayout(enemyContainer);
    enemyLayout->setAlignment(Qt::AlignCenter | Qt::AlignBottom);
    enemyLayout->setSpacing(40);

    // Single placeholder enemy widget
    QWidget *enemyWidget = new QWidget(enemyContainer);
    enemyWidget->setFixedSize(250, 400);
    enemyWidget->setStyleSheet("background: transparent;");
    enemyWidgets.append(enemyWidget);
    QVBoxLayout *enemyInnerLayout = new QVBoxLayout(enemyWidget);
    enemyInnerLayout->setContentsMargins(0, 0, 0, 10);
    enemyInnerLayout->setSpacing(5);

    // Enemy intent (attack/defend indicator)
    intentLabel = new QLabel(getIntentText(enemies[0]), enemyWidget);
    intentLabel->setAlignment(Qt::AlignCenter);
    intentLabel->setFixedHeight(30);
    intentLabel->setStyleSheet(
        "color: #f87171; font-size: 16px; font-weight: bold;"
        "background: transparent; border-radius: 8px; padding: 4px;"
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

void BattlePage::setupClickOverlays()
{
    // Enemy overlay — covers the full enemyWidget area
    enemyClickOverlay = new QPushButton(this);
    enemyClickOverlay->setStyleSheet("background: transparent; border: none;");
    enemyClickOverlay->setCursor(Qt::ArrowCursor);
    enemyClickOverlay->hide();   // only shown when targeting

    // Player overlay — covers the full playerWidget area
    playerClickOverlay = new QPushButton(this);
    playerClickOverlay->setStyleSheet("background: transparent; border: none;");
    playerClickOverlay->setCursor(Qt::ArrowCursor);
    playerClickOverlay->hide();

    connect(enemyClickOverlay, &QPushButton::clicked, this, [this]() {
        if (pendingCard && !enemies.isEmpty())
            playCardWithAnimation(pendingCard, selectedProxy, enemies[0]);
    });

    connect(playerClickOverlay, &QPushButton::clicked, this, [this]() {
        if (pendingCard)
            playCardWithAnimation(pendingCard, selectedProxy, nullptr);
    });
}
void BattlePage::repositionOverlays()
{
    if (enemyClickOverlay && !enemyWidgets.isEmpty() && enemyWidgets[0])
    {
        QWidget* w = enemyWidgets[0];
        QPoint p = w->mapTo(this, QPoint(0, 0));
        enemyClickOverlay->setGeometry(p.x(), p.y(), w->width(), w->height());
    }

    if (playerClickOverlay && playerWidget)
    {
        QPoint p = playerWidget->mapTo(this, QPoint(0, 0));
        playerClickOverlay->setGeometry(p.x(), p.y(),
                                        playerWidget->width(),
                                        playerWidget->height());
    }

    if (enemyClickOverlay)  enemyClickOverlay->raise();
    if (playerClickOverlay) playerClickOverlay->raise();
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


    animScene = new QGraphicsScene(bottomBar);
    animScene->setSceneRect(0, 0, 1280, 720);

    animView = new QGraphicsView(animScene, this);
    animView->setStyleSheet("background: transparent; border: none;");
    animView->setFrameShape(QFrame::NoFrame);
    animView->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    animView->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    animView->setRenderHint(QPainter::Antialiasing);
    animView->setResizeAnchor(QGraphicsView::NoAnchor);
    animView->setTransformationAnchor(QGraphicsView::NoAnchor);
    animView->setViewportUpdateMode(QGraphicsView::FullViewportUpdate);
    animView->viewport()->setAutoFillBackground(false);
    animView->setAttribute(Qt::WA_TransparentForMouseEvents);

    animView->setGeometry(0, 0, 1280, 720);

    animView->raise();



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

    // --- Enemy widget clicked as target ---
    if (obj->property("isEnemyTarget").toBool() &&
        event->type() == QEvent::MouseButtonPress)
    {
        QWidget* w = qobject_cast<QWidget*>(obj);
        int idx = enemyWidgets.indexOf(w);
        if (idx >= 0 && idx < enemies.size() && pendingCard)
        {
            Enemy* target = enemies[idx];
            playCardWithAnimation(pendingCard, selectedProxy, target);
        }
        return true;
    }

    // --- Player widget clicked as target ---
    if (obj->property("isPlayerTarget").toBool() &&
        event->type() == QEvent::MouseButtonPress)
    {
        if (pendingCard)
            playCardWithAnimation(pendingCard, selectedProxy, nullptr);
        return true;
    }

    QPushButton* card = qobject_cast<QPushButton*>(obj);
    if (!card) return QWidget::eventFilter(obj, event);

    QGraphicsProxyWidget* proxy = card->property("proxy").value<QGraphicsProxyWidget*>();
    if (!proxy) return QWidget::eventFilter(obj, event);

    Card* cardData = card->property("cardData").value<Card*>();
    if (pendingCard && cardData != pendingCard)
        return QWidget::eventFilter(obj, event);

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
        // If this card is the selected one, don't animate back down
        Card* cardData = card->property("cardData").value<Card*>();
        if (cardData && cardData == pendingCard)
            return QWidget::eventFilter(obj, event);  // skip leave animation


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


    int block = player->getBlock();
    bool hasBlock = block > 0;

    if (playerBlockIconLabel && playerBlockLabel)
    {
        playerBlockIconLabel->setVisible(hasBlock);
        if (hasBlock)
            playerBlockLabel->setText(QString::number(block));
    }

    // Blue glow on HP bar while block > 0
    if (hasBlock)
    {
        if (!playerHPBar->graphicsEffect())
        {
            auto* glow = new QGraphicsDropShadowEffect();
            glow->setColor(QColor(96, 165, 250, 255));
            glow->setBlurRadius(30);
            glow->setOffset(0, 0);
            playerHPBar->setGraphicsEffect(glow);
        }

        playerHPBar->setStyleSheet(
            "QProgressBar { background: #1a1a1a; border: 2px solid #333;"
            "border-radius: 6px; color: white; font-size: 12px; text-align: center; }"
            "QProgressBar::chunk { background: #60a5fa; border-radius: 4px; }"
            );
    }
    else
    {
        playerHPBar->setGraphicsEffect(nullptr);
        playerHPBar->setStyleSheet(
            "QProgressBar { background: #1a1a1a; border: 2px solid #333;"
            "border-radius: 6px; color: white; font-size: 12px; text-align: center; }"
            "QProgressBar::chunk { background: #e63946; border-radius: 4px; }"
            );
    }



    energyValueLabel->setText(QString("%1/%2")
                                  .arg(player->getCurrentEnergy())
                                  .arg(player->getMaxEnergy()));

    if (!enemies.isEmpty() && !enemies[0]->isDead()) {
        enemyHPBar->setMaximum(enemies[0]->getMaxHealth());
        enemyHPBar->setValue(enemies[0]->getCurrentHealth());
        intentLabel->setText(getIntentText(enemies[0]));
    }

    if (!animatingCard)
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
        }
        else {
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
        proxy->setCacheMode(QGraphicsItem::NoCache);
        proxy->setPos(x, y);
        proxy->setRotation(rotation);
        proxy->setTransformOriginPoint(cardW / 2.0, cardH / 2.0);
        proxy->setZValue(i);
        proxy->setData(0, y);                  // ← add this
        proxy->setData(1, (double)rotation);   // ← add this

        btn->setProperty("proxy",           QVariant::fromValue(proxy));
        btn->setProperty("defaultY",        y);
        btn->setProperty("defaultRotation", rotation);
        btn->setProperty("index",           i);
        btn->setProperty("cardData",        QVariant::fromValue(card));
        btn->installEventFilter(this);

        QPointer<QGraphicsProxyWidget> safeProxy = proxy;   // ← add this
        connect(btn, &QPushButton::clicked, [this, card, safeProxy]() {
            if (safeProxy)
                onCardClicked(card, safeProxy.data());
        });
    }
}

void BattlePage::onCardClicked(Card* card, QGraphicsProxyWidget* proxy)
{

    if (pendingCard == card)
    {
        clearSelection();
        if (proxy)
        {
            QPropertyAnimation* moveDown = new QPropertyAnimation(proxy, "pos");
            moveDown->setDuration(200);
            moveDown->setStartValue(proxy->pos());
            moveDown->setEndValue(QPointF(proxy->pos().x(), proxy->data(0).toInt()));  // ← fixed
            moveDown->setEasingCurve(QEasingCurve::OutCubic);
            moveDown->start(QAbstractAnimation::DeleteWhenStopped);

            QPropertyAnimation* rotateBack = new QPropertyAnimation(proxy, "rotation");
            rotateBack->setDuration(200);
            rotateBack->setStartValue(proxy->rotation());
            rotateBack->setEndValue(proxy->data(1).toDouble());  // ← fixed
            rotateBack->setEasingCurve(QEasingCurve::OutCubic);
            rotateBack->start(QAbstractAnimation::DeleteWhenStopped);

            QPropertyAnimation* scaleDown = new QPropertyAnimation(proxy, "scale");
            scaleDown->setDuration(200);
            scaleDown->setStartValue(proxy->scale());
            scaleDown->setEndValue(1.0);
            scaleDown->setEasingCurve(QEasingCurve::OutCubic);
            scaleDown->start(QAbstractAnimation::DeleteWhenStopped);

            proxy->setGraphicsEffect(nullptr);
        }
        return;
    }

    if (pendingCard != nullptr)
        return;

    // --- Select this card ---
    clearSelection();   // deselect any previously selected card first
    pendingCard   = card;
    selectedProxy = proxy;

    // Show target highlights based on card type
    CardTarget target = getCardTarget(card);
    if (target == CardTarget::Enemy)
        showEnemyHighlights();
    else if (target == CardTarget::Player)
        showPlayerHighlight();
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
    if (!enemy || !intentLabel)
        return;

    intentLabel->setText(getIntentText(enemy));
}


QString BattlePage::enemyImagePath(Enemy* enemy)
{
    if (!enemy) return QString();

    QString cleanName = enemy->getName();
    cleanName.remove(' ');

    return QString(":/Enemy/%1.png").arg(cleanName);
}

BattlePage::CardTarget BattlePage::getCardTarget(Card* card)
{
    if (!card) return CardTarget::None;
    if (card->getType() == CardType::Attack)
        return CardTarget::Enemy;
    // Skills and Powers that target player (Defend, Flex, etc.)
    return CardTarget::Player;
}
void BattlePage::showEnemyHighlights()
{
    clearHighlights();
    for (QWidget* w : std::as_const(enemyWidgets))
    {
        if (!w) continue;
        auto* glow = new QGraphicsDropShadowEffect();
        glow->setColor(QColor(220, 50, 50, 255));
        glow->setBlurRadius(60);
        glow->setOffset(0, 0);
        w->setGraphicsEffect(glow);
        enemyGlowEffects.append(glow);
    }
    if (enemyClickOverlay) { enemyClickOverlay->setCursor(Qt::PointingHandCursor); enemyClickOverlay->raise(); enemyClickOverlay->show(); }
}

void BattlePage::showPlayerHighlight()
{
    clearHighlights();
    if (!playerWidget) return;
    auto* glow = new QGraphicsDropShadowEffect();
    glow->setColor(QColor(50, 200, 50, 255));
    glow->setBlurRadius(60);
    glow->setOffset(0, 0);
    playerWidget->setGraphicsEffect(glow);
    playerGlowEffect = glow;
    if (playerClickOverlay) { playerClickOverlay->setCursor(Qt::PointingHandCursor); playerClickOverlay->raise(); playerClickOverlay->show(); }
}

void BattlePage::clearHighlights()
{
    for (QWidget* w : std::as_const(enemyWidgets))
        if (w) w->setGraphicsEffect(nullptr);
    enemyGlowEffects.clear();

    if (playerWidget) playerWidget->setGraphicsEffect(nullptr);
    playerGlowEffect = nullptr;

    if (enemyClickOverlay)  enemyClickOverlay->hide();
    if (playerClickOverlay) playerClickOverlay->hide();
}

void BattlePage::clearSelection()
{
    pendingCard   = nullptr;
    selectedProxy = nullptr;
    clearHighlights();
}

void BattlePage::playCardWithAnimation(Card* card,
                                       QGraphicsProxyWidget* proxy,
                                       Enemy* target)
{
    Card* cardToPlay = card;
    QGraphicsProxyWidget* proxyToAnim = proxy;

    clearSelection();

    if (!proxyToAnim)
    {
        combatManager->playCard(cardToPlay, target);
        return;
    }

    if (player->getCurrentEnergy() < cardToPlay->getEnergyCost())
    {
        // Not enough energy — snap card back to default position
        QPropertyAnimation* moveDown = new QPropertyAnimation(proxyToAnim, "pos");
        moveDown->setDuration(200);
        moveDown->setStartValue(proxyToAnim->pos());
        moveDown->setEndValue(QPointF(proxyToAnim->pos().x(),
                                      proxyToAnim->data(0).toInt()));
        moveDown->setEasingCurve(QEasingCurve::OutCubic);
        moveDown->start(QAbstractAnimation::DeleteWhenStopped);

        QPropertyAnimation* scaleDown = new QPropertyAnimation(proxyToAnim, "scale");
        scaleDown->setDuration(200);
        scaleDown->setStartValue(proxyToAnim->scale());
        scaleDown->setEndValue(1.0);
        scaleDown->setEasingCurve(QEasingCurve::OutCubic);
        scaleDown->start(QAbstractAnimation::DeleteWhenStopped);

        proxyToAnim->setGraphicsEffect(nullptr);
        return;
    }

    animatingCard = true;
    // ── transfer proxy to animScene before animating ──
    double currentRot   = proxyToAnim->rotation();
    double currentScale = proxyToAnim->scale();

    QPoint viewPt = handView->mapFromScene(proxyToAnim->pos());
    QPoint globalPt = handView->mapToGlobal(viewPt);
    QPointF animScenePos = animView->mapToScene(animView->mapFromGlobal(globalPt));


    handScene->removeItem(proxyToAnim);
    animScene->addItem(proxyToAnim);
    proxyToAnim->setPos(animScenePos);
    proxyToAnim->setRotation(currentRot);
    proxyToAnim->setScale(currentScale);

    // Step 1: fly to center
    QPointF center(1280 / 2.0 - 75, 720 / 2.0 - 110);

    auto* flyToCenter = new QPropertyAnimation(proxyToAnim, "pos");
    flyToCenter->setDuration(200);
    flyToCenter->setStartValue(proxyToAnim->pos());
    flyToCenter->setEndValue(center);
    flyToCenter->setEasingCurve(QEasingCurve::OutCubic);

    connect(flyToCenter, &QPropertyAnimation::finished, this,
            [this, proxyToAnim, cardToPlay, target]()
            {
                // Step 2: fly to discard pile and shrink

                QPoint discardInAnimView = animView->mapFromGlobal
                    (
                         discardPileBtn->mapToGlobal(
                         QPoint(discardPileBtn->width() / 2,
                         discardPileBtn->height() / 2))
                    );

                QPointF discardScene = animView->mapToScene(discardInAnimView);

                QSizeF proxySize = proxyToAnim->size();
                discardScene -= QPointF(proxySize.width() / 2.0,proxySize.height() / 2.0);

                auto* flyOut = new QPropertyAnimation(proxyToAnim, "pos");
                flyOut->setDuration(300);
                flyOut->setStartValue(proxyToAnim->pos());
                flyOut->setEndValue(discardScene);
                flyOut->setEasingCurve(QEasingCurve::InCubic);

                auto* shrink = new QPropertyAnimation(proxyToAnim, "scale");
                shrink->setDuration(300);
                shrink->setStartValue(proxyToAnim->scale());
                shrink->setEndValue(0.0);
                shrink->setEasingCurve(QEasingCurve::InCubic);

                // Step 3: after animation — remove from scene and play card
                connect(flyOut, &QPropertyAnimation::finished, this,
                        [this, proxyToAnim, cardToPlay, target]()
                        {
                            animScene->removeItem(proxyToAnim);
                            animatingCard = false;
                            if (target && cardToPlay->getType() == CardType::Attack)
                            {
                                QWidget* enemyW = enemyWidgets.isEmpty() ? nullptr : enemyWidgets[0];
                                animateAttack(playerWidget, enemyW, [this, cardToPlay, target]() {
                                    combatManager->playCard(cardToPlay, target);
                                });
                            }
                            else
                            {
                                combatManager->playCard(cardToPlay, target);
                            }
                        });

                flyOut->start(QAbstractAnimation::DeleteWhenStopped);
                shrink->start(QAbstractAnimation::DeleteWhenStopped);
            });


    flyToCenter->start(QAbstractAnimation::DeleteWhenStopped);

}


void BattlePage::showEvent(QShowEvent* e)
{
    QWidget::showEvent(e);
    repositionBlockIcon();
}

void BattlePage::repositionBlockIcon()
{
    if (!playerBlockIconLabel || !playerHPBar || !playerWidget) return;
    // Map HP bar position into playerWidget coordinates
    QPoint hpBarInWidget = playerHPBar->mapTo(playerWidget, QPoint(0, 0));
    // Place block icon to the left of the HP bar, vertically centered on it
    int iconX = hpBarInWidget.x() - 40;
    int iconY = hpBarInWidget.y() - 10;
    playerBlockIconLabel->move(iconX, iconY);
    playerBlockIconLabel->raise();
    playerBlockLabel->move(0, 0);
    playerBlockLabel->resize(36, 36);
}
void BattlePage::animateAttack(QWidget* attacker, QWidget* target, std::function<void()> onDone)
{
    if (!attacker || !target)
    {
        if (onDone) onDone();
        return;
    }


    QPixmap snapshot = attacker->grab();
    if (snapshot.isNull())
    {
        if (onDone) onDone();
        return;
    }

    QLabel* ghost = new QLabel(this);
    ghost->setPixmap(snapshot);
    ghost->setFixedSize(attacker->size());
    ghost->setAttribute(Qt::WA_TransparentForMouseEvents);
    ghost->setStyleSheet("background: transparent; border: none;");
    ghost->setScaledContents(false);

    const QPoint origin = attacker->mapTo(this, QPoint(0, 0));
    ghost->move(origin);
    ghost->show();
    ghost->raise();

    // hiding the real one
    const bool wasVisible = attacker->isVisible();
    attacker->hide();

    QPoint attackerCenter = attacker->mapTo(this, attacker->rect().center());
    QPoint targetCenter   = target->mapTo(this, target->rect().center());

    attackerCenter = origin + QPoint(attacker->width() / 2, attacker->height() / 2);

    QPointF dir = QPointF(targetCenter - attackerCenter);
    const float len = std::sqrt(dir.x() * dir.x() + dir.y() * dir.y());
    const QPointF nudge = (len > 0.f) ? dir / len * 35.0f : QPointF(0, 0);

    const QPoint forward = origin + QPoint(int(nudge.x()), int(nudge.y()));

    auto* goForward = new QPropertyAnimation(ghost, "pos");
    goForward->setDuration(120);
    goForward->setStartValue(origin);
    goForward->setEndValue(forward);
    goForward->setEasingCurve(QEasingCurve::OutCubic);

    auto* goBack = new QPropertyAnimation(ghost, "pos");
    goBack->setDuration(200);
    goBack->setStartValue(forward);
    goBack->setEndValue(origin);
    goBack->setEasingCurve(QEasingCurve::OutBounce);

    connect(goForward, &QPropertyAnimation::finished, this,
            [goBack, onDone]() {
                goBack->start(QAbstractAnimation::DeleteWhenStopped);
                if (onDone) onDone(); // deal damage as it returns
            });

    connect(goBack, &QPropertyAnimation::finished, this,
            [ghost, attacker, wasVisible]() {
                ghost->deleteLater();
                if (wasVisible)
                    attacker->show();
            });

    goForward->start(QAbstractAnimation::DeleteWhenStopped);
}
