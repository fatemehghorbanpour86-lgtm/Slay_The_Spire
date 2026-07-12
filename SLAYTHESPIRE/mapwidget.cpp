#include "mapwidget.h"
#include "mapnode.h"
#include "audiomanager.h"
#include <QPen>

MapWidget::MapWidget(Map* gameMap, QWidget *parent)
    : QWidget(parent), map(gameMap)
{
    // بک‌گراند این ویجت رو کاملاً شفاف نگه می‌داریم تا عکس مپ که روی
    // MapPage ست شده از پشتش دیده بشه
    setAttribute(Qt::WA_TranslucentBackground);
    setStyleSheet("background: transparent;");

    createNodeButtons();
}

void MapWidget::createNodeButtons() {
    const QVector<MapFloor>& floors = map->getFloors();
    int totalFloors = floors.size();

    int mapHeight = ((totalFloors - 1) * VERTICAL_SPACING) + 150 + VERTICAL_SPACING_BOSS;

    // setFixedSize به‌جای setMinimumSize: عرض همین الان قطعی می‌شه
    // و دیگه لازم نیست منتظر layout بمونیم تا width() درست برگرده
    setFixedSize(MAP_CONTENT_WIDTH, mapHeight);

    for (int i = 0; i < totalFloors; ++i) {
        const QVector<MapNode*>& nodes = floors[i].getNodes();
        int nodeCount = nodes.size();

        // همیشه بر اساس عرض ثابت مپ سنتر می‌کنیم، نه this->width() لحظه‌ای
        // int rowWidth = nodeCount * HORIZONTAL_SPACING;
        // int startX = (MAP_CONTENT_WIDTH - rowWidth) / 2 + (HORIZONTAL_SPACING - NODE_SIZE) / 2;
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

    if (node->getType() == NodeType::Boss)
    {
        if(map->getCurrentAct() == 1)
            stateStr = "hexaghost";
        else
            stateStr = "champ";
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
    }
    return "";
}

void MapWidget::paintEvent(QPaintEvent *event) {
    Q_UNUSED(event)

    // توجه: دیگه drawPrimitive(PE_Widget) صدا نمی‌زنیم چون اون یه
    // بک‌گراند مات می‌کشید و جلوی دیده‌شدن عکس مپِ MapPage رو می‌گرفت.
    // این ویجت باید کاملاً شفاف بمونه، فقط خطوط مسیر رو می‌کشیم.

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

                QColor lineColor = QColor(24, 28, 34);

                if (parentNode->isVisited() && childNode->isVisited()) {
                    lineColor = QColor(186, 177, 0);
                } else if ((parentNode == map->getCurrentNode() || parentNode->isVisited()) && childNode->isAvailable()) {
                    lineColor = QColor(121, 180, 255);
                }

                painter.setPen(QPen(lineColor, 4, Qt::DashLine, Qt::RoundCap));
                painter.drawLine(p1, p2);
            }
        }
    }

    const MapNode* current = map->getCurrentNode();

    if (current && nodeButtons.contains(const_cast<MapNode*>(current)))
    {
        QPushButton* btn = nodeButtons.value(const_cast<MapNode*>(current));

        QRect rect = btn->geometry().adjusted(-4, -4, 4, 4);

        QPen pen(QColor(255, 215, 0));
        pen.setWidth(4);

        painter.setPen(pen);
        painter.setBrush(Qt::NoBrush);

        painter.drawEllipse(rect);
    }
}