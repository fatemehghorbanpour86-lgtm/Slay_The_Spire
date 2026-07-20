#ifndef EVENTMANAGER_H
#define EVENTMANAGER_H

class Event;

class EventManager
{
public:
    EventManager();
    ~EventManager();

    EventManager(const EventManager&) = delete;
    EventManager& operator=(const EventManager&) = delete;

    // Builds and returns a brand-new Event appropriate for the given Act.
    // Ownership transfers to the caller.
    Event* selectEvent(int act) const;

    // True if 'event' is the Colosseum special-case Event.
    // Colosseum never gets an EventPage: GameManager should instead
    // start Combat directly, exactly like an Elite Node (per doc).
    static bool isColosseum(Event* event);

private:
    static Event* createSharedEvent(int index);
    static Event* createAct1OnlyEvent(int index);
    static Event* createAct2OnlyEvent(int index);
};

#endif // EVENTMANAGER_H
