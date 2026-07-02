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
    void chooseIntent() override;
    void executeMove(Player* player) override;
    bool isSplitRequested() const;

private:
    void performSlam(Player* player);
    void performGoopSpray();
    void performSplit();
};

#endif // BOSSENEMIES_H
