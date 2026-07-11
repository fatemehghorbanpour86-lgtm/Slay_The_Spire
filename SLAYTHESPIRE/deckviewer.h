#ifndef DECKVIEWER_H
#define DECKVIEWER_H

#include <QDialog>
#include "player.h"
#include "card.h"

class DeckViewerDialog : public QDialog
{
    Q_OBJECT
public:
    explicit DeckViewerDialog(Player* player, QWidget* parent = nullptr);

private:
    Player* player;

    void setupUI();
    static QString cardImagePath(const Card* card);
};


#endif // DECKVIEWER_H
