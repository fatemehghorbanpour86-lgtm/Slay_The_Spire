#ifndef MAPNODE_H
#define MAPNODE_H
#include <QVector>

enum class NodeType
{
    Monster,
    Elite,
    Campfire,
    Event,
    Treasure,
    Shop,
    Boss
};

/*
 * MapNode
 * -------
 * Represents a single room (node) on the map.
 *
 * Responsibilities:
 * - Store this node's own information, such as:
 *     - room type
 *     - floor and column position
 *     - visited state
 *     - available state
 * - Keep links to its parent and child nodes so the map graph
 *   can be traversed.
 *
 * The parent/child pointers are non-owning. MapNode does not create
 * or delete other nodes; the Map class owns and manages all MapNodes.
 *
 * MapNode also does not know how the map is generated or managed.
 * It only stores information about itself (including its own floor index),
 * making the class simple, reusable, and easy to test independently.
 */

class MapNode
{
public:
    MapNode(int id, NodeType type, int floor, int column);

    int getId() const;
    NodeType getType() const;
    int getFloor() const;
    int getColumn() const;

    void setType(NodeType type);

    // Returns whether the player has already visited this node.
    // This flag is set only by Map::moveToNode().
    bool isVisited() const;

    // Returns whether the player can currently move to this node.
    // The UI should allow interaction only with available nodes.
    bool isAvailable() const;

    // Marks this node as visited.
    // Intended to be called only by Map::moveToNode().
    void markVisited();

    // Sets whether this node is currently available.
    // Intended to be called only by Map::updateAvailableNodes().
    void setAvailable(bool value);

    void addParent(MapNode* parent);
    void addChild(MapNode* child);

    const QVector<MapNode*>& getParents() const;
    const QVector<MapNode*>& getChildren() const;


private:
    int id;

    NodeType type;
    int floor;
    int column;
    bool visited;
    bool available;

    QVector<MapNode*> parents;
    QVector<MapNode*> children;
};

#endif // MAPNODE_H
