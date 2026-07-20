#include "eventmanager.h"
#include "event.h"

#include <QRandomGenerator>

EventManager::EventManager()
{
}

EventManager::~EventManager()
{
}

Event* EventManager::selectEvent(int act) const
{
    static const int SHARED_COUNT = 2; // Ominous Forge, Golden Shrine

    const int actOnlyCount = (act == 1) ? 2 : 5;

    const int total = SHARED_COUNT + actOnlyCount;
    const int roll = QRandomGenerator::global()->bounded(total);

    if (roll < SHARED_COUNT)
        return createSharedEvent(roll);

    const int actIndex = roll - SHARED_COUNT;

    if (act == 1)
        return createAct1OnlyEvent(actIndex);

    return createAct2OnlyEvent(actIndex);
}

Event* EventManager::createSharedEvent(int index)
{
    switch (index)
    {
    case 0: return new OminousForge();
    case 1: return new GoldenShrine();
    }

    return new OminousForge(); // Safe fallback, should not normally be reached.
}

Event* EventManager::createAct1OnlyEvent(int index)
{
    switch (index)
    {
    case 0: return new GoldenIdol();
    case 1: return new TheCleric();
    }

    return new GoldenIdol();
}

Event* EventManager::createAct2OnlyEvent(int index)
{
    switch (index)
    {
    case 0: return new PleadingVagrant();
    case 1: return new TheJoust();
    case 2: return new FaceTrader();
    case 3: return new Augmenter();
    case 4: return new Colosseum();
    }

    return new PleadingVagrant();
}

bool EventManager::isColosseum(Event* event)
{
    return dynamic_cast<Colosseum*>(event) != nullptr;
}
