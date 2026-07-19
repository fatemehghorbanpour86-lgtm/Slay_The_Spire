#ifndef TREASUREPAGE_H
#define TREASUREPAGE_H

#include <QWidget>
#include <QPushButton>
#include <QStyleOption>
#include <QPainter>
#include "treasure.h"

class Player;
class Map;
class TopBarWidget; // reused as-is from campfirepage.h - no new TopBar is created

class TreasurePage : public QWidget
{
    Q_OBJECT

public:
    explicit TreasurePage(Player* player, Map* map, QWidget* parent = nullptr);

signals:
    void treasureFinished();
    void settingsRequested();

private slots:
    void onChestClicked();
    void onRewardClicked();
    void onBackClicked();

    void onRelicButtonClicked();
    void onDeckButtonClicked();
    void onSettingsButtonClicked();

private:
    Player* player;
    Map* map;
    TreasureManager treasureManager;

    TopBarWidget* topBar;
    QPushButton* chestBtn;
    QPushButton* rewardBtn;
    QPushButton* backBtn;

    bool chestOpened;

    void setupUI();
    void refreshChestVisual();
    void refreshRewardVisual();

protected:
    void paintEvent(QPaintEvent*) override
    {
        QStyleOption opt;
        opt.initFrom(this);
        QPainter p(this);
        style()->drawPrimitive(QStyle::PE_Widget, &opt, &p, this);
    }
};

#endif // TREASUREPAGE_H
