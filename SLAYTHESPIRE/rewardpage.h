#ifndef REWARDPAGE_H
#define REWARDPAGE_H


#include <QWidget>
#include <QVBoxLayout>
#include <QPushButton>
#include <QStyleOption>
#include <QPainter>
#include "rewardsystem.h"

class Player;
class Reward;
class Relic;


class RewardPage : public QWidget
{
    Q_OBJECT

public:
    explicit RewardPage(Player* player, RewardSystem* rewardSystem, QWidget* parent = nullptr);

signals:
    // Emitted once the player presses Continue. Any reward still pending
    // at that point has already been discarded (see RewardSystem::discardRemaining()).
    void continueClicked();

private:
    Player* player;
    RewardSystem* rewardSystem;

    QVBoxLayout* rewardsLayout;
    QPushButton* continueBtn;

    void setupUI();
    void rebuildRewardButtons();

    // Builds exactly one button per Reward - including the Card reward
    // and the Boss Relic reward, both of which open a selection dialog
    // on click instead of applying anything directly.
    QPushButton* createRewardButton(Reward* reward);

    void onGoldClicked(Reward* reward, QPushButton* button);
    void onPotionClicked(Reward* reward, QPushButton* button);
    void onRelicClicked(Reward* reward, QPushButton* button);
    void onCardClicked(Reward* reward, QPushButton* button);
    void onBossRelicClicked(Reward* reward, QPushButton* button);

    void onContinueClicked();

protected:
    void paintEvent(QPaintEvent*) override
    {
        QStyleOption opt;
        opt.initFrom(this);
        QPainter p(this);
        style()->drawPrimitive(QStyle::PE_Widget, &opt, &p, this);
    }
};

#endif // REWARDPAGE_H
