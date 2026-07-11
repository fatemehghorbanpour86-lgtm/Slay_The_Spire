#ifndef RELICVIEWER_H
#define RELICVIEWER_H

#include <QDialog>
#include "player.h"
#include "relic.h"

class RelicViewerDialog : public QDialog
{
    Q_OBJECT
public:
    explicit RelicViewerDialog(Player* player, QWidget* parent = nullptr);

private:
    Player* player;

    void setupUI();
    static QString relicIconPath(const Relic* relic);
};

#endif // RELICVIEWER_H
