#ifndef CARDSELECTION_H
#define CARDSELECTION_H

#include <QDialog>
#include <QVector>
#include <QPropertyAnimation>

class Card;

class CardSelectionDialog : public QDialog
{
    Q_OBJECT

public:
    explicit CardSelectionDialog(const QVector<Card*>& cardChoices, QWidget* parent = nullptr);

    Card* getChosenCard() const;

protected:
    bool eventFilter(QObject* watched, QEvent* event) override;

private:
    QVector<Card*> cardChoices;
    Card* chosenCard;

    void setupUI();

    QMap<QPushButton*, QRect> originalGeometry;
    QMap<QPushButton*, QPropertyAnimation*> activeAnimations;
};

#endif // CARDSELECTION_H
