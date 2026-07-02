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
void KingSlime::chooseIntent(Player* player)
{
    Q_UNUSED(player)

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




HexaGhost::HexaGhost()
    : Enemy("HexaGhost", 250),
    cyclePosition(0),
    dividerUsed(false)
{
}
void HexaGhost::chooseIntent(Player* player)
{
    if(getTurnCount() == 0)
    {
        setIntent(Intent::Unknown);
        setCurrentMove(Activate);
        setIntentDamage(0);
        setIntentHits(0);
        return;
    }

    if(!dividerUsed)
    {
        int perHit = 0;

        if(player != nullptr)
        {
            perHit = (player->getCurrentHealth() / 12) + 1;
        }

        setIntent(Intent::Attack);
        setCurrentMove(Divider);
        setIntentDamage(perHit);
        setIntentHits(6);
        return;
    }

    switch(cyclePosition)
    {
    case 0:
        setCurrentMove(Sear);
        setIntent(Intent::AttackDebuff);
        setIntentDamage(6);
        setIntentHits(1);
        break;

    case 1:
        setCurrentMove(Tackle);
        setIntent(Intent::Attack);
        setIntentDamage(5);
        setIntentHits(2);
        break;

    case 2:
        setCurrentMove(Sear);
        setIntent(Intent::AttackDebuff);
        setIntentDamage(6);
        setIntentHits(1);
        break;

    case 3:
        setCurrentMove(Inflame);
        setIntent(Intent::DefendBuff);
        setIntentDamage(0);
        setIntentHits(0);
        break;

    case 4:
        setCurrentMove(Tackle);
        setIntent(Intent::Attack);
        setIntentDamage(5);
        setIntentHits(2);
        break;

    case 5:
        setCurrentMove(Sear);
        setIntent(Intent::AttackDebuff);
        setIntentDamage(6);
        setIntentHits(1);
        break;

    case 6:
        setCurrentMove(Inferno);
        setIntent(Intent::Attack);
        setIntentDamage(2);
        setIntentHits(6);
        break;
    }
}
void HexaGhost::executeMove(Player *player)
{
    if(player == nullptr)
    {
        return;
    }

    switch(getCurrentMove())
    {
    case Activate:
        // Does nothing.
        break;

    case Divider:
        performDivider(player);
        break;

    case Sear:
        performSear(player);
        break;

    case Tackle:
        performTackle(player);
        break;

    case Inflame:
        performInflame();
        break;

    case Inferno:
        performInferno(player);
        break;
    }

    if(dividerUsed)
    {
        cyclePosition = (cyclePosition + 1) % 7;
    }
}
void HexaGhost::performDivider(Player* player)
{
    if(!player) return;

    int perHit = (player->getCurrentHealth() / 12) + 1;
    // Recomputed here in case Player HP changed between chooseIntent() and
    // executeMove() (e.g. some other effect damaged/healed the player mid-turn).

    for(int i = 0; i < getIntentHits(); i++)
    {
        // TODO CombatCalculator(Ana)
        // CombatCalculator::dealDamage(this, player, perHit);
    }

    dividerUsed = true;
    cyclePosition = 0;
}
void HexaGhost::performSear(Player* player)
{
    if(!player) return;

    // TODO CombatCalculator(Ana)
    // CombatCalculator::dealDamage(this, player, 6);

    // TODO CombatDeck
    // combatDeck->addToDiscardPile(new Burn());
}
void HexaGhost::performTackle(Player* player)
{
    if(!player) return;

    for(int i = 0; i < getIntentHits(); i++)
    {
        // TODO CombatCalculator(Ana)
        // CombatCalculator::dealDamage(this, player, 5);
    }
}
void HexaGhost::performInflame()
{
    this->addEffect(Effect::Type::Strength, Effect::Category::Buff, 2, -1);
    this->addBlock(12);
}
void HexaGhost::performInferno(Player* player)
{
    if(!player) return;

    for(int i = 0; i < getIntentHits(); i++)
    {
        // TODO CombatCalculator(Ana)
        // CombatCalculator::dealDamage(this, player, 2);
    }

    // TODO CombatDeck
    // Add 3 BURN cards + upgrade all existing BURN cards to BURN+.
    // combatDeck->addToDiscardPile(new Burn());
    // combatDeck->addToDiscardPile(new Burn());
    // combatDeck->addToDiscardPile(new Burn());
    // combatDeck->upgradeAllOfType<Burn>();  // needs Deck-side support
}