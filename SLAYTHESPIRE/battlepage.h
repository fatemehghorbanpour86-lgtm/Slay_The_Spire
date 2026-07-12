#ifndef BATTLEPAGE_H
#define BATTLEPAGE_H

#include <QWidget>
#include <QLabel>
#include <QPushButton>
#include <QProgressBar>
#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QGraphicsScene>   // added: needed for handScene member
#include <QGraphicsView>    // added: needed for handView member

#include <QMap>
#include <QList>

#include "combatmanager.h"
#include "outlinedlabel.h"

class Player;
class Enemy;
class Card;


class BattlePage : public QWidget
{
    Q_OBJECT

public:
    explicit BattlePage(Player* player = nullptr,
                        QVector<Enemy*> enemies = {},
                        QWidget* parent = nullptr);

private slots:
    void updateStats();
    void onCardClicked(Card* card);
    void onBattleWon();
    void onBattleLost();

signals:
    void battleEnded();

private:

    CombatManager* combatManager;
    Player* player;
    QVector<Enemy*> enemies;

    // -- Three main sections of the battle screen --
    QWidget *topBar;        // Top bar: HP, relics, potions
    QWidget *battleField;   // Battle area: player + enemies
    QWidget *bottomBar;     // Bottom bar: energy, hand cards, End Turn

    // -- Hand cards rendering (arc layout + hover animation) --
    QGraphicsScene *handScene = nullptr;   // added: holds the card proxies
    QGraphicsView  *handView  = nullptr;   // added: displays handScene

    void setupTopBar();
    void setupBattleField();
    void setupBottomBar();
    void refreshHand();

    QPushButton *endTurnBtn = nullptr;
    QLabel *playerHpLabel = nullptr;
    QLabel *playerBlockLabel = nullptr;
    OutlinedLabel *energyValueLabel = nullptr;
    QLabel *intentLabel = nullptr;
    QProgressBar *playerHPBar = nullptr;
    QProgressBar *enemyHPBar = nullptr;
    QString cardImagePath(const Card* card);


protected:
    bool eventFilter(QObject *obj, QEvent *event) override;
};

#endif // BATTLEPAGE_H