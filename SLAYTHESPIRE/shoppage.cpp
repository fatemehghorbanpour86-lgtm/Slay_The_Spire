#include "ShopPage.h"

#include "cardremovaldialog.h"
#include "potion.h"
#include "deckviewer.h"
#include "PileViewerDialog.h"
#include "relicviewer.h"
#include "audiomanager.h"

#include <QDebug>
#include <QLayoutItem>
#include <QSpacerItem>
#include <QCoreApplication>
#include <QDir>

ShopPage::ShopPage(Player *player, QWidget *parent)
    : QWidget(parent),
    player(player),
    shopLogic(new Shop())
{
    shopLogic->generateStock();

    QPixmap pixmap(":/cursor.png");
    QPixmap scaledPixmap = pixmap.scaled(30, 40, Qt::KeepAspectRatio, Qt::SmoothTransformation);
    QCursor customCursor(scaledPixmap, 0, 0);
    this->setCursor(customCursor);

    mainLayout = new QVBoxLayout(this);
    mainLayout->setContentsMargins(0, 0, 0, 0);
    mainLayout->setSpacing(0);

    // Background
    bg = new QLabel(this);
    bg->setGeometry(0, 0, 1280, 720);
    bg->lower();

    topBar = new QWidget(this);
    shopField = new QWidget(this);
    bottomBar = new QWidget(this);

    topBar->setFixedHeight(50);
    shopField->setFixedHeight(580);
    bottomBar->setFixedHeight(90);

    mainLayout->addWidget(topBar);
    mainLayout->addWidget(shopField);
    mainLayout->addWidget(bottomBar);

    setupTopBar();
    setupShopField();
    updateUI();
}

ShopPage::~ShopPage()
{
    delete shopLogic;
}

void ShopPage::setupTopBar()
{
    QHBoxLayout *layout = new QHBoxLayout(topBar);
    layout->setContentsMargins(20, 6, 20, 6);
    layout->setSpacing(0);

    QString baseDir = QCoreApplication::applicationDirPath();
    QString BtnPath = QDir(baseDir).filePath("assets/image/cursorBtn.png");
    QPixmap buttonHoverPixmap(BtnPath);
    QPixmap scaledHover = buttonHoverPixmap.scaled(40, 61, Qt::KeepAspectRatio, Qt::SmoothTransformation);
    QCursor buttonHoverCursor(scaledHover, scaledHover.width() / 2, 10);

    // ===== LEFT GROUP: name/class + HP + gold + potions =====
    QHBoxLayout *leftGroup = new QHBoxLayout();
    leftGroup->setSpacing(10);

    QLabel *classLabel = new QLabel("the Ironclad", topBar);
    classLabel->setStyleSheet("color: #cfcfcf; font-size: 13px; background: transparent;");

    QLabel *heartIcon = new QLabel(topBar);
    heartIcon->setFixedSize(45, 45);
    heartIcon->setStyleSheet("background: transparent;");
    heartIcon->setPixmap(QPixmap(":/heartIcon.png").scaled(
        45, 45,
        Qt::KeepAspectRatio,
        Qt::SmoothTransformation
        ));
    heartIcon->setAlignment(Qt::AlignCenter);

    playerHpLabel = new QLabel(
        QString("%1/%2")
            .arg(player->getCurrentHealth())
            .arg(player->getMaxHealth()),
        topBar
        );
    playerHpLabel->setStyleSheet("color: #e63946; font-size: 14px; font-weight: bold; background: transparent;");

    QLabel *goldIcon = new QLabel(topBar);
    goldIcon->setFixedSize(45, 45);
    goldIcon->setStyleSheet("background: transparent;");
    goldIcon->setPixmap(QPixmap(":/moneyPouch.png").scaled(
        45, 45,
        Qt::KeepAspectRatio,
        Qt::SmoothTransformation
        ));
    goldIcon->setAlignment(Qt::AlignCenter);

    goldValueLabel = new QLabel(QString::number(player->getGold()), topBar);
    goldValueLabel->setStyleSheet("color: #f5c518; font-size: 14px; font-weight: bold; background: transparent;");

    leftGroup->addWidget(classLabel);
    leftGroup->addSpacing(15);
    leftGroup->addWidget(heartIcon);
    leftGroup->addWidget(playerHpLabel);
    leftGroup->addSpacing(10);
    leftGroup->addWidget(goldIcon);
    leftGroup->addWidget(goldValueLabel);
    leftGroup->addSpacing(15);

    // Visual Potion slots (Disabled & non-interactive in the shop)
    potionButtons.clear();
    for (int i = 0; i < 3; ++i)
    {
        QPushButton *potionBtn = new QPushButton(topBar);
        potionBtn->setFixedSize(30, 30);
        potionBtn->setEnabled(false);
        potionBtn->setFocusPolicy(Qt::NoFocus);
        potionBtn->setStyleSheet(
            "QPushButton {"
            "   background: rgba(255,255,255,20); border: 1px solid #666; border-radius: 4px;"
            "}"
            "QPushButton:disabled {"
            "   opacity: 1;"
            "}"
            );
        leftGroup->addWidget(potionBtn);
        leftGroup->addSpacing(4);
        potionButtons.append(potionBtn);
    }

    // ===== RIGHT GROUP: relic, map, deck =====
    QHBoxLayout *rightGroup = new QHBoxLayout();
    rightGroup->setSpacing(14);

    QPushButton *relicBtn = new QPushButton(topBar);
    relicBtn->setFixedSize(30, 30);
    relicBtn->setCursor(buttonHoverCursor);
    relicBtn->setStyleSheet(
        "QPushButton {"
        "   background: transparent;"
        "   border: none;"
        "   border-image: url(:/map/relicIcon.png);"
        "}"
        "QPushButton:pressed {"
        "   margin: 2px 2px 2px 2px;"
        "}"
        );

    connect(relicBtn, &QPushButton::pressed,
            this, []()
            {
                AudioManager::instance().play(AudioManager::Sound::ButtonClick);
            });
    connect(relicBtn, &QPushButton::clicked, this, [this]() {
        RelicViewerDialog dialog(player, this);
        dialog.exec();
    });

    QLabel *mapIcon = new QLabel(topBar);
    mapIcon->setFixedSize(60, 60);
    mapIcon->setStyleSheet("background: transparent; margin-top: -20px;");
    mapIcon->setPixmap(QPixmap(":/mapIcon.png").scaled(
        55, 55,
        Qt::KeepAspectRatio,
        Qt::SmoothTransformation
        ));
    mapIcon->setAlignment(Qt::AlignCenter);

    QPushButton *deckBtn = new QPushButton(topBar);
    deckBtn->setFixedSize(45, 45);
    deckBtn->setCursor(buttonHoverCursor);
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

    rightGroup->addWidget(relicBtn);
    rightGroup->addWidget(mapIcon);
    rightGroup->addWidget(deckBtn);

    // ===== Assemble =====
    layout->addLayout(leftGroup);
    layout->addStretch();
    layout->addStretch();
    layout->addLayout(rightGroup);

    topBar->setLayout(layout);

    connect(deckBtn, &QPushButton::pressed,
            this, []()
            {
                AudioManager::instance().play(AudioManager::Sound::ButtonClick);
            });
    connect(deckBtn, &QPushButton::clicked, this, [this]()
            {
                if (player)
                {
                    PileViewerDialog dialog(player, PileType::Deck, PileViewerMode::ViewOnly, this);
                    dialog.exec();
                }
            });
}

void ShopPage::setupShopField()
{
    QString baseDir = QCoreApplication::applicationDirPath();
    QString BtnPath = QDir(baseDir).filePath("assets/image/cursorBtn.png");
    QPixmap buttonHoverPixmap(BtnPath);
    QPixmap scaledHover = buttonHoverPixmap.scaled(40, 61, Qt::KeepAspectRatio, Qt::SmoothTransformation);
    QCursor buttonHoverCursor(scaledHover, scaledHover.width() / 2, 10);

    // ===== Greeting state =====
    playerContainer = new QWidget(shopField);
    playerContainer->setFixedSize(250, 350);
    playerContainer->setStyleSheet("background: transparent;");

    QLabel *pImg = new QLabel(playerContainer);
    pImg->setGeometry(10, 10, 230, 290);
    pImg->setPixmap(QPixmap(":/ironclad.png").scaled(
        230,
        290,
        Qt::KeepAspectRatio,
        Qt::SmoothTransformation
        ));
    pImg->setAlignment(Qt::AlignCenter);
    pImg->setStyleSheet("background: transparent;");

    merchantBtn = new QPushButton(shopField);
    merchantBtn->setFixedSize(320, 320);
    merchantBtn->setIcon(QIcon(":/Merchant.png"));
    merchantBtn->setIconSize(QSize(300, 300));
    merchantBtn->setStyleSheet(
        "background: transparent;"
        "border: none;"
        );
    merchantBtn->setCursor(buttonHoverCursor);
    connect(merchantBtn, &QPushButton::pressed,
            this, []()
            {
                AudioManager::instance().play(AudioManager::Sound::ButtonClick);
            });
    connect(merchantBtn, &QPushButton::clicked, this, &ShopPage::onMerchantClicked);

    // ===== Browsing state =====
    inventoryContainer = new QWidget(shopField);
    inventoryContainer->setFixedSize(1000, 540);
    inventoryContainer->setStyleSheet(
        "background: rgba(0,0,0,100);"
        "border-radius: 20px;"
        );
    inventoryContainer->hide();

    // Leave button
    leaveBtn = new QPushButton("Leave", bottomBar);
    leaveBtn->setFixedSize(150, 50);
    leaveBtn->setCursor(buttonHoverCursor);
    leaveBtn->setStyleSheet(
        "background: #8b0000;"
        "color: white;"
        "font-weight: bold;"
        "border: none;"
        "border-radius: 8px;"
        );
    connect(leaveBtn, &QPushButton::pressed,
            this, []()
            {
                AudioManager::instance().play(AudioManager::Sound::ButtonClick);
            });
    connect(leaveBtn, &QPushButton::clicked, this, &ShopPage::onLeaveClicked);
}

void ShopPage::onMerchantClicked()
{
    currentState = ShopState::Browsing;
    updateUI();
}

void ShopPage::onLeaveClicked()
{
    currentState = ShopState::Greeting;
    updateUI();

    emit shoppingDone();
}

QString ShopPage::getPotionImagePath(const QString &potionName)
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

static void clearLayout(QLayout *layout)
{
    if (layout == nullptr)
        return;

    while (QLayoutItem *item = layout->takeAt(0))
    {
        if (QWidget *widget = item->widget())
        {
            delete widget;
        }
        else if (QLayout *childLayout = item->layout())
        {
            clearLayout(childLayout);
        }

        delete item;
    }
}

void ShopPage::populateInventory()
{
    if (QLayout *oldLayout = inventoryContainer->layout())
    {
        clearLayout(oldLayout);
        delete oldLayout;
    }

    QVBoxLayout *mainInvLayout = new QVBoxLayout(inventoryContainer);
    mainInvLayout->setContentsMargins(30, 40, 30, 35);
    mainInvLayout->setSpacing(45);

    QPixmap pixmap(":/cursor.png");
    QPixmap scaledPixmap = pixmap.scaled(30, 40, Qt::KeepAspectRatio, Qt::SmoothTransformation);
    QCursor customCursor(scaledPixmap, 0, 0);

    QString baseDir = QCoreApplication::applicationDirPath();
    QString BtnPath = QDir(baseDir).filePath("assets/image/cursorBtn.png");
    QPixmap buttonHoverPixmap(BtnPath);
    QPixmap scaledHover = buttonHoverPixmap.scaled(40, 61, Qt::KeepAspectRatio, Qt::SmoothTransformation);
    QCursor buttonHoverCursor(scaledHover, scaledHover.width() / 2, 10);

    // =========================
    // Row 1: cards
    // =========================
    QHBoxLayout *cardsLayout = new QHBoxLayout();
    cardsLayout->setSpacing(22);
    cardsLayout->setContentsMargins(0, 0, 0, 0);

    const auto &cards = shopLogic->getCardOffers();

    for (int i = 0; i < cards.size(); ++i)
    {
        QWidget *cardWidget = new QWidget(inventoryContainer);
        cardWidget->setFixedSize(150, 240);
        cardWidget->setStyleSheet("background: transparent;");

        QVBoxLayout *cardLayout = new QVBoxLayout(cardWidget);
        cardLayout->setContentsMargins(0, 0, 0, 0);
        cardLayout->setSpacing(4);

        QPushButton *cardBtn = new QPushButton(cardWidget);
        cardBtn->setFixedSize(150, 200);
        cardBtn->setCursor(buttonHoverCursor);
        cardBtn->setStyleSheet(
            "background: transparent;"
            "border: 1px solid rgba(255,215,0,50);"
            "border-radius: 6px;"
            );

        QPixmap cardPixmap(DeckViewerDialog::cardImagePath(cards[i].card));
        cardBtn->setIcon(QIcon(cardPixmap));
        cardBtn->setIconSize(QSize(140, 190));

        QLabel *priceLabel = new QLabel(
            QString::number(cards[i].price) + " G",
            cardWidget
            );
        priceLabel->setAlignment(Qt::AlignCenter);
        priceLabel->setStyleSheet(
            cards[i].onSale
                ? "color: #ff4500; font-size: 14px; font-weight: bold; background: transparent;"
                : "color: #f5c518; font-size: 14px; font-weight: bold; background: transparent;"
            );

        cardLayout->addWidget(cardBtn);
        cardLayout->addWidget(priceLabel);

        cardsLayout->addWidget(cardWidget);
        connect(cardBtn, &QPushButton::pressed,
                this, []()
                {
                    AudioManager::instance().play(AudioManager::Sound::ButtonClick);
                });
        connect(cardBtn, &QPushButton::clicked, this, [this, i]()
                {
                    onBuyCard(i);
                });
    }

    cardsLayout->addStretch();
    mainInvLayout->addLayout(cardsLayout);
    mainInvLayout->addSpacing(40);

    // =========================
    // Row 2: potions + card removal
    // =========================
    QHBoxLayout *bottomRowLayout = new QHBoxLayout();
    bottomRowLayout->setSpacing(18);
    bottomRowLayout->setContentsMargins(0, 0, 0, 0);

    const auto &potions = shopLogic->getPotionOffers();

    for (int i = 0; i < potions.size(); ++i)
    {
        QWidget *potWidget = new QWidget(inventoryContainer);
        potWidget->setFixedSize(110, 140);
        potWidget->setStyleSheet("background: transparent;");

        QVBoxLayout *potLayout = new QVBoxLayout(potWidget);
        potLayout->setContentsMargins(0, 0, 0, 0);
        potLayout->setSpacing(6);

        QPushButton *potBtn = new QPushButton(potWidget);
        potBtn->setFixedSize(90, 90);
        potBtn->setCursor(buttonHoverCursor);
        potBtn->setStyleSheet(
            "background: rgba(255,255,255,10);"
            "border-radius: 45px;"
            "border: 1px solid rgba(255,255,255,30);"
            );
        potBtn->setIcon(QIcon(getPotionImagePath(potions[i].potion->getName())));
        potBtn->setIconSize(QSize(70, 70));

        QLabel *pPrice = new QLabel(
            QString::number(potions[i].price) + " G",
            potWidget
            );
        pPrice->setAlignment(Qt::AlignCenter);
        pPrice->setStyleSheet(
            "color: #f5c518;"
            "font-size: 13px;"
            "font-weight: bold;"
            "background: transparent;"
            );

        potLayout->addWidget(potBtn);
        potLayout->addWidget(pPrice);

        bottomRowLayout->addWidget(potWidget);
        connect(potBtn, &QPushButton::pressed,
                this, []()
                {
                    AudioManager::instance().play(AudioManager::Sound::ButtonClick);
                });
        connect(potBtn, &QPushButton::clicked, this, [this, i]()
                {
                    onBuyPotion(i);
                });
    }

    bottomRowLayout->addSpacing(70);

    QWidget *removalWidget = new QWidget(inventoryContainer);
    removalWidget->setFixedSize(180, 230);
    removalWidget->setStyleSheet("background: transparent;");

    QVBoxLayout *removalLayout = new QVBoxLayout(removalWidget);
    removalLayout->setContentsMargins(0, 0, 0, 0);
    removalLayout->setSpacing(5);
    removalLayout->setAlignment(Qt::AlignCenter);

    QPushButton *remBtn = new QPushButton(removalWidget);
    remBtn->setFixedSize(150, 190);

    // Check if the card removal service has already been purchased
    if (cardRemovalSold)
    {
        // Apply "Sold Out" visual state
        remBtn->setIcon(QIcon(":/sold_out.png"));
        remBtn->setIconSize(QSize(130, 170));
        remBtn->setCursor(customCursor);
        remBtn->setEnabled(false);
        remBtn->setStyleSheet("background: transparent; border: none;");
    }
    else
    {
        // Apply active state with hover effects
        remBtn->setIcon(QIcon(":/cardRemoval.png"));
        remBtn->setCursor(buttonHoverCursor);
        remBtn->setIconSize(QSize(130, 170));
        remBtn->setStyleSheet(
            "QPushButton {"
            "   background: transparent;"
            "   border: 2px dashed rgba(255,255,255,60);"
            "   border-radius: 10px;"
            "}"
            "QPushButton:hover {"
            "   border: 2px dashed rgba(255,215,0,100);"
            "   background: rgba(255,255,255,10);"
            "}"
            );
    }

    QLabel *remPrice = new QLabel(removalWidget);

    // Update price label based on availability
    if (cardRemovalSold)
    {
        remPrice->setText("SOLD");
        remPrice->setStyleSheet("color: #ff4500; font-size: 14px; font-weight: bold;");
    }
    else
    {
        remPrice->setText(QString::number(player->getCardRemovalCost()) + " G");
        remPrice->setStyleSheet("color: #f5c518; font-size: 13px; font-weight: bold;");
    }

    remPrice->setAlignment(Qt::AlignCenter);

    removalLayout->addWidget(remBtn);
    removalLayout->addWidget(remPrice);

    bottomRowLayout->addWidget(removalWidget);
    bottomRowLayout->addStretch();

    // Connect the button click signal
    connect(remBtn, &QPushButton::pressed,
            this, []()
            {
                AudioManager::instance().play(AudioManager::Sound::ButtonClick);
            });
    connect(remBtn, &QPushButton::clicked, this, [this]()
            {
                // Safety check: do nothing if already sold
                if (cardRemovalSold)
                {
                    return;
                }

                int cost = player->getCardRemovalCost();

                // Check if player has sufficient gold
                if (player->getGold() < cost)
                {
                    qDebug() << "Insufficient gold for card removal.";
                    return;
                }

                // Open the modal removal dialog
                CardRemovalDialog dialog(player, this);

                if (dialog.exec() == QDialog::Accepted)
                {
                    Card* cardToRemove = dialog.getSelectedCard();

                    if (cardToRemove)
                    {
                        // Execute the removal logic through the shop logic controller
                        if (shopLogic->removeCard(player, cardToRemove))
                        {
                            qDebug() << "Card successfully removed from Master Deck.";

                            // Set the flag to true so it shows "Sold Out" on next UI update
                            cardRemovalSold = true;

                            // Safely trigger UI refresh to avoid widget-in-use crashes
                            this->updateUI();
                        }
                    }
                }
            });

    mainInvLayout->addLayout(bottomRowLayout);
}

void ShopPage::updateTopBarStats()
{
    if (!player)
        return;

    // 1. Sync textual HP and Gold details
    goldValueLabel->setText(QString::number(player->getGold()));
    playerHpLabel->setText(
        QString("%1/%2")
            .arg(player->getCurrentHealth())
            .arg(player->getMaxHealth())
        );

    // 2. Dynamic visual refresh of the top-bar potion slots
    const auto &potions = player->getPotions();
    for (int i = 0; i < potionButtons.size(); ++i)
    {
        if (i < potions.size() && potions[i] != nullptr)
        {
            potionButtons[i]->setIcon(QIcon(getPotionImagePath(potions[i]->getName())));
            potionButtons[i]->setIconSize(QSize(24, 24));
        }
        else
        {
            potionButtons[i]->setIcon(QIcon());
        }
    }
}

void ShopPage::updateUI()
{
    if (currentState == ShopState::Greeting)
    {
        bg->setGeometry(0, 0, 1280, 720);
        bg->setPixmap(QPixmap(":/battlebackground.png").scaled(
            1280, 720,
            Qt::IgnoreAspectRatio,
            Qt::SmoothTransformation
            ));
        bg->lower();

        playerContainer->show();
        merchantBtn->show();
        inventoryContainer->hide();
        leaveBtn->hide();

        positionGreetingActors();
    }
    else
    {
        bg->setGeometry(0, 0, 1280, 720);
        bg->setPixmap(QPixmap(":/shop_background.png").scaled(
            1280, 720,
            Qt::IgnoreAspectRatio,
            Qt::SmoothTransformation
            ));
        bg->lower();

        playerContainer->hide();
        merchantBtn->hide();
        inventoryContainer->show();
        leaveBtn->show();

        inventoryContainer->setGeometry(140, 15, 1000, 540);

        populateInventory();
    }

    // Keep the top bar synced with the current player state
    updateTopBarStats();
}

void ShopPage::onBuyCard(int index)
{
    if (shopLogic->buyCard(player, index))
    {
        updateUI();
    }
    else
    {
        qDebug() << "Not enough gold to buy card.";
    }
}

void ShopPage::onBuyPotion(int index)
{
    if (shopLogic->buyPotion(player, index))
    {
        updateUI();
    }
    else
    {
        qDebug() << "Not enough gold to buy potion.";
    }
}

void ShopPage::positionGreetingActors()
{
    playerContainer->setGeometry(200, 150, 250, 350);
    merchantBtn->setGeometry(700, 180, 320, 320);
}
