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


class OminousForge : public Event
{
public:
    OminousForge();
    void chooseOption(Player* player, int optionIndex) override;

private:
    // [Forge]: Upgrade a card from MasterDeck
    void handleForge(Player* player);

    // [Rummage]: Gain WarpedTongs relic + receive a Curse card
    void handleRummage(Player* player);
};


class GoldenIdol : public Event
{
public:

    GoldenIdol();

    void chooseOption(Player* player, int optionIndex) override;

private:

    void handleSteal(Player* player);
};


class Augmenter : public Event
{
public:
    Augmenter();
    void chooseOption(Player* player, int optionIndex) override;

private:

    void handleTestJAX(Player* player);

    void handleBecomeTestSubject(Player* player);

    void handleIngestMutagens(Player* player);
};


class FaceTrader : public Event
{
public:
    FaceTrader();
    void chooseOption(Player* player, int optionIndex) override;

private:

    void handleTouch(Player* player);

    void handleTrade(Player* player);
};


#endif  // EVENT_H