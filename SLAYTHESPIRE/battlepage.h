#ifndef BATTLEPAGE_H
#define BATTLEPAGE_H

#include <QWidget>
#include <QLabel>
#include <QPushButton>
#include <QProgressBar>
#include <QHBoxLayout>
#include <QVBoxLayout>

namespace Ui {
class BattlePage;
}

class BattlePage : public QWidget
{
    Q_OBJECT

public:
    explicit BattlePage(QWidget *parent = nullptr);
    ~BattlePage() override;

signals:
    void battleEnded();

private:
    Ui::BattlePage *ui;

    // -- Three main sections of the battle screen --
    QWidget *topBar;        // Top bar: HP, relics, potions
    QWidget *battleField;   // Battle area: player + enemies
    QWidget *bottomBar;     // Bottom bar: energy, hand cards, End Turn

    void setupTopBar();
    void setupBattleField();
    void setupBottomBar();
};

#endif // BATTLEPAGE_H