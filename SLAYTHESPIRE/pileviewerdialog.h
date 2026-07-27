#ifndef PILEVIEWERDIALOG_H
#define PILEVIEWERDIALOG_H

#include <QDialog>
#include <QVector>
#include <QPushButton>
#include <QLabel>
#include <QScrollArea>
#include <QGridLayout>
#include <QString>
#include <QEvent>

class Player;
class Card;

enum class PileType
{
    Draw,
    Discard,
    Exhaust,
    Deck
};

enum class PileViewerMode
{
    ViewOnly,
    SelectCard
};

class PileViewerDialog : public QDialog
{
    Q_OBJECT

public:
    explicit PileViewerDialog(Player *player,
                              PileType pileType,
                              PileViewerMode mode = PileViewerMode::ViewOnly,
                              QWidget *parent = nullptr);
    ~PileViewerDialog();

signals:
    void cardSelected(Card *card);

protected:
    bool eventFilter(QObject *obj, QEvent *event) override;

private:
    void setupUI();
    void populatePile();

    QString getTitleText() const;
    const QVector<Card*>& getCardsForPile() const;

    void applyHoverStyle(QPushButton *cardBtn);
    void removeHoverStyle(QPushButton *cardBtn);
    void applySelectedStyle(QPushButton *cardBtn);
    void removeSelectedStyle(QPushButton *cardBtn);

    void animateCardToHover(QPushButton *cardBtn);
    void animateCardToNormal(QPushButton *cardBtn);
    void animateCardToSelected(QPushButton *cardBtn);

    void toggleCardSelection(QPushButton *cardBtn, Card *card);
    void clearSelection();

    void onLeaveOrCancelClicked();
    void onConfirmSelectionClicked();

private:
    Player *player;
    PileType pileType;
    PileViewerMode mode;

    QLabel *titleLabel;
    QScrollArea *scrollArea;
    QWidget *scrollContainer;
    QGridLayout *gridLayout;
    QPushButton *leaveBtn;
    QPushButton *confirmBtn;

    Card *selectedCard;
    QPushButton *selectedCardButton;

    const QString cardStyle =
        "QPushButton {"
        "   background: transparent;"
        "   border: none;"
        "   border-radius: 10px;"
        "}";

    const QString selectedCardStyle =
        "QPushButton {"
        "   background: transparent;"
        "   border: none;"
        "   border-radius: 10px;"
        "}";

    const QString confirmBtnDisabledStyle =
        "QPushButton {"
        "   background-color: #444444;"
        "   color: #aaaaaa;"
        "   font-size: 16px;"
        "   font-weight: bold;"
        "   border: 1px solid #666666;"
        "   border-radius: 6px;"
        "}";

    const QString confirmBtnEnabledStyle =
        "QPushButton {"
        "   background-color: #8b0000;"
        "   color: white;"
        "   font-size: 16px;"
        "   font-weight: bold;"
        "   border: 1px solid #c1121f;"
        "   border-radius: 6px;"
        "}"
        "QPushButton:hover {"
        "   background-color: #b30000;"
        "}";
};

#endif // PILEVIEWERDIALOG_H
