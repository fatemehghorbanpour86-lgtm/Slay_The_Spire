#ifndef BATTLEPAGE_H
#define BATTLEPAGE_H

#include <QWidget>
#include <QLabel>
#include <QPushButton>
#include <QProgressBar>
#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QGraphicsScene>
#include <QGraphicsView>
#include <QGraphicsDropShadowEffect>
#include <QPointer>
#include <QGraphicsProxyWidget>
#include <QVector>
#include <QString>

#include <functional>

#include "combatmanager.h"
#include "effect.h"
#include "outlinedlabel.h"

class Player;
class Enemy;
class Card;
class QGraphicsProxyWidget;
class QShowEvent;
class QEvent;

class BattlePage : public QWidget
{
    Q_OBJECT

public:
    explicit BattlePage(Player* player = nullptr,
                        QVector<Enemy*> enemies = {},
                        QWidget* parent = nullptr);

signals:
    void battleEnded();

private slots:
    void updateStats();
    void onCardClicked(Card* card, QGraphicsProxyWidget* proxy);
    void onBattleWon();
    void onBattleLost();
    void onDrawPileClicked();
    void onDiscardPileClicked();
    void updateEnemyIntent(Enemy* enemy);

private:
    struct EnemyUI
    {
        Enemy* enemy = nullptr;
        QPointer<QWidget> widget;
        QPointer<QProgressBar> hpBar;
        QPointer<QLabel> intentLabel;
        QPointer<QWidget> effectsWidget;
        QPointer<QHBoxLayout> effectsLayout;
        QPointer<QPushButton> clickOverlay;
    };

    QVector<EnemyUI> enemyUIs;

    QWidget* enemyContainer = nullptr;
    QHBoxLayout* enemyLayout = nullptr;

    CombatManager* combatManager = nullptr;
    Player* player = nullptr;
    QVector<Enemy*> enemies;

    QWidget* topBar = nullptr;
    QWidget* battleField = nullptr;
    QWidget* bottomBar = nullptr;

    QGraphicsScene* handScene = nullptr;
    QGraphicsView* handView = nullptr;

    QPushButton* endTurnBtn = nullptr;

    QLabel* playerHpLabel = nullptr;
    QLabel* playerBlockLabel = nullptr;
    QLabel* goldValueLabel = nullptr;
    OutlinedLabel* energyValueLabel = nullptr;
    QProgressBar* playerHPBar = nullptr;

    QPushButton* drawPileBtn = nullptr;
    QPushButton* discardPileBtn = nullptr;
    QLabel* drawPileCountLabel = nullptr;
    QLabel* discardPileCountLabel = nullptr;

    QWidget* playerWidget = nullptr;
    QPushButton* playerClickOverlay = nullptr;

    Card* pendingCard = nullptr;
    QPointer<QGraphicsProxyWidget> selectedProxy;

    QGraphicsDropShadowEffect* playerGlowEffect = nullptr;

    bool animatingCard = false;

    QGraphicsScene* animScene = nullptr;
    QGraphicsView* animView = nullptr;

    QLabel* playerBlockIconLabel = nullptr;
    int lastPlayerBlock = 0;

    QWidget* playerEffectsWidget = nullptr;
    QHBoxLayout* playerEffectsLayout = nullptr;

    enum class CardTarget
    {
        Enemy,
        Player,
        None
    };

    void setupTopBar();
    void setupBattleField();
    void setupBottomBar();
    void setupEffectsUI();
    void setupClickOverlays();

    void refreshHand();
    void updateEffectsUI();

    void showEnemyHighlights();
    void showPlayerHighlight();
    void clearHighlights();
    void clearSelection();

    void repositionOverlays();
    void repositionBlockIcon();

    void playCardWithAnimation(Card* card, QGraphicsProxyWidget* proxy, Enemy* target);
    void animateAttack(QWidget* attacker,
                       QWidget* target,
                       std::function<void()> onDone = nullptr);

    CardTarget getCardTarget(Card* card);

    QString cardImagePath(const Card* card);
    QString enemyImagePath(Enemy* enemy);
    QString effectImagePath(const Effect* effect);
    QString getIntentText(Enemy* enemy);

    QWidget* findWidgetForEnemy(Enemy* enemy)
    {
        if (!enemy)
            return nullptr;

        for (const EnemyUI& ui : enemyUIs)
        {
            if (ui.enemy == enemy)
                return ui.widget;
        }

        return nullptr;
    }

    EnemyUI* findEnemyUi(Enemy* enemy)
    {
        if (!enemy)
            return nullptr;

        for (EnemyUI& ui : enemyUIs)
        {
            if (ui.enemy == enemy)
                return &ui;
        }

        return nullptr;
    }

protected:
    void showEvent(QShowEvent* e) override;
    bool eventFilter(QObject* obj, QEvent* event) override;
};

#endif // BATTLEPAGE_H
