#ifndef MAPPAGE_H
#define MAPPAGE_H
#include <QWidget>
#include <QScrollArea>
#include <QProgressBar>
#include <QLabel>
#include <QPushButton>
#include <QFrame>
#include <QVBoxLayout>
#include <QStyleOption>
#include <QPainter>
#include "map.h"
#include "mapwidget.h"
#include "player.h"

class MapPage : public QWidget {
    Q_OBJECT
public:
    explicit MapPage(Map* gameMap, Player* player, QWidget *parent = nullptr);
    void updateTopBarData();
    Player* getPlayer() const;
    Map* getMap() const;

private slots:
    void onNodeClicked(int nodeId);
    void onRelicButtonClicked();
    void onDeckButtonClicked();
    void onSettingsButtonClicked();

private:
    Map* map;
    Player* player;
    MapWidget* mapWidget;

    QLabel* cardLabel;
    QWidget* topBarWidget;
    QProgressBar* hpBar;
    QLabel* goldLabel;
    QLabel* actLabel;
    QLabel* floorIconLabel;
    QLabel* floorNumberLabel;
    QLabel* relicLabel;
    QPushButton* relicBtn;
    QPushButton* deckBtn;
    QPushButton* settingsBtn;
    QFrame* potionsPanel;
    QLabel* potionSlots[3];

    void setupUI();
    void createTopBar(QVBoxLayout* mainLayout);

protected:
    void paintEvent(QPaintEvent *) override{
        QStyleOption opt;
        opt.initFrom(this);
        QPainter p(this);
        style()->drawPrimitive(QStyle::PE_Widget, &opt, &p, this);
    }

signals:
    void battleRequested();
    void campfireEntered();
};
#endif // MAPPAGE_H