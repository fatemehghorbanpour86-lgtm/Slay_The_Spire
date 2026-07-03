#include "relic.h"

Relic::Relic(const QString& name, const QString& description, Tier tier)
    : name(name), description(description), tier(tier), counter(-1)
{
}

QString Relic::getName() const
{
    return name;
}
QString Relic::getDescription() const
{
    return description;
}
Relic::Tier Relic::getTier() const
{
    return tier;
}

int Relic::getCounter() const
{
    return counter;
}
void Relic::setCounter(int value)
{
    counter = value;
}

void Relic::onCombatStart(Player* player)
{
    Q_UNUSED(player)
}
void Relic::onCombatEnd(Player* player)
{
    Q_UNUSED(player)
}
void Relic::onTurnStart(Player* player)
{
    Q_UNUSED(player)
}
void Relic::onTurnEnd(Player* player)
{
    Q_UNUSED(player)
}

void Relic::onCardPlayed(Player* player, Card* card)
{
    Q_UNUSED(player)
    Q_UNUSED(card)
}

void Relic::onEnemyDeath(Player* player, Enemy* enemy)
{
    Q_UNUSED(player)
    Q_UNUSED(enemy)
}

int Relic::modifyDamageTaken(int damage)
{
    return damage;
}
int Relic::modifyDamageDealt(int damage)
{
    return damage;
}

bool Relic::canRest() const
{
    return true;
}
bool Relic::canUsePotions() const
{
    return true;
}

void Relic::onChestOpened(Player* player)
{
    Q_UNUSED(player)
}

void Relic::onEquip(Player* player)
{
    Q_UNUSED(player)
}