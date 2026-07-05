#ifndef RELICSYSTEM_H
#define RELICSYSTEM_H

#include <QVector>
#include <QString>
#include "relic.h"
#include "normalrelics.h"

class Card;
class Enemy;

struct RelicSaveData
{
    RelicId id = RelicId::BurningBlood;
    QString name;
    Relic::Tier tier = Relic::Tier::Starter;
    int counter = -1;
};


class RelicSystem
{
public:
    RelicSystem() = default;
    ~RelicSystem();

    // Non-copyable: RelicSystem owns raw Relic* pointers, so copying
    // the container would cause double deletion
    RelicSystem(const RelicSystem&) = delete;
    RelicSystem& operator=(const RelicSystem&) = delete;

    void addRelic(Relic* relic, Player* player);
    // Stores the relic AND immediately calls its onEquip(player).
    // Every relic activates exactly once, at the moment it is
    // obtained - regardless of source (Boss reward, Shop, Event,
    // Chest...).

    bool removeRelic(Relic* relic);
    // Removes and deletes the given relic instance.
    // Returns false if the relic was not found.

    bool hasRelic(RelicId id) const;
    Relic* getRelic(RelicId id) const;

    const QVector<Relic*>& getAllRelics() const;

    void clear();

    void onCombatStart(Player* player);
    void onCombatEnd(Player* player);
    void onTurnStart(Player* player);
    void onTurnEnd(Player* player);
    void onCardPlayed(Player* player, Card* card);
    void onEnemyDeath(Player* player, Enemy* enemy);
    void onChestOpened(Player* player);

    int modifyDamageTaken(int damage) const;
    int modifyDamageDealt(int damage) const;

    bool canRest() const;
    bool canUsePotions() const;
    bool canPlayCard(Player* player) const;

    QVector<RelicSaveData> extractState() const;

    static void grantRandomRelics(Player* player, Relic::Tier tier, int count);

private:
    QVector<Relic*> relics;
};


#endif // RELICSYSTEM_H
