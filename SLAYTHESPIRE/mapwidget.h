#ifndef MAPWIDGET_H
#define MAPWIDGET_H
#include <QWidget>
#include <QPushButton>
#include <QHash>
#include <QPainter>
#include <QStyleOption>
#include "map.h"
#include "mapanimation.h"

class MapWidget : public QWidget
{
    Q_OBJECT
public:
    explicit MapWidget(Map* gameMap, QWidget *parent = nullptr);
    void refreshUI();

signals:
    void nodeClicked(int nodeId);
    void animationStarted();
    void animationFinished();

protected:
    void paintEvent(QPaintEvent *event) override;

private:
    Map* map;
    QHash<MapNode*, QPushButton*> nodeButtons;

    MapSelectionAnimator* selectionAnimator;

    // Nodes that were Available at the moment a selection began, other
    // than the selected one - shown as (visually only) Locked for the
    // duration of the selection animation.
    QSet<MapNode*> temporarilyLockedNodes;

    // True from the moment the golden ring finishes until the whole
    // selection sequence completes. Used purely to decide whether
    // currently-unused (previously blue) paths should be painted gray.
    bool animationLockActive = false;

    static const int MAP_CONTENT_WIDTH = 1400;
    static const int NODE_SIZE = 64;
    static const int VERTICAL_SPACING = 170;
    static const int VERTICAL_SPACING_BOSS = 300;
    static const int HORIZONTAL_SPACING = 200;

    void createNodeButtons();
    QString getNodeStyleSheet(MapNode* node);
    QString getTooltipText(NodeType type);

    // Applies getNodeStyleSheet() to every Node button without touching
    // their enabled/cursor state (used both by refreshUI() and by the
    // animation's instant "lock other Nodes" step).
    void updateNodeStyles();

    // Begins the click -> animate -> nodeClicked() sequence for 'node'.
    // No-op if 'node' isn't available or an animation is already running.
    void startNodeSelection(MapNode* node);

private slots:
    void onAnimationProgress();
    void onCircleFinished();
    void onSelectionFinished(MapNode* node);
};
#endif // MAPWIDGET_H