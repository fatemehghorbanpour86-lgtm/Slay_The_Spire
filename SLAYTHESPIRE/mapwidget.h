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

    static const int MAP_CONTENT_WIDTH = 1400;
    static const int NODE_SIZE = 64;
    static const int VERTICAL_SPACING = 170;    // فاصله بیشتر بین فلورها
    static const int VERTICAL_SPACING_BOSS = 300;
    static const int HORIZONTAL_SPACING = 200;

    void createNodeButtons();
    QString getNodeStyleSheet(MapNode* node);
    QString getTooltipText(NodeType type);
};
#endif // MAPWIDGET_H