#ifndef EVENTPAGE_H
#define EVENTPAGE_H

#include <QWidget>
#include <QLabel>
#include <QVBoxLayout>
#include <QPushButton>
#include <QStyleOption>
#include <QPainter>

class Player;
class Map;
class Event;
class TopBarWidget;


class EventPage : public QWidget
{
    Q_OBJECT

public:
    explicit EventPage(Player* player, Map* map, Event* event, QWidget* parent = nullptr);

signals:
    // Emitted once the player has picked an option and the Event's
    // effects have already been fully applied (TopBar already
    // refreshed). GameManager listens to this to close the Event,
    // autosave, and return to the Map.
    void eventResolved();

private slots:
    void onOptionClicked(int optionIndex);

private:
    Player* player;
    Map* map;
    Event* event; // not owned

    TopBarWidget* topBar = nullptr;
    QLabel* nameLabel = nullptr;
    QLabel* descriptionLabel = nullptr;
    QVBoxLayout* optionsLayout = nullptr;

    void setupUI();
    void populateOptions();

    QString eventBackgroundPath() const;

protected:
    void paintEvent(QPaintEvent*) override
    {
        QStyleOption opt;
        opt.initFrom(this);
        QPainter p(this);
        style()->drawPrimitive(QStyle::PE_Widget, &opt, &p, this);
    }
};


#endif // EVENTPAGE_H
