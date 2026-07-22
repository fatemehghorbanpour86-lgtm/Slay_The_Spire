#include "memorygame.h"
#include <QRandomGenerator>
#include <algorithm>

MemoryGameModel::MemoryGameModel()
    : moveCount(0),
    matchedPairCount(0),
    firstSelectedIndex(-1),
    secondSelectedIndex(-1)
{
    newGame();
}

void MemoryGameModel::newGame()
{
    cards.clear();

    // TOTAL_PAIRS distinct image ids (0 .. TOTAL_PAIRS-1), each appearing
    // exactly twice - together they make up TOTAL_CARDS cards.
    for (int imageId = 0; imageId < TOTAL_PAIRS; ++imageId)
    {
        cards.append(MemoryCard(imageId));
        cards.append(MemoryCard(imageId));
    }

    std::shuffle(cards.begin(), cards.end(), *QRandomGenerator::global());

    moveCount = 0;
    matchedPairCount = 0;

    resetSelection();
}

const QVector<MemoryCard>& MemoryGameModel::getCards() const
{
    return cards;
}

bool MemoryGameModel::canSelect(int index) const
{
    if (index < 0 || index >= cards.size())
        return false;

    if (hasPendingPair())
        return false; // A two-card comparison is already in progress.

    if (cards[index].isMatched() || cards[index].isFaceUp())
        return false;

    return true;
}

void MemoryGameModel::selectCard(int index)
{
    if (index < 0 || index >= cards.size())
        return;

    cards[index].setFaceUp(true);

    if (firstSelectedIndex == -1)
    {
        firstSelectedIndex = index;
    }
    else if (secondSelectedIndex == -1)
    {
        secondSelectedIndex = index;
    }
}

bool MemoryGameModel::hasPendingPair() const
{
    return firstSelectedIndex != -1 && secondSelectedIndex != -1;
}

int MemoryGameModel::getFirstSelectedIndex() const
{
    return firstSelectedIndex;
}

int MemoryGameModel::getSecondSelectedIndex() const
{
    return secondSelectedIndex;
}

bool MemoryGameModel::resolvePendingPair()
{
    if (!hasPendingPair())
        return false;

    // A "Move" is picking two cards, regardless of the outcome - so the
    // counter increases exactly once here.
    moveCount++;

    MemoryCard& first = cards[firstSelectedIndex];
    MemoryCard& second = cards[secondSelectedIndex];

    const bool isMatch = (first.getImageId() == second.getImageId());

    if (isMatch)
    {
        first.setMatched(true);
        second.setMatched(true);

        matchedPairCount++;

        resetSelection();
    }

    // On a mismatch faceUp is intentionally left as-is: the widget flips
    // both cards back down after its own delay/animation, then calls
    // clearPendingSelection().

    return isMatch;
}

void MemoryGameModel::clearPendingSelection()
{
    if (firstSelectedIndex != -1 && !cards[firstSelectedIndex].isMatched())
        cards[firstSelectedIndex].setFaceUp(false);

    if (secondSelectedIndex != -1 && !cards[secondSelectedIndex].isMatched())
        cards[secondSelectedIndex].setFaceUp(false);

    resetSelection();
}

int MemoryGameModel::getMoveCount() const
{
    return moveCount;
}

int MemoryGameModel::getMovesRemaining() const
{
    const int remaining = MAX_MOVES - moveCount;
    return remaining < 0 ? 0 : remaining;
}

int MemoryGameModel::getMatchedPairCount() const
{
    return matchedPairCount;
}

bool MemoryGameModel::isGameWon() const
{
    return matchedPairCount >= TOTAL_PAIRS;
}

bool MemoryGameModel::isGameLost() const
{
    return !isGameWon() && moveCount >= MAX_MOVES;
}

void MemoryGameModel::createRandomNormalRelic()
{
    // TODO: once this Room is wired into GameManager/Player, replace
    // this stub with something like:
    //   Relic* relic = RelicSystem::createRandomRelic(player, Relic::Tier::Normal);
    //   if (relic) player->addRelic(relic);
}

void MemoryGameModel::resetSelection()
{
    firstSelectedIndex = -1;
    secondSelectedIndex = -1;
}