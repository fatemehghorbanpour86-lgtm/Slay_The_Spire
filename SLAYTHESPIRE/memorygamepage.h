#ifndef MEMORYGAMEPAGE_H
#define MEMORYGAMEPAGE_H

#include <QWidget>
#include <QDialog>
#include <QLabel>
#include <QPushButton>
#include <QVBoxLayout>
#include <QStyleOption>
#include <QPainter>
#include <QVector>
#include <QString>
#include <functional>

#include "memorygame.h"

class TopBarWidget;
class Player;
class Map;

/*
 * MemoryRewardDialog
 * -------------------
 * Shown when the player wins the Memory Game (all pairs found within
 * MemoryGameModel::MAX_MOVES moves).
 *
 * Contains exactly one Reward button: a random Normal Relic. Claiming it
 * currently only calls the MemoryGameModel::createRandomNormalRelic()
 * stub and closes the dialog - actually granting the relic to the
 * Player is left for the future RelicSystem integration, the same way
 * RewardPage's buttons call into RewardSystem.
 */
class MemoryRewardDialog : public QDialog
{
    Q_OBJECT

public:
    explicit MemoryRewardDialog(QWidget* parent = nullptr);

private slots:
    void onRewardClicked();

private:
    void setupUI();

    QPushButton* rewardBtn;
};


/*
 * MemoryLoseDialog
 * ----------------
 * Shown when the player uses all MemoryGameModel::MAX_MOVES moves
 * without finding every pair. Purely informational for now - the
 * "Continue" button only closes the dialog; returning to the Map is
 * left to the future GameManager wiring.
 */
class MemoryLoseDialog : public QDialog
{
    Q_OBJECT

public:
    explicit MemoryLoseDialog(QWidget* parent = nullptr);

private:
    void setupUI();

    QPushButton* continueBtn;
};


/*
 * MemoryGameWidget
 * -----------------
 * UI/Page for the Memory Game mini-game Room.
 *
 * Follows the same structural pattern as CampfirePage/TreasurePage:
 *  - Reuses the shared TopBarWidget (declared in campfirepage.h) -
 *    no new top bar is created.
 *  - A dedicated background hook (#MemoryGamePage) is left without an
 *    image for now, exactly like the other Room pages, so a background
 *    can be dropped in later without touching this class.
 *  - All game rules/state live in MemoryGameModel; this class only
 *    renders that state and drives the Qt animations.
 *
 * Not connected to Map, GameManager, or RewardSystem yet. The
 * memoryGameWon() / memoryGameLost() / requestReward() signals below
 * exist purely as future integration points - nothing currently
 * connects to them, exactly as requested.
 */
class MemoryGameWidget : public QWidget
{
    Q_OBJECT

public:
    explicit MemoryGameWidget(Player* player, Map* map, QWidget* parent = nullptr);

signals:
    // ---- Future integration hooks (GameManager). Not connected anywhere yet. ----
    void memoryGameWon();
    void memoryGameLost();
    void requestReward();

    void settingsRequested();

private slots:
    void onCardClicked(int index);

private:
    Player* player;
    Map* map;

    MemoryGameModel model;

    TopBarWidget* topBar = nullptr;
    QLabel* instructionLabel = nullptr;
    QWidget* cardsContainer = nullptr;

    QVector<QPushButton*> cardButtons;

    // True while a two-card comparison and/or a flip animation is in
    // progress; blocks all further card clicks until it clears.
    bool inputLocked = false;

    static const int ROWS = 2;
    static const int COLUMNS = 4;
    static const int CARD_WIDTH = 150;
    static const int CARD_HEIGHT = 200;
    static const int CARD_SPACING = 20;

    // Duration of one half of the flip (shrink OR grow). The full flip
    // (shrink + image swap + grow) therefore takes 2x this long.
    static const int FLIP_HALF_DURATION_MS = 150;

    void setupUI();
    void createCardButtons();
    void refreshInstructionLabel();
    void refreshCardVisual(int index);

    QString backImagePath() const;
    QString frontImagePath(int imageId) const;

    // Card-flip animation: shrinks the card horizontally down to a
    // sliver, swaps its pixmap at the midpoint (via onMidpoint), then
    // expands it back to full width. Used both for revealing a card and
    // for flipping a mismatched pair back down.
    void playFlipAnimation(int index, std::function<void()> onMidpoint);

    // Called once the second card of a Move has finished its reveal
    // animation: resolves the pending pair and reacts to a match/mismatch.
    void handleSecondCardResolved();

    // Waits MemoryGameModel::FLIP_DELAY_MS, then flips both mismatched
    // cards back face-down and re-enables them.
    void finishMismatch(int firstIndex, int secondIndex);

    // Shows the Reward or Lose dialog once the model reports the game
    // is won/lost.
    void checkGameEndState();

protected:
    void paintEvent(QPaintEvent*) override
    {
        QStyleOption opt;
        opt.initFrom(this);
        QPainter p(this);
        style()->drawPrimitive(QStyle::PE_Widget, &opt, &p, this);
    }
};


#endif // MEMORYGAMEPAGE_H
