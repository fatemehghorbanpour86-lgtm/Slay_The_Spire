#include "eventsystem.h"

#include "event.h"
#include "player.h"

#include <QRandomGenerator>

EventSystem::EventSystem()
{
}

EventSystem::~EventSystem()
{
    qDeleteAll(events);
    events.clear();
}

//////////////////////////////////////////////////////////
// Registration
//////////////////////////////////////////////////////////

void EventSystem::registerEvent(Event* event)
{
    if (event == nullptr)
        return;

    events.append(event);
}

//////////////////////////////////////////////////////////
// Random Selection
//////////////////////////////////////////////////////////

Event* EventSystem::getRandomEvent() const
{
    if (events.isEmpty())
        return nullptr;

    int index = QRandomGenerator::global()->bounded(events.size());

    return events[index];
}

//////////////////////////////////////////////////////////
// Execution
//////////////////////////////////////////////////////////

void EventSystem::startRandomEvent(Player* player)
{
    if (player == nullptr)
        return;

    Event* event = getRandomEvent();

    if (event == nullptr)
        return;

    event->start(player);
}

//////////////////////////////////////////////////////////
// Utility
//////////////////////////////////////////////////////////

const QVector<Event*>& EventSystem::getEvents() const
{
    return events;
}