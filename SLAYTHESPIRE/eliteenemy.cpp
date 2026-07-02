#include "eliteenemy.h"

#include "player.h"

#include <QRandomGenerator>


GremlinNob::GremlinNob()
    : Enemy("Gremlin Knob", QRandomGenerator::global()->bounded(82,87))
{
    enrageActive = false;
}

bool GremlinNob::isEnraged() const
{
    return enrageActive;
}

void GremlinNob::chooseIntent(Player* player)
{
    Q_UNUSED(player)

    if(getTurnCount() == 0)
    {
        setCurrentMove(Bellow);

        setIntent(Intent::Buff);

        setIntentDamage(0);

        setIntentHits(1);

        return;
    }

    int random = QRandomGenerator::global()->bounded(100);

    if(random < 67)
    {
        setCurrentMove(Rush);

        setIntent(Intent::Attack);

        setIntentDamage(14);

        setIntentHits(1);
    }

    else
    {
        setCurrentMove(SkullBash);

        setIntent(Intent::AttackDebuff);

        setIntentDamage(6);

        setIntentHits(1);
    }
}

void GremlinNob::executeMove(Player *player)
{
    if(player == nullptr)
    {
        return;
    }

    switch(getCurrentMove())
    {

     case Bellow:

        enrageActive = true;

        // Gremlin Nob enters Enrage state.
        // Whenever the player plays a Skill card,
        // CombatManager should grant:
        // addEffect(Strength, Buff, 2)

        break;

     case Rush:

        // TODO CombatCalculator(Ana)
        // CombatCalculator::dealDamage(this, player, 14);

        break;

    case SkullBash:

        // TODO CombatCalculator
        // CombatCalculator::dealDamage(this, player, 6);

        player->addEffect(
            Effect::Type::Vulnerable,
            Effect::Category::Debuff,
            1,2);

        break;
    }
}

//======================================================
//  Sentry
//======================================================

Sentry::Sentry(bool startsWithBeam)
    : Enemy("Sentry", QRandomGenerator::global()->bounded(38,43))
{
    this->startsWithBeam = startsWithBeam;
}

void Sentry::chooseIntent(Player* player)
{
    Q_UNUSED(player)

    // First turn depends on which Sentry this is.
    if(getTurnCount() == 0)
    {
        if(startsWithBeam)
        {
            setCurrentMove(Beam);

            setIntent(Intent::Attack);

            setIntentDamage(9);

            setIntentHits(1);
        }
        else
        {
            setCurrentMove(Bolt);

            setIntent(Intent::Debuff);

            setIntentDamage(0);

            setIntentHits(1);
        }

        return;
    }


    if(getCurrentMove() == Beam)
    {
        setCurrentMove(Bolt);

        setIntent(Intent::Debuff);

        setIntentDamage(0);

        setIntentHits(1);
    }
    else
    {
        setCurrentMove(Beam);

        setIntent(Intent::Attack);

        setIntentDamage(9);

        setIntentHits(1);
    }
}

void Sentry::executeMove(Player* player)
{
    if(player == nullptr)
    {
        return;
    }

    switch(getCurrentMove())
    {
    case Beam:

        // TODO CombatCalculator(Ana)
        // CombatCalculator::dealDamage(this, player, 9);

        break;

    case Bolt:

        // TODO CombatDeck
        // Shuffle 2 Dazed status cards into the player's Discard Pile.
        // combatDeck->addToDiscardPile(new Dazed());
        // combatDeck->addToDiscardPile(new Dazed());

        break;
    }
}



BookOfStabbing::BookOfStabbing()
    : Enemy("Book of Stabbing", QRandomGenerator::global()->bounded(160,163))
{
    multiStabCount = 0;
}
void BookOfStabbing::chooseIntent(Player* player)
{
    Q_UNUSED(player)

    int random = QRandomGenerator::global()->bounded(1,101);

    if(random <= 85)
    {
        int hits = multiStabCount + 2;

        setIntent(Intent::Attack);
        setCurrentMove(MultiStab);
        setIntentDamage(6);
        setIntentHits(hits);
    }
    else
    {
        setIntent(Intent::Attack);
        setCurrentMove(SingleStab);
        setIntentDamage(21);
        setIntentHits(1);
    }
}
void BookOfStabbing::executeMove(Player *player)
{
    if(player == nullptr)
    {
        return;
    }

    switch(getCurrentMove())
    {
    case MultiStab:
    {
        performMultiStab(player);
        break;
    }
    case SingleStab:
        performSingleStab(player);
        break;
    }
}
void BookOfStabbing::performMultiStab(Player *player)
{
    int hits = getIntentHits();
    for(int i = 0; i < hits; i++)
    {
        // TODO CombatCalculator(Ana)
        // CombatCalculator::dealDamage(this, player, 6);
    }
    multiStabCount++;
}
void BookOfStabbing::performSingleStab(Player *player)
{
    // TODO CombatCalculator(Ana)
    // CombatCalculator::dealDamage(this, player, 21);
}