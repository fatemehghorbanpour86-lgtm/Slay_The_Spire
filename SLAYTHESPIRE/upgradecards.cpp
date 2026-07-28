#include "upgradecards.h"
#include "player.h"
#include "masterdeck.h"
#include "card.h"
#include "campfire.h"
#include "deckviewer.h"
#include "audiomanager.h"
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QLabel>
#include <QPushButton>
#include <QPixmap>
#include <QIcon>
#include <QSize>
#include <QDir>
#include <QCoreApplication>
#include <QPropertyAnimation>
#include <QEvent>

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
    setFixedSize(700, 500);
    setStyleSheet("QDialog { border-image: url(:/RestSite/UpgradeViewer.png); border: none; background: transparent; }"
                  );

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
    upgradeBtn->setCursor(buttonHoverCursor);

    QPushButton* cancelBtn = new QPushButton();
    cancelBtn->setObjectName("upgradeCancelBtn");
    cancelBtn->setFixedSize(150, 100);
    cancelBtn->setStyleSheet("QPushButton { border-image: url(:/RestSite/NoBtn.png); }"
                             "QPushButton:pressed { "
                             "   margin: 5px 5px 5px 5px; "
                             "}");
    cancelBtn->setCursor(buttonHoverCursor);

    buttonsLayout->addWidget(cancelBtn);
    buttonsLayout->addWidget(upgradeBtn);

    mainLayout->addLayout(buttonsLayout);

    connect(upgradeBtn, &QPushButton::pressed,
            this, []()
            {
                AudioManager::instance().play(AudioManager::Sound::ButtonClick);
            });
    connect(cancelBtn, &QPushButton::pressed,
            this, []()
            {
                AudioManager::instance().play(AudioManager::Sound::ButtonClick);
            });
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
    setFixedSize(917, 620);
    setStyleSheet("QDialog { border-image: url(:/card/CardViewer.png); }"
                  "QScrollArea { border: none; background: transparent; }"
                  );

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

    QPixmap pixmap(":/cursor.png");
    QPixmap scaledPixmap = pixmap.scaled(30, 40, Qt::KeepAspectRatio, Qt::SmoothTransformation);
    QCursor customCursor(scaledPixmap, 0, 0);
    this->setCursor(customCursor);

    QString baseDir = QCoreApplication::applicationDirPath();
    QString BtnPath = QDir(baseDir).filePath("assets/image/cursorBtn.png");
    QPixmap buttonHoverPixmap(BtnPath);
    QPixmap scaledHover = buttonHoverPixmap.scaled(40, 61, Qt::KeepAspectRatio, Qt::SmoothTransformation);
    QCursor buttonHoverCursor(scaledHover, scaledHover.width() / 2, 10);

    const int paddingX = static_cast<int>(CARD_WIDTH * 0.15);
    const int paddingY = static_cast<int>(CARD_HEIGHT * 0.15);

    int row = 0;
    int col = 0;

    for (Card* card : player->getMasterDeck()->getCards())
    {
        if (!card || card->getIsUpgraded())
            continue;

        QWidget* wrapper = new QWidget();
        wrapper->setFixedSize(CARD_WIDTH + paddingX, CARD_HEIGHT+ paddingY);
        wrapper->setStyleSheet("background: transparent;");

        QPushButton* cardBtn = new QPushButton(wrapper);
        cardBtn->setGeometry(paddingX / 2, paddingY / 2, CARD_WIDTH, CARD_HEIGHT);
        cardBtn->setFlat(true);
        cardBtn->setCursor(buttonHoverCursor);

        QString imgPath = DeckViewerDialog::cardImagePath(card);
        cardBtn->setStyleSheet(QString("QPushButton { border-image: url(%1); border: none; background: transparent; }").arg(imgPath));

        connect(cardBtn, &QPushButton::pressed,
                this, []()
                {
                    AudioManager::instance().play(AudioManager::Sound::ButtonClick);
                });
        connect(cardBtn, &QPushButton::clicked, this, [this, card]() {
            openPreview(card);
        });

        cardBtn->setAttribute(Qt::WA_Hover, true);
        cardBtn->installEventFilter(this);

        gridLayout->addWidget(wrapper, row, col);

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

bool UpgradeCardsDialog::eventFilter(QObject* watched, QEvent* event)
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
        grown.moveCenter(base.center());

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