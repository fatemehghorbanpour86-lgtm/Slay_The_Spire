#ifndef MAPFLOOR_H
#define MAPFLOOR_H
#include "mapnode.h"
/*
 * MapFloor
 * --------
 * Represents a single floor (horizontal row) of the map.
 *
 * Responsibilities:
 * - Store references to all MapNodes that belong to this floor.
 * - Provide convenient access to those nodes (by index/column, node count, etc.).
 *
 * MapFloor does not own the MapNodes it references. It only stores
 * non-owning pointers. The Map class is responsible for creating,
 * owning, and destroying all MapNode objects.
 *
 * Because MapFloor does not manage memory, it remains lightweight,
 * easy to copy, and safe to store inside containers such as
 * QVector<MapFloor> without risking double deletion.
 */


class MapFloor
{
public:
    explicit MapFloor(int floorNumber = 0);

    int getFloorNumber() const;
    int getNodeCount() const;

    void setFloorNumber(int number);

    void addNode(MapNode* node);

    const QVector<MapNode*>& getNodes() const;

    MapNode* getNodeByColumn(int column) const;

private:
    int floorNumber;
    QVector<MapNode*> nodes;
};

#endif // MAPFLOOR_H
