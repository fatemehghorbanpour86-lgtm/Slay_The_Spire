#include "bossenemies.h"
#include "player.h"
#include <QRandomGenerator>


KingSlime::KingSlime()
    : Enemy("King Slime", 140),
    m_splitReady(false),
    m_splitExecuted(false),
    m_splitRequested(false)
{
}
void KingSlime::chooseIntent()
{
    if(getTurnCount() == 0)
    {
        setIntent(Intent::Unknown);
        setCurrentMove(Preparing);
        setIntentDamage(0);
        setIntentHits(0);
        return;
    }


    if(!m_splitExecuted && (getCurrentHealth() <= (getMaxHealth() / 2)))
    {
        m_splitReady = true;
        setIntent(Intent::Unknown);
        setCurrentMove(Preparing); // placeholder, executeMove() will route to performSplit()
        setIntentDamage(0);
        setIntentHits(0);
        return;
    }

    switch(getCurrentMove())
    {
    case Preparing:
        setCurrentMove(Slam);
        setIntent(Intent::Attack);
        setIntentDamage(35);
        setIntentHits(1);
        break;

    case Slam:
        setCurrentMove(GoopSpray);
        setIntent(Intent::Debuff);
        setIntentDamage(0);
        setIntentHits(0);
        break;

    case GoopSpray:
        setCurrentMove(Preparing);
        setIntent(Intent::Unknown);
        setIntentDamage(0);
        setIntentHits(0);
        break;
    }
}
void KingSlime::executeMove(Player *player)
{
    if(player == nullptr)
    {
        return;
    }

    if(m_splitReady)
    {
        performSplit();
        return;
    }

    switch(getCurrentMove())
    {
    case Preparing:
        // Does nothing.
        break;

    case Slam:
        performSlam(player);
        break;

    case GoopSpray:
        performGoopSpray();
        break;
    }
}
void KingSlime::performSlam(Player* player)
{
    if(!player) return;

    // TODO CombatCalculator(Ana)
    // CombatCalculator::dealDamage(this, player, 35);
}
void KingSlime::performGoopSpray()
{
    // TODO CombatDeck
    // Add 3 SLIME status cards to the player's Discard Pile.
    // combatDeck->addToDiscardPile(new Slime());
    // combatDeck->addToDiscardPile(new Slime());
    // combatDeck->addToDiscardPile(new Slime());
}
void KingSlime::performSplit()
{
    m_splitReady = false;
    m_splitExecuted = true;
    m_splitRequested = true;
    // CombatManager should check isSplitRequested() right after this executes
    // and replace King Slime with 2 Large Slime instances,
    // each initialized with King Slime's current HP
    // (same integration pattern as LargeSlime::isSplitRequested()).
}
bool KingSlime::isSplitRequested() const
{
    return m_splitRequested;
}



