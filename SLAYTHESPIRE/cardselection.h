#ifndef CARDSELECTION_H
#define CARDSELECTION_H

#include <QDialog>
#include <QVector>

class Card;

class CardSelectionDialog : public QDialog
{
    Q_OBJECT

public:
    explicit CardSelectionDialog(const QVector<Card*>& cardChoices, QWidget* parent = nullptr);

    Card* getChosenCard() const;

private:
    QVector<Card*> cardChoices;
    Card* chosenCard;

    void setupUI();
};

#endif // CARDSELECTION_H
