#ifndef EVENTPAGE_H
#define EVENTPAGE_H

#include <QWidget>
#include <QLabel>
#include <QVBoxLayout>
#include <QPushButton>
#include <QStyleOption>
#include <QPainter>
#include <QDialog>
#include <QScrollArea>

class Player;
class Map;
class Event;
class TopBarWidget;
class Card;

class RemoveCardDialog : public QDialog
{
    Q_OBJECT
public:
    explicit RemoveCardDialog(Player* playerPtr, QWidget* parent = nullptr);

private:
    void setupUI();
    void populateCards();

    Player* player;
    QScrollArea* scrollArea;
    QWidget* gridContainer;
    QGridLayout* gridLayout;

    const int CARD_WIDTH = 162;
    const int CARD_HEIGHT = 214;
    const int COLUMNS = 4;
};




class TransformCardsDialog : public QDialog
{
    Q_OBJECT
public:
    explicit TransformCardsDialog(Player* playerPtr, QWidget* parent = nullptr);

private:
    void setupUI();
    void populateCards();

    Player* player;
    Card* firstSelectedCard;

    QScrollArea* scrollArea;
    QWidget* gridContainer;
    QGridLayout* gridLayout;

    const int CARD_WIDTH = 162;
    const int CARD_HEIGHT = 214;
    const int COLUMNS = 4;
};




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
