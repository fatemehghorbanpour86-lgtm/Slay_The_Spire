#ifndef TREASUREGUESSPAGE_H
#define TREASUREGUESSPAGE_H

#include <QWidget>
#include <QPushButton>
#include <QLabel>
#include <QVector>
#include <QStyleOption>
#include <QPainter>

#include "treasureguess.h"

class Player;
class Map;
class TopBarWidget; // reused as-is from campfirepage.h - no new TopBar is created

class TreasureGuessPage : public QWidget
{
    Q_OBJECT

public:
    explicit TreasureGuessPage(Player* player, Map* map, QWidget* parent = nullptr);

signals:
    // Emitted the moment the player successfully claims the reward
    // (i.e. presses the reward button after picking the correct chest).
    void rewardClaimed();

    // Emitted when the player presses "Proceed" - the round is fully
    // over (win, claimed, or wrong guess). GameManager will eventually
    // connect this to return to the Map, just like
    // TreasurePage::treasureFinished().
    void proceedRequested();

    void settingsRequested();

private slots:
    void onChestClicked(int index);
    void onRewardClicked();
    void onProceedClicked();

    void onRelicButtonClicked();
    void onDeckButtonClicked();
    void onSettingsButtonClicked();

private:
    Player* player;
    Map* map;
    TreasureGuessModel model;

    TopBarWidget* topBar = nullptr;
    QLabel* titleLabel = nullptr;
    QLabel* messageLabel = nullptr;
    QVector<QPushButton*> chestButtons;
    QPushButton* rewardBtn = nullptr;
    QPushButton* proceedBtn = nullptr;

    void setupUI();

    // Swaps a single chest's art from ChestClose to ChestOpen.
    void refreshChestVisual(int index);

    // Positions/styles the reward button above the opened chest and
    // shows it (mirrors TreasurePage::refreshRewardVisual()).
    void showReward();

    // Displays the "wrong chest" message.
    void showFailureMessage();

    // Reveals the Proceed button (bottom-right corner).
    void showProceedButton();

    // Resets all widgets to their initial per-round appearance -
    // called once after the model is (re)initialized.
    void updateUI();

protected:
    void paintEvent(QPaintEvent*) override
    {
        QStyleOption opt;
        opt.initFrom(this);
        QPainter p(this);
        style()->drawPrimitive(QStyle::PE_Widget, &opt, &p, this);
    }
};

#endif // TREASUREGUESSPAGE_H
