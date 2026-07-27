#include "mapanimation.h"
#include "mapnode.h"

MapSelectionAnimator::MapSelectionAnimator(QObject* parent)
    : QObject(parent),
    circleAnimation(new QVariantAnimation(this)),
    pathAnimation(new QVariantAnimation(this)),
    selectedNode(nullptr),
    fromNode(nullptr),
    circleActive(false),
    pathActive(false)
{
    // ---- Golden ring animation ----
    circleAnimation->setStartValue(0);
    circleAnimation->setEndValue(360);
    circleAnimation->setDuration(CIRCLE_DURATION_MS);
    circleAnimation->setEasingCurve(QEasingCurve::OutCubic);

    connect(circleAnimation, &QVariantAnimation::valueChanged, this, [this]()
            {
                emit progressChanged();
            });

    connect(circleAnimation, &QVariantAnimation::finished, this, [this]()
            {
                circleActive = false;
                emit progressChanged();
                emit circleFinished();

                if (fromNode != nullptr)
                {
                    // There is a real previous Node: animate the path
                    // from it to the selected Node next.
                    startPathAnimation();
                }
                else
                {
                    // Nothing to animate (e.g. first move on the Act) -
                    // the whole sequence is already complete.
                    emit selectionAnimationFinished(selectedNode);
                }
            });

    // ---- Traveling-light path animation ----
    pathAnimation->setStartValue(0.0);
    pathAnimation->setEndValue(1.0);
    pathAnimation->setDuration(PATH_DURATION_MS);
    pathAnimation->setEasingCurve(QEasingCurve::InOutCubic);

    connect(pathAnimation, &QVariantAnimation::valueChanged, this, [this]()
            {
                emit progressChanged();
            });

    connect(pathAnimation, &QVariantAnimation::finished, this, [this]()
            {
                pathActive = false;
                emit progressChanged();
                emit selectionAnimationFinished(selectedNode);
            });
}

void MapSelectionAnimator::start(MapNode* selected, MapNode* from)
{
    selectedNode = selected;
    fromNode = from;

    startCircleAnimation();
}

void MapSelectionAnimator::startCircleAnimation()
{
    circleActive = true;
    circleAnimation->stop();
    circleAnimation->start();
}

void MapSelectionAnimator::startPathAnimation()
{
    pathActive = true;
    pathAnimation->stop();
    pathAnimation->start();
}

bool MapSelectionAnimator::isAnimating() const
{
    return circleActive || pathActive;
}

int MapSelectionAnimator::circleSweepAngle() const
{
    return circleAnimation->currentValue().toInt();
}

bool MapSelectionAnimator::isCircleActive() const
{
    return circleActive;
}

qreal MapSelectionAnimator::pathProgress() const
{
    return pathAnimation->currentValue().toReal();
}

bool MapSelectionAnimator::isPathActive() const
{
    return pathActive;
}

MapNode* MapSelectionAnimator::getSelectedNode() const
{
    return selectedNode;
}

MapNode* MapSelectionAnimator::getFromNode() const
{
    return fromNode;
}
