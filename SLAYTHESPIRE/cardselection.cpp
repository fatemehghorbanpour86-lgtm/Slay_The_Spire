#include "cardselection.h"
#include "card.h"
#include "deckviewer.h"
#include "audiomanager.h"

#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QPushButton>
#include <QDir>
#include <QCoreApplication>

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
    setFixedSize(720, 350);

    QPixmap pixmap(":/cursor.png");
    QPixmap scaledPixmap = pixmap.scaled(30, 40, Qt::KeepAspectRatio, Qt::SmoothTransformation);
    QCursor customCursor(scaledPixmap, 0, 0);
    this->setCursor(customCursor);

    QString baseDir = QCoreApplication::applicationDirPath();
    QString BtnPath = QDir(baseDir).filePath("assets/image/cursorBtn.png");
    QPixmap buttonHoverPixmap(BtnPath);
    QPixmap scaledHover = buttonHoverPixmap.scaled(40, 61, Qt::KeepAspectRatio, Qt::SmoothTransformation);
    QCursor buttonHoverCursor(scaledHover, scaledHover.width() / 2, 10);

    QVBoxLayout* mainLayout = new QVBoxLayout(this);
    mainLayout->setContentsMargins(30, 30, 30, 30);

    QHBoxLayout* cardsLayout = new QHBoxLayout();
    cardsLayout->setSpacing(20);
    cardsLayout->setAlignment(Qt::AlignCenter);

    const int CARD_WIDTH = 165;
    const int CARD_HEIGHT = 220;

    const int paddingX = static_cast<int>(CARD_WIDTH * 0.15);
    const int paddingY = static_cast<int>(CARD_HEIGHT * 0.15);

    for (Card* card : std::as_const(cardChoices))
    {
        if (!card)
            continue;

        QWidget* wrapper = new QWidget(this);
        wrapper->setFixedSize(CARD_WIDTH + paddingX, CARD_HEIGHT + paddingY);
        wrapper->setStyleSheet("background: transparent;");

        QPushButton* cardBtn = new QPushButton(wrapper);
        cardBtn->setGeometry(paddingX / 2, paddingY / 2, CARD_WIDTH, CARD_HEIGHT);
        cardBtn->setFlat(true);
        cardBtn->setCursor(buttonHoverCursor);

        cardBtn->setStyleSheet(
            QString("QPushButton { border: none; background: transparent; "
                    "border-image: url(%1); }")
                .arg(DeckViewerDialog::cardImagePath(card))
            );

        connect(cardBtn, &QPushButton::pressed, this, []() {
            AudioManager::instance().play(AudioManager::Sound::ButtonClick);
        });

        connect(cardBtn, &QPushButton::clicked, this, [this, card]() {
            chosenCard = card;
            accept();
        });

        cardBtn->setAttribute(Qt::WA_Hover, true);
        cardBtn->installEventFilter(this);

        cardsLayout->addWidget(wrapper);
    }

    mainLayout->addLayout(cardsLayout);
}

bool CardSelectionDialog::eventFilter(QObject* watched, QEvent* event)
{
    QPushButton* button = qobject_cast<QPushButton*>(watched);
    if (!button)
        return QDialog::eventFilter(watched, event);

    if (event->type() == QEvent::Enter)
    {
        if (!originalGeometry.contains(button))
            originalGeometry[button] = button->geometry();

        const QRect base = originalGeometry[button];

        const qreal scaleFactor = 1.15;
        const int newW = static_cast<int>(base.width() * scaleFactor);
        const int newH = static_cast<int>(base.height() * scaleFactor);

        QRect grown(0, 0, newW, newH);
        grown.moveCenter(base.center()); // مرکز ثابت می‌ماند

        if (activeAnimations.contains(button))
        {
            QPropertyAnimation* oldAnim = activeAnimations.take(button);
            oldAnim->stop();
            oldAnim->deleteLater();
        }

        if (button->parentWidget()) {
            button->parentWidget()->raise();
        }

        button->raise();

        QPropertyAnimation* anim = new QPropertyAnimation(button, "geometry", button);
        anim->setDuration(100);
        anim->setStartValue(button->geometry());
        anim->setEndValue(grown);
        anim->setEasingCurve(QEasingCurve::OutCubic);

        activeAnimations[button] = anim;
        anim->start(QAbstractAnimation::DeleteWhenStopped);

        connect(anim, &QPropertyAnimation::destroyed, this, [this, button]() {
            activeAnimations.remove(button);
        });
    }
    else if (event->type() == QEvent::Leave)
    {
        if (!originalGeometry.contains(button))
            return QDialog::eventFilter(watched, event);

        const QRect base = originalGeometry[button];

        if (activeAnimations.contains(button))
        {
            QPropertyAnimation* oldAnim = activeAnimations.take(button);
            oldAnim->stop();
            oldAnim->deleteLater();
        }

        QPropertyAnimation* anim = new QPropertyAnimation(button, "geometry", button);
        anim->setDuration(100);
        anim->setStartValue(button->geometry());
        anim->setEndValue(base);
        anim->setEasingCurve(QEasingCurve::OutCubic);

        activeAnimations[button] = anim;
        anim->start(QAbstractAnimation::DeleteWhenStopped);

        connect(anim, &QPropertyAnimation::destroyed, this, [this, button]() {
            activeAnimations.remove(button);
        });
    }

    return QDialog::eventFilter(watched, event);
}