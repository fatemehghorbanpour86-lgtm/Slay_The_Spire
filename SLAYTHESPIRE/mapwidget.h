#ifndef MAPWIDGET_H
#define MAPWIDGET_H

#include <QWidget>
#include <QPushButton>
#include <QHash>
#include <QPainter>
#include <QStyleOption>
#include "map.h"

class MapWidget : public QWidget
{
    Q_OBJECT
public:
    explicit MapWidget(Map* gameMap, QWidget *parent = nullptr);
    void refreshUI();

signals:
    void nodeClicked(int nodeId);

protected:
    void paintEvent(QPaintEvent *event) override;

private:
    Map* map;
    QHash<MapNode*, QPushButton*> nodeButtons;

    void createNodeButtons();
    QString getNodeStyleSheet(MapNode* node);
    QString getTooltipText(NodeType type);
};

#endif // MAPWIDGET_H
