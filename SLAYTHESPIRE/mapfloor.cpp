#include "mapfloor.h"
#include "mapnode.h"

MapFloor::MapFloor(int floorNumber)
    : floorNumber(floorNumber)
{
}

int MapFloor::getFloorNumber() const
{
    return floorNumber;
}

void MapFloor::setFloorNumber(int number)
{
    floorNumber = number;
}

void MapFloor::addNode(MapNode* node)
{
    if (node == nullptr)
        return;

    nodes.append(node);
}

const QVector<MapNode*>& MapFloor::getNodes() const
{
    return nodes;
}

int MapFloor::getNodeCount() const
{
    return nodes.size();
}

MapNode* MapFloor::getNodeByColumn(int column) const
{
    if (column < 0 || column >= nodes.size())
        return nullptr;

    return nodes[column];
}