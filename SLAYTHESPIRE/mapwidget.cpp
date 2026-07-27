#include "mapwidget.h"
#include "mapnode.h"
#include "audiomanager.h"
#include <QPen>
#include <QDir>
#include <QCoreApplication>

MapWidget::MapWidget(Map* gameMap, QWidget *parent)
    : QWidget(parent), map(gameMap)
{
    // بک‌گراند این ویجت رو کاملاً شفاف نگه می‌داریم تا عکس مپ که روی
    // MapPage ست شده از پشتش دیده بشه
    setAttribute(Qt::WA_TranslucentBackground);
    setStyleSheet("background: transparent;");

    selectionAnimator = new MapSelectionAnimator(this);

    connect(selectionAnimator, &MapSelectionAnimator::progressChanged,
            this, &MapWidget::onAnimationProgress);
    connect(selectionAnimator, &MapSelectionAnimator::circleFinished,
            this, &MapWidget::onCircleFinished);
    connect(selectionAnimator, &MapSelectionAnimator::selectionAnimationFinished,
            this, &MapWidget::onSelectionFinished);

    createNodeButtons();
}

void MapWidget::createNodeButtons() {
    const QVector<MapFloor>& floors = map->getFloors();
    int totalFloors = floors.size();

    int mapHeight = ((totalFloors - 1) * VERTICAL_SPACING) + 150 + VERTICAL_SPACING_BOSS;


    setFixedSize(MAP_CONTENT_WIDTH, mapHeight);

    for (int i = 0; i < totalFloors; ++i) {
        const QVector<MapNode*>& nodes = floors[i].getNodes();
        int nodeCount = nodes.size();

        int startX = 0;
        if(nodeCount == 1)
        {
            startX = (MAP_CONTENT_WIDTH / 2) - (NODE_SIZE * 3.5);
        }
        else if(nodeCount == 2)
        {
            startX = (MAP_CONTENT_WIDTH / 2) - (HORIZONTAL_SPACING / 2 + NODE_SIZE * 1.5);
        }
        else if(nodeCount == 3)
        {
            startX = (MAP_CONTENT_WIDTH / 2) - (HORIZONTAL_SPACING + NODE_SIZE * 1.5);
        }
        else
        {
            startX = (MAP_CONTENT_WIDTH / 2) - ((HORIZONTAL_SPACING * 1.5) + (NODE_SIZE * 1.5));
        }

        int yPos = 0;
        if(nodeCount == 1)
        {
            yPos = mapHeight - ((i + 1) * VERTICAL_SPACING + VERTICAL_SPACING_BOSS);
        }
        else
        {
            yPos = mapHeight - ((i + 1) * VERTICAL_SPACING);
        }

        for (int j = 0; j < nodeCount; ++j) {
            MapNode* node = nodes[j];
            QPushButton* btn = new QPushButton(this);
            if(node->getType() == NodeType::Boss)
            {
                btn->setFixedSize(NODE_SIZE * 5, NODE_SIZE * 5);
            }
            else
            {
                btn->setFixedSize(NODE_SIZE, NODE_SIZE);
            }
            btn->setStyleSheet("background: transparent; border: none;");

            int xPos = startX + (j * HORIZONTAL_SPACING);
            btn->move(xPos, yPos);
            btn->setToolTip(getTooltipText(node->getType()));

            connect(btn, &QPushButton::pressed,
                    this, []()
                    {
                        AudioManager::instance().play(AudioManager::Sound::ButtonClick);
                    });

            connect(btn, &QPushButton::clicked, this, [this, node]() {
                if (node->isAvailable()) {
                    // emit nodeClicked(node->getId());
                    startNodeSelection(node);
                }
            });

            nodeButtons.insert(node, btn);
        }
    }
    refreshUI();
}

void MapWidget::startNodeSelection(MapNode* node)
{
    if (node == nullptr || !node->isAvailable())
        return;

    if (selectionAnimator->isAnimating())
        return;

    emit animationStarted();

    // Step 1: disable every Node so nothing else can be clicked while
    // the sequence plays out.
    for (auto it = nodeButtons.begin(); it != nodeButtons.end(); ++it)
    {
        it.value()->setEnabled(false);
    }

    // Step 2 onward is entirely driven by MapSelectionAnimator; the
    // resulting emitted signals below take care of the rest.
    selectionAnimator->start(node, const_cast<MapNode*>(map->getCurrentNode()));
}

void MapWidget::onAnimationProgress()
{
    update();
}

void MapWidget::onCircleFinished()
{
    // Instant (non-animated) visual changes: every Node that was
    // Available a moment ago, other than the one just chosen, now
    // looks Locked; the paths leading to them turn gray on next paint.
    animationLockActive = true;
    temporarilyLockedNodes.clear();

    MapNode* selected = selectionAnimator->getSelectedNode();
    const QVector<const MapNode*> availableNodes = map->getAvailableNodes();

    for (const MapNode* constNode : availableNodes)
    {
        MapNode* node = const_cast<MapNode*>(constNode);

        if (node != selected)
        {
            temporarilyLockedNodes.insert(node);
        }
    }

    updateNodeStyles();
    update();
}

void MapWidget::onSelectionFinished(MapNode* node)
{
    animationLockActive = false;
    temporarilyLockedNodes.clear();

    emit animationFinished();

    if (node != nullptr)
    {
        // Exactly the same signal, with exactly the same meaning, that
        // used to be emitted directly on click. MapPage's handling of
        // it (moveToNode + refreshUI + nodeEntered) is untouched.
        emit nodeClicked(node->getId());
    }
}

void MapWidget::updateNodeStyles()
{
    for (auto it = nodeButtons.begin(); it != nodeButtons.end(); ++it)
    {
        MapNode* node = it.key();
        QPushButton* btn = it.value();

        btn->setStyleSheet(getNodeStyleSheet(node));
    }
}

void MapWidget::refreshUI() {

    QPixmap pixmap(":/cursor.png");
    QPixmap scaledPixmap = pixmap.scaled(30, 40, Qt::KeepAspectRatio, Qt::SmoothTransformation);
    QCursor customCursor(scaledPixmap, 0, 0);
    this->setCursor(customCursor);

    QString baseDir = QCoreApplication::applicationDirPath();
    QString BtnPath = QDir(baseDir).filePath("assets/image/cursorBtn.png");
    QPixmap buttonHoverPixmap(BtnPath);
    QPixmap scaledHover = buttonHoverPixmap.scaled(40, 61, Qt::KeepAspectRatio, Qt::SmoothTransformation);
    QCursor buttonHoverCursor(scaledHover, scaledHover.width() / 2, 10);

    for (auto it = nodeButtons.begin(); it != nodeButtons.end(); ++it) {
        MapNode* node = it.key();
        QPushButton* btn = it.value();

        btn->setStyleSheet(getNodeStyleSheet(node));

        if (node->isAvailable()) {
            btn->setEnabled(true);
            btn->setCursor(buttonHoverCursor);
        } else {
            btn->setEnabled(false);
            btn->setCursor(customCursor);
        }
    }
    update();
}

QString MapWidget::getNodeStyleSheet(MapNode* node) {
    QString stateStr;

    if (node->getType() == NodeType::Boss)
    {
        if(map->getCurrentAct() == 1)
            stateStr = "hexaghost";
        else
            stateStr = "champ";
    }
    else if (temporarilyLockedNodes.contains(node))
    {
        // Purely visual, temporary override while a selection animation
        // is in progress - the real Map state (node->isAvailable()) is
        // left completely untouched.
        stateStr = "locked";
    }
    else
    {
        if (node->isVisited()) stateStr = "visited";
        else if (node->isAvailable()) stateStr = "available";
        else stateStr = "locked";
    }

    QString typeStr;
    switch (node->getType()) {
    case NodeType::Monster: typeStr = "enemy"; break;
    case NodeType::Elite: typeStr = "elite"; break;
    case NodeType::Campfire: typeStr = "campfire"; break;
    case NodeType::Event: typeStr = "event"; break;
    case NodeType::Treasure: typeStr = "treasure"; break;
    case NodeType::Shop: typeStr = "shop"; break;
    case NodeType::Boss: typeStr = "boss"; break;
    case NodeType::MiniGame: typeStr = "minigame"; break;
    }

    QString imagePath = QString(":/map/%1_%2.png").arg(typeStr, stateStr);

    return QString(
               "QPushButton {"
               "border-image: url(%1);"
               "border: none;"
               "background: transparent;"
               "}"
               "QPushButton:pressed { "
               "   margin: 5px 5px 5px 5px; "
               "}"
               ).arg(imagePath);
}

QString MapWidget::getTooltipText(NodeType type) {
    switch (type) {
    case NodeType::Monster: return "Normal Enemy";
    case NodeType::Elite: return "Elite Enemy";
    case NodeType::Campfire: return "Rest Site";
    case NodeType::Event: return "Unknown";
    case NodeType::Treasure: return "Treasure";
    case NodeType::Shop: return "Merchant";
    case NodeType::Boss: return "Boss";
    case NodeType::MiniGame: return "Mini Game";
    }
    return "";
}

void MapWidget::paintEvent(QPaintEvent *event) {
    Q_UNUSED(event)

    QPainter painter(this);
    painter.setRenderHint(QPainter::Antialiasing);

    MapNode* selectedNode = selectionAnimator->getSelectedNode();
    MapNode* fromNode = selectionAnimator->getFromNode();
    const bool selectionInProgress = selectionAnimator->isAnimating();

    const QVector<MapFloor>& floors = map->getFloors();
    for (int i = 0; i < floors.size(); ++i) {
        const QVector<MapNode*>& nodes = floors[i].getNodes();
        for (MapNode* parentNode : nodes) {
            QPushButton* parentBtn = nodeButtons.value(parentNode);
            if (!parentBtn) continue;

            QPoint p1 = parentBtn->geometry().center();

            const QVector<MapNode*>& children = parentNode->getChildren();
            for (MapNode* childNode : children) {
                QPushButton* childBtn = nodeButtons.value(childNode);
                if (!childBtn) continue;

                QPoint p2 = childBtn->geometry().center();

                QColor lineColor = QColor(24, 28, 34);
                bool wasAvailableEdge = false;

                if (parentNode->isVisited() && childNode->isVisited()) {
                    lineColor = QColor(186, 177, 0);
                } else if ((parentNode == map->getCurrentNode() || parentNode->isVisited()) && childNode->isAvailable()) {
                    lineColor = QColor(121, 180, 255);
                    wasAvailableEdge = true;
                }

                // The single edge currently being animated to Gold
                // (current Node -> the Node the player just picked).
                const bool isSelectedEdge = selectionInProgress &&
                                            fromNode != nullptr &&
                                            parentNode == fromNode && childNode == selectedNode;

                if (animationLockActive && wasAvailableEdge && !isSelectedEdge)
                {
                    // Every other path that used to be Available (blue)
                    // becomes gray for the rest of the selection sequence.
                    lineColor = QColor(24, 28, 34);
                }

                if (isSelectedEdge)
                {
                    const qreal t = selectionAnimator->isPathActive()
                    ? selectionAnimator->pathProgress()
                    : 0.0;

                    const QPointF travelPoint = QPointF(p1) + (QPointF(p2 - p1) * t);

                    // Traveling light: the portion already covered is
                    // gold, the remainder still shows the path's normal
                    // (pre-selection) color.
                    painter.setPen(QPen(QColor(186, 177, 0), 4, Qt::DashLine, Qt::RoundCap));
                    painter.drawLine(p1, travelPoint.toPoint());

                    painter.setPen(QPen(QColor(121, 180, 255), 4, Qt::DashLine, Qt::RoundCap));
                    painter.drawLine(travelPoint.toPoint(), p2);
                }
                else
                {
                    painter.setPen(QPen(lineColor, 4, Qt::DashLine, Qt::RoundCap));
                    painter.drawLine(p1, p2);
                }
            }
        }
    }

    const MapNode* current = map->getCurrentNode();

    if (current &&!selectionInProgress && nodeButtons.contains(const_cast<MapNode*>(current)))
    {
        QPushButton* btn = nodeButtons.value(const_cast<MapNode*>(current));

        QRect rect = btn->geometry().adjusted(-14, -14, 14, 14);

        QPen pen(QColor(255, 215, 0));
        pen.setWidth(6);

        painter.setPen(pen);
        painter.setBrush(Qt::NoBrush);

        painter.drawEllipse(rect);
    }

    // Golden selection ring, only while it is actively being drawn
    // (i.e. before the player's chosen Node is actually entered).
    if (selectionInProgress && selectedNode != nullptr)
    {
        QPushButton* selectedBtn = nodeButtons.value(selectedNode);

        if (selectedBtn)
        {
            QRect ringRect = selectedBtn->geometry().adjusted(-14, -14, 14, 14);

            QPen ringPen(QColor(255, 215, 0));
            ringPen.setWidth(6);
            ringPen.setCapStyle(Qt::RoundCap);

            painter.setPen(ringPen);
            painter.setBrush(Qt::NoBrush);

            // QPainter angles: 0 = 3 o'clock, positive span = counter-
            // clockwise - matches "starts at 0, sweeps counter-clockwise".
            const int sweepAngle = selectionAnimator->isCircleActive()
                                       ? selectionAnimator->circleSweepAngle()
                                       : 360;
            painter.drawArc(ringRect, 0, sweepAngle * 16);
        }
    }
}