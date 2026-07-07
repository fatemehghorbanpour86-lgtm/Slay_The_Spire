#ifndef EVENT_H
#define EVENT_H

#include <QString>
#include <QVector>


class Player;

struct EventOption
{
    QString text;
    bool enabled = true;

    EventOption(const QString& text = "", bool enabled = true)
        : text(text), enabled(enabled)
    {
    }
};


class Event
{

protected:

    QString name;
    QString description;
    QVector<EventOption> options;

public:

    Event(const QString& name, const QString& description);

    virtual ~Event() = default;

    //----------------------------------
    // Event Flow
    //----------------------------------

    virtual void start(Player* player);

    virtual void chooseOption(Player* player, int optionIndex) = 0;

    //----------------------------------
    // Getters
    //----------------------------------

    const QString& getName() const;

    const QString& getDescription() const;

    const QVector<EventOption>& getOptions() const;
};

#endif  // EVENT_H