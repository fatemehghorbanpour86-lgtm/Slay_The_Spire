#ifndef RELICSELECTION_H
#define RELICSELECTION_H

#include <QDialog>
#include <QVector>
#include <QString>

class Relic;

class RelicSelectionDialog : public QDialog
{
    Q_OBJECT

public:
    explicit RelicSelectionDialog(const QVector<Relic*>& relicChoices, QWidget* parent = nullptr);

    Relic* getChosenRelic() const;

private:
    QVector<Relic*> relicChoices;
    Relic* chosenRelic;

    void setupUI();
};

#endif // RELICSELECTION_H
