#include "cardselection.h"
#include "card.h"
#include "deckviewer.h"
#include "audiomanager.h"

#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QPushButton>

CardSelectionDialog::CardSelectionDialog(const QVector<Card*>& cardChoices, QWidget* parent)
    : QDialog(parent), cardChoices(cardChoices), chosenCard(nullptr)
{
    setupUI();
}

Card* CardSelectionDialog::getChosenCard() const
{
    return chosenCard;
}

void CardSelectionDialog::setupUI()
{
    setWindowTitle("Choose a Card");
    setStyleSheet("QDialog { border-image: url(:/Reward/SelectionViewer.png); border: none; background: transparent; }");
    setFixedSize(650, 350);

    QVBoxLayout* mainLayout = new QVBoxLayout(this);
    mainLayout->setContentsMargins(30, 30, 30, 30);

    QHBoxLayout* cardsLayout = new QHBoxLayout();
    cardsLayout->setSpacing(20);
    cardsLayout->setAlignment(Qt::AlignCenter);

    const int CARD_WIDTH = 165;
    const int CARD_HEIGHT = 220;

    for (Card* card : std::as_const(cardChoices))
    {
        if (!card)
            continue;

        QPushButton* cardBtn = new QPushButton(this);
        cardBtn->setFixedSize(CARD_WIDTH, CARD_HEIGHT);
        cardBtn->setFlat(true);
        cardBtn->setCursor(Qt::PointingHandCursor);
        cardBtn->setStyleSheet(
            QString("QPushButton { border: none; background: transparent; "
                    "border-image: url(%1); }"
                    "QPushButton:pressed { margin: 5px 5px 5px 5px; }" )
                .arg(DeckViewerDialog::cardImagePath(card))
            );

        connect(cardBtn, &QPushButton::pressed, this, []() {
            AudioManager::instance().play(AudioManager::Sound::ButtonClick);
        });

        connect(cardBtn, &QPushButton::clicked, this, [this, card]() {
            chosenCard = card;
            accept();
        });

        cardsLayout->addWidget(cardBtn);
    }

    mainLayout->addLayout(cardsLayout);
}
