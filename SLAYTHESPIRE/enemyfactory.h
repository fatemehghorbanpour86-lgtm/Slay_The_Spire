#ifndef ENEMYFACTORY_H
#define ENEMYFACTORY_H

#include <QVector>

class Enemy;

class EnemyFactory
{
public:

    enum class EncounterId
    {
        // ---- Act 1: first two encounters (no repeats) ----
        Act1_Cultist,
        Act1_JawWorm,
        Act1_TwoLouse,
        Act1_TwoSmallSlime,

        // ---- Act 1: later encounters ----
        Act1_BlueSlaver,
        Act1_RedSlaver,
        Act1_ThreeLouse,
        Act1_LargeSlime,

        // ---- Act 2: first two encounters (no repeats) ----
        Act2_SphericGuardian,
        Act2_ThreeCultist,
        Act2_LooterMugger,
        Act2_SentryPlusGuardian,

        // ---- Act 2: later encounters ----
        Act2_ThreeLargeSlime,
        Act2_GremlinKnob,

        // ---- Elites ----
        Elite_GremlinNob,
        Elite_ThreeSentries,
        Elite_BookOfStabbing,
        Elite_Taskmaster,

        // ---- Bosses ----
        Boss_KingSlime,
        Boss_HexaGhost,
        Boss_TheChamp
    };

    // Builds fresh Enemy instances (caller/CombatManager takes ownership).
    static QVector<Enemy*> createEncounter(EncounterId id);

    static QVector<EncounterId> act1FirstPool();
    static QVector<EncounterId> act1LaterPool();
    static QVector<EncounterId> act2FirstPool();
    static QVector<EncounterId> act2LaterPool();
    static QVector<EncounterId> elitePool();

    // Act 1 has two possible bosses (King Slime / HexaGhost) per the doc;
    // Act 2 always uses The Champ.
    static EncounterId randomBossForAct(int act);
};

#endif // ENEMYFACTORY_H
