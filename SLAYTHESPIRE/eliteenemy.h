#ifndef ELITEENEMY_H
#define ELITEENEMY_H

#include "enemy.h"

class GremlinNob : public Enemy
{
private:

    enum Move
    {
        Bellow,
        Rush,
        SkullBash
    };

    bool enrageActive;

public:

    GremlinNob();

    bool isEnraged() const;

    void chooseIntent() override;

    void executeMove(Player* player) override;
};

#endif // ELITEENEMY_H
