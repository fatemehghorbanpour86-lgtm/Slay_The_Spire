#include "PileViewerDialog.h"
#include "player.h"
#include "combatdeck.h"
#include "masterdeck.h"
#include "deckviewer.h"

#include <QVBoxLayout>
#include <QHBoxLayout>

PileViewerDialog::PileViewerDialog(Player *player, PileType pileType, QWidget *parent)
    : QDialog(parent),
    player(player),
    pileType(pileType),
    titleLabel(nullptr),
    scrollArea(nullptr),
    scrollContainer(nullptr),
    gridLayout(nullptr),
    leaveBtn(nullptr)
{
    setWindowFlags(Qt::FramelessWindowHint | Qt::Dialog);
    setAttribute(Qt::WA_TranslucentBackground);
    setModal(true);
    setFixedSize(1000, 600);

    setupUI();
    populatePile();
}

PileViewerDialog::~PileViewerDialog()
{
}

void PileViewerDialog::setupUI()
{
    QVBoxLayout *mainLayout = new QVBoxLayout(this);
    mainLayout->setContentsMargins(30, 30, 30, 30);
    mainLayout->setSpacing(20);

    QWidget *container = new QWidget(this);
    container->setObjectName("DialogContainer");
    container->setStyleSheet(
        "#DialogContainer {"
        "   background-color: qlineargradient(spread:pad, x1:0, y1:0, x2:0, y2:1,"
        "                                     stop:0 rgba(30, 30, 30, 240),"
        "                                     stop:1 rgba(15, 15, 15, 250));"
        "   border: 2px solid #5c5c5c;"
        "   border-radius: 16px;"
        "}"
        );

    QVBoxLayout *containerLayout = new QVBoxLayout(container);
    containerLayout->setContentsMargins(20, 20, 20, 20);
    containerLayout->setSpacing(15);

    // Title label
    titleLabel = new QLabel(getTitleText(), this);
    titleLabel->setAlignment(Qt::AlignCenter);
    titleLabel->setStyleSheet(
        "color: #e63946;"
        "font-size: 24px;"
        "font-weight: bold;"
        "background: transparent;"
        );

    containerLayout->addWidget(titleLabel);

    // Scroll Area Setup
    scrollArea = new QScrollArea(this);
    scrollArea->setWidgetResizable(true);
    scrollArea->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    scrollArea->setVerticalScrollBarPolicy(Qt::ScrollBarAsNeeded);
    scrollArea->setStyleSheet(
        "QScrollArea {"
        "   background: transparent;"
        "   border: 1px solid rgba(255, 255, 255, 30);"
        "   border-radius: 8px;"
        "}"
        "QScrollBar:vertical {"
        "   border: none;"
        "   background: rgba(0, 0, 0, 50);"
        "   width: 10px;"
        "}"
        "QScrollBar::handle:vertical {"
        "   background: #555;"
        "   min-height: 20px;"
        "   border-radius: 5px;"
        "}"
        "QScrollBar::handle:vertical:hover {"
        "   background: #e63946;"
        "}"
        );

    scrollContainer = new QWidget(scrollArea);
    scrollContainer->setStyleSheet("background: transparent;");

    gridLayout = new QGridLayout(scrollContainer);
    gridLayout->setSpacing(20);
    gridLayout->setContentsMargins(15, 15, 15, 15);

    scrollContainer->setLayout(gridLayout);
    scrollArea->setWidget(scrollContainer);

    containerLayout->addWidget(scrollArea);

    // Leave Button Layout
    QHBoxLayout *buttonsLayout = new QHBoxLayout();
    buttonsLayout->setAlignment(Qt::AlignCenter);

    leaveBtn = new QPushButton("Leave", this);
    leaveBtn->setFixedSize(160, 45);
    leaveBtn->setStyleSheet(
        "QPushButton {"
        "   background-color: #8b0000;"
        "   color: white;"
        "   font-size: 16px;"
        "   font-weight: bold;"
        "   border: 1px solid #b30000;"
        "   border-radius: 6px;"
        "}"
        "QPushButton:hover {"
        "   background-color: #b30000;"
        "}"
        );

    buttonsLayout->addWidget(leaveBtn);

    containerLayout->addLayout(buttonsLayout);
    mainLayout->addWidget(container);

    connect(leaveBtn, &QPushButton::clicked, this, &PileViewerDialog::reject);
}

QString PileViewerDialog::getTitleText() const
{
    switch (pileType)
    {
    case PileType::Draw:
    {
        return "Draw Pile";
    }
    case PileType::Discard:
    {
        return "Discard Pile";
    }
    case PileType::Deck:
    {
        return "Master Deck";
    }
    }

    return "Pile";
}

const QVector<Card*>& PileViewerDialog::getCardsForPile() const
{
    if (pileType == PileType::Draw)
    {
        return player->getCombatDeck()->getDrawPile();
    }
    else if (pileType == PileType::Discard)
    {
        return player->getCombatDeck()->getDiscardPile();
    }

    return player->getMasterDeck()->getCards();
}

void PileViewerDialog::populatePile()
{
    if (!player)
    {
        return;
    }

    const QVector<Card*> &cards = getCardsForPile();

    int row = 0;
    int col = 0;
    const int maxColumns = 5;

    for (Card *card : cards)
    {
        if (!card)
        {
            continue;
        }

        QWidget *cardWidget = new QWidget(scrollContainer);
        cardWidget->setFixedSize(140, 190);
        cardWidget->setStyleSheet("background: transparent;");

        QVBoxLayout *cardLayout = new QVBoxLayout(cardWidget);
        cardLayout->setContentsMargins(0, 0, 0, 0);

        QPushButton *cardBtn = new QPushButton(cardWidget);
        cardBtn->setFixedSize(140, 190);
        cardBtn->setStyleSheet(cardStyle);
        cardBtn->setCursor(Qt::PointingHandCursor);

        QPixmap pixmap(DeckViewerDialog::cardImagePath(card));
        cardBtn->setIcon(QIcon(pixmap));
        cardBtn->setIconSize(QSize(130, 180));

        cardLayout->addWidget(cardBtn);

        gridLayout->addWidget(cardWidget, row, col);

        col++;
        if (col >= maxColumns)
        {
            col = 0;
            row++;
        }
    }
}
