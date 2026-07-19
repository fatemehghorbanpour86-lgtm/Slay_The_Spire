#include "campfirepage.h"
#include "upgradecards.h"
#include "player.h"
#include "map.h"
#include "masterdeck.h"
#include "potion.h"
#include "relic.h"
#include "audiomanager.h"
#include <QHBoxLayout>
#include <QVBoxLayout>
#include "relicviewer.h"
#include "deckviewer.h"

static const QString BONE_COLOR = "#E8DCC0";

TopBarWidget::TopBarWidget(QWidget *parent)
    : QWidget(parent)
{
    setupUI();
}

void TopBarWidget::setupUI()
{
    setFixedHeight(60);
    setObjectName("TopBar");
    setStyleSheet(
        "#TopBar {"
        "background-color: #2C444F ;"
        "}"
        );
    setAttribute(Qt::WA_StyledBackground, true);

    QHBoxLayout* topBarLayout = new QHBoxLayout(this);
    topBarLayout->setContentsMargins(20, 5, 20, 5);

    // --- Left: player info ---
    hpBar = new QProgressBar();
    hpBar->setFixedSize(150, 20);
    hpBar->setTextVisible(true);
    hpBar->setAlignment(Qt::AlignCenter);
    hpBar->setStyleSheet(
        "QProgressBar { border: 1px solid #555; border-radius: 3px; "
        "background-color: #333; color: " + BONE_COLOR + "; font-weight: bold; }"
                       "QProgressBar::chunk { background-color: #e74c3c; width: 1px; }");

    goldLabel = new QLabel("💰 0");
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
    connect(relicBtn, &QPushButton::clicked, this, &TopBarWidget::relicClicked);

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

    // --- Center: Act X | floor icon | floor number ---
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

    // --- Right: Deck and Settings ---
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
    connect(deckBtn, &QPushButton::clicked, this, &TopBarWidget::deckClicked);

    cardLabel = new QLabel("0", this);
    cardLabel->setStyleSheet("font-size: 18px; font-weight: bold; color: " + BONE_COLOR + ";");
    cardLabel->move(1187, 26);
    cardLabel->raise();

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
    connect(settingsBtn, &QPushButton::clicked, this, &TopBarWidget::settingsClicked);

    topBarLayout->addWidget(deckBtn);
    topBarLayout->addSpacing(10);
    topBarLayout->addWidget(settingsBtn);
}

void TopBarWidget::updateData(Player* player, Map* map)
{
    if (!player)
        return;

    hpBar->setMaximum(player->getMaxHealth());
    hpBar->setValue(player->getCurrentHealth());
    hpBar->setFormat(QString("%1 / %2").arg(player->getCurrentHealth()).arg(player->getMaxHealth()));

    double hpPercent = static_cast<double>(player->getCurrentHealth()) /
                       player->getMaxHealth();

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

    hpBar->setStyleSheet(QString(
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

    goldLabel->setText(QString("💰 %1").arg(player->getGold()));

    relicLabel->setText(QString("x%1").arg(player->getAllRelics().size()));

    cardLabel->setText(QString("%1").arg(player->getMasterDeck()->getCardCount()));

    if (map)
    {
        actLabel->setText(QString("Act %1").arg(map->getCurrentAct()));
        floorNumberLabel->setText(QString::number(map->getCurrentFloorIndex()));
        actLabel->setVisible(true);
        floorIconLabel->setVisible(true);
        floorNumberLabel->setVisible(true);
    }
    else
    {
        actLabel->setVisible(false);
        floorIconLabel->setVisible(false);
        floorNumberLabel->setVisible(false);
    }

    for (int i = 0; i < 3; ++i) {
        if (i < player->getPotionCount() && player->getPotion(i) != nullptr) {
            QString potionName = player->getPotion(i)->getName().toLower().replace(" ", "_");
            QString iconPath = QString(":/Potion/%1.png").arg(potionName);
            potionSlots[i]->setStyleSheet(QString("border-image: url(%1); background: transparent;").arg(iconPath));
        } else {
            potionSlots[i]->setStyleSheet("border-image: url(:/Potion/potionEmpty.png); background: transparent;");
        }
    }
}


// ======================================================
//  CampfirePage
// ======================================================

CampfirePage::CampfirePage(Player* playerPtr, Map* mapPtr, QWidget* parent)
    : QWidget(parent), player(playerPtr), map(mapPtr)
{
    AudioManager::instance().play(AudioManager::Sound::Campfire);
    setupUI();
    topBar->updateData(player, map);
    setFixedSize(1280,720);
    refreshButtonStates();
}

void CampfirePage::setupUI()
{
    setObjectName("CampfirePage");
    setStyleSheet(
        "#CampfirePage {"
        "background-image: url(:/RestSite/RestSite.png);"
        "background-repeat: no-repeat;"
        "background-position: center;"
        "background-color: #1a1410;"
        "}"
        "QToolTip { color: #facc15; background-color: #1f2937; border: 1px solid #b91c1c;"
        "border-radius: 4px; padding: 6px; font-weight: bold; font-family: Tahoma;"
        "}"
        );

    QVBoxLayout* mainLayout = new QVBoxLayout(this);
    mainLayout->setContentsMargins(0, 0, 0, 0);
    mainLayout->setSpacing(0);

    topBar = new TopBarWidget(this);
    connect(topBar, &TopBarWidget::relicClicked, this, &CampfirePage::onRelicButtonClicked);
    connect(topBar, &TopBarWidget::deckClicked, this, &CampfirePage::onDeckButtonClicked);
    connect(topBar, &TopBarWidget::settingsClicked, this, &CampfirePage::onSettingsButtonClicked);
    mainLayout->addWidget(topBar);

    mainLayout->addStretch();


    QHBoxLayout* buttonsLayout = new QHBoxLayout();
    buttonsLayout->setSpacing(70);
    buttonsLayout->setAlignment(Qt::AlignCenter);

    auto makeButton = [this](const QString& objectName) {
        QPushButton* btn = new QPushButton(this);
        //btn->setObjectName(objectName);
        btn->setFixedSize(180, 130);
        btn->setCursor(Qt::PointingHandCursor);
        btn->setStyleSheet(
            QString("QPushButton { border-image: url(:/RestSite/%1.png); border: none; background: transparent; }"
            "QPushButton:disabled { border-image: url(:/RestSite/%1Dis.png); border: none; background: transparent; }"
            "QPushButton:pressed { margin: 5px 5px 5px 5px; }" ).arg(objectName)
            );
        connect(btn, &QPushButton::pressed, this, []() {
            AudioManager::instance().play(AudioManager::Sound::ButtonClick);
        });
        return btn;
    };

    restBtn  = makeButton("RestBtn");
    smithBtn = makeButton("SmithBtn");
    liftBtn  = makeButton("GiryaBtn");


    leaveBtn = new QPushButton(this);
    leaveBtn->setFixedSize(140, 80);
    leaveBtn->move(0, 450);
    leaveBtn->setCursor(Qt::PointingHandCursor);
    leaveBtn->setStyleSheet(
        "QPushButton { border-image: url(:/RestSite/BackBtn.png); border: none; background: transparent; }"
        "QPushButton:pressed { margin: 5px 5px 5px 5px; }"
        );
    connect(leaveBtn, &QPushButton::pressed, this, []() {
        AudioManager::instance().play(AudioManager::Sound::ButtonClick);
    });
    leaveBtn->raise();



    connect(restBtn,  &QPushButton::clicked, this, &CampfirePage::onRestClicked);
    connect(smithBtn, &QPushButton::clicked, this, &CampfirePage::onSmithClicked);
    connect(liftBtn,  &QPushButton::clicked, this, &CampfirePage::onLiftClicked);
    connect(leaveBtn, &QPushButton::clicked, this, &CampfirePage::onLeaveClicked);

    auto createButtonContainer = [this](QPushButton* btn, const QString& labelText) {
        QVBoxLayout* container = new QVBoxLayout();
        container->setAlignment(Qt::AlignCenter);
        container->setSpacing(10);
        container->addWidget(btn);
        QLabel* label = new QLabel(labelText, this);
        label->setAlignment(Qt::AlignCenter);
        label->setStyleSheet("color: #facc15; font-family: Tahoma; font-weight: bold; font-size: 20px;");
        container->addWidget(label);

        return container;
    };

    buttonsLayout->addLayout(createButtonContainer(restBtn, "Rest"));
    buttonsLayout->addLayout(createButtonContainer(smithBtn, "Smith"));
    buttonsLayout->addLayout(createButtonContainer(liftBtn, "Girya"));

    mainLayout->addLayout(buttonsLayout);
    mainLayout->addSpacing(450);
}

void CampfirePage::refreshButtonStates()
{
    // ----- Rest -----
    bool canRest = player->canRest();
    restBtn->setEnabled(canRest);

    if (canRest)
    {
        restBtn->setToolTip("Heal 20% of your Max HP.");
    }
    else if (player->hasRelic(RelicId::CoffeeDripper))
    {
        restBtn->setToolTip("You cannot Rest because of Coffee Dripper.");
    }
    else
    {
        restBtn->setToolTip("You cannot Rest here.");
    }

    // ----- Lift -----
    bool hasGirya = player->hasRelic(RelicId::Girya);
    bool canLift = player->canLift();

    liftBtn->setEnabled(canLift);

    if (!hasGirya)
    {
        liftBtn->setToolTip("You need Girya to Lift.");
    }
    else if (canLift)
    {
        liftBtn->setToolTip("Gain +1 permanent Strength.");
    }
    else
    {
        liftBtn->setToolTip("Maximum lifts reached.");
    }

    // ----- Smith / Leave -----
    smithBtn->setToolTip("Upgrade one card in your deck.");
    leaveBtn->setToolTip("Leave without doing anything.");
}

void CampfirePage::onRestClicked()
{
    if (campfireLogic.rest(player))
    {
        topBar->updateData(player, map);
        emit leaveCampfire();
    }
}

void CampfirePage::onSmithClicked()
{
    UpgradeCardsDialog dialog(player, &campfireLogic, this);

    if (dialog.exec() == QDialog::Accepted)
    {
        emit leaveCampfire();
    }
}

void CampfirePage::onLiftClicked()
{
    if (campfireLogic.lift(player))
    {
        topBar->updateData(player, map);
        emit leaveCampfire();
    }
}

void CampfirePage::onLeaveClicked()
{
    emit leaveCampfire();
}

void CampfirePage::onRelicButtonClicked()
{
    RelicViewerDialog dialog(player, this);
    dialog.exec();
}

void CampfirePage::onDeckButtonClicked()
{
    DeckViewerDialog dialog(player, this);
    dialog.exec();
}

void CampfirePage::onSettingsButtonClicked()
{
    emit settingsRequested();
}