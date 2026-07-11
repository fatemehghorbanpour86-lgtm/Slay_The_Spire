#include "deckviewer.h"
#include "masterdeck.h"

#include <QVBoxLayout>
#include <QGridLayout>
#include <QScrollArea>
#include <QLabel>
#include <QWidget>

DeckViewerDialog::DeckViewerDialog(Player* player, QWidget* parent)
    : QDialog(parent), player(player)
{
    setWindowTitle("Deck");
    setFixedSize(770, 620);
    setStyleSheet("QDialog { border-image: url(:/card/CardViewer.png); }");

    setupUI();
}

QString DeckViewerDialog::cardImagePath(const Card* card)
{
    if (!card)
        return QString();

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

void DeckViewerDialog::setupUI()
{
    QVBoxLayout* mainLayout = new QVBoxLayout(this);
    mainLayout->setContentsMargins(40, 45, 50, 50);

    QScrollArea* scrollArea = new QScrollArea(this);
    scrollArea->setWidgetResizable(true);
    scrollArea->setFrameShape(QFrame::NoFrame);
    scrollArea->setStyleSheet("QScrollArea { background: transparent; border: none; }");
    scrollArea->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    scrollArea->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    scrollArea->viewport()->setAutoFillBackground(false);
    scrollArea->viewport()->setStyleSheet("background: transparent;");

    QWidget* container = new QWidget();
    container->setStyleSheet("background: transparent;");

    QGridLayout* gridLayout = new QGridLayout(container);
    gridLayout->setSpacing(20);
    gridLayout->setAlignment(Qt::AlignTop | Qt::AlignLeft);

    const int columns = 4;
    const QSize cardSize(150, 194);

    if (player && player->getMasterDeck())
    {
        const QVector<Card*>& cards = player->getMasterDeck()->getCards();

        for (int i = 0; i < cards.size(); ++i)
        {
            Card* card = cards[i];
            if (!card)
                continue;

            QLabel* imageLabel = new QLabel();
            imageLabel->setFixedSize(cardSize);
            imageLabel->setStyleSheet(
                QString("border-image: url(%1); background: transparent;")
                    .arg(cardImagePath(card))
                );

            gridLayout->addWidget(imageLabel, i / columns, i % columns);
        }
    }

    scrollArea->setWidget(container);
    mainLayout->addWidget(scrollArea);
}