#include "mapnode.h"


MapNode::MapNode(int id, NodeType type, int floor, int column)
    : id(id),
    type(type),
    floor(floor),
    column(column),
    visited(false),
    available(false)
{
}

int MapNode::getId() const
{
    return id;
}

NodeType MapNode::getType() const
{
    return type;
}

int MapNode::getFloor() const
{
    return floor;
}

int MapNode::getColumn() const
{
    return column;
}

void MapNode::setType(NodeType newType)
{
    type = newType;
}

bool MapNode::isVisited() const
{
    return visited;
}

bool MapNode::isAvailable() const
{
    return available;
}

void MapNode::markVisited()
{
    visited = true;
}

void MapNode::setAvailable(bool value)
{
    available = value;
}

void MapNode::addParent(MapNode* parent)
{
    if (parent == nullptr)
        return;

    if (!parents.contains(parent))
        parents.append(parent);
}

void MapNode::addChild(MapNode* child)
{
    if (child == nullptr)
        return;

    if (!children.contains(child))
        children.append(child);
}

const QVector<MapNode*>& MapNode::getParents() const
{
    return parents;
}

const QVector<MapNode*>& MapNode::getChildren() const
{
    return children;
}