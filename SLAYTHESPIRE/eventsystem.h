#ifndef EVENTSYSTEM_H
#define EVENTSYSTEM_H

#include <QVector>

class Player;
class Event;

class EventSystem
{
private:
    QVector<Event*> events;

public:
    EventSystem();
    ~EventSystem();

    EventSystem(const EventSystem&) = delete;
    EventSystem& operator=(const EventSystem&) = delete;

    //----------------------------------
    // Registration
    //----------------------------------

    void registerEvent(Event* event);

    //----------------------------------
    // Event Selection
    //----------------------------------

    Event* getRandomEvent() const;

    //----------------------------------
    // Execution
    //----------------------------------

    void startRandomEvent(Player* player);

    //----------------------------------
    // Utility
    //----------------------------------

    const QVector<Event*>& getEvents() const;
};

#endif