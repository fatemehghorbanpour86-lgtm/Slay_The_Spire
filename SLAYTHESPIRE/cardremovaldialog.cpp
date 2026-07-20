#include "CardRemovalDialog.h"
#include "player.h"
#include "masterdeck.h"
#include "deckviewer.h" //for DeckViewerDialog::cardImagePath
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QGraphicsDropShadowEffect>

CardRemovalDialog::CardRemovalDialog(Player *player, QWidget *parent)
    : QDialog(parent),
    player(player),
    selectedCard(nullptr),
    selectedButton(nullptr)
{
    setWindowFlags(Qt::FramelessWindowHint | Qt::Dialog);
    setAttribute(Qt::WA_TranslucentBackground);
    setModal(true);
    setFixedSize(1000, 600);

    setupUI();
    populateDeck();
}

CardRemovalDialog::~CardRemovalDialog()
{
}

void CardRemovalDialog::setupUI()
{
    QVBoxLayout *mainLayout = new QVBoxLayout(this);
    mainLayout->setContentsMargins(30, 30, 30, 30);
    mainLayout->setSpacing(20);

    QWidget *container = new QWidget(this);
    container->setObjectName("DialogContainer");
    container->setStyleSheet(
        "#DialogContainer {"
        "   background-color: qlineargradient(spread:pad, x1:0, y1:0, x2:0, y2:1, "
        "                                     stop:0 rgba(30, 30, 30, 240), "
        "                                     stop:1 rgba(15, 15, 15, 250));"
        "   border: 2px solid #5c5c5c;"
        "   border-radius: 16px;"
        "}"
        );

    QVBoxLayout *containerLayout = new QVBoxLayout(container);
    containerLayout->setContentsMargins(20, 20, 20, 20);
    containerLayout->setSpacing(15);

    // tittle
    titleLabel = new QLabel("Select a Card to Remove", this);
    titleLabel->setAlignment(Qt::AlignCenter);
    titleLabel->setStyleSheet(
        "color: #f5c518;"
        "font-size: 24px;"
        "font-weight: bold;"
        "background: transparent;"
        );
    containerLayout->addWidget(titleLabel);

    //  (Scroll Area)
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
        "   margin: 0px 0px 0px 0px;"
        "}"
        "QScrollBar::handle:vertical {"
        "   background: #555;"
        "   min-height: 20px;"
        "   border-radius: 5px;"
        "}"
        "QScrollBar::handle:vertical:hover {"
        "   background: #f5c518;"
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

    //  confirm and cancel
    QHBoxLayout *buttonsLayout = new QHBoxLayout();
    buttonsLayout->setSpacing(30);
    buttonsLayout->setAlignment(Qt::AlignCenter);

    confirmBtn = new QPushButton("Confirm", this);
    confirmBtn->setFixedSize(160, 45);
    confirmBtn->setEnabled(false); // تا زمانی که کارتی انتخاب نشود غیرفعال است
    confirmBtn->setStyleSheet(
        "QPushButton {"
        "   background-color: #2b7a1d;"
        "   color: white;"
        "   font-size: 16px;"
        "   font-weight: bold;"
        "   border: 1px solid #3eaf2a;"
        "   border-radius: 6px;"
        "}"
        "QPushButton:hover {"
        "   background-color: #3eaf2a;"
        "}"
        "QPushButton:disabled {"
        "   background-color: #333333;"
        "   color: #777777;"
        "   border: 1px solid #444;"
        "}"
        );

    cancelBtn = new QPushButton("Cancel", this);
    cancelBtn->setFixedSize(160, 45);
    cancelBtn->setStyleSheet(
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

    buttonsLayout->addWidget(cancelBtn);
    buttonsLayout->addWidget(confirmBtn);

    containerLayout->addLayout(buttonsLayout);
    mainLayout->addWidget(container);

    connect(confirmBtn, &QPushButton::clicked, this, &CardRemovalDialog::onConfirm);
    connect(cancelBtn, &QPushButton::clicked, this, &CardRemovalDialog::onCancel);
}

void CardRemovalDialog::populateDeck()
{
    if (!player || !player->getMasterDeck())
        return;

    const QVector<Card*>& cards = player->getMasterDeck()->getCards();

    int row = 0;
    int col = 0;
    const int maxColumns = 5; // تعداد ۵ کارت در هر ردیف اسکرول

    for (Card* card : cards)
    {
        QWidget *cardWidget = new QWidget(scrollContainer);
        cardWidget->setFixedSize(140, 190);

        QVBoxLayout *cardLayout = new QVBoxLayout(cardWidget);
        cardLayout->setContentsMargins(0, 0, 0, 0);

        QPushButton *cardBtn = new QPushButton(cardWidget);
        cardBtn->setFixedSize(140, 190);
        cardBtn->setStyleSheet(normalCardStyle);

        QPixmap pixmap(DeckViewerDialog::cardImagePath(card));
        cardBtn->setIcon(QIcon(pixmap));
        cardBtn->setIconSize(QSize(130, 180));
        cardBtn->setCursor(Qt::PointingHandCursor);

        cardLayout->addWidget(cardBtn);

        connect(cardBtn, &QPushButton::clicked, this, [this, card, cardBtn]() {
            onCardClicked(card, cardBtn);
        });

        gridLayout->addWidget(cardWidget, row, col);

        col++;
        if (col >= maxColumns)
        {
            col = 0;
            row++;
        }
    }
}

void CardRemovalDialog::onCardClicked(Card* card, QPushButton* button)
{
    // اگر کارت جدیدی کلیک شده، کارت قبلی را از حالت استایل انتخابی خارج می‌کنیم
    if (selectedButton)
    {
        selectedButton->setStyleSheet(normalCardStyle);
    }

    selectedCard = card;
    selectedButton = button;

    // استایل کارت انتخاب‌شده جدید را اعمال می‌کنیم
    selectedButton->setStyleSheet(selectedCardStyle);
    confirmBtn->setEnabled(true);
}

void CardRemovalDialog::onConfirm()
{
    accept(); // دیالوگ را با کد QDialog::Accepted می‌بندد
}

void CardRemovalDialog::onCancel()
{
    reject(); // دیالوگ را با کد QDialog::Rejected می‌بندد
}

Card* CardRemovalDialog::getSelectedCard() const
{
    return selectedCard;
}
