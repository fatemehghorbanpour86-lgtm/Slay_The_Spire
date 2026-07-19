#ifndef CAMPFIREPAGE_H
#define CAMPFIREPAGE_H

#include <QWidget>
#include <QPushButton>
#include <QLabel>
#include <QProgressBar>
#include <QFrame>
#include <QStyleOption>
#include <QPainter>
#include "campfire.h"
#include "player.h"

class Map;

class TopBarWidget : public QWidget
{
    Q_OBJECT

public:
    explicit TopBarWidget(QWidget *parent = nullptr);
    void updateData(Player* player, Map* map = nullptr);

signals:
    void relicClicked();
    void deckClicked();
    void settingsClicked();

private:
    QProgressBar* hpBar;
    QLabel* goldLabel;
    QPushButton* relicBtn;
    QLabel* relicLabel;
    QFrame* potionsPanel;
    QLabel* potionSlots[3];
    QLabel* actLabel;
    QLabel* floorIconLabel;
    QLabel* floorNumberLabel;
    QPushButton* deckBtn;
    QLabel* cardLabel;
    QPushButton* settingsBtn;

    void setupUI();
};

class CampfirePage : public QWidget
{
    Q_OBJECT

public:
    explicit CampfirePage(Player* player, Map* map, QWidget* parent = nullptr);

signals:
    void leaveCampfire();
    void settingsRequested();

private slots:
    void onRestClicked();
    void onSmithClicked();
    void onLiftClicked();
    void onLeaveClicked();

    void onRelicButtonClicked();
    void onDeckButtonClicked();
    void onSettingsButtonClicked();

private:
    Player* player;
    Map* map;
    Campfire campfireLogic; // stateless helper, see campfire.h

    TopBarWidget* topBar;
    QLabel* fireLabel;
    QPushButton* restBtn;
    QPushButton* smithBtn;
    QPushButton* liftBtn;
    QPushButton* leaveBtn;

    void setupUI();
    void refreshButtonStates();

protected:
    void paintEvent(QPaintEvent *) override{
        QStyleOption opt;
        opt.initFrom(this);
        QPainter p(this);
        style()->drawPrimitive(QStyle::PE_Widget, &opt, &p, this);
    }
};

#endif // CAMPFIREPAGE_H
