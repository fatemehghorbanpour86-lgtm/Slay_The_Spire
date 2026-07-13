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

QString UpgradePreviewDialog::UpgradeCardImagePath(const Card* card)
{
    if (!card)
        return QString();

    QString cleanName = card->getName();
    cleanName.remove(' ');
    cleanName.remove('\'');
    cleanName.remove('.');

    if (cleanName.endsWith('+'))
        cleanName.chop(1);

    return QString(":/card/%1Plus.png").arg(cleanName);
}

void UpgradePreviewDialog::setupUI()
{
    setWindowTitle("Upgrade Card");
    // setStyleSheet("QDialog { background-color: #1f2937; }");

    setFixedSize(700, 500);
    setStyleSheet("QDialog { border-image: url(:/RestSite/UpgradeViewer.png); border: none; background: transparent; }"
                  );

    QVBoxLayout* mainLayout = new QVBoxLayout(this);
    mainLayout->setContentsMargins(0, 0, 0, 0);
    mainLayout->setSpacing(0);


    QHBoxLayout* previewLayout = new QHBoxLayout();
    previewLayout->setContentsMargins(50, 60, 50, 0);
    previewLayout->setSpacing(20);

    QLabel* currentImage = new QLabel();
    currentImage->setFixedSize(170, 220);
    currentImage->setStyleSheet(QString("border-image: url(%1); background: transparent;")
                                    .arg(DeckViewerDialog::cardImagePath(card)));

    QLabel* arrowLabel = new QLabel();
    arrowLabel->setFixedSize(180, 100);
    arrowLabel->setStyleSheet("border-image: url(:/RestSite/FlashLabel.png); background: transparent;");
    arrowLabel->setAlignment(Qt::AlignCenter);

    QLabel* upgradedImage = new QLabel();
    upgradedImage->setFixedSize(170, 220);
    upgradedImage->setStyleSheet(QString("border-image: url(%1); background: transparent;")
                                     .arg(UpgradeCardImagePath(card)));

    previewLayout->addWidget(currentImage);
    previewLayout->addWidget(arrowLabel);
    previewLayout->addWidget(upgradedImage);

    mainLayout->addLayout(previewLayout);
    //mainLayout->addSpacing(20);

    QHBoxLayout* buttonsLayout = new QHBoxLayout();
    buttonsLayout->setContentsMargins(0, 0, 0, 0);
    buttonsLayout->setSpacing(400);
    buttonsLayout->setAlignment(Qt::AlignCenter);

    QPushButton* upgradeBtn = new QPushButton();
    upgradeBtn->setObjectName("upgradeConfirmBtn");
    upgradeBtn->setFixedSize(150, 100);
    upgradeBtn->setStyleSheet("QPushButton { border-image: url(:/RestSite/YesBtn.png); }"
                              "QPushButton:pressed { "
                              "   margin: 5px 5px 5px 5px; "
                              "}");
    upgradeBtn->setCursor(Qt::PointingHandCursor);

    QPushButton* cancelBtn = new QPushButton();
    cancelBtn->setObjectName("upgradeCancelBtn");
    cancelBtn->setFixedSize(150, 100);
    cancelBtn->setStyleSheet("QPushButton { border-image: url(:/RestSite/NoBtn.png); }"
                             "QPushButton:pressed { "
                             "   margin: 5px 5px 5px 5px; "
                             "}");
    cancelBtn->setCursor(Qt::PointingHandCursor);

    buttonsLayout->addWidget(cancelBtn);
    buttonsLayout->addWidget(upgradeBtn);

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
    setFixedSize(850, 620);
    setStyleSheet("QDialog { border-image: url(:/card/CardViewer.png); }"
                  "QScrollArea { border: none; background: transparent; }"
                  );
    // resize(650, 500);
    // setStyleSheet(
    //     "QDialog { background-color: #1f2937; }"
    //     "QScrollArea { border: none; background: transparent; }"
    //     );

    QVBoxLayout* mainLayout = new QVBoxLayout(this);
    mainLayout->setContentsMargins(40, 45, 50, 50);
    scrollArea = new QScrollArea(this);
    scrollArea->setWidgetResizable(true);
    scrollArea->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    gridContainer = new QWidget();
    gridContainer->setStyleSheet("background: transparent;");

    gridLayout = new QGridLayout(gridContainer);
    gridLayout->setSpacing(20);

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
        cardBtn->setStyleSheet("QIcon:pressed { "
                               "   margin: 5px 5px 5px 5px; "
                               "}");
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