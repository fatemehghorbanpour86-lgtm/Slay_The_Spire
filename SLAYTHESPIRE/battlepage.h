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

    // -- Hand cards rendering (arc layout + hover animation) --
    QGraphicsScene *handScene = nullptr;   // added: holds the card proxies
    QGraphicsView  *handView  = nullptr;   // added: displays handScene

    void setupTopBar();
    void setupBattleField();
    void setupBottomBar();

protected:
    bool eventFilter(QObject *obj, QEvent *event) override;
};

#endif // BATTLEPAGE_H