#ifndef COMBATMANAGER_H
#define COMBATMANAGER_H

#include <QObject>
#include <QVector>

class Player;
class Enemy;
class Card;
class Potion;
class CombatCalculator;

enum class CombatState
{
    BattleStart,
    TurnStart,
    PlayerAction,
    TurnEnd,
    EnemyTurn,
    BattleWon,
    BattleLost
};

class CombatManager : public QObject
{
    Q_OBJECT

public:
    explicit CombatManager(Player* player, QVector<Enemy*> enemies, QObject* parent = nullptr);
    ~CombatManager();

    void startCombat();

    bool playCard(Card* card, Enemy* target = nullptr);
    bool usePotion(Potion* potion, Enemy* target = nullptr);

    void endTurn();

    CombatState getCurrentState() const;
    const QVector<Enemy*>& getEnemies() const;

signals:
    void combatStateChanged(CombatState state);
    void statsUpdated();

    void combatStarted();
    void playerTurnStarted();
    void enemyTurnStarted();
    void cardPlayed(Card* card, Enemy* target);
    void enemyIntentUpdated(Enemy* enemy);

    void battleWon();
    void battleLost();
    void combatEnded(bool playerWon);

private:
    void changeState(CombatState newState);

    void handleBattleStart();
    void handleTurnStart();
    void handleTurnEnd();
    void handleEnemyTurn();

    void checkWinLossCondition();

private:
    CombatState currentState;
    Player* player;
    QVector<Enemy*> enemies;

    CombatCalculator* calculator;
    int turnCount;

private slots:
    void onEnemyDied(Enemy* enemy);

};

#endif // COMBATMANAGER_H