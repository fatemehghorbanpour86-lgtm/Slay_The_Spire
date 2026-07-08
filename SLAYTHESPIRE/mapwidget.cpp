#include "mapwidget.h"
#include <QPen>

MapWidget::MapWidget(Map* gameMap, QWidget *parent)
    : QWidget(parent), map(gameMap)
{
    createNodeButtons();
}

void MapWidget::createNodeButtons() {
    const int nodeSize = 64;
    const int verticalSpacing = 120;
    const int horizontalSpacing = 100;

    const QVector<MapFloor>& floors = map->getFloors();
    int totalFloors = floors.size();

    int mapHeight = totalFloors * verticalSpacing + 100;
    setMinimumSize(800, mapHeight);

    for (int i = 0; i < totalFloors; ++i) {
        const QVector<MapNode*>& nodes = floors[i].getNodes();
        int nodeCount = nodes.size();

        int startX = (this->width() - (nodeCount * horizontalSpacing)) / 2;

        int yPos = mapHeight - ((i + 1) * verticalSpacing);

        for (int j = 0; j < nodeCount; ++j) {
            MapNode* node = nodes[j];
            QPushButton* btn = new QPushButton(this);
            btn->setFixedSize(nodeSize, nodeSize);

            int xPos = startX + (j * horizontalSpacing);
            btn->move(xPos, yPos);
            btn->setToolTip(getTooltipText(node->getType()));

            connect(btn, &QPushButton::clicked, this, [this, node]() {
                if (node->isAvailable()) {
                    emit nodeClicked(node->getId());
                }
            });

            nodeButtons.insert(node, btn);
        }
    }
    refreshUI();
}

void MapWidget::refreshUI() {
    for (auto it = nodeButtons.begin(); it != nodeButtons.end(); ++it) {
        MapNode* node = it.key();
        QPushButton* btn = it.value();

        btn->setStyleSheet(getNodeStyleSheet(node));

        if (node->isAvailable()) {
            btn->setEnabled(true);
            btn->setCursor(Qt::PointingHandCursor);
        } else {

            btn->setEnabled(false);
            btn->setCursor(Qt::ArrowCursor);
        }
    }
    update();
}

QString MapWidget::getNodeStyleSheet(MapNode* node) {
    QString stateStr;

    // وضعیت منطقی گره
    if (node->getType() == NodeType::Boss)
    {
        if(map->getCurrentAct() == 1)
        {
            stateStr = "hexaghost";
        }
        else stateStr = "champ";
    }
    else
    {
        if (node->isVisited()) stateStr = "visited";
        else if (node->isAvailable()) stateStr = "available";
        else stateStr = "locked";
    }

    // نوع گره
    QString typeStr;
    switch (node->getType()) {
    case NodeType::Monster: typeStr = "enemy"; break;
    case NodeType::Elite: typeStr = "elite"; break;
    case NodeType::Campfire: typeStr = "campfire"; break;
    case NodeType::Event: typeStr = "event"; break;
    case NodeType::Treasure: typeStr = "treasure"; break;
    case NodeType::Shop: typeStr = "shop"; break;
    case NodeType::Boss: typeStr = "boss"; break;
    }

    QString imagePath = QString("map/%1_%2.png").arg(typeStr, stateStr);

    QString border = (map->getCurrentNode() == node) ? "border: 3px solid gold; border-radius: 32px;" : "border: none;";

    return QString("QPushButton { background-image: url(%1); background-repeat: no-repeat; background-position: center; background-color: transparent; %2 }").arg(imagePath, border);
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
    }
    return "";
}

void MapWidget::paintEvent(QPaintEvent *event) {

    QStyleOption opt;
    opt.initFrom(this);
    QPainter p(this);
    style()->drawPrimitive(QStyle::PE_Widget, &opt, &p, this);


    Q_UNUSED(event)
    QPainter painter(this);
    painter.setRenderHint(QPainter::Antialiasing);

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

                // تعیین رنگ مسیر
                QColor lineColor = QColor(100, 100, 100); // خاکستری (Locked)

                if (parentNode->isVisited() && childNode->isVisited()) {
                    lineColor = QColor(255, 215, 0); // طلایی (Visited Path)
                } else if ((parentNode == map->getCurrentNode() || parentNode->isVisited()) && childNode->isAvailable()) {
                    lineColor = QColor(100, 150, 255); // آبی (Available)
                }

                painter.setPen(QPen(lineColor, 4, Qt::DashLine, Qt::RoundCap));
                painter.drawLine(p1, p2);
            }
        }
    }
}