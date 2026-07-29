#ifndef SHOPPAGE_H
#define SHOPPAGE_H

#include <QWidget>
#include <QLabel>
#include <QPushButton>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QVector>

#include "Player.h"
#include "shop.h"

class ShopPage : public QWidget
{
    Q_OBJECT

public:
    explicit ShopPage(Player *player, QWidget *parent = nullptr);
    ~ShopPage();

private slots:
    void onMerchantClicked();
    void onLeaveClicked();
    void onBuyCard(int index);
    void onBuyPotion(int index);

private:
    enum class ShopState
    {
        Greeting,
        Browsing
    };

private:
    void setupTopBar();
    void setupShopField();
    void updateUI();
    void populateInventory();
    void positionGreetingActors();
    void updateTopBarStats();
    QString getPotionImagePath(const QString &potionName);

    bool cardRemovalSold = false;

private:
    ShopState currentState = ShopState::Greeting;

    Player *player;
    Shop *shopLogic;

    // Main UI structure
    QVBoxLayout *mainLayout;
    QWidget *topBar;
    QWidget *shopField;
    QWidget *bottomBar;

    // Greeting state widgets
    QWidget *playerContainer;
    QPushButton *merchantBtn;
    QPushButton *proceedBtn;

    // Browsing state widgets
    QWidget *inventoryContainer;
    QPushButton *leaveBtn;

    // Top bar labels and elements
    QLabel *playerHpLabel;
    QLabel *deckCountLabel = nullptr;
    QLabel *goldValueLabel;
    QLabel *bg;

    // Disabled potion buttons for visual display in the top bar
    QVector<QPushButton*> potionButtons;

signals:
    void shoppingDone();
};

#endif // SHOPPAGE_H
