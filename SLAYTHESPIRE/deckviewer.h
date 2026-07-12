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
    static QString cardImagePath(const Card* card);

private:
    Player* player;

    void setupUI();
};


#endif // DECKVIEWER_H
