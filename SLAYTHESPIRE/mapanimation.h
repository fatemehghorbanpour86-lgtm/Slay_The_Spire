#ifndef MAPANIMATION_H
#define MAPANIMATION_H

#include <QObject>
#include <QVariantAnimation>

class MapNode;

class MapSelectionAnimator : public QObject
{
    Q_OBJECT

public:
    explicit MapSelectionAnimator(QObject* parent = nullptr);

    // Starts the full selection sequence for 'selected'. 'from' may be
    // nullptr (e.g. the very first move on a freshly generated Act,
    // where there is no previous Node/path to animate).
    void start(MapNode* selected, MapNode* from);

    bool isAnimating() const;

    // Golden ring, only meaningful while isCircleActive() is true.
    // Degrees swept so far, 0..360, starting at 0 and sweeping
    // counter-clockwise (matches QPainter::drawArc's convention).
    int circleSweepAngle() const;
    bool isCircleActive() const;

    // Traveling light along the selected path, only meaningful while
    // isPathActive() is true. 0.0 (start of path) .. 1.0 (destination).
    qreal pathProgress() const;
    bool isPathActive() const;

    MapNode* getSelectedNode() const;
    MapNode* getFromNode() const;

signals:
    // Emitted on every tick of either animation - the caller should
    // trigger a repaint.
    void progressChanged();

    // Emitted the instant the golden ring finishes drawing. The caller
    // should now (instantly, no animation needed) lock the other
    // available Nodes and gray out the paths that lead to them.
    void circleFinished();

    // Emitted once the entire sequence has completed. The caller should
    // proceed with whatever used to happen immediately on click.
    void selectionAnimationFinished(MapNode* selectedNode);

private:
    void startCircleAnimation();
    void startPathAnimation();

    QVariantAnimation* circleAnimation;
    QVariantAnimation* pathAnimation;

    MapNode* selectedNode;
    MapNode* fromNode;

    bool circleActive;
    bool pathActive;

    static const int CIRCLE_DURATION_MS = 600;
    static const int PATH_DURATION_MS = 700;
};

#endif // MAPANIMATION_H
