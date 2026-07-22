#ifndef DECKVIEWER_H
#define DECKVIEWER_H

#include <QDialog>
#include <QHash>
#include "player.h"
#include "card.h"

class QLabel;
class QPropertyAnimation;

class DeckViewerDialog : public QDialog
{
    Q_OBJECT
public:
    explicit DeckViewerDialog(Player* player, QWidget* parent = nullptr);
    static QString cardImagePath(const Card* card);

private:
    Player* player;

    void setupUI();
protected:
    bool eventFilter(QObject* watched, QEvent* event) override;

private:
    // هر لیبل، جای اصلی خودش (قبل از بزرگ شدن) رو نگه می‌داره
    QHash<QLabel*, QRect> originalGeometry;
    // انیمیشن فعلی هر لیبل، تا اگه وسط انیمیشن قبلی دوباره هاور شد تداخل نشه
    QHash<QLabel*, QPropertyAnimation*> activeAnimations;
};


#endif // DECKVIEWER_H
