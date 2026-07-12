#include "upgradecards.h"
#include "player.h"
#include "masterdeck.h"
#include "card.h"
#include "campfire.h"
#include "deckviewer.h"
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QLabel>
#include <QPushButton>
#include <QPixmap>
#include <QIcon>
#include <QSize>

UpgradePreviewDialog::UpgradePreviewDialog(Card* cardPtr, Player* playerPtr, Campfire* campfirePtr, QWidget* parent)
    : QDialog(parent), card(cardPtr), player(playerPtr), campfire(campfirePtr)
{
    setupUI();
}

void UpgradePreviewDialog::setupUI()
{
    setWindowTitle("Upgrade Card");
    setStyleSheet("QDialog { background-color: #1f2937; }");

    QVBoxLayout* mainLayout = new QVBoxLayout(this);

    QHBoxLayout* previewLayout = new QHBoxLayout();
    previewLayout->setSpacing(20);

    QLabel* currentImage = new QLabel();
    currentImage->setFixedSize(150, 220);
    currentImage->setPixmap(QPixmap(DeckViewerDialog::cardImagePath(card))
                                .scaled(150, 220, Qt::KeepAspectRatio, Qt::SmoothTransformation));

    QLabel* arrowLabel = new QLabel(QString::fromUtf8("\xE2\x96\xB6  \xE2\x96\xB6  \xE2\x96\xB6"));
    arrowLabel->setStyleSheet("color: #facc15; font-size: 22px; font-weight: bold;");
    arrowLabel->setAlignment(Qt::AlignCenter);

    QLabel* upgradedImage = new QLabel();
    upgradedImage->setFixedSize(150, 220);
    upgradedImage->setPixmap(QPixmap(DeckViewerDialog::cardImagePath(card))
                                 .scaled(150, 220, Qt::KeepAspectRatio, Qt::SmoothTransformation));

    previewLayout->addWidget(currentImage);
    previewLayout->addWidget(arrowLabel);
    previewLayout->addWidget(upgradedImage);

    mainLayout->addLayout(previewLayout);
    mainLayout->addSpacing(20);

    QHBoxLayout* buttonsLayout = new QHBoxLayout();
    buttonsLayout->setAlignment(Qt::AlignCenter);
    buttonsLayout->setSpacing(200);

    QPushButton* upgradeBtn = new QPushButton("Upgrade");
    upgradeBtn->setObjectName("upgradeConfirmBtn");
    upgradeBtn->setFixedSize(120, 40);
    upgradeBtn->setStyleSheet("QPushButton { border-image: url(:/RestSite/YesBtn.png); }"
                              "QPushButton:pressed { "
                              "   margin: 5px 5px 5px 5px; "
                              "}");
    upgradeBtn->setCursor(Qt::PointingHandCursor);

    QPushButton* cancelBtn = new QPushButton("Cancel");
    cancelBtn->setObjectName("upgradeCancelBtn");
    cancelBtn->setFixedSize(120, 40);
    cancelBtn->setStyleSheet("QPushButton { border-image: url(:/RestSite/NoBtn.png); }"
                             "QPushButton:pressed { "
                             "   margin: 5px 5px 5px 5px; "
                             "}");
    cancelBtn->setCursor(Qt::PointingHandCursor);

    buttonsLayout->addWidget(upgradeBtn);
    buttonsLayout->addWidget(cancelBtn);

    mainLayout->addLayout(buttonsLayout);

    connect(upgradeBtn, &QPushButton::clicked, this, &UpgradePreviewDialog::onUpgradeClicked);
    connect(cancelBtn, &QPushButton::clicked, this, &QDialog::reject);
}

void UpgradePreviewDialog::onUpgradeClicked()
{
    if (campfire && campfire->smith(player, card))
    {
        accept();
    }
    else
    {
        reject();
    }
}


UpgradeCardsDialog::UpgradeCardsDialog(Player* playerPtr, Campfire* campfirePtr, QWidget* parent)
    : QDialog(parent), player(playerPtr), campfire(campfirePtr)
{
    setupUI();
    populateCards();
}

void UpgradeCardsDialog::setupUI()
{
    setWindowTitle("Upgrade Cards");
    resize(650, 500);
    setStyleSheet(
        "QDialog { background-color: #1f2937; }"
        "QScrollArea { border: none; background: transparent; }"
        );

    QVBoxLayout* mainLayout = new QVBoxLayout(this);

    scrollArea = new QScrollArea(this);
    scrollArea->setWidgetResizable(true);
    scrollArea->setVerticalScrollBarPolicy(Qt::ScrollBarAsNeeded);
    scrollArea->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);

    gridContainer = new QWidget();
    gridContainer->setStyleSheet("background: transparent;");

    gridLayout = new QGridLayout(gridContainer);
    gridLayout->setSpacing(12);

    scrollArea->setWidget(gridContainer);
    mainLayout->addWidget(scrollArea);
}

void UpgradeCardsDialog::populateCards()
{
    if (!player || !player->getMasterDeck())
        return;

    int row = 0;
    int col = 0;

    for (Card* card : player->getMasterDeck()->getCards())
    {
        if (!card || card->getIsUpgraded())
            continue;

        QPushButton* cardBtn = new QPushButton();
        cardBtn->setFixedSize(CARD_WIDTH, CARD_HEIGHT);
        cardBtn->setIcon(QIcon(DeckViewerDialog::cardImagePath(card)));
        cardBtn->setIconSize(QSize(CARD_WIDTH, CARD_HEIGHT));
        cardBtn->setFlat(true);
        cardBtn->setCursor(Qt::PointingHandCursor);
        cardBtn->setStyleSheet("QPushButton { border: none; background: transparent; }");

        connect(cardBtn, &QPushButton::clicked, this, [this, card]() {
            openPreview(card);
        });

        gridLayout->addWidget(cardBtn, row, col);

        col++;
        if (col >= COLUMNS)
        {
            col = 0;
            row++;
        }
    }
}

void UpgradeCardsDialog::openPreview(Card* card)
{
    UpgradePreviewDialog preview(card, player, campfire, this);

    if (preview.exec() == QDialog::Accepted)
    {
        // The upgrade actually happened: the whole Campfire visit is over.
        accept();
    }
    // On Cancel: stay open, same (still un-upgraded) card list.
}