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

            imageLabel->setAttribute(Qt::WA_Hover, true);
            imageLabel->installEventFilter(this);

            gridLayout->addWidget(imageLabel, i / columns, i % columns);
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
        // اولین باری که این لیبل هاور می‌شه، geometry اصلیش رو ذخیره کن
        if (!originalGeometry.contains(label))
            originalGeometry[label] = label->geometry();

        const QRect base = originalGeometry[label];

        // اندازه بزرگ‌تر (مثلاً ۱۵٪ بزرگ‌تر)، ولی مرکزش همون مرکز قبلی بمونه
        const qreal scaleFactor = 1.15;
        const int newW = static_cast<int>(base.width() * scaleFactor);
        const int newH = static_cast<int>(base.height() * scaleFactor);

        QRect grown(0, 0, newW, newH);
        grown.moveCenter(base.center());

        // اگه انیمیشن قبلی هنوز در حال اجراست، متوقفش کن
        if (activeAnimations.contains(label))
        {
            activeAnimations[label]->stop();
            activeAnimations[label]->deleteLater();
        }

        label->setMinimumSize(0, 0);
        label->setMaximumSize(QWIDGETSIZE_MAX, QWIDGETSIZE_MAX);

        label->raise(); // بیاد روی لیبل‌های کناری تا موقع بزرگ شدن بریده نشه

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
            activeAnimations[label]->stop();
            activeAnimations[label]->deleteLater();
        }

        QPropertyAnimation* anim = new QPropertyAnimation(label, "geometry", label);
        anim->setDuration(100);
        anim->setStartValue(label->geometry());
        anim->setEndValue(base);
        anim->setEasingCurve(QEasingCurve::OutCubic);

        activeAnimations[label] = anim;
        anim->start(QAbstractAnimation::DeleteWhenStopped);

        connect(anim, &QPropertyAnimation::finished, this, [label, base]() {
            label->setFixedSize(base.size());
        });

        activeAnimations[label] = anim;

        connect(anim, &QPropertyAnimation::destroyed, this, [this, label]() {
            activeAnimations.remove(label);
        });
    }

    return QDialog::eventFilter(watched, event);
}