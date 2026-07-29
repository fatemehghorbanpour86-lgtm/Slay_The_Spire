#ifndef DEFEATPAGE_H
#define DEFEATPAGE_H

#include <QWidget>
#include <QPixmap>
#include "leaderboardmanager.h"

class Player;
class Map;
class QLabel;
class QPushButton;
class QFrame;

class DefeatPage : public QWidget
{
    Q_OBJECT

public:
    // Calculates and saves the score internally using LeaderboardManager,
    // then displays the resulting breakdown + rank.
    explicit DefeatPage(Player* player,
                        Map* map,
                        const QString& username,
                        QWidget* parent = nullptr);

signals:
    void leaderboardRequested();
    void mainMenuRequested();

protected:
    void paintEvent(QPaintEvent* event) override;
    void resizeEvent(QResizeEvent* event) override;

private:
    void loadAssets();
    void setupUi();
    void setupStyles();
    QWidget* createStatItem(const QString& label, int value);

    Player* player = nullptr;
    Map* map = nullptr;
    QString username;
    ScoreBreakdown breakdown;
    int rank = -1;

    QPixmap backgroundPixmap;

    QFrame* scorePanel = nullptr;
    QLabel* totalScoreLabel = nullptr;
    QLabel* rankLabel = nullptr;
    QPushButton* leaderboardButton = nullptr;
    QPushButton* mainMenuButton = nullptr;
};

#endif // DEFEATPAGE_H