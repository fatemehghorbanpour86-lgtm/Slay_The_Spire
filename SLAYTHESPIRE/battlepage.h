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

#include <functional>


#include "combatmanager.h"
#include "effect.h"
#include "outlinedlabel.h"
#include "qgraphicseffect.h"

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
    void onCardClicked(Card* card, QGraphicsProxyWidget* proxy);
    void onBattleWon();
    void onBattleLost();
    void onDrawPileClicked();
    void onDiscardPileClicked();
    void updateEnemyIntent(Enemy* enemy);


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
    QLabel *goldValueLabel;
    OutlinedLabel *energyValueLabel = nullptr;
    QLabel *intentLabel = nullptr;
    QProgressBar *playerHPBar = nullptr;
    QProgressBar *enemyHPBar = nullptr;
    QString cardImagePath(const Card* card);
    QPushButton *drawPileBtn;
    QPushButton *discardPileBtn;
    QLabel      *drawPileCountLabel;
    QLabel      *discardPileCountLabel;


    QString getIntentText(Enemy* enemy);

    QString enemyImagePath(Enemy* enemy);


    // -- Card selection state --
    Card* pendingCard = nullptr;           // card currently selected
    QGraphicsProxyWidget* selectedProxy = nullptr;  // its proxy in the scene

    QVector<QGraphicsDropShadowEffect*> enemyGlowEffects;
    QGraphicsDropShadowEffect* playerGlowEffect = nullptr;

    // -- Enemy widget tracking (needed to know where to draw rings) --
    QVector<QWidget*> enemyWidgets;   // one per enemy, filled in setupBattleField
    QWidget* playerWidget = nullptr;  // player widget, save as member

    QPushButton* enemyClickOverlay  = nullptr;   // one per enemy for now
    QPushButton* playerClickOverlay = nullptr;


    enum class CardTarget { Enemy, Player, None };
    CardTarget getCardTarget(Card* card);

    void showEnemyHighlights();
    void showPlayerHighlight();
    void clearHighlights();
    void clearSelection();

    void repositionOverlays();
    void setupClickOverlays();

void playCardWithAnimation(Card* card,QGraphicsProxyWidget* proxy, Enemy* target);

bool animatingCard = false;


QGraphicsScene* animScene = nullptr;
QGraphicsView*  animView  = nullptr;

QLabel *playerBlockIconLabel = nullptr;
int    lastPlayerBlock = 0;

void repositionBlockIcon();


void showEvent(QShowEvent* e)override;

void animateAttack(QWidget* attacker, QWidget* target, std::function<void()> onDone = nullptr);


QWidget* playerEffectsWidget = nullptr;
QHBoxLayout* playerEffectsLayout = nullptr;

QWidget* enemyEffectsWidget = nullptr;
QHBoxLayout* enemyEffectsLayout = nullptr;

void setupEffectsUI();
void updateEffectsUI();
QString effectImagePath(const Effect* effect);


protected:
    bool eventFilter(QObject *obj, QEvent *event) override;
};

#endif // BATTLEPAGE_H