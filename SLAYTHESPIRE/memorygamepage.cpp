#include "memorygamepage.h"
#include "campfirepage.h" // TopBarWidget
#include "relicviewer.h"
#include "deckviewer.h"

#include <QIcon>
#include <QPixmap>
#include <QTimer>
#include <QPropertyAnimation>
#include <QEasingCurve>
#include <QAbstractAnimation>

//======================================================
//  MemoryRewardDialog
//======================================================

MemoryRewardDialog::MemoryRewardDialog(QWidget* parent)
    : QDialog(parent), rewardBtn(nullptr)
{
    setupUI();
}

void MemoryRewardDialog::setupUI()
{
    setWindowTitle("Memory Game Reward");
    setStyleSheet(
        "QDialog { border-image: url(:/Reward/SelectionViewer.png); border: none; background: transparent; }"
        );
    setFixedSize(400, 300);

    QVBoxLayout* mainLayout = new QVBoxLayout(this);
    mainLayout->setContentsMargins(30, 30, 30, 30);
    mainLayout->setAlignment(Qt::AlignCenter);

    QLabel* title = new QLabel("You found every pair!", this);
    title->setAlignment(Qt::AlignCenter);
    title->setWordWrap(true);
    title->setStyleSheet(
        "color: #facc15; font-size: 20px; font-weight: bold; background: transparent;");
    mainLayout->addWidget(title);

    mainLayout->addSpacing(20);

    rewardBtn = new QPushButton(this);
    rewardBtn->setFixedSize(220, 60);
    rewardBtn->setCursor(Qt::PointingHandCursor);
    rewardBtn->setText("Random Relic");
    rewardBtn->setStyleSheet(
        "QPushButton { border-image: url(:/Reward/RelicReward.png); font-size: 15px;"
        "font-weight: bold; border: 1px solid rgba(255,255,255,40); }"
        "QPushButton:pressed { margin: 5px 5px 5px 5px; }"
        );

    connect(rewardBtn, &QPushButton::clicked, this, &MemoryRewardDialog::onRewardClicked);

    mainLayout->addWidget(rewardBtn, 0, Qt::AlignHCenter);
}

void MemoryRewardDialog::onRewardClicked()
{
    // Backend stub only - see MemoryGameModel::createRandomNormalRelic().
    // Actually granting the relic to the Player is left for the future
    // RelicSystem integration.
    MemoryGameModel::createRandomNormalRelic();

    accept();
}


//======================================================
//  MemoryLoseDialog
//======================================================

MemoryLoseDialog::MemoryLoseDialog(QWidget* parent)
    : QDialog(parent), continueBtn(nullptr)
{
    setupUI();
}

void MemoryLoseDialog::setupUI()
{
    setWindowTitle("Challenge Failed");
    setStyleSheet(
        "QDialog { border-image: url(:/Reward/SelectionViewer.png); border: none; background: transparent; }"
        );
    setFixedSize(420, 260);

    QVBoxLayout* mainLayout = new QVBoxLayout(this);
    mainLayout->setContentsMargins(30, 30, 30, 30);
    mainLayout->setAlignment(Qt::AlignCenter);

    QLabel* messageLabel = new QLabel("You couldn't complete the challenge.", this);
    messageLabel->setAlignment(Qt::AlignCenter);
    messageLabel->setWordWrap(true);
    messageLabel->setStyleSheet(
        "color: #E8DCC0; font-size: 18px; font-weight: bold; background: transparent;");
    mainLayout->addWidget(messageLabel);

    mainLayout->addSpacing(20);

    continueBtn = new QPushButton("Continue", this);
    continueBtn->setFixedSize(160, 50);
    continueBtn->setCursor(Qt::PointingHandCursor);
    continueBtn->setStyleSheet(
        "QPushButton { border-image: url(:/SettingPushButton.png); font-size: 15px;"
        "font-weight: bold; border: 1px solid rgba(255,255,255,40); }"
        "QPushButton:pressed { margin: 5px 5px 5px 5px; }"
        );

    // Only closes the dialog for now - GameManager will decide what
    // happens next (e.g. return to Map) once this Room is wired in.
    connect(continueBtn, &QPushButton::clicked, this, &QDialog::accept);

    mainLayout->addWidget(continueBtn, 0, Qt::AlignHCenter);
}


//======================================================
//  MemoryGameWidget
//======================================================

MemoryGameWidget::MemoryGameWidget(Player* playerPtr, Map* mapPtr, QWidget* parent)
    : QWidget(parent), player(playerPtr), map(mapPtr)
{
    setupUI();

    if (topBar)
        topBar->updateData(player, map);
}

void MemoryGameWidget::setupUI()
{
    setObjectName("MemoryGamePage");

    // Background image intentionally left unset - only the styling hook
    // exists so an image can be dropped in later, the same way
    // CampfirePage/TreasurePage set "background-image" on their own
    // page-level selector.
    setStyleSheet(
        "#MemoryGamePage {"
        "background-repeat: no-repeat;"
        "background-position: center;"
        "background-color: #1a1410;"
        "}"
        "QToolTip { color: #facc15; background-color: #1f2937; border: 1px solid #b91c1c;"
        "border-radius: 4px; padding: 6px; font-weight: bold; font-family: Tahoma;"
        "}"
        );
    setFixedSize(1280, 720);

    QVBoxLayout* mainLayout = new QVBoxLayout(this);
    mainLayout->setContentsMargins(0, 0, 0, 0);
    mainLayout->setSpacing(0);

    // --- Shared TopBar (same widget as Map/Campfire/Treasure/Event) ---
    topBar = new TopBarWidget(this);

    connect(topBar, &TopBarWidget::relicClicked, this, [this]()
            {
                if (!player)
                    return;

                RelicViewerDialog dialog(player, this);
                dialog.exec();
            });

    connect(topBar, &TopBarWidget::deckClicked, this, [this]()
            {
                if (!player)
                    return;

                DeckViewerDialog dialog(player, this);
                dialog.exec();
            });

    connect(topBar, &TopBarWidget::settingsClicked, this, [this]()
            {
                emit settingsRequested();
            });

    mainLayout->addWidget(topBar);

    mainLayout->addStretch(1);

    // --- Instruction / moves-remaining label ---
    instructionLabel = new QLabel(this);
    instructionLabel->setAlignment(Qt::AlignCenter);
    instructionLabel->setWordWrap(true);
    instructionLabel->setStyleSheet(
        "color: #facc15; font-size: 22px; font-weight: bold; background: transparent;");
    mainLayout->addWidget(instructionLabel, 0, Qt::AlignHCenter);

    mainLayout->addSpacing(30);

    // --- Card grid container (2 rows x 4 columns, centered) ---
    cardsContainer = new QWidget(this);
    const int containerWidth = COLUMNS * CARD_WIDTH + (COLUMNS - 1) * CARD_SPACING;
    const int containerHeight = ROWS * CARD_HEIGHT + (ROWS - 1) * CARD_SPACING;
    cardsContainer->setFixedSize(containerWidth, containerHeight);
    cardsContainer->setStyleSheet("background: transparent;");
    mainLayout->addWidget(cardsContainer, 0, Qt::AlignHCenter);

    mainLayout->addStretch(2);

    createCardButtons();
    refreshInstructionLabel();
}

void MemoryGameWidget::createCardButtons()
{
    cardButtons.clear();
    cardButtons.resize(MemoryGameModel::TOTAL_CARDS);

    for (int i = 0; i < MemoryGameModel::TOTAL_CARDS; ++i)
    {
        const int row = i / COLUMNS;
        const int col = i % COLUMNS;

        const int x = col * (CARD_WIDTH + CARD_SPACING);
        const int y = row * (CARD_HEIGHT + CARD_SPACING);

        QPushButton* btn = new QPushButton(cardsContainer);
        btn->setGeometry(x, y, CARD_WIDTH, CARD_HEIGHT);
        btn->setCursor(Qt::PointingHandCursor);
        btn->setIconSize(QSize(CARD_WIDTH, CARD_HEIGHT));
        btn->setStyleSheet(
            "QPushButton { border: 2px solid #5a5a54; border-radius: 8px; background: #2b2b28; }"
            "QPushButton:disabled { border: 2px solid #3a3a36; }"
            );

        cardButtons[i] = btn;

        connect(btn, &QPushButton::clicked, this, [this, i]()
                {
                    onCardClicked(i);
                });

        refreshCardVisual(i);
    }
}

void MemoryGameWidget::refreshInstructionLabel()
{
    if (!instructionLabel)
        return;

    if (model.getMoveCount() == 0)
    {
        instructionLabel->setText(
            QString("Find all %1 matching pairs in %2 moves or less to earn a reward!")
                .arg(MemoryGameModel::TOTAL_PAIRS)
                .arg(MemoryGameModel::MAX_MOVES));
    }
    else
    {
        instructionLabel->setText(
            QString("Moves Remaining: %1").arg(model.getMovesRemaining()));
    }
}

QString MemoryGameWidget::backImagePath() const
{
    // Placeholder path - drop the shared card-back image in under this
    // resource prefix later (e.g. add a /MemoryGame qresource section).
    return ":/MemoryGame/CardBack.png";
}

QString MemoryGameWidget::frontImagePath(int imageId) const
{
    // Placeholder path - drop the 4 front images in as Card0..Card3.
    return QString(":/MemoryGame/Card%1.png").arg(imageId);
}

void MemoryGameWidget::refreshCardVisual(int index)
{
    if (index < 0 || index >= cardButtons.size())
        return;

    QPushButton* btn = cardButtons[index];
    if (!btn)
        return;

    const MemoryCard& card = model.getCards()[index];

    const QString imagePath = (card.isFaceUp() || card.isMatched())
                                  ? frontImagePath(card.getImageId())
                                  : backImagePath();

    const QPixmap pixmap(imagePath);
    btn->setIcon(QIcon(pixmap.scaled(
        CARD_WIDTH, CARD_HEIGHT, Qt::IgnoreAspectRatio, Qt::SmoothTransformation)));
    btn->setIconSize(QSize(CARD_WIDTH, CARD_HEIGHT));

    if (card.isMatched())
        btn->setEnabled(false);
}

void MemoryGameWidget::playFlipAnimation(int index, std::function<void()> onMidpoint)
{
    if (index < 0 || index >= cardButtons.size())
        return;

    QPushButton* btn = cardButtons[index];
    if (!btn)
        return;

    const QRect fullGeometry = btn->geometry();
    const QRect thinGeometry(
        fullGeometry.center().x(),
        fullGeometry.y(),
        0,
        fullGeometry.height());

    // Step 1: shrink the card horizontally down to a sliver.
    auto* shrink = new QPropertyAnimation(btn, "geometry", this);
    shrink->setDuration(FLIP_HALF_DURATION_MS);
    shrink->setStartValue(fullGeometry);
    shrink->setEndValue(thinGeometry);
    shrink->setEasingCurve(QEasingCurve::InCubic);

    connect(shrink, &QPropertyAnimation::finished, this,
            [this, btn, fullGeometry, onMidpoint]()
            {
                // Step 2: swap the pixmap at the midpoint of the flip.
                if (onMidpoint)
                    onMidpoint();

                // Step 3: expand the card back out to full width.
                auto* grow = new QPropertyAnimation(btn, "geometry", this);
                grow->setDuration(FLIP_HALF_DURATION_MS);
                grow->setStartValue(btn->geometry());
                grow->setEndValue(fullGeometry);
                grow->setEasingCurve(QEasingCurve::OutCubic);
                grow->start(QAbstractAnimation::DeleteWhenStopped);
            });

    shrink->start(QAbstractAnimation::DeleteWhenStopped);
}

void MemoryGameWidget::onCardClicked(int index)
{
    if (inputLocked)
        return;

    if (!model.canSelect(index))
        return;

    inputLocked = true;

    if (index >= 0 && index < cardButtons.size() && cardButtons[index])
        cardButtons[index]->setEnabled(false);

    model.selectCard(index);

    playFlipAnimation(index, [this, index]()
                      {
                          refreshCardVisual(index);
                      });

    // Wait for the reveal flip to finish, then decide whether this was
    // the first card of the Move (just unlock) or the second (resolve
    // the pending pair).
    const int totalFlipDuration = FLIP_HALF_DURATION_MS * 2 + 20;

    QTimer::singleShot(totalFlipDuration, this, [this]()
                       {
                           if (model.hasPendingPair())
                           {
                               handleSecondCardResolved();
                           }
                           else
                           {
                               inputLocked = false;
                           }
                       });
}

void MemoryGameWidget::handleSecondCardResolved()
{
    const int first = model.getFirstSelectedIndex();
    const int second = model.getSecondSelectedIndex();

    const bool isMatch = model.resolvePendingPair();

    refreshInstructionLabel();

    if (isMatch)
    {
        // Matched cards stay face-up and disabled permanently -
        // refreshCardVisual() already handles disabling them.
        refreshCardVisual(first);
        refreshCardVisual(second);

        checkGameEndState();

        if (!model.isGameWon())
            inputLocked = false;

        return;
    }

    // Mismatch: keep input locked, wait, then flip both back down.
    QTimer::singleShot(MemoryGameModel::FLIP_DELAY_MS, this,
                       [this, first, second]()
                       {
                           finishMismatch(first, second);
                       });
}

void MemoryGameWidget::finishMismatch(int firstIndex, int secondIndex)
{
    playFlipAnimation(firstIndex, [this, firstIndex]()
                      {
                          refreshCardVisual(firstIndex);
                      });

    playFlipAnimation(secondIndex, [this, secondIndex]()
                      {
                          refreshCardVisual(secondIndex);
                      });

    model.clearPendingSelection();

    const int totalFlipDuration = FLIP_HALF_DURATION_MS * 2 + 20;

    QTimer::singleShot(totalFlipDuration, this,
                       [this, firstIndex, secondIndex]()
                       {
                           const auto& cards = model.getCards();

                           if (firstIndex >= 0 && firstIndex < cardButtons.size() &&
                               cardButtons[firstIndex] && !cards[firstIndex].isMatched())
                           {
                               cardButtons[firstIndex]->setEnabled(true);
                           }

                           if (secondIndex >= 0 && secondIndex < cardButtons.size() &&
                               cardButtons[secondIndex] && !cards[secondIndex].isMatched())
                           {
                               cardButtons[secondIndex]->setEnabled(true);
                           }

                           checkGameEndState();

                           if (!model.isGameLost())
                               inputLocked = false;
                       });
}

void MemoryGameWidget::checkGameEndState()
{
    if (model.isGameWon())
    {
        inputLocked = true;

        emit memoryGameWon();

        MemoryRewardDialog dialog(this);
        dialog.exec();

        emit requestReward();
        return;
    }

    if (model.isGameLost())
    {
        inputLocked = true;

        emit memoryGameLost();

        MemoryLoseDialog dialog(this);
        dialog.exec();
    }
}
