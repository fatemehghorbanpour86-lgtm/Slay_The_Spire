#ifndef BOSSENEMIES_H
#define BOSSENEMIES_H

#include "enemy.h"

class KingSlime : public Enemy
{
private:
    enum Move
    {
        Preparing,
        Slam,
        GoopSpray
    };

    bool m_splitReady;
    bool m_splitExecuted;
    bool m_splitRequested;

public:
    KingSlime();
    void chooseIntent(Player* player) override;
    void executeMove(Player* player) override;
    bool isSplitRequested() const;

private:
    void performSlam(Player* player);
    void performGoopSpray();
    void performSplit();
};

class HexaGhost : public Enemy
{
private:
    enum Move
    {
        Activate,
        Divider,
        Sear,
        Tackle,
        Inflame,
        Inferno
    };

    int cyclePosition;
    bool dividerUsed;

public:
    HexaGhost();
    void chooseIntent(Player* player) override;
    void executeMove(Player* player) override;

private:
    void performDivider(Player* player);
    void performSear(Player* player);
    void performTackle(Player* player);
    void performInflame();
    void performInferno(Player* player);
};

class TheChamp : public Enemy
{
private:
    enum Move
    {
        Taunt,
        DefensiveStance,
        Gloat,
        FaceSlap,
        HeavySlash
    };

    bool isTauntTurn;
    // Toggles every turn. True on turns 0, 2, 4... (every other turn is Taunt).

public:
    TheChamp();
    void chooseIntent(Player* player) override;
    void executeMove(Player* player) override;

private:
    void performTaunt(Player* player);
    void performDefensiveStance();
    void performGloat();
    void performFaceSlap(Player* player);
    void performHeavySlash(Player* player);
};

#endif // BOSSENEMIES_H
