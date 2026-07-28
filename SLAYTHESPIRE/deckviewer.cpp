#include "deckviewer.h"
#include "masterdeck.h"

#include <QVBoxLayout>
#include <QGridLayout>
#include <QScrollArea>
#include <QLabel>
#include <QWidget>
#include <QPropertyAnimation>
#include <QEvent>


DeckViewerDialog::DeckViewerDialog(Player* player, QWidget* parent)
    : QDialog(parent), player(player)
{
    setWindowTitle("Deck");
    setFixedSize(917, 620);
    setStyleSheet("QDialog { border-image: url(:/card/CardViewer.png); }");

    QPixmap pixmap(":/cursor.png");
    QPixmap scaledPixmap = pixmap.scaled(30, 40, Qt::KeepAspectRatio, Qt::SmoothTransformation);
    QCursor customCursor(scaledPixmap, 0, 0);
    this->setCursor(customCursor);

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
    scrollArea->viewport()->setAutoFillBackground(false);
    scrollArea->viewport()->setStyleSheet("background: transparent;");

    QWidget* container = new QWidget();
    container->setStyleSheet("background: transparent;");

    QGridLayout* gridLayout = new QGridLayout(container);
    gridLayout->setSpacing(20);
    gridLayout->setAlignment(Qt::AlignTop | Qt::AlignLeft);

    const int columns = 4;
    const QSize cardSize(162, 214);

    const int paddingX = static_cast<int>(cardSize.width() * 0.15);
    const int paddingY = static_cast<int>(cardSize.height() * 0.15);

    if (player && player->getMasterDeck())
    {
        const QVector<Card*>& cards = player->getMasterDeck()->getCards();

        for (int i = 0; i < cards.size(); ++i)
        {
            Card* card = cards[i];
            if (!card)
                continue;

            QWidget* wrapper = new QWidget();
            wrapper->setFixedSize(cardSize.width() + paddingX, cardSize.height() + paddingY);
            wrapper->setStyleSheet("background: transparent;");

            QLabel* imageLabel = new QLabel(wrapper);
            imageLabel->setGeometry(paddingX / 2, paddingY / 2, cardSize.width(), cardSize.height());
            imageLabel->setStyleSheet(
                QString("border-image: url(%1); background: transparent;")
                    .arg(cardImagePath(card))
                );

            imageLabel->setAttribute(Qt::WA_Hover, true);
            imageLabel->installEventFilter(this);

            gridLayout->addWidget(wrapper, i / columns, i % columns);
        }
    }

    scrollArea->setWidget(container);
    mainLayout->addWidget(scrollArea);
}

bool DeckViewerDialog::eventFilter(QObject* watched, QEvent* event)
{
    QLabel* label = qobject_cast<QLabel*>(watched);

    if (!label)
        return QDialog::eventFilter(watched, event);

    if (event->type() == QEvent::Enter)
    {
        if (!originalGeometry.contains(label))
            originalGeometry[label] = label->geometry();

        const QRect base = originalGeometry[label];

        const qreal scaleFactor = 1.15;
        const int newW = static_cast<int>(base.width() * scaleFactor);
        const int newH = static_cast<int>(base.height() * scaleFactor);

        QRect grown(0, 0, newW, newH);
        grown.moveCenter(base.center());

        if (activeAnimations.contains(label))
        {
            QPropertyAnimation* oldAnim = activeAnimations.take(label);
            oldAnim->stop();
            oldAnim->deleteLater();
        }

        if (label->parentWidget()) {
            label->parentWidget()->raise();
        }

        label->raise();

        QPropertyAnimation* anim = new QPropertyAnimation(label, "geometry", label);
        anim->setDuration(100);
        anim->setStartValue(label->geometry());
        anim->setEndValue(grown);
        anim->setEasingCurve(QEasingCurve::OutCubic);

        activeAnimations[label] = anim;
        anim->start(QAbstractAnimation::DeleteWhenStopped);

        connect(anim, &QPropertyAnimation::destroyed, this, [this, label]() {
            activeAnimations.remove(label);
        });
    }
    else if (event->type() == QEvent::Leave)
    {
        if (!originalGeometry.contains(label))
            return QDialog::eventFilter(watched, event);

        const QRect base = originalGeometry[label];

        if (activeAnimations.contains(label))
        {
            QPropertyAnimation* oldAnim = activeAnimations.take(label);
            oldAnim->stop();
            oldAnim->deleteLater();
        }

        QPropertyAnimation* anim = new QPropertyAnimation(label, "geometry", label);
        anim->setDuration(100);
        anim->setStartValue(label->geometry());
        anim->setEndValue(base);
        anim->setEasingCurve(QEasingCurve::OutCubic);

        activeAnimations[label] = anim;
        anim->start(QAbstractAnimation::DeleteWhenStopped);

        connect(anim, &QPropertyAnimation::destroyed, this, [this, label]() {
            activeAnimations.remove(label);
        });
    }

    return QDialog::eventFilter(watched, event);
}