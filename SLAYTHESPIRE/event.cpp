#include "event.h"

#include "player.h"

Event::Event(const QString& name, const QString& description)
    : name(name), description(description)
{
}

// Event Flow

void Event::start(Player* player)
{
    Q_UNUSED(player)

    // UI should display:
    //   - Event name
    //   - Description
    //   - Available options
}

// Getters

const QString& Event::getName() const
{
    return name;
}

const QString& Event::getDescription() const
{
    return description;
}

const QVector<EventOption>& Event::getOptions() const
{
    return options;
}