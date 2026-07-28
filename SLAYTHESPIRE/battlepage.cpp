#include "battlepage.h"
#include "attackcards.h"
#include "combatcalculator.h"
#include "combatdeck.h"
#include "enemy.h"
#include "normalenemies.h"
#include "player.h"
#include "deckviewer.h"

#include "outlinedlabel.h"
#include "potion.h"
#include "qtimer.h"

#include "pileviewerdialog.h"
#include "skillcards.h"
#include "statuscards.h"

#include <QPropertyAnimation>
#include <QGraphicsProxyWidget>
#include <QGraphicsView>
#include <QGraphicsScene>
#include <QGraphicsDropShadowEffect>
#include <QEasingCurve>
#include <QPointer>
#include <QPropertyAnimation>
#include <QEasingCurve>
#include <QApplication>
#include <QToolTip>
#include <QDir>


static QString makeEffectTooltipHtml(const Effect* effect)
{
    if (!effect) return {};

    QString title = effect->getName().toHtmlEscaped();
    QString desc  = effect->getTooltip().toHtmlEscaped();

    desc.replace("\n", "<br>");

    return QString(R"(
        <div style="
            min-width: 170px;
            max-width: 240px;
            color: #f3e7c2;
            font-family: 'Segoe UI';
            font-size: 12px;
            line-height: 1.35;
        ">
            <div style="
                color: #f0c674;
                font-size: 13px;
                font-weight: 700;
                margin-bottom: 4px;
            ">%1</div>
            <div>%2</div>
        </div>
    )").arg(title, desc);
}

static void applyBattleTooltipStyleOnce()
{
    static bool applied = false;
    if (applied) return;
    applied = true;

    qApp->setStyleSheet(qApp->styleSheet() + R"(
        QToolTip {
            background-color: rgba(22, 16, 12, 245);
            color: #f3e7c2;
            border: 1px solid #c89b3c;
            border-radius: 8px;
            padding: 6px 8px;
        }
    )");
}

static QString makePotionTooltipHtml(const Potion* potion)
{
    if (!potion) return {};

    const QString title = potion->getName().toHtmlEscaped();
    QString desc = potion->getDescription().toHtmlEscaped();
    desc.replace("\n", "<br>");

    return QString(R"(
        <div style="
            min-width: 170px;
            max-width: 240px;
            color: #f3e7c2;
            font-family: 'Segoe UI';
            font-size: 12px;
            line-height: 1.35;
        ">
            <div style="
                color: #f0c674;
                font-size: 13px;
                font-weight: 700;
                margin-bottom: 4px;
            ">%1</div>
            <div>%2</div>
        </div>
    )").arg(title, desc);
}



BattlePage::BattlePage(Player* player, QVector<Enemy*> enemies, QWidget* parent)
    : QWidget(parent), player(player), enemies(enemies)
{
    if (this->player == nullptr)
        this->player = new Player("َAna", 80);

    if (this->enemies.isEmpty())
        this->enemies.append (new Cultist);

   // setupTestDeck();


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

    applyBattleTooltipStyleOnce();

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
    battleField->raise();
    QTimer::singleShot(0, this, &BattlePage::repositionOverlays);



    combatManager = new CombatManager(player, enemies, this);

    connect(combatManager, &CombatManager::statsUpdated, this, &BattlePage::updateStats);
    connect(combatManager, &CombatManager::battleWon,    this, &BattlePage::onBattleWon);
    connect(combatManager, &CombatManager::battleLost,   this, &BattlePage::onBattleLost);
    connect(endTurnBtn,    &QPushButton::clicked,        combatManager, &CombatManager::endTurn);
    connect(combatManager, &CombatManager::enemyIntentUpdated, this, &BattlePage::updateEnemyIntent);
    connect(combatManager, &CombatManager::enemiesChanged, this, &BattlePage::rebuildEnemyUI);

    connect(combatManager, &CombatManager::requestPileSelection,
            this, [this](PileType pileType)
            {
                if (pileType != PileType::Exhaust)
                    return;

                PileViewerDialog dialog(this->player,
                                        PileType::Exhaust,
                                        PileViewerMode::SelectCard,
                                        this);

                connect(&dialog, &PileViewerDialog::cardSelected,
                        this, [this, &dialog](Card* selectedCard)
                        {
                            combatManager->handleExhumeSelection(selectedCard);
                            dialog.accept();
                        });

                connect(&dialog, &QDialog::rejected,
                        this, [this]()
                        {
                            combatManager->cancelExhumeSelection();
                        });

                dialog.exec();
            });


    connect(combatManager, &CombatManager::enemyAttacking, this, [this](Enemy* e)
            {
                QWidget* enemyW = findWidgetForEnemy(e);
                if (enemyW)
                {
                    animateAttack(enemyW, playerWidget);
                }
            });

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

    // Potion slots
    potionButtons.clear();
    for (int i = 0; i < 3; i++)
    {
        QPushButton *potionBtn = new QPushButton(topBar);
        potionBtn->setFixedSize(30, 30);
        potionBtn->setCursor(Qt::PointingHandCursor);
        potionBtn->setStyleSheet(
            "QPushButton {"
            "background: rgba(255,255,255,20); border: 1px solid #666; border-radius: 4px;"
            "}"
            "QPushButton:hover:!disabled { border: 1px solid #fff; }"
            );

        connect(potionBtn, &QPushButton::clicked, this, [this, i]() {
            if (waitingForPotionTarget)
            {
                clearPotionSelection();
                return;
            }

            const auto& potions = player->getPotions();
            if (i >= potions.size() || !potions[i])
                return;

            Potion* potion = potions[i];

            if (isPotionTargeted(potion))
            {

                if (pendingCard)
                    clearSelection();

                pendingPotion = potion;
                waitingForPotionTarget = true;

                showEnemyPotionHighlights();
            }

            else
            {
                Enemy* target = nullptr;
                if (combatManager->usePotion(potion, target))
                {
                    updateStats();
                }
            }
        });


        leftGroup->addWidget(potionBtn);
        leftGroup->addSpacing(4);
        potionButtons.append(potionBtn);
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


    QPushButton *deckBtn = new QPushButton(topBar);
    deckBtn->setFixedSize(45, 45);
    deckBtn->setCursor(Qt::PointingHandCursor);
    deckBtn->setIcon(QIcon(QPixmap(":/deckIcon.png").scaled(
        45, 45,
        Qt::KeepAspectRatio,
        Qt::SmoothTransformation
        )));
    deckBtn->setIconSize(QSize(45, 45));
    deckBtn->setStyleSheet(
        "QPushButton {"
        "   background: transparent;"
        "   border: none;"
        "}"
        );

    // Deck Count Label


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
    rightGroup->addWidget(deckBtn);
    rightGroup->addWidget(settingsIcon);

    // ===== Assemble =====
    layout->addLayout(leftGroup);
    layout->addStretch();
    layout->addLayout(centerGroup);
    layout->addStretch();
    layout->addLayout(rightGroup);

    connect(deckBtn, &QPushButton::clicked, this, [this]()
            {
                PileViewerDialog dialog(player,
                            PileType::Deck,
                            PileViewerMode::ViewOnly,
                            this);
                dialog.exec();
            });
}

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

    // Effect icon
    playerEffectsWidget = new QWidget(playerWidget);
    playerEffectsWidget->setFixedHeight(32);
    playerEffectsLayout = new QHBoxLayout(playerEffectsWidget);
    playerEffectsLayout->setContentsMargins(0, 2, 0, 0);
    playerEffectsLayout->setSpacing(4);
    playerEffectsLayout->setAlignment(Qt::AlignLeft | Qt::AlignVCenter);

    playerLayout->addWidget(playerEffectsWidget, 0, Qt::AlignHCenter);

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
    enemyContainer = new QWidget(battleField);
    enemyLayout = new QHBoxLayout(enemyContainer);
    enemyLayout->setAlignment(Qt::AlignCenter | Qt::AlignBottom);
    enemyLayout->setSpacing(30);


    enemyUIs.clear();

    for (Enemy* enemy : std::as_const(enemies))
    {
        if (!enemy)
            continue;

        enemyUIs.append(createEnemyUI(enemy));
    }



    layout->addWidget(playerWidget, 0, Qt::AlignBottom);
    layout->addStretch(1);
    layout->addWidget(enemyContainer, 0, Qt::AlignBottom);
}

void BattlePage::setupClickOverlays()
{
    for (EnemyUI& ui : enemyUIs)
    {
        if (ui.clickOverlay)
        {
            ui.clickOverlay->deleteLater();
            ui.clickOverlay = nullptr;
        }
    }

    if (playerClickOverlay)
    {
        playerClickOverlay->deleteLater();
        playerClickOverlay = nullptr;
    }

    for (EnemyUI& ui : enemyUIs)
    {
        if (!ui.widget || !ui.enemy)
            continue;

        QPushButton* overlay = new QPushButton(this);
        overlay->setStyleSheet("background: transparent; border: none;");
        overlay->setCursor(Qt::ArrowCursor);
        overlay->hide();

        ui.clickOverlay = overlay;

        connect(overlay, &QPushButton::clicked, this,
                [this, enemy = ui.enemy]()
                {
                    if (!enemy || enemy->isDead())
                        return;

                    if (waitingForPotionTarget && pendingPotion)
                    {
                        if (combatManager->usePotion(pendingPotion, enemy))
                            updateStats();

                        clearPotionSelection();
                        return;
                    }

                    if (pendingCard)
                    {
                        playCardWithAnimation(pendingCard, selectedProxy, enemy);
                        return;
                    }
                });

    }

    playerClickOverlay = new QPushButton(this);
    playerClickOverlay->setStyleSheet("background: transparent; border: none;");
    playerClickOverlay->setCursor(Qt::ArrowCursor);
    playerClickOverlay->hide();

    connect(playerClickOverlay, &QPushButton::clicked, this, [this]()
            {
                if (pendingCard)
                    playCardWithAnimation(pendingCard, selectedProxy, nullptr);
            });
}

void BattlePage::repositionOverlays()
{
    for (const EnemyUI& ui : std::as_const(enemyUIs))
    {
        if (!ui.widget || !ui.clickOverlay)
            continue;

        const QPoint p = ui.widget->mapTo(this, QPoint(0, 0));
        ui.clickOverlay->setGeometry(p.x(), p.y(), ui.widget->width(), ui.widget->height());
        ui.clickOverlay->raise();
    }

    if (playerClickOverlay && playerWidget)
    {
        const QPoint p = playerWidget->mapTo(this, QPoint(0, 0));
        playerClickOverlay->setGeometry(
            p.x(),
            p.y(),
            playerWidget->width(),
            playerWidget->height()
            );
        playerClickOverlay->raise();
    }
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

    // ===== Exhaust Pile button (bottom-right) =====
    QString baseDir = QCoreApplication::applicationDirPath();
    QString exhaustPath = QDir(baseDir).filePath("assets/image/exhaustPile.png");
    QPixmap exhaustPixmap = QPixmap(exhaustPath).scaled(
        115, 138,
        Qt::KeepAspectRatio,
        Qt::SmoothTransformation);

    QSize exhaustSize = exhaustPixmap.size();

    exhaustPileBtn = new QPushButton(this);
    exhaustPileBtn->setFixedSize(exhaustSize);
    exhaustPileBtn->move(1035, 592);
    exhaustPileBtn->setCursor(Qt::PointingHandCursor);
    exhaustPileBtn->setFlat(true);
    exhaustPileBtn->setStyleSheet(
        "QPushButton {"
        "  border: none;"
        "  background: transparent;"
        "  padding: 0px;"
        "}"
        );
    exhaustPileBtn->setIcon(QIcon(exhaustPixmap));
    exhaustPileBtn->setIconSize(exhaustSize);

    auto* exhaustGlow = new QGraphicsDropShadowEffect(exhaustPileBtn);
    exhaustGlow->setColor(QColor(255, 0, 0, 255));
    exhaustGlow->setBlurRadius(90);
    exhaustGlow->setOffset(0, 0);
    exhaustPileBtn->setGraphicsEffect(exhaustGlow);


    exhaustPileBtn->raise();

    exhaustPileCountLabel = new QLabel(exhaustPileBtn);
    exhaustPileCountLabel->setGeometry(0, 0, exhaustSize.width(), exhaustSize.height());
    exhaustPileCountLabel->setAlignment(Qt::AlignCenter);
    exhaustPileCountLabel->setStyleSheet(
        "color: white;"
        "font-size: 16px;"
        "font-weight: bold;"
        "background: transparent;"
        );
    exhaustPileCountLabel->setAttribute(Qt::WA_TransparentForMouseEvents, true);
    exhaustPileCountLabel->raise();

    connect(exhaustPileBtn, &QPushButton::clicked,
            this, &BattlePage::onExhaustPileClicked);

}

bool BattlePage::eventFilter(QObject* obj, QEvent* event)
{
    // --- Enemy widget clicked as target ---
    if (obj->property("isEnemyTarget").toBool() &&
        event->type() == QEvent::MouseButtonPress)
    {
        QWidget* w = qobject_cast<QWidget*>(obj);
        Enemy* target = nullptr;

        for (const EnemyUI& ui : std::as_const(enemyUIs))
        {
            if (ui.widget == w || ui.clickOverlay == w)
            {
                target = ui.enemy;
                break;
            }
        }

        if (target && !target->isDead())
        {
            if (waitingForPotionTarget && pendingPotion)
            {
                if (combatManager->usePotion(pendingPotion, target))
                    updateStats();

                clearPotionSelection();
                return true;
            }

            if (pendingCard)
            {
                playCardWithAnimation(pendingCard, selectedProxy, target);
                return true;
            }
        }

        return QWidget::eventFilter(obj, event);
    }

    // --- Player widget clicked as target ---
    if (obj->property("isPlayerTarget").toBool() &&
        event->type() == QEvent::MouseButtonPress)
    {
        if (pendingCard)
        {
            playCardWithAnimation(pendingCard, selectedProxy, nullptr);
            return true;
        }
        return QWidget::eventFilter(obj, event);
    }

    QPushButton* card = qobject_cast<QPushButton*>(obj);
    if (!card)
        return QWidget::eventFilter(obj, event);

    QGraphicsProxyWidget* proxy = card->property("proxy").value<QGraphicsProxyWidget*>();
    if (!proxy)
        return QWidget::eventFilter(obj, event);

    Card* cardData = card->property("cardData").value<Card*>();

    if (pendingCard && cardData != pendingCard)
        return QWidget::eventFilter(obj, event);

    // --- Hover Enter Animation ---
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

        const bool playableNow = isCardPlayableNow(cardData);

        if (playableNow)
        {
            QGraphicsDropShadowEffect* glow = new QGraphicsDropShadowEffect();
            glow->setColor(QColor(0, 150, 255, 255));
            glow->setBlurRadius(80);
            glow->setOffset(0, 0);
            proxy->setGraphicsEffect(glow);
        }
        else
        {
            proxy->setGraphicsEffect(nullptr);
        }

    }
    // --- Hover Leave Animation ---
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
    CombatDeck* deck = player ? player->getCombatDeck() : nullptr;

    if (player)
    {
        if (playerHPBar)
        {
            playerHPBar->setMaximum(player->getMaxHealth());
            playerHPBar->setValue(player->getCurrentHealth());
        }

        if (playerHpLabel)
        {
            playerHpLabel->setText(QString("%1 / %2")
                                       .arg(player->getCurrentHealth())
                                       .arg(player->getMaxHealth()));
        }

         int block = player->getBlock();
         bool hasBlock = block > 0;

        if (playerBlockIconLabel)
            playerBlockIconLabel->setVisible(hasBlock);

        if (playerBlockLabel)
        {
            playerBlockLabel->setVisible(hasBlock);
            if (hasBlock)
                playerBlockLabel->setText(QString::number(block));
            else
                playerBlockLabel->clear();
        }

        if (playerHPBar)
        {
            if (hasBlock)
            {
                if (!playerHPBar->graphicsEffect())
                {
                    auto* glow = new QGraphicsDropShadowEffect(playerHPBar);
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
        }

        if (energyValueLabel)
        {
            energyValueLabel->setText(QString("%1/%2")
                                          .arg(player->getCurrentEnergy())
                                          .arg(player->getMaxEnergy()));
        }
    }
    else
    {
        if (playerHPBar)
        {
            playerHPBar->setMaximum(1);
            playerHPBar->setValue(0);

            if (QGraphicsEffect* effect = playerHPBar->graphicsEffect())
            {
                playerHPBar->setGraphicsEffect(nullptr);
                effect->deleteLater();
            }

            playerHPBar->setStyleSheet(
                "QProgressBar { background: #1a1a1a; border: 2px solid #333;"
                "border-radius: 6px; color: white; font-size: 12px; text-align: center; }"
                "QProgressBar::chunk { background: #e63946; border-radius: 4px; }"
                );
        }

        if (playerHpLabel)
            playerHpLabel->setText("0 / 0");

        if (playerBlockIconLabel)
            playerBlockIconLabel->hide();

        if (playerBlockLabel)
        {
            playerBlockLabel->hide();
            playerBlockLabel->clear();
        }

        if (energyValueLabel)
            energyValueLabel->setText("0/0");
    }

    for (EnemyUI& ui : enemyUIs)
    {
        if (!ui.enemy)
            continue;

        if (ui.enemy->isDead())
        {
            if (ui.hpBar)
            {
                ui.hpBar->setValue(0);
                ui.hpBar->hide();
            }

            if (ui.intentLabel)
                ui.intentLabel->setText("Defeated");

            if (ui.widget)
            {
                ui.widget->setEnabled(false);
                ui.widget->setAttribute(Qt::WA_TransparentForMouseEvents, true);

                auto *ghost = new QGraphicsOpacityEffect(ui.widget);
                ghost->setOpacity(0.45);
                ui.widget->setGraphicsEffect(ghost);
            }

            if (ui.clickOverlay)
            {
                ui.clickOverlay->setEnabled(false);
                ui.clickOverlay->setAttribute(Qt::WA_TransparentForMouseEvents, true);
                ui.clickOverlay->hide();
            }

            if (ui.blockWidget)
                ui.blockWidget->hide();

            continue;
        }

        int block = ui.enemy->getBlock();
        bool hasBlock = block > 0;


        if (ui.hpBar)
        {
            ui.hpBar->setMaximum(ui.enemy->getMaxHealth());
            ui.hpBar->setValue(ui.enemy->getCurrentHealth());


            if (hasBlock)
            {
                if (!ui.hpBar->graphicsEffect())
                {
                    auto* glow = new QGraphicsDropShadowEffect(ui.hpBar);
                    glow->setColor(QColor(96, 165, 250, 255));
                    glow->setBlurRadius(20);
                    glow->setOffset(0, 0);
                    ui.hpBar->setGraphicsEffect(glow);
                }

                ui.hpBar->setStyleSheet(
                    "QProgressBar { background: #1a1a1a; border: 2px solid #333;"
                    "border-radius: 6px; color: white; font-size: 11px; text-align: center; }"
                    "QProgressBar::chunk { background: #60a5fa; border-radius: 4px; }"
                    );
            }
            else
            {
                ui.hpBar->setGraphicsEffect(nullptr);
                ui.hpBar->setStyleSheet(
                    "QProgressBar { background: #1a1a1a; border: 2px solid #333;"
                    "border-radius: 6px; color: white; font-size: 11px; text-align: center; }"
                    "QProgressBar::chunk { background: #e63946; border-radius: 4px; }"
                    );
            }
        }


        if (ui.blockWidget)
        {
            ui.blockWidget->setVisible(hasBlock);
            if (hasBlock && ui.blockValueLabel)
            {
                ui.blockValueLabel->setText(QString::number(block));
            }
        }

        if (ui.intentLabel)
            ui.intentLabel->setText(getIntentText(ui.enemy));

        if (ui.widget)
        {
            ui.widget->setEnabled(true);
            ui.widget->show();
        }

        if (ui.clickOverlay)
            ui.clickOverlay->setEnabled(true);
    }

    if (!animatingCard)
        refreshHand();

    if (deck)
    {
        if (drawPileCountLabel)
            drawPileCountLabel->setText(QString::number(deck->getDrawPile().size()));

        if (discardPileCountLabel)
            discardPileCountLabel->setText(QString::number(deck->getDiscardPile().size()));

        if (exhaustPileCountLabel)
        {
            exhaustPileCountLabel->setText(QString::number(deck->getExhaustPile().size()));
        }

    }
    else
    {
        if (drawPileCountLabel)
            drawPileCountLabel->setText("0");

        if (discardPileCountLabel)
            discardPileCountLabel->setText("0");

        if (exhaustPileCountLabel)
            exhaustPileCountLabel->setText("0");
    }

    updateEffectsUI();

    updatePotionUI();
}

void BattlePage::refreshHand()
{

    handScene->clear();

    if (!player)
        return;

    CombatDeck* deck = player->getCombatDeck();
    if (!deck)
        return;

    const auto& hand = deck->getHand();
    // Scene is already empty; nothing left to layout.
    if (hand.isEmpty())
        return;

    const int cardW    = 150;
    const int cardH    = 220;
    const int spacing  = 105;
    const int viewW    = 900;
    const float maxRot = 12.0f;
    const int arcH     = 20;
    const int baseY    = 400 - cardH + 120;

    const int totalW = (hand.size() - 1) * spacing + cardW;
    const int startX = (viewW - totalW) / 2;

    for (int i = 0; i < hand.size(); ++i) {
        Card* card = hand[i];
        if (!card)
            continue;

        const float norm = hand.size() > 1
                               ? (i - (hand.size() - 1) / 2.0f) / ((hand.size() - 1) / 2.0f)
                               : 0.0f;

        const int arcOffset  = static_cast<int>(arcH * (1.0f - norm * norm));
        const float rotation = maxRot * norm;
        const int x = startX + i * spacing;
        const int y = baseY - arcOffset;

        // Single QPushButton per card (no nested child widgets) so hover/click
        // events stay simple and predictable.
        QPushButton* btn = new QPushButton();
        btn->setFixedSize(cardW, cardH);
        btn->setCursor(Qt::PointingHandCursor);

        QPixmap px(DeckViewerDialog::cardImagePath(card));

        if (!px.isNull()) {
            const QPixmap scaled = px.scaled(
                cardW, cardH,
                Qt::IgnoreAspectRatio,
                Qt::SmoothTransformation);
            btn->setIcon(QIcon(scaled));
            btn->setIconSize(QSize(cardW, cardH));
            btn->setStyleSheet(
                "QPushButton { border: none; background: transparent; padding: 0; }");
        }
        else
        {
            // Fallback when the card art is missing.
            const QString typeColor =
                (card->getType() == CardType::Attack) ? "#7f1d1d" : "#1e3a5f";
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
        if (!proxy)
            continue;

        proxy->setFlag(QGraphicsItem::ItemClipsChildrenToShape, false);
        proxy->setFlag(QGraphicsItem::ItemClipsToShape, false);
        proxy->setCacheMode(QGraphicsItem::NoCache);
        proxy->setPos(x, y);
        proxy->setRotation(rotation);
        proxy->setTransformOriginPoint(cardW / 2.0, cardH / 2.0);
        proxy->setZValue(i);

        // Store rest pose for hover / cancel animations.
        proxy->setData(0, y);
        proxy->setData(1, static_cast<double>(rotation));

        btn->setProperty("proxy", QVariant::fromValue(proxy));
        btn->setProperty("defaultY", y);
        btn->setProperty("defaultRotation", rotation);
        btn->setProperty("index", i);
        btn->setProperty("cardData", QVariant::fromValue(card));
        btn->installEventFilter(this);

        // QPointer auto-nulls if the proxy is destroyed (e.g. next refreshHand).
        // This protects the click lambda from use-after-free.
        QPointer<QGraphicsProxyWidget> safeProxy = proxy;
        connect(btn, &QPushButton::clicked, [this, card, safeProxy]() {
             if (safeProxy)
                onCardClicked(card, safeProxy.data());
        });
    }
}

void BattlePage::onCardClicked(Card* card, QGraphicsProxyWidget* proxy)
{
    if (!card || !proxy)
        return;

    if (pendingCard == card)
    {
        clearSelection();
        resetCardToHandPose(proxy);
        return;
    }

    if (pendingCard != nullptr)
        return;

    if (!isCardPlayableNow(card))
    {
        clearSelection();
        animateUnplayableCard(proxy);
        return;
    }

    clearSelection();

    pendingCard = card;
    selectedProxy = proxy;

    if (selectedProxy)
        selectedProxy->setZValue(105);

    CardTarget target = getCardTarget(card);

    if (target == CardTarget::Enemy)
    {
        showEnemyHighlights();

        int baseDamage  = card->getBaseDamage();
        int damageHits  = card->getDamageHits();

        for (const EnemyUI& ui : std::as_const(enemyUIs))
        {
            if (ui.enemy && !ui.enemy->isDead())
            {
                int perHitDamage = combatManager->getCalculator()->calculateDamage(player, ui.enemy, baseDamage);

                int totalDamage = perHitDamage * damageHits;

                updateDamagePreview(ui.enemy, totalDamage);
            }
        }
    }
    else if (target == CardTarget::Player)
    {
        showPlayerHighlight();
    }


    else if (target == CardTarget::Player)
        showPlayerHighlight();
}

void BattlePage::onBattleWon()
{
    emit combatResult(true);
    emit battleEnded();
}

void BattlePage::onBattleLost()
{
    emit combatResult(false);
    emit battleEnded();
}

void BattlePage::onDrawPileClicked()
{
    PileViewerDialog dialog(player,
                            PileType::Draw,
                            PileViewerMode::ViewOnly,
                            this);
    dialog.exec();
}

void BattlePage::onExhaustPileClicked()
{
    PileViewerDialog dialog(player,
                            PileType::Exhaust,
                            PileViewerMode::ViewOnly,
                            this);
    dialog.exec();
}


void BattlePage::onDiscardPileClicked()
{

    PileViewerDialog dialog(player,
                            PileType::Discard,
                            PileViewerMode::ViewOnly,
                            this);
    dialog.exec();
}


QString BattlePage::getIntentText(Enemy* enemy)
{
    if (!enemy || enemy->isDead())
        return "Defeated";

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
    if (!enemy)
        return;

    for (EnemyUI& ui : enemyUIs)
    {
        if (ui.enemy != enemy)
            continue;

        if (!ui.intentLabel)
            return;

        if (enemy->isDead())
        {
            ui.intentLabel->setText("Defeated");
            continue;
        }

            ui.intentLabel->setText(getIntentText(enemy));

        return;
    }
}



QString BattlePage::enemyImagePath(Enemy* enemy)
{
    if (!enemy) return QString();

    QString cleanName = enemy->getName();

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

    for (EnemyUI& ui : enemyUIs)
    {
        if (!ui.enemy || !ui.widget)
            continue;

        if (ui.enemy->isDead())
        {
                ui.widget->setEnabled(false);
                ui.widget->setAttribute(Qt::WA_TransparentForMouseEvents, true);

                auto *ghost = new QGraphicsOpacityEffect(ui.widget);
                ghost->setOpacity(0.45);
                ui.widget->setGraphicsEffect(ghost);

                ui.clickOverlay->setEnabled(false);
                ui.clickOverlay->setAttribute(Qt::WA_TransparentForMouseEvents, true);
                ui.clickOverlay->hide();

            continue;
        }

        auto* glow = new QGraphicsDropShadowEffect(ui.widget);
        glow->setColor(QColor(220, 50, 50, 255));
        glow->setBlurRadius(60);
        glow->setOffset(0, 0);

        ui.widget->setGraphicsEffect(glow);

        if (ui.clickOverlay)
        {
            ui.clickOverlay->setCursor(Qt::PointingHandCursor);
            ui.clickOverlay->raise();
            ui.clickOverlay->show();
            ui.clickOverlay->setEnabled(true);
        }
    }
}


void BattlePage::showPlayerHighlight()
{
    clearHighlights();

    if (!playerWidget)
        return;

    auto* glow = new QGraphicsDropShadowEffect(playerWidget);
    glow->setColor(QColor(50, 200, 50, 255));
    glow->setBlurRadius(60);
    glow->setOffset(0, 0);

    playerWidget->setGraphicsEffect(glow);
    playerGlowEffect = glow;

    if (playerClickOverlay)
    {
        playerClickOverlay->setCursor(Qt::PointingHandCursor);
        playerClickOverlay->raise();
        playerClickOverlay->show();
        playerClickOverlay->setEnabled(true);
    }
}

void BattlePage::clearHighlights()
{
    for (EnemyUI& ui : enemyUIs)
    {
        if (ui.enemy->isDead())
        {
            ui.widget->setEnabled(false);
            ui.widget->setAttribute(Qt::WA_TransparentForMouseEvents, true);

            auto *ghost = new QGraphicsOpacityEffect(ui.widget);
            ghost->setOpacity(0.45);
            ui.widget->setGraphicsEffect(ghost);

            ui.clickOverlay->setEnabled(false);
            ui.clickOverlay->setAttribute(Qt::WA_TransparentForMouseEvents, true);
            ui.clickOverlay->hide();

            continue;
        }

        if (ui.widget)
        {
            ui.widget->setGraphicsEffect(nullptr);
            ui.widget->unsetCursor();
        }

        if (ui.clickOverlay)
        {
            ui.clickOverlay->hide();
            ui.clickOverlay->setEnabled(false);
            ui.clickOverlay->unsetCursor();
        }
    }

    if (playerWidget)
    {
        playerWidget->setGraphicsEffect(nullptr);
        playerWidget->unsetCursor();
    }

    if (playerClickOverlay)
    {
        playerClickOverlay->hide();
        playerClickOverlay->setEnabled(false);
        playerClickOverlay->unsetCursor();
    }

    playerGlowEffect = nullptr;
}

void BattlePage::clearSelection()
{
    pendingCard   = nullptr;
    selectedProxy = nullptr;
    clearHighlights();

    for (const EnemyUI& ui : std::as_const(enemyUIs))
    {
        if (ui.enemy)
            clearDamagePreview(ui.enemy);
    }

}
void BattlePage::playCardWithAnimation(Card* card,
                                       QGraphicsProxyWidget* proxy,
                                       Enemy* target)
{
    QGraphicsProxyWidget* proxyToAnim = proxy;
    Card* cardToPlay = card;
    Enemy* targetEnemy = target;

    clearSelection();

    // Not enough energy: bounce the card back; do NOT set animatingCard.
    if (!cardToPlay || !proxyToAnim || !isCardPlayableNow(cardToPlay))
    {
        clearSelection();
        resetCardToHandPose(proxyToAnim);
        return;
    }

    clearSelection();

    animatingCard = true;

    // Capture pose, then transfer ownership from handScene -> animScene.
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
            [this, proxyToAnim, cardToPlay, targetEnemy]()
            {
                // Step 2: fly to correct pile + shrink.
                QPushButton* pileBtn = cardToPlay->doesExhaust()
                                           ? exhaustPileBtn
                                           : discardPileBtn;

                QPoint pileInAnimView = animView->mapFromGlobal(
                    pileBtn->mapToGlobal(
                        QPoint(pileBtn->width() / 2,
                               pileBtn->height() / 2)));

                QPointF pileScene = animView->mapToScene(pileInAnimView);

                QSizeF proxySize = proxyToAnim->size();
                pileScene -= QPointF(proxySize.width() / 2.0,
                                     proxySize.height() / 2.0);

                auto* flyOut = new QPropertyAnimation(proxyToAnim, "pos");
                flyOut->setDuration(300);
                flyOut->setStartValue(proxyToAnim->pos());
                flyOut->setEndValue(pileScene);
                flyOut->setEasingCurve(QEasingCurve::InCubic);

                auto* shrink = new QPropertyAnimation(proxyToAnim, "scale");
                shrink->setDuration(300);
                shrink->setStartValue(proxyToAnim->scale());
                shrink->setEndValue(0.0);
                shrink->setEasingCurve(QEasingCurve::InCubic);

                connect(flyOut, &QPropertyAnimation::finished, this,
                        [this, proxyToAnim, cardToPlay, targetEnemy]()
                        {
                            animScene->removeItem(proxyToAnim);
                            animatingCard = false;

                            if (targetEnemy &&
                                cardToPlay->getType() == CardType::Attack)
                            {
                                QWidget* enemyW = findWidgetForEnemy(targetEnemy);

                                animateAttack(playerWidget, enemyW,
                                              [this, cardToPlay, targetEnemy]()
                                              {
                                                  combatManager->playCard(cardToPlay, targetEnemy);
                                              });
                            }
                            else
                            {
                                combatManager->playCard(cardToPlay, targetEnemy);
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

    QWidget* realAttacker = attacker;
    QWidget* attackerOverlay = nullptr;

    for (EnemyUI& ui : enemyUIs)
    {
        QWidget* enemyWidget = ui.widget;

        if (enemyWidget &&
            (enemyWidget == attacker || enemyWidget->isAncestorOf(attacker)))
        {
            realAttacker = enemyWidget;
            attackerOverlay = ui.clickOverlay;
            break;
        }
    }

    const QPixmap snapshot = realAttacker->grab();
    if (snapshot.isNull())
    {
        if (onDone) onDone();
        return;
    }

    const QPoint origin = realAttacker->mapTo(this, QPoint(0, 0));
    const QSize attackerSize = realAttacker->size();

    QPointer<QWidget> safeAttacker = realAttacker;
    QPointer<QWidget> safeAttackerOverlay = attackerOverlay;

    const bool overlayWasVisible = attackerOverlay && attackerOverlay->isVisible();
    const bool overlayWasEnabled = attackerOverlay && attackerOverlay->isEnabled();

    auto* opacityEffect = new QGraphicsOpacityEffect(realAttacker);
    opacityEffect->setOpacity(0.0);
    realAttacker->setGraphicsEffect(opacityEffect);

    if (attackerOverlay)
    {
        attackerOverlay->hide();
        attackerOverlay->setEnabled(false);
    }

    auto* ghost = new QLabel(this);
    QPointer<QLabel> safeGhost = ghost;

    ghost->setPixmap(snapshot);
    ghost->setFixedSize(attackerSize);
    ghost->setAttribute(Qt::WA_TransparentForMouseEvents);
    ghost->setStyleSheet("background: transparent; border: none;");
    ghost->setScaledContents(false);
    ghost->move(origin);
    ghost->show();
    ghost->raise();

    const QPoint attackerCenter = origin + QPoint(attackerSize.width() / 2,
                                                  attackerSize.height() / 2);
    const QPoint targetCenter = target->mapTo(this, target->rect().center());

    QPointF dir = QPointF(targetCenter - attackerCenter);
    const float len = std::sqrt(dir.x() * dir.x() + dir.y() * dir.y());
    const QPointF nudge = (len > 0.f) ? dir / len * 35.0f : QPointF(0, 0);

    const QPoint forward = origin + QPoint(int(nudge.x()), int(nudge.y()));

    auto* goForward = new QPropertyAnimation(ghost, "pos", ghost);
    goForward->setDuration(120);
    goForward->setStartValue(origin);
    goForward->setEndValue(forward);
    goForward->setEasingCurve(QEasingCurve::OutCubic);

    auto* goBack = new QPropertyAnimation(ghost, "pos", ghost);
    goBack->setDuration(200);
    goBack->setStartValue(forward);
    goBack->setEndValue(origin);
    goBack->setEasingCurve(QEasingCurve::OutBounce);

    connect(goForward, &QPropertyAnimation::finished, this,
            [goBack, onDone]() {
                goBack->start();

                if (onDone)
                    onDone();
            });

    connect(goBack, &QPropertyAnimation::finished, this,
            [safeGhost,
             safeAttacker,
             safeAttackerOverlay,
             overlayWasVisible,
             overlayWasEnabled]() {
                if (safeAttacker)
                    safeAttacker->setGraphicsEffect(nullptr);

                if (safeAttackerOverlay)
                {
                    safeAttackerOverlay->setEnabled(overlayWasEnabled);

                    if (overlayWasVisible)
                        safeAttackerOverlay->show();
                }

                if (safeGhost)
                    safeGhost->deleteLater();
            });

    goForward->start();
}


QString BattlePage::effectImagePath(const Effect* effect)
{
    if (!effect) return QString();

    QString cleanName = effect->getName();
    cleanName.remove(' ');
    cleanName.remove('\'');
    cleanName.remove('.');

    if(cleanName == "Enrage" || cleanName == "Berserk" || cleanName == "Entangle" || cleanName == "Rage")
    {
        QString baseDir = QCoreApplication::applicationDirPath();
        QString EffectPath = QDir(baseDir).filePath(QString("assets/Effect/%1Eff.png").arg(cleanName));
        return EffectPath;
    }
    else
    {
        return QString(":/Effect/%1Eff.png").arg(cleanName);
    }
}

void BattlePage::updateEffectsUI()
{
    // =========================
    // Update player effects
    // =========================
    if (playerEffectsLayout && playerEffectsWidget && player)
    {
        QLayoutItem* item;
        while ((item = playerEffectsLayout->takeAt(0)) != nullptr)
        {
            if (item->widget())
                item->widget()->deleteLater();
            delete item;
        }

        const auto& playerEffects = player->getEffects();
        for (Effect* effect : playerEffects)
        {
            if (!effect || effect->isExpired())
                continue;

            QWidget* iconContainer = new QWidget(playerEffectsWidget);
            iconContainer->setFixedSize(24, 24);
            iconContainer->setToolTip(makeEffectTooltipHtml(effect));
            iconContainer->setAttribute(Qt::WA_AlwaysShowToolTips, true);
            iconContainer->setMouseTracking(true);

            QLabel* iconLabel = new QLabel(iconContainer);
            iconLabel->setGeometry(1, 1, 22, 22);
            iconLabel->setScaledContents(true);
            iconLabel->setPixmap(
                QPixmap(effectImagePath(effect)).scaled(
                    22, 22,
                    Qt::KeepAspectRatio,
                    Qt::SmoothTransformation));
            iconLabel->setAttribute(Qt::WA_TransparentForMouseEvents, true);

            int displayValue = effect->getDisplayValue();
            if (effect->shouldShowNumber())
            {
                QLabel* numLabel = new QLabel(iconContainer);
                numLabel->setText(QString::number(displayValue));
                numLabel->setAlignment(Qt::AlignRight | Qt::AlignBottom);
                numLabel->setGeometry(8, 8, 14, 14);
                numLabel->setAttribute(Qt::WA_TransparentForMouseEvents, true);
                numLabel->setStyleSheet(
                    "color: #ffffff;"
                    "font-size: 9px;"
                    "font-weight: bold;"
                    "background: transparent;"
                    "border: none;");
            }

            playerEffectsLayout->addWidget(iconContainer);
        }
    }

    // =========================
    // Update enemies effects
    // =========================
    for (EnemyUI& enemyUI : enemyUIs)
    {
        if (!enemyUI.effectsLayout || !enemyUI.effectsWidget || !enemyUI.enemy)
            continue;

        QLayoutItem* item;
        while ((item = enemyUI.effectsLayout->takeAt(0)) != nullptr)
        {
            if (item->widget())
                item->widget()->deleteLater();
            delete item;
        }

        const auto& enemyEffects = enemyUI.enemy->getEffects();
        for (Effect* effect : enemyEffects)
        {
            if (!effect || effect->isExpired())
                continue;

            QWidget* iconContainer = new QWidget(enemyUI.effectsWidget);
            iconContainer->setFixedSize(24, 24);
            iconContainer->setToolTip(makeEffectTooltipHtml(effect));
            iconContainer->setAttribute(Qt::WA_AlwaysShowToolTips, true);
            iconContainer->setMouseTracking(true);

            QLabel* iconLabel = new QLabel(iconContainer);
            iconLabel->setGeometry(1, 1, 22, 22);
            iconLabel->setScaledContents(true);
            iconLabel->setPixmap(
                QPixmap(effectImagePath(effect)).scaled(
                    22, 22,
                    Qt::KeepAspectRatio,
                    Qt::SmoothTransformation));
            iconLabel->setAttribute(Qt::WA_TransparentForMouseEvents, true);

            int displayValue = effect->getDisplayValue();
            if (effect->shouldShowNumber())
            {
                QLabel* numLabel = new QLabel(iconContainer);
                numLabel->setText(QString::number(displayValue));
                numLabel->setAlignment(Qt::AlignRight | Qt::AlignBottom);
                numLabel->setGeometry(8, 8, 14, 14);
                numLabel->setAttribute(Qt::WA_TransparentForMouseEvents, true);
                numLabel->setStyleSheet(
                    "color: #ffffff;"
                    "font-size: 9px;"
                    "font-weight: bold;"
                    "background: transparent;"
                    "border: none;");
            }

            enemyUI.effectsLayout->addWidget(iconContainer);
        }
    }
}

bool BattlePage::isCardPlayableNow(Card* card) const
{
    if (!card || !player)
        return false;

    if (!card->canPlay())
        return false;

    if (player->getCurrentEnergy() < card->getEnergyCost())
        return false;

    if (player->hasEffect(Effect::Type::Entangle) && card->getType() == CardType::Attack)
        return false;

    return true;
}



void BattlePage::resetCardToHandPose(QGraphicsProxyWidget* proxy)
{
    if (!proxy)
        return;

    QPropertyAnimation* moveDown = new QPropertyAnimation(proxy, "pos");
    moveDown->setDuration(200);
    moveDown->setStartValue(proxy->pos());
    moveDown->setEndValue(QPointF(proxy->pos().x(), proxy->data(0).toInt()));
    moveDown->setEasingCurve(QEasingCurve::OutCubic);
    moveDown->start(QAbstractAnimation::DeleteWhenStopped);

    QPropertyAnimation* rotateBack = new QPropertyAnimation(proxy, "rotation");
    rotateBack->setDuration(200);
    rotateBack->setStartValue(proxy->rotation());
    rotateBack->setEndValue(proxy->data(1).toDouble());
    rotateBack->setEasingCurve(QEasingCurve::OutCubic);
    rotateBack->start(QAbstractAnimation::DeleteWhenStopped);

    QPropertyAnimation* scaleDown = new QPropertyAnimation(proxy, "scale");
    scaleDown->setDuration(200);
    scaleDown->setStartValue(proxy->scale());
    scaleDown->setEndValue(1.0);
    scaleDown->setEasingCurve(QEasingCurve::OutCubic);

    connect(scaleDown, &QPropertyAnimation::finished, this, [proxy]() {
        if (proxy)
            proxy->setGraphicsEffect(nullptr);
    });

    scaleDown->start(QAbstractAnimation::DeleteWhenStopped);
}


void BattlePage::animateUnplayableCard(QGraphicsProxyWidget* proxy)
{
    if (!proxy)
        return;

    const QPointF origin = proxy->pos();

    auto* shake = new QPropertyAnimation(proxy, "pos");
    shake->setDuration(180);
    shake->setKeyValueAt(0.0, origin);
    shake->setKeyValueAt(0.20, origin + QPointF(-8, 0));
    shake->setKeyValueAt(0.40, origin + QPointF(8, 0));
    shake->setKeyValueAt(0.60, origin + QPointF(-5, 0));
    shake->setKeyValueAt(0.80, origin + QPointF(5, 0));
    shake->setKeyValueAt(1.0, origin);
    shake->setEasingCurve(QEasingCurve::OutCubic);

    connect(shake, &QPropertyAnimation::finished, this, [this, proxy]() {
        resetCardToHandPose(proxy);
    });

    shake->start(QAbstractAnimation::DeleteWhenStopped);
}

void BattlePage::updateDamagePreview(Enemy* enemy, int damage)
{
    EnemyUI* ui = findEnemyUi(enemy);
    if (!ui || !ui->damageIconLabel || !ui->damageValueLabel)
        return;

    if (damage <= 0)
    {
        ui->damageIconLabel->hide();
        ui->damageValueLabel->hide();
        return;
    }

    ui->damageIconLabel->show();
    ui->damageValueLabel->show();
    ui->damageValueLabel->setText(QString::number(damage));
}

void BattlePage::clearDamagePreview(Enemy* enemy)
{
    updateDamagePreview(enemy, 0);
}

void BattlePage::updatePotionUI()
{
    auto potions = player->getPotions();

    for (int i = 0; i < 3; i++)
    {
        QPushButton* btn = potionButtons.at(i);

        if (i < potions.size())
        {
            Potion* potion = potions.at(i);

            btn->setIcon(QIcon(getPotionImagePath(potion->getName())));
            btn->setIconSize(QSize(28, 28));
            btn->setToolTip(makePotionTooltipHtml(potion));
        }
        else
        {
            btn->setIcon(QIcon(getPotionImagePath("Potion Empty")));
            btn->setToolTip(QString());
        }
    }
}


QString BattlePage::getPotionImagePath(const QString &potionName)
{
    if (potionName == "Block Potion")
    {
        return ":/Potion/block_potion.png";
    }
    else if (potionName == "Energy Potion")
    {
        return ":/Potion/energy_potion.png";
    }
    else if (potionName == "Fairy in a Bottle")
    {
        return ":/Potion/fairy_in_a_bottle.png";
    }
    else if (potionName == "Fire Potion")
    {
        return ":/Potion/fire_potion.png";
    }
    else if (potionName == "Swift Potion")
    {
        return ":/Potion/swift_potion.png";
    }
    else if (potionName == "Potion Empty")
    {
        return ":/Potion/potionEmpty.png";
    }

    return ":/Potion/potionEmpty.png";
}

void BattlePage::showEnemyPotionHighlights()
{
   showEnemyHighlights();
}

void BattlePage::clearPotionSelection()
{
    pendingPotion = nullptr;
    waitingForPotionTarget = false;

    clearHighlights();
}

bool BattlePage::isPotionTargeted(Potion* potion) const
{
    if (!potion)
        return false;

    return potion->getName() == "Fire Potion";
}

void BattlePage::rebuildEnemyUI()
{
    if (!combatManager || !enemyLayout || !enemyContainer)
        return;

    // به روز رسانی لیست لوکال دشمنان از روی CombatManager
    enemies = combatManager->getEnemies();

    // ۱. حذف دکمه‌های Overlay کلیک قبلی برای جلوگیری از dangling pointer
    for (EnemyUI& ui : enemyUIs)
    {
        if (ui.clickOverlay)
        {
            ui.clickOverlay->deleteLater();
            ui.clickOverlay = nullptr;
        }
    }

    // ۲. خالی کردن کامل لایوت قدیمی دشمنان و حذف فیزیکی ویجت‌ها از صحنه
    while (QLayoutItem* item = enemyLayout->takeAt(0))
    {
        if (QWidget* w = item->widget())
            w->deleteLater();
        delete item;
    }

    enemyUIs.clear();

    // ۳. ساخت مجدد تمام دشمنان (جدید و قدیم)
    for (Enemy* enemy : std::as_const(enemies))
    {
        if (!enemy)
            continue;

        enemyUIs.append(createEnemyUI(enemy));
    }

    // ۴. بازنشانی لایه بالایی دکمه‌ها، وضعیت‌ها، Intent و موقعیت کلیک‌ها
    setupClickOverlays();
    updateStats();

    for (Enemy* enemy : std::as_const(enemies))
    {
        updateEnemyIntent(enemy);
    }

    repositionOverlays();
}

BattlePage::EnemyUI BattlePage::createEnemyUI(Enemy* enemy)
{
    BattlePage::EnemyUI ui{};

    if (!enemy || !enemyContainer || !enemyLayout)
        return ui;

    ui.enemy = enemy;

    ui.widget = new QWidget(enemyContainer);
    ui.widget->setFixedSize(220, 380);
    ui.widget->setStyleSheet("background: transparent;");
    enemyLayout->addWidget(ui.widget);

    QVBoxLayout* enemyInnerLayout = new QVBoxLayout(ui.widget);
    enemyInnerLayout->setContentsMargins(0, 0, 0, 10);
    enemyInnerLayout->setSpacing(5);

    // ۱. قصد دشمن (Intent)
    ui.intentLabel = new QLabel(getIntentText(enemy), ui.widget);
    ui.intentLabel->setAlignment(Qt::AlignCenter);
    ui.intentLabel->setFixedHeight(30);
    ui.intentLabel->setStyleSheet(
        "color: #f87171; font-size: 15px; font-weight: bold;"
        "background: transparent; border-radius: 8px; padding: 2px;"
        );

    // ۲. تصویر دشمن
    QLabel* enemyImg = new QLabel(ui.widget);
    enemyImg->setFixedSize(180, 200);
    enemyImg->setAlignment(Qt::AlignBottom | Qt::AlignHCenter);
    enemyImg->setStyleSheet("background: transparent;");

    QPixmap px(enemyImagePath(enemy));
    enemyImg->setPixmap(px.scaled(
        180, 200,
        Qt::KeepAspectRatio,
        Qt::SmoothTransformation
        ));

    // ۳. نوار سلامت (HP Bar) با استایل‌دهی کامل
    ui.hpBar = new QProgressBar(ui.widget);
    ui.hpBar->setRange(0, enemy->getMaxHealth());
    ui.hpBar->setValue(enemy->getCurrentHealth());
    ui.hpBar->setFixedSize(100, 16);
    ui.hpBar->setTextVisible(true);
    ui.hpBar->setFormat("%v / %m");
    ui.hpBar->setStyleSheet(
        "QProgressBar {"
        "  background: #1a1a1a;"
        "  border: 2px solid #333;"
        "  border-radius: 6px;"
        "  color: white;"
        "  font-size: 11px;"
        "  text-align: center;"
        "}"
        "QProgressBar::chunk {"
        "  background: #e63946;"
        "  border-radius: 4px;"
        "}"
        );

    // ۴. پیش‌نمایش آسیب (Damage Preview)
    QWidget* damagePreviewWidget = new QWidget(ui.widget);
    damagePreviewWidget->setFixedSize(70, 40);
    damagePreviewWidget->setStyleSheet("background: transparent;");

    ui.damageIconLabel = new QLabel(damagePreviewWidget);
    ui.damageIconLabel->setGeometry(0, 0, 40, 40);
    ui.damageIconLabel->setPixmap(
        QPixmap(":/attack.png").scaled(
            40, 40,
            Qt::KeepAspectRatio,
            Qt::SmoothTransformation
            )
        );
    ui.damageIconLabel->setStyleSheet("background: transparent;");
    ui.damageIconLabel->hide();

    ui.damageValueLabel = new QLabel(damagePreviewWidget);
    ui.damageValueLabel->setGeometry(35, 8, 35, 24);
    ui.damageValueLabel->setStyleSheet(
        "color: #ff4d4d;"
        "font-size: 16px;"
        "font-weight: bold;"
        "background: transparent;"
        );
    ui.damageValueLabel->hide();

    // ۵. ویجت بلاک / شیلد (Block Widget)
    ui.blockWidget = new QWidget(ui.widget);
    ui.blockWidget->setFixedSize(36, 36);
    ui.blockWidget->setStyleSheet("background: transparent;");

    ui.blockIconLabel = new QLabel(ui.blockWidget);
    ui.blockIconLabel->setGeometry(0, 0, 36, 36);
    ui.blockIconLabel->setPixmap(QPixmap(":/defendIcon.png").scaled(
        36, 36, Qt::KeepAspectRatio, Qt::SmoothTransformation
        ));
    ui.blockIconLabel->setStyleSheet("background: transparent;");

    ui.blockValueLabel = new QLabel(ui.blockWidget);
    ui.blockValueLabel->setGeometry(0, 0, 36, 36);
    ui.blockValueLabel->setAlignment(Qt::AlignCenter);
    ui.blockValueLabel->setStyleSheet(
        "color: black; font-size: 13px; font-weight: bold; background: transparent;"
        );

    // ۶. ظرف نگهدارنده نوار سلامت، دفاع و آسیب
    QWidget* hpDamageContainer = new QWidget(ui.widget);
    hpDamageContainer->setStyleSheet("background: transparent;");
    QHBoxLayout* hpDamageLayout = new QHBoxLayout(hpDamageContainer);
    hpDamageLayout->setContentsMargins(0, 0, 0, 0);
    hpDamageLayout->setSpacing(2);

    hpDamageLayout->addWidget(ui.blockWidget);
    hpDamageLayout->addWidget(ui.hpBar);
    hpDamageLayout->addWidget(damagePreviewWidget);

    // ۷. افکت‌های وضعیت روی دشمن (Status Effects)
    ui.effectsWidget = new QWidget(ui.widget);
    ui.effectsWidget->setFixedHeight(28);
    ui.effectsLayout = new QHBoxLayout(ui.effectsWidget);
    ui.effectsLayout->setContentsMargins(0, 2, 0, 0);
    ui.effectsLayout->setSpacing(4);
    ui.effectsLayout->setAlignment(Qt::AlignCenter);

    // سرهم کردن اجزا در لایه عمودی اصلی دشمن
    enemyInnerLayout->addWidget(ui.intentLabel, 0, Qt::AlignHCenter);
    enemyInnerLayout->addStretch();
    enemyInnerLayout->addWidget(enemyImg, 0, Qt::AlignHCenter | Qt::AlignBottom);
    enemyInnerLayout->addWidget(hpDamageContainer, 0, Qt::AlignHCenter);
    enemyInnerLayout->addWidget(ui.effectsWidget, 0, Qt::AlignHCenter);

    return ui;
}






/*

void BattlePage::setupTestDeck()
{
    if (!player)
        return;

    MasterDeck* deck = player->getMasterDeck();
    if (!deck)
        return;

    QVector<Card*> oldCards = deck->getCards();

    for (Card* card : oldCards)
        deck->removeCard(card);

    for (int i = 0; i < 5; ++i)
    {
        deck->addCard(new Strike());
    }
    for (int i = 0; i < 4; ++i)
    {
       deck->addCard(new Defend());
    }
    deck->addCard(new Reaper());
    deck->addCard(new Reaper());

    deck->addCard(new Impervious());
    deck->addCard(new Exhume());
    deck->addCard(new Exhume());
}
*/
