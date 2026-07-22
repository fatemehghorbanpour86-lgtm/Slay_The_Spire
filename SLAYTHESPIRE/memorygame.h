#ifndef MEMORYGAME_H
#define MEMORYGAME_H

#include <QVector>
#include "memorycard.h"


class MemoryGameModel
{
public:

    // ---------------------------------
    // Mini-game constants
    // ---------------------------------
    static constexpr int TOTAL_CARDS = 8;
    static constexpr int TOTAL_PAIRS = 4;
    static constexpr int MAX_MOVES = 6;
    static constexpr int FLIP_DELAY_MS = 700;

    MemoryGameModel();

    // ---------------------------------
    // Setup
    // ---------------------------------

    // Builds a fresh set of TOTAL_CARDS cards (TOTAL_PAIRS distinct
    // images, each appearing twice), shuffles them, and resets the
    // move/match counters. Call this every time the Room is entered so
    // the layout is different on each visit.
    void newGame();

    // ---------------------------------
    // Card Access
    // ---------------------------------

    const QVector<MemoryCard>& getCards() const;

    // ---------------------------------
    // Selection Flow
    // ---------------------------------

    // True if the card at 'index' can currently be clicked: it must not
    // already be matched or already face-up, and no pending two-card
    // comparison may currently be in progress.
    bool canSelect(int index) const;

    // Flips the given card face-up and registers it as the first or
    // second pick of the current move. Caller must have already checked
    // canSelect(index).
    void selectCard(int index);

    // True once exactly two cards are face-up and awaiting comparison
    // (i.e. immediately after the second selectCard() call of a move).
    bool hasPendingPair() const;

    int getFirstSelectedIndex() const;
    int getSecondSelectedIndex() const;

    // Compares the two pending cards and counts this as one completed
    // Move (moveCount is incremented exactly once per call, regardless
    // of the outcome - matching the doc's definition of "Move" as
    // picking two cards, not each individual click).
    //
    // On a match: both cards are marked matched (they stay face-up
    // permanently) and the pending selection is cleared automatically.
    //
    // On a mismatch: faceUp state is left untouched here. The caller
    // (MemoryGameWidget) is expected to flip both cards back down after
    // its own delay/animation, then call clearPendingSelection().
    //
    // Returns true if the two pending cards matched.
    bool resolvePendingPair();

    // Flips both pending (mismatched) cards back face-down and clears
    // the pending selection. No-op if there is no pending pair, or if a
    // pending card has since become matched.
    void clearPendingSelection();

    // ---------------------------------
    // Progress / End State
    // ---------------------------------

    int getMoveCount() const;
    int getMovesRemaining() const;
    int getMatchedPairCount() const;

    // True once all TOTAL_PAIRS pairs have been matched.
    bool isGameWon() const;

    // True once MAX_MOVES moves have been used without winning.
    bool isGameLost() const;

    // ---------------------------------
    // Reward (stub - not yet connected to RelicSystem)
    // ---------------------------------

    // TODO (RelicSystem integration): should return a freshly created
    // random Normal-tier Relic, mirroring
    // RelicSystem::createRandomRelic(player, Relic::Tier::Normal).
    // Deliberately left unimplemented for now - only the call site
    // exists so MemoryRewardDialog already has something to invoke, and
    // wiring it up later (once this Room has a Player to grant to) is a
    // one-line change.
    static void createRandomNormalRelic();

private:
    QVector<MemoryCard> cards;

    int moveCount;
    int matchedPairCount;

    int firstSelectedIndex;
    int secondSelectedIndex;

    void resetSelection();
};

#endif // MEMORYGAME_H
