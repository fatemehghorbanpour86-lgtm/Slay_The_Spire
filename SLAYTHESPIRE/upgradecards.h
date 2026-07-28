#ifndef UPGRADECARDS_H
#define UPGRADECARDS_H

#include <QDialog>
#include <QScrollArea>
#include <QGridLayout>
#include <QWidget>

class Player;
class Campfire;
class Card;
class QPropertyAnimation;

class UpgradePreviewDialog : public QDialog
{
    Q_OBJECT

public:
    explicit UpgradePreviewDialog(Card* card, Player* player, Campfire* campfire, QWidget* parent = nullptr);
    static QString UpgradeCardImagePath(const Card* card);

private slots:
    void onUpgradeClicked();

private:
    Card* card;
    Player* player;
    Campfire* campfire;

    void setupUI();
};

class UpgradeCardsDialog : public QDialog
{
    Q_OBJECT

public:
    explicit UpgradeCardsDialog(Player* player, Campfire* campfire, QWidget* parent = nullptr);

protected:
    bool eventFilter(QObject* watched, QEvent* event) override;

private:
    Player* player;
    Campfire* campfire;

    QScrollArea* scrollArea;
    QWidget* gridContainer;
    QGridLayout* gridLayout;

    static const int CARD_WIDTH = 162;
    static const int CARD_HEIGHT = 214;
    static const int COLUMNS = 4;

    void setupUI();
    void populateCards();
    void openPreview(Card* card);


    QMap<QWidget*, QRect> originalGeometry;
    QMap<QWidget*, QPropertyAnimation*> activeAnimations;

};

#endif // UPGRADECARDS_H
