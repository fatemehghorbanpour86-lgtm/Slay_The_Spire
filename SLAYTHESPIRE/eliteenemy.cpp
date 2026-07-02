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

void GremlinNob::chooseIntent()
{

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