#ifndef PILEVIEWERDIALOG_H
#define PILEVIEWERDIALOG_H

#include <QDialog>
#include <QVector>
#include <QPushButton>
#include <QLabel>
#include <QScrollArea>
#include <QGridLayout>

class Player;
class Card;

enum class PileType
{
    Draw,
    Discard,
    Deck
};

class PileViewerDialog : public QDialog
{
    Q_OBJECT

public:
    explicit PileViewerDialog(Player *player, PileType pileType, QWidget *parent = nullptr);
    ~PileViewerDialog();

private:
    void setupUI();
    void populatePile();

    QString getTitleText() const;
    const QVector<Card*>& getCardsForPile() const;

private:
    Player *player;
    PileType pileType;

    // UI elements
    QLabel *titleLabel;
    QScrollArea *scrollArea;
    QWidget *scrollContainer;
    QGridLayout *gridLayout;
    QPushButton *leaveBtn;

    // Card style rules
    const QString cardStyle =
        "QPushButton {"
        "   background: transparent;"
        "   border: 2px solid transparent;"
        "   border-radius: 6px;"
        "}"
        "QPushButton:hover {"
        "   border: 2px solid rgba(230, 57, 70, 200);"
        "   background: rgba(255, 255, 255, 10);"
        "}";
};

#endif // PILEVIEWERDIALOG_H