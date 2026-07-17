#include "enemyfactory.h"
#include "normalenemies.h"
#include "eliteenemy.h"
#include "bossenemies.h"

#include <QRandomGenerator>

QVector<Enemy*> EnemyFactory::createEncounter(EncounterId id)
{
    QVector<Enemy*> enemies;

    switch (id)
    {
    // ---- Act 1: first two ----
    case EncounterId::Act1_Cultist:
        enemies.append(new Cultist());
        break;
    case EncounterId::Act1_JawWorm:
        enemies.append(new JawWorm());
        break;
    case EncounterId::Act1_TwoLouse:
        enemies.append(new Louse());
        enemies.append(new Louse());
        break;
    case EncounterId::Act1_TwoSmallSlime:
        enemies.append(new SmallSlime());
        enemies.append(new SmallSlime());
        break;

    // ---- Act 1: later ----
    case EncounterId::Act1_BlueSlaver:
        enemies.append(new BlueSlaver());
        break;
    case EncounterId::Act1_RedSlaver:
        enemies.append(new RedSlaver());
        break;
    case EncounterId::Act1_ThreeLouse:
        enemies.append(new Louse());
        enemies.append(new Louse());
        enemies.append(new Louse());
        break;
    case EncounterId::Act1_LargeSlime:
        enemies.append(new LargeSlime());
        break;

    // ---- Act 2: first two ----
    case EncounterId::Act2_SphericGuardian:
        enemies.append(new SphericGuardian());
        break;
    case EncounterId::Act2_ThreeCultist:
        enemies.append(new Cultist());
        enemies.append(new Cultist());
        enemies.append(new Cultist());
        break;
    case EncounterId::Act2_LooterMugger:
        enemies.append(new Thief(Thief::Looter));
        enemies.append(new Thief(Thief::Mugger));
        break;
    case EncounterId::Act2_SentryPlusGuardian:
        enemies.append(new Sentry(true));
        enemies.append(new SphericGuardian());
        break;

    // ---- Act 2: later ----
    case EncounterId::Act2_ThreeLargeSlime:
        enemies.append(new LargeSlime());
        enemies.append(new LargeSlime());
        enemies.append(new LargeSlime());
        break;
    case EncounterId::Act2_GremlinKnob:
        enemies.append(new GremlinNob());
        break;

    // ---- Elites ----
    case EncounterId::Elite_GremlinNob:
        enemies.append(new GremlinNob());
        break;
    case EncounterId::Elite_ThreeSentries:
        enemies.append(new Sentry(false));
        enemies.append(new Sentry(true));
        enemies.append(new Sentry(false));
        break;
    case EncounterId::Elite_BookOfStabbing:
        enemies.append(new BookOfStabbing());
        break;
    case EncounterId::Elite_Taskmaster:
        // Taskmaster always comes with 1 Blue Slaver + 1 Red Slaver (see eliteenemy.cpp comment).
        enemies.append(new Taskmaster());
        enemies.append(new BlueSlaver());
        enemies.append(new RedSlaver());
        break;

    // ---- Bosses ----
    case EncounterId::Boss_KingSlime:
        enemies.append(new KingSlime());
        break;
    case EncounterId::Boss_HexaGhost:
        enemies.append(new HexaGhost());
        break;
    case EncounterId::Boss_TheChamp:
        enemies.append(new TheChamp());
        break;
    }

    return enemies;
}

QVector<EnemyFactory::EncounterId> EnemyFactory::act1FirstPool()
{
    return {
        EncounterId::Act1_Cultist,
        EncounterId::Act1_JawWorm,
        EncounterId::Act1_TwoLouse,
        EncounterId::Act1_TwoSmallSlime
    };
}

QVector<EnemyFactory::EncounterId> EnemyFactory::act1LaterPool()
{
    return {
        EncounterId::Act1_BlueSlaver,
        EncounterId::Act1_RedSlaver,
        EncounterId::Act1_ThreeLouse,
        EncounterId::Act1_LargeSlime
    };
}

QVector<EnemyFactory::EncounterId> EnemyFactory::act2FirstPool()
{
    return {
        EncounterId::Act2_SphericGuardian,
        EncounterId::Act2_ThreeCultist,
        EncounterId::Act2_LooterMugger,
        EncounterId::Act2_SentryPlusGuardian
    };
}

QVector<EnemyFactory::EncounterId> EnemyFactory::act2LaterPool()
{
    return {
        EncounterId::Act2_ThreeLargeSlime,
        EncounterId::Act2_GremlinKnob
    };
}

QVector<EnemyFactory::EncounterId> EnemyFactory::elitePool()
{
    return {
        EncounterId::Elite_GremlinNob,
        EncounterId::Elite_ThreeSentries,
        EncounterId::Elite_BookOfStabbing,
        EncounterId::Elite_Taskmaster
    };
}

EnemyFactory::EncounterId EnemyFactory::randomBossForAct(int act)
{
    if (act <= 1)
    {
        QVector<EncounterId> pool = { EncounterId::Boss_KingSlime, EncounterId::Boss_HexaGhost };
        return pool[QRandomGenerator::global()->bounded(pool.size())];
    }

    return EncounterId::Boss_TheChamp;
}