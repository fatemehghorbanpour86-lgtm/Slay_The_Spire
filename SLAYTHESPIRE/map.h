#ifndef MAP_H
#define MAP_H
#include "mapfloor.h"
#include "mapnode.h"
#include <QVector>

/*
 * MapNodeState and MapState are simple data classes used only for
 * saving and loading the map.
 *
 * They do not store pointers. Instead, they store node IDs
 * (parentIds and childIds), because pointers are only valid while
 * the game is running. After restarting the game, memory addresses
 * change, so saved pointers cannot be used.
 *
 * During loading, the game recreates the nodes and reconnects them
 * using the saved IDs.
 */

struct MapNodeState
{
    int id = -1;
    NodeType type = NodeType::Monster;
    int floor = 0;
    int column = 0;
    bool visited = false;
    bool available = false;
    QVector<int> parentIds;
    QVector<int> childIds;
};

struct MapState
{
    int currentAct = 1;
    int currentFloorIndex = 0;
    int currentNodeId = -1; // -1 means the player has not entered any node yet.

    QVector<int> visitedNodeIds;
    QVector<int> availableNodeIds;

    QVector<MapNodeState> nodes; // Full graph, floor/column/type/connections included.
};



/*
 * Map
 * ---
 * Owns and manages the entire map for the current act.
 * It is responsible for generating the map, handling player movement,
 * and providing map information to other systems.
 *
 * Map is the only class that owns all MapNode objects and is responsible
 * for their lifetime. Other classes (MapFloor, parent/child links) only
 * store non-owning pointers.
 *
 * Map does not contain any gameplay logic for Battle, Shop, Campfire,
 * Event, or UI. It only reports the type of the current node.
 * The GameManager decides what happens based on that information.
 */

class Map
{
public:
    Map();
    ~Map();

    // Copying is disabled because Map owns all MapNodes.
    // Use extractState()/restoreState() for a deep copy if needed.
    Map(const Map&) = delete;
    Map& operator=(const Map&) = delete;

    void generate();     // (Re)generates the map for the current Act.
    void reset();         // Full reset back to Act 1, then generates.
    void goToNextAct();   // Advances currentAct and regenerates the map. Does nothing if already on the last Act.

    int getCurrentAct() const;
    int getCurrentFloorIndex() const;

    // True once the player has entered at least one node this Act
    // (false right after generate(), before the first move).
    bool hasCurrentNode() const;
    const MapNode* getCurrentNode() const;
    NodeType getCurrentNodeType() const;

    // All nodes the player is allowed to move to right now
    QVector<const MapNode*> getAvailableNodes() const;

    // Full floor-by-floor layout of the map, read-only, for drawing the
    // whole map in the UI
    const QVector<MapFloor>& getFloors() const;

    bool moveToNode(int nodeId);

    // Packages the entire current map state (act, floor, current node,
    // full graph) into a plain, pointer-free struct for SaveManager.
    MapState extractState() const;

    // Rebuilds the map (floors, nodes, and their connections) from a
    // previously saved MapState, restoring position and progress exactly.
    void restoreState(const MapState& state);

private:

    // Creates the 11 empty MapFloor slots (10 normal + 1 boss) for the Act.
    void generateFloors();

    // Fills each floor with 2-4 nodes (1 node for the boss floor), each
    // getting a unique id; nodes start as a placeholder Monster type.
    void generateNodes();


    // Wires parent/child links between consecutive floors so every node
    // has at least one child and one parent.
    void generateConnections();

    // Decides the final NodeType of every node: fixed floors (first,
    // boss, treasure, two guaranteed-Campfire floors) plus weighted
    // random choices elsewhere, respecting the adjacency rule.
    void assignNodeTypes();

    // Checks the just-generated graph for dead ends (a missing child or
    // parent); generate() retries if this returns false.
    bool validateMap() const;

    // Deletes every node and empties floors/allNodes;
    void clear();

    // Recomputes which nodes are selectable: floor 0 at game start, or
    // the children of currentNode after any move.
    void updateAvailableNodes();

    MapNode* findNodeById(int id) const;

    // Links two nodes together (parent -> child and child -> parent)
    void connectNodes(MapNode* parent, MapNode* child);

    // Proportionally converts a column index from one floor's node count
    // to the equivalent column in a floor with a different node count.
    int mapColumn(int column, int fromCount, int toCount) const;

    // Rolls a random NodeType using the doc's weighted distribution,
    // excluding Shop on the two floors right before the boss.
    NodeType pickWeightedType(int floorIndex) const;

    // Repeatedly rolls pickWeightedType until it finds one that doesn't
    // violate the adjacency rule for this node's parents (or falls back
    // to Monster after enough failed attempts).
    NodeType pickTypeAvoidingAdjacency(MapNode* node, int floorIndex) const;

    // True if placing "candidate" on this node would put two Elite/
    // Campfire/Treasure rooms back-to-back along any path.
    bool violatesAdjacencyRule(MapNode* node, NodeType candidate) const;

    static const int FLOORS_PER_ACT = 10;
    static const int TOTAL_ACTS = 2;
    static const int MIN_NODES_PER_FLOOR = 2;
    static const int MAX_NODES_PER_FLOOR = 4;
    static const int TREASURE_FLOOR_INDEX = 5; // 0-indexed => "Floor 6" in the doc.

    QVector<MapFloor> floors;
    QVector<MapNode*> allNodes;

    int currentAct;
    int currentFloorIndex;
    MapNode* currentNode;

    QVector<int> guaranteedCampfireFloors;

    int nextNodeId;
};

#endif // MAP_H
