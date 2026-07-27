#include "PileViewerDialog.h"
#include "player.h"
#include "combatdeck.h"
#include "masterdeck.h"
#include "deckviewer.h"

#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QPixmap>
#include <QIcon>
#include <QSize>
#include <QGraphicsDropShadowEffect>
#include <QPropertyAnimation>
#include <QEasingCurve>
#include <QVariant>
#include <QWidget>
#include <QMouseEvent>
#include <QAbstractAnimation>
#include <QRect>
#include <QColor>

PileViewerDialog::PileViewerDialog(Player *player,
                                   PileType pileType,
                                   PileViewerMode mode,
                                   QWidget *parent)
    : QDialog(parent),
    player(player),
    pileType(pileType),
    mode(mode),
    titleLabel(nullptr),
    scrollArea(nullptr),
    scrollContainer(nullptr),
    gridLayout(nullptr),
    leaveBtn(nullptr),
    confirmBtn(nullptr),
    selectedCard(nullptr),
    selectedCardButton(nullptr)
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
        "}");

    QVBoxLayout *containerLayout = new QVBoxLayout(container);
    containerLayout->setContentsMargins(20, 20, 20, 20);
    containerLayout->setSpacing(15);

    titleLabel = new QLabel(getTitleText(), this);
    titleLabel->setAlignment(Qt::AlignCenter);
    titleLabel->setStyleSheet(
        "color: #e63946;"
        "font-size: 24px;"
        "font-weight: bold;"
        "background: transparent;");

    containerLayout->addWidget(titleLabel);

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
        "}");

    scrollContainer = new QWidget(scrollArea);
    scrollContainer->setStyleSheet("background: transparent;");

    gridLayout = new QGridLayout(scrollContainer);
    gridLayout->setHorizontalSpacing(10);
    gridLayout->setVerticalSpacing(14);
    gridLayout->setContentsMargins(8, 8, 8, 8);

    scrollContainer->setLayout(gridLayout);
    scrollArea->setWidget(scrollContainer);

    containerLayout->addWidget(scrollArea);

    QHBoxLayout *buttonsLayout = new QHBoxLayout();
    buttonsLayout->setAlignment(Qt::AlignCenter);
    buttonsLayout->setSpacing(20);

    leaveBtn = new QPushButton(
        mode == PileViewerMode::SelectCard ? "Cancel" : "Leave",
        this);
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
        "}");

    buttonsLayout->addWidget(leaveBtn);

    if (mode == PileViewerMode::SelectCard)
    {
        confirmBtn = new QPushButton("Select", this);
        confirmBtn->setFixedSize(160, 45);
        confirmBtn->setEnabled(false);
        confirmBtn->setStyleSheet(confirmBtnDisabledStyle);
        buttonsLayout->addWidget(confirmBtn);

        connect(confirmBtn, &QPushButton::clicked,
                this, &PileViewerDialog::onConfirmSelectionClicked);
    }

    containerLayout->addLayout(buttonsLayout);
    mainLayout->addWidget(container);

    connect(leaveBtn, &QPushButton::clicked,
            this, &PileViewerDialog::onLeaveOrCancelClicked);
}

QString PileViewerDialog::getTitleText() const
{
    if (mode == PileViewerMode::SelectCard && pileType == PileType::Exhaust)
    {
        return "Choose a card from Exhaust Pile";
    }

    switch (pileType)
    {
    case PileType::Draw:
        return "Draw Pile";
    case PileType::Discard:
        return "Discard Pile";
    case PileType::Exhaust:
        return "Exhaust Pile";
    case PileType::Deck:
        return "Master Deck";
    }

    return "Pile";
}

const QVector<Card *> &PileViewerDialog::getCardsForPile() const
{
    if (pileType == PileType::Draw)
    {
        return player->getCombatDeck()->getDrawPile();
    }
    else if (pileType == PileType::Discard)
    {
        return player->getCombatDeck()->getDiscardPile();
    }
    else if (pileType == PileType::Exhaust)
    {
        return player->getCombatDeck()->getExhaustPile();
    }

    return player->getMasterDeck()->getCards();
}

void PileViewerDialog::populatePile()
{
    if (!player)
    {
        return;
    }

    const QVector<Card *> &cards = getCardsForPile();

    int row = 0;
    int col = 0;
    const int maxColumns = 4;

    for (Card *card : cards)
    {
        if (!card)
        {
            continue;
        }

        QWidget *cardWidget = new QWidget(scrollContainer);
        cardWidget->setFixedSize(190, 255);
        cardWidget->setStyleSheet("background: transparent;");

        QPushButton *cardBtn = new QPushButton(cardWidget);
        cardBtn->setGeometry(25, 30, 140, 190);
        cardBtn->setStyleSheet(cardStyle);
        cardBtn->setCursor(Qt::PointingHandCursor);

        QPixmap pixmap(DeckViewerDialog::cardImagePath(card));
        cardBtn->setIcon(QIcon(pixmap));
        cardBtn->setIconSize(QSize(140, 190));

        cardBtn->setProperty("cardPtr", QVariant::fromValue(card));

        cardBtn->setProperty("defaultGeometry", QRect(25, 30, 140, 190));
        cardBtn->setProperty("hoverGeometry", QRect(10, 10, 170, 230));
        cardBtn->setProperty("selectedGeometry", QRect(10, 10, 170, 230));

        cardBtn->setProperty("defaultIconSize", QSize(140, 190));
        cardBtn->setProperty("hoverIconSize", QSize(170, 230));
        cardBtn->setProperty("selectedIconSize", QSize(170, 230));

        cardBtn->setProperty("isSelected", false);

        cardBtn->installEventFilter(this);

        gridLayout->addWidget(cardWidget, row, col, Qt::AlignCenter);

        col++;
        if (col >= maxColumns)
        {
            col = 0;
            row++;
        }
    }
}

bool PileViewerDialog::eventFilter(QObject *obj, QEvent *event)
{
    QPushButton *cardBtn = qobject_cast<QPushButton *>(obj);
    if (!cardBtn)
    {
        return QDialog::eventFilter(obj, event);
    }

    Card *card = cardBtn->property("cardPtr").value<Card *>();
    const bool isSelected = cardBtn->property("isSelected").toBool();

    if (event->type() == QEvent::Enter)
    {
        if (isSelected)
        {
            applySelectedStyle(cardBtn);
            animateCardToSelected(cardBtn);
        }
        else
        {
            applyHoverStyle(cardBtn);
            animateCardToHover(cardBtn);
        }

        return true;
    }
    else if (event->type() == QEvent::Leave)
    {
        if (isSelected)
        {
            applySelectedStyle(cardBtn);
            animateCardToSelected(cardBtn);
        }
        else
        {
            removeHoverStyle(cardBtn);
            animateCardToNormal(cardBtn);
        }

        return true;
    }
    else if (event->type() == QEvent::MouseButtonPress)
    {
        if (mode == PileViewerMode::SelectCard)
        {
            toggleCardSelection(cardBtn, card);
            return true;
        }
    }

    return QDialog::eventFilter(obj, event);
}

void PileViewerDialog::applyHoverStyle(QPushButton *cardBtn)
{
    if (!cardBtn)
    {
        return;
    }

    cardBtn->setStyleSheet(cardStyle);

    QGraphicsDropShadowEffect *glow = new QGraphicsDropShadowEffect(cardBtn);
    glow->setColor(QColor(230, 57, 70, 255));
    glow->setBlurRadius(75);
    glow->setOffset(0, 0);
    cardBtn->setGraphicsEffect(glow);

    cardBtn->raise();
}

void PileViewerDialog::removeHoverStyle(QPushButton *cardBtn)
{
    if (!cardBtn)
    {
        return;
    }

    if (!cardBtn->property("isSelected").toBool())
    {
        cardBtn->setStyleSheet(cardStyle);
        cardBtn->setGraphicsEffect(nullptr);
    }
}

void PileViewerDialog::applySelectedStyle(QPushButton *cardBtn)
{
    if (!cardBtn)
    {
        return;
    }

    cardBtn->setStyleSheet(cardStyle);

    QGraphicsDropShadowEffect *glow = new QGraphicsDropShadowEffect(cardBtn);
    glow->setColor(QColor(230, 57, 70, 255));
    glow->setBlurRadius(75);
    glow->setOffset(0, 0);
    cardBtn->setGraphicsEffect(glow);

    cardBtn->raise();
}

void PileViewerDialog::removeSelectedStyle(QPushButton *cardBtn)
{
    if (!cardBtn)
    {
        return;
    }

    cardBtn->setStyleSheet(cardStyle);
    cardBtn->setGraphicsEffect(nullptr);
}

void PileViewerDialog::animateCardToHover(QPushButton *cardBtn)
{
    if (!cardBtn)
    {
        return;
    }

    QRect hoverRect = cardBtn->property("hoverGeometry").toRect();
    QSize hoverIconSize = cardBtn->property("hoverIconSize").toSize();

    QPropertyAnimation *geometryAnim = new QPropertyAnimation(cardBtn, "geometry");
    geometryAnim->setDuration(220);
    geometryAnim->setStartValue(cardBtn->geometry());
    geometryAnim->setEndValue(hoverRect);
    geometryAnim->setEasingCurve(QEasingCurve::OutBack);
    geometryAnim->start(QAbstractAnimation::DeleteWhenStopped);

    QPropertyAnimation *iconAnim = new QPropertyAnimation(cardBtn, "iconSize");
    iconAnim->setDuration(220);
    iconAnim->setStartValue(cardBtn->iconSize());
    iconAnim->setEndValue(hoverIconSize);
    iconAnim->setEasingCurve(QEasingCurve::OutBack);
    iconAnim->start(QAbstractAnimation::DeleteWhenStopped);
}

void PileViewerDialog::animateCardToNormal(QPushButton *cardBtn)
{
    if (!cardBtn)
    {
        return;
    }

    QRect defaultRect = cardBtn->property("defaultGeometry").toRect();
    QSize defaultIconSize = cardBtn->property("defaultIconSize").toSize();

    QPropertyAnimation *geometryAnim = new QPropertyAnimation(cardBtn, "geometry");
    geometryAnim->setDuration(200);
    geometryAnim->setStartValue(cardBtn->geometry());
    geometryAnim->setEndValue(defaultRect);
    geometryAnim->setEasingCurve(QEasingCurve::OutCubic);
    geometryAnim->start(QAbstractAnimation::DeleteWhenStopped);

    QPropertyAnimation *iconAnim = new QPropertyAnimation(cardBtn, "iconSize");
    iconAnim->setDuration(200);
    iconAnim->setStartValue(cardBtn->iconSize());
    iconAnim->setEndValue(defaultIconSize);
    iconAnim->setEasingCurve(QEasingCurve::OutCubic);
    iconAnim->start(QAbstractAnimation::DeleteWhenStopped);
}

void PileViewerDialog::animateCardToSelected(QPushButton *cardBtn)
{
    if (!cardBtn)
    {
        return;
    }

    QRect selectedRect = cardBtn->property("selectedGeometry").toRect();
    QSize selectedIconSize = cardBtn->property("selectedIconSize").toSize();

    QPropertyAnimation *geometryAnim = new QPropertyAnimation(cardBtn, "geometry");
    geometryAnim->setDuration(220);
    geometryAnim->setStartValue(cardBtn->geometry());
    geometryAnim->setEndValue(selectedRect);
    geometryAnim->setEasingCurve(QEasingCurve::OutBack);
    geometryAnim->start(QAbstractAnimation::DeleteWhenStopped);

    QPropertyAnimation *iconAnim = new QPropertyAnimation(cardBtn, "iconSize");
    iconAnim->setDuration(220);
    iconAnim->setStartValue(cardBtn->iconSize());
    iconAnim->setEndValue(selectedIconSize);
    iconAnim->setEasingCurve(QEasingCurve::OutBack);
    iconAnim->start(QAbstractAnimation::DeleteWhenStopped);
}

void PileViewerDialog::toggleCardSelection(QPushButton *cardBtn, Card *card)
{
    if (!cardBtn || !card)
    {
        return;
    }

    if (selectedCardButton == cardBtn && selectedCard == card)
    {
        cardBtn->setProperty("isSelected", false);
        removeSelectedStyle(cardBtn);
        animateCardToNormal(cardBtn);

        selectedCardButton = nullptr;
        selectedCard = nullptr;

        if (confirmBtn)
        {
            confirmBtn->setEnabled(false);
            confirmBtn->setStyleSheet(confirmBtnDisabledStyle);
        }

        return;
    }

    clearSelection();

    selectedCardButton = cardBtn;
    selectedCard = card;

    cardBtn->setProperty("isSelected", true);
    applySelectedStyle(cardBtn);
    animateCardToSelected(cardBtn);

    if (confirmBtn)
    {
        confirmBtn->setEnabled(true);
        confirmBtn->setStyleSheet(confirmBtnEnabledStyle);
    }
}

void PileViewerDialog::clearSelection()
{
    if (!selectedCardButton)
    {
        selectedCard = nullptr;

        if (confirmBtn)
        {
            confirmBtn->setEnabled(false);
            confirmBtn->setStyleSheet(confirmBtnDisabledStyle);
        }

        return;
    }

    selectedCardButton->setProperty("isSelected", false);
    removeSelectedStyle(selectedCardButton);
    animateCardToNormal(selectedCardButton);

    selectedCardButton = nullptr;
    selectedCard = nullptr;

    if (confirmBtn)
    {
        confirmBtn->setEnabled(false);
        confirmBtn->setStyleSheet(confirmBtnDisabledStyle);
    }
}

void PileViewerDialog::onLeaveOrCancelClicked()
{
    clearSelection();
    reject();
}

void PileViewerDialog::onConfirmSelectionClicked()
{
    if (mode != PileViewerMode::SelectCard)
    {
        return;
    }

    if (!selectedCard)
    {
        return;
    }

    emit cardSelected(selectedCard);
    accept();
}
