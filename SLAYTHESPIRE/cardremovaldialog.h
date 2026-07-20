#ifndef CARDREMOVALDIALOG_H
#define CARDREMOVALDIALOG_H

#include <QDialog>
#include <QVector>
#include <QPushButton>
#include <QLabel>
#include <QScrollArea>
#include <QGridLayout>

class Player;
class Card;

class CardRemovalDialog : public QDialog
{
    Q_OBJECT

public:
    explicit CardRemovalDialog(Player *player, QWidget *parent = nullptr);
    ~CardRemovalDialog();

    // returning the selected card for remove
    Card* getSelectedCard() const;

private slots:
    void onCardClicked(Card* card, QPushButton* button);
    void onConfirm();
    void onCancel();

private:
    void setupUI();
    void populateDeck();

    Player *player;
    Card *selectedCard;
    QPushButton *selectedButton;

    // UI elements
    QLabel *titleLabel;
    QScrollArea *scrollArea;
    QWidget *scrollContainer;
    QGridLayout *gridLayout;

    QPushButton *confirmBtn;
    QPushButton *cancelBtn;

    // card style
    const QString normalCardStyle =
        "QPushButton {"
        "   background: transparent;"
        "   border: 2px solid transparent;"
        "   border-radius: 6px;"
        "}"
        "QPushButton:hover {"
        "   border: 2px solid rgba(255, 215, 0, 150);"
        "   background: rgba(255, 255, 255, 10);"
        "}";

    const QString selectedCardStyle =
        "QPushButton {"
        "   background: rgba(255, 215, 0, 20);"
        "   border: 3px solid #f5c518;"
        "   border-radius: 6px;"
        "}";
};

#endif // CARDREMOVALDIALOG_H
