#ifndef EVENTRELICS_H
#define EVENTRELICS_H

#include "relic.h"

class WarpedTongs : public Relic
{
public:
    WarpedTongs();

    void onCombatStart(Player* player) override;
};

#endif // EVENTRELICS_H
