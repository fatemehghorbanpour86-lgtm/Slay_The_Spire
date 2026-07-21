#ifndef EVENT_H
#define EVENT_H

#include <QString>
#include <QVector>
#include <QObject>
#include <QWidget>

class Player;
class Enemy;

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

    virtual bool chooseOption(Player* player, int optionIndex, QWidget* parentWidget = nullptr) = 0;

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
    bool chooseOption(Player* player, int optionIndex, QWidget* parentWidget = nullptr) override;

private:
    // [Forge]: Upgrade a card from MasterDeck
    bool handleForge(Player* player, QWidget* parentWidget);

    // [Rummage]: Gain WarpedTongs relic + receive a Curse card
    void handleRummage(Player* player);
};


class GoldenIdol : public Event
{
public:

    GoldenIdol();

    bool chooseOption(Player* player, int optionIndex, QWidget* parentWidget = nullptr) override;

private:

    void handleSteal(Player* player);
};


class Augmenter : public Event
{
public:
    Augmenter();
    bool chooseOption(Player* player, int optionIndex, QWidget* parentWidget = nullptr) override;

private:

    void handleTestJAX(Player* player);

    bool handleBecomeTestSubject(Player* player, QWidget* parentWidget);

    void handleIngestMutagens(Player* player);
};


class FaceTrader : public Event
{
public:
    FaceTrader();
    bool chooseOption(Player* player, int optionIndex, QWidget* parentWidget = nullptr) override;

private:

    void handleTouch(Player* player);

    void handleTrade(Player* player);
};

class TheCleric : public Event
{
public:
    TheCleric();
    bool chooseOption(Player* player, int optionIndex, QWidget* parentWidget = nullptr) override;

private:

    void handleHeal(Player* player);

    bool handlePurify(Player* player, QWidget* parentWidget);
};



class GoldenShrine : public Event
{
public:
    GoldenShrine();
    bool chooseOption(Player* player, int optionIndex, QWidget* parentWidget = nullptr) override;

private:

    void handlePray(Player* player);
};


class TheJoust : public Event
{
public:
    TheJoust();
    bool chooseOption(Player* player, int optionIndex, QWidget* parentWidget = nullptr) override;

private:

    void handleMurderer(Player* player);

    void handleOwner(Player* player);
};


class PleadingVagrant : public Event
{
public:
    PleadingVagrant();
    bool chooseOption(Player* player, int optionIndex, QWidget* parentWidget = nullptr) override;

private:

    void handleGiveGold(Player* player);

    void handleRob(Player* player);

    void grantRandomNormalRelic(Player* player);
};


class Colosseum : public QObject, public Event
{
    Q_OBJECT

public:
    explicit Colosseum(QObject* parent = nullptr);
    bool chooseOption(Player* player, int optionIndex, QWidget* parentWidget = nullptr) override;

signals:

    // TODO (GameManager): Connect to this signal to create a CombatManager
    // with the given enemies and call startCombat().
    // After battleWon signal from CombatManager, call grantEliteReward(player).
    void requestEliteCombat(Player* player, QVector<Enemy*> enemies);

private:

    // [Fight]: Trigger a random Elite encounter
    void handleFight(Player* player);

    // Grants the standard Elite reward after combat is won.
    // TODO (GameManager): Call this after battleWon signal is received.
    void grantEliteReward(Player* player);

    // Builds a random Elite encounter from the available Elite enemies.
    QVector<Enemy*> buildRandomEliteEncounter() const;
};

#endif  // EVENT_H