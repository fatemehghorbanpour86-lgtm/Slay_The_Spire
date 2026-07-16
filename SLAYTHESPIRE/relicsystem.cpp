#include "relicsystem.h"
#include <utility>
#include <QRandomGenerator>
#include "normalrelics.h"
#include "bossrelics.h"
#include "eventrelics.h"
#include "player.h"

RelicSystem::RelicSystem(QObject *parent) : QObject(parent)
{

}

RelicSystem::~RelicSystem()
{
    clear();
}

void RelicSystem::addRelic(Relic* relic, Player* player)
{
    if (relic == nullptr)
        return;

    relics.append(relic);
    relic->onEquip(player);
}

bool RelicSystem::removeRelic(Relic* relic)
{
    int index = relics.indexOf(relic);

    if (index == -1)
        return false;

    delete relics[index];
    relics.removeAt(index);

    return true;
}

bool RelicSystem::hasRelic(RelicId id) const
{
    return getRelic(id) != nullptr;
}

Relic* RelicSystem::getRelic(RelicId id) const
{
    for (Relic* relic : relics)
    {
        if (relic != nullptr && relic->getId() == id)
            return relic;
    }

    return nullptr;
}

const QVector<Relic*>& RelicSystem::getAllRelics() const
{
    return relics;
}

void RelicSystem::clear()
{
    qDeleteAll(relics);
    relics.clear();
}

void RelicSystem::onCombatStart(Player* player)
{
    for (Relic* relic : std::as_const(relics))
        relic->onCombatStart(player);
}

void RelicSystem::onCombatEnd(Player* player)
{
    for (Relic* relic : std::as_const(relics))
        relic->onCombatEnd(player);
}

void RelicSystem::onTurnStart(Player* player)
{
    for (Relic* relic : std::as_const(relics))
        relic->onTurnStart(player);
}

void RelicSystem::onTurnEnd(Player* player)
{
    for (Relic* relic : std::as_const(relics))
        relic->onTurnEnd(player);
}

void RelicSystem::onCardPlayed(Player* player, Card* card)
{
    for (Relic* relic : std::as_const(relics))
        relic->onCardPlayed(player, card);
}

void RelicSystem::onEnemyDeath(Player* player, Enemy* enemy)
{
    for (Relic* relic : std::as_const(relics))
        relic->onEnemyDeath(player, enemy);
}

void RelicSystem::onChestOpened(Player* player)
{
    for (Relic* relic : std::as_const(relics))
        relic->onChestOpened(player);
}

int RelicSystem::modifyDamageTaken(int damage) const
{
    for (Relic* relic : std::as_const(relics))
        damage = relic->modifyDamageTaken(damage);

    return damage;
}

int RelicSystem::modifyDamageDealt(int damage) const
{
    for (Relic* relic : std::as_const(relics))
        damage = relic->modifyDamageDealt(damage);

    return damage;
}

bool RelicSystem::canRest() const
{
    for (Relic* relic : std::as_const(relics))
    {
        if (relic != nullptr && !relic->canRest())
            return false;
    }

    return true;
}

bool RelicSystem::canLift() const
{
    for (Relic* relic : std::as_const(relics))
    {
        if (relic != nullptr && relic->canLift())
            return true;
    }
    return false;
}

bool RelicSystem::lift(Player* player)
{
    for (Relic* relic : std::as_const(relics))
    {
        if (relic != nullptr && relic->canLift())
            return relic->lift(player);
    }
    return false;
}

bool RelicSystem::canUsePotions() const
{
    for (Relic* relic : std::as_const(relics))
    {
        if (!relic->canUsePotions())
            return false;
    }

    return true;
}

bool RelicSystem::canPlayCard(Player* player) const
{
    for (Relic* relic : std::as_const(relics))
    {
        if (!relic->canPlayCard(player))
            return false;
    }

    return true;
}

QVector<RelicSaveData> RelicSystem::extractState() const
{
    QVector<RelicSaveData> state;

    for (Relic* relic : std::as_const(relics))
    {
        RelicSaveData data;
        data.id = relic->getId();
        data.name = relic->getName();
        data.tier = relic->getTier();
        data.counter = relic->getCounter();

        state.append(data);
    }

    return state;
}

void RelicSystem::grantRandomRelics(Player* player, Relic::Tier tier, int count)
{
    Q_UNUSED(tier)

    if (!player || count <= 0)
        return;

    QVector<Relic*> pool;
    pool.append(new Girya());
    pool.append(new IceCream());
    pool.append(new Shuriken());
    pool.append(new Kunai());
    pool.append(new Anchor());
    pool.append(new HappyFlower());
    pool.append(new Orichalcum());
    pool.append(new Vajra());

    // Remove anything the player already owns - they cannot receive
    // a duplicate relic.
    for (int i = pool.size() - 1; i >= 0; --i)
    {
        if (player->hasRelic(pool[i]->getId()))
        {
            delete pool[i];
            pool.removeAt(i);
        }
    }

    // Shuffle
    for (int i = pool.size() - 1; i > 0; --i)
    {
        int j = QRandomGenerator::global()->bounded(i + 1);
        pool.swapItemsAt(i, j);
    }

    int grantCount = qMin(count, pool.size());

    for (int i = 0; i < grantCount; ++i)
    {
        player->addRelic(pool[i]); // ownership transfers to RelicSystem
    }

    // Anything left over in the pool was not selected - free it.
    for (int i = grantCount; i < pool.size(); ++i)
    {
        delete pool[i];
    }
}

Relic* RelicSystem::createRelicById(RelicId id)
{
    switch (id)
    {
    //starter
    case RelicId::BurningBlood:      return new BurningBlood();

    //normal
    case RelicId::Girya:             return new Girya();
    case RelicId::IceCream:          return new IceCream();
    case RelicId::Shuriken:          return new Shuriken();
    case RelicId::Kunai:             return new Kunai();
    case RelicId::Anchor:            return new Anchor();
    case RelicId::HappyFlower:       return new HappyFlower();
    case RelicId::Orichalcum:        return new Orichalcum();
    case RelicId::Vajra:             return new Vajra();

    //boss
    case RelicId::CallingBell:       return new CallingBell();
    case RelicId::MarkOfPain:        return new MarkOfPain();
    case RelicId::VelvetChoker:      return new VelvetChoker();
    case RelicId::BlackStar:         return new BlackStar();
    case RelicId::CoffeeDripper:     return new CoffeeDripper();

    //event
    case RelicId::WarpedTongs:       return new WarpedTongs();
    case RelicId::MutagenicStrength: return new MutagenicStrength();
    case RelicId::CultistHeadpiece:  return new CultistHeadpiece();
    case RelicId::GremlinVisage:     return new GremlinVisage();
    case RelicId::FaceOfCleric:      return new FaceOfCleric();
    case RelicId::GoldenIdolRelic:   return new GoldenIdolRelic();
    }

    return nullptr;
}

void RelicSystem::restoreState(const QVector<RelicSaveData>& data, Player* player)
{
    clear();

    for (const RelicSaveData& relicData : data)
    {
        Relic* relic = createRelicById(relicData.id);

        if (!relic)
            continue;

        relic->setCounter(relicData.counter);
        addRelic(relic, player);
    }
}

QVector<Relic*> RelicSystem::buildRelicPool(Relic::Tier tier)
{
    QVector<Relic*> pool;

    if (tier == Relic::Tier::Normal)
    {
        pool.append(new Girya());
        pool.append(new IceCream());
        pool.append(new Shuriken());
        pool.append(new Kunai());
        pool.append(new Anchor());
        pool.append(new HappyFlower());
        pool.append(new Orichalcum());
        pool.append(new Vajra());
    }
    else if (tier == Relic::Tier::Boss)
    {
        pool.append(new CallingBell());
        pool.append(new MarkOfPain());
        pool.append(new VelvetChoker());
        pool.append(new BlackStar());
        pool.append(new CoffeeDripper());
    }

    return pool;
}

Relic* RelicSystem::createRandomRelic(Player* player, Relic::Tier tier, const QVector<RelicId>& exclude)
{
    QVector<Relic*> pool = buildRelicPool(tier);

    for (int i = pool.size() - 1; i >= 0; --i)
    {
        bool owned = player && player->hasRelic(pool[i]->getId());
        bool excluded = exclude.contains(pool[i]->getId());

        if (owned || excluded)
        {
            delete pool[i];
            pool.removeAt(i);
        }
    }

    if (pool.isEmpty())
        return nullptr;

    int index = QRandomGenerator::global()->bounded(pool.size());
    Relic* chosen = pool[index];
    pool.removeAt(index);

    qDeleteAll(pool);

    return chosen;
}

QVector<Relic*> RelicSystem::createRandomRelics(Player* player, Relic::Tier tier, int count)
{
    QVector<Relic*> result;
    QVector<RelicId> excluded;

    for (int i = 0; i < count; ++i)
    {
        Relic* relic = createRandomRelic(player, tier, excluded);

        if (relic == nullptr)
            break; // pool for this tier is exhausted

        excluded.append(relic->getId());
        result.append(relic);
    }

    return result;
}