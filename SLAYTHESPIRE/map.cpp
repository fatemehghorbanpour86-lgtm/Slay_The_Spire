#include "map.h"

#include <QRandomGenerator>
#include <QHash>
#include <QtGlobal>
#include <cmath>
#include <utility>

Map::Map()
    : currentAct(1),
    currentFloorIndex(0),
    currentNode(nullptr),
    nextNodeId(0)
{
    generate();
}

Map::~Map()
{
    qDeleteAll(allNodes);
}


void Map::generate()
{
    const int maxGenerationAttempts = 10;

    for (int attempt = 0; attempt < maxGenerationAttempts; ++attempt)
    {
        clear();

        generateFloors();
        generateNodes();
        generateConnections();
        assignNodeTypes();

        if (validateMap())
            break;
    }

    currentFloorIndex = 0;
    currentNode = nullptr;

    updateAvailableNodes();
}

void Map::reset()
{
    currentAct = 1;
    generate();
}

void Map::goToNextAct()
{
    if (currentAct >= TOTAL_ACTS)
        return;

    currentAct++;
    generate();
}

void Map::clear()
{
    qDeleteAll(allNodes);
    allNodes.clear();
    floors.clear();

    currentNode = nullptr;
    nextNodeId = 0;
}

void Map::generateFloors()
{
    const int totalFloors = FLOORS_PER_ACT + 1; // +1 for the Boss floor.

    for (int i = 0; i < totalFloors; ++i)
    {
        floors.append(MapFloor(i));
    }
}

void Map::generateNodes()
{
    const int bossFloorIndex = floors.size() - 1;

    for (int i = 0; i < floors.size(); ++i)
    {
        const int nodeCount = (i == bossFloorIndex)
        ? 1
        : QRandomGenerator::global()->bounded(MIN_NODES_PER_FLOOR,
                                              MAX_NODES_PER_FLOOR + 1);

        for (int column = 0; column < nodeCount; ++column)
        {
            MapNode* node = new MapNode(nextNodeId++, NodeType::Monster, i, column);
            floors[i].addNode(node);
            allNodes.append(node);
        }
    }
}

int Map::mapColumn(int column, int fromCount, int toCount) const
{
    if (fromCount <= 1)
        return 0;

    const double ratio = static_cast<double>(column) / static_cast<double>(fromCount - 1);
    const int mapped = static_cast<int>(std::round(ratio * (toCount - 1)));

    return qBound(0, mapped, toCount - 1);
}

void Map::connectNodes(MapNode* parent, MapNode* child)
{
    if (parent == nullptr || child == nullptr)
        return;

    parent->addChild(child);
    child->addParent(parent);
}

void Map::generateConnections()
{
    for (int i = 0; i < floors.size() - 1; ++i)
    {
        const QVector<MapNode*>& currentNodes = floors[i].getNodes();
        const QVector<MapNode*>& nextNodes = floors[i + 1].getNodes();

        const int currentCount = currentNodes.size();
        const int nextCount = nextNodes.size();

        QVector<QVector<int>> connections(currentCount);

        auto addEdge = [&](int c, int n) {
            if (!connections[c].contains(n)) {
                connections[c].append(n);
            }
        };

        for (int c = 0; c < currentCount; ++c) {
            addEdge(c, mapColumn(c, currentCount, nextCount));
        }
        for (int n = 0; n < nextCount; ++n) {
            addEdge(mapColumn(n, nextCount, currentCount), n);
        }

        for (int c = 0; c < currentCount; ++c) {

            int n_min = 0;
            for (int prev_c = 0; prev_c < c; ++prev_c) {
                for (int n : std::as_const(connections[prev_c])) {
                    n_min = qMax(n_min, n);
                }
            }

            int n_max = nextCount - 1;
            for (int next_c = c + 1; next_c < currentCount; ++next_c) {
                for (int n : std::as_const(connections[next_c])) {
                    n_max = qMin(n_max, n);
                }
            }

            for (int n = n_min; n <= n_max; ++n) {
                if (!connections[c].contains(n)) {
                    if (QRandomGenerator::global()->bounded(100) < 35) {
                        addEdge(c, n);
                    }
                }
            }
        }

        for (int c = 0; c < currentCount; ++c) {
            for (int n : std::as_const(connections[c])) {
                connectNodes(currentNodes[c], nextNodes[n]);
            }
        }
    }
}

NodeType Map::pickWeightedType(int floorIndex) const
{
    struct WeightedType
    {
        NodeType type;
        int weight;
    };

    const QVector<WeightedType> options = {
                                           {NodeType::Monster,  45},
                                           {NodeType::Event,    20},
                                           {NodeType::Elite,    15},
                                           {NodeType::Campfire, 12},
                                           {NodeType::Shop,      8},
                                           };

    const bool nearBoss = (floorIndex >= FLOORS_PER_ACT - 2);

    QVector<WeightedType> filtered;
    int totalWeight = 0;
    bool ItsOk = true;

    for (const WeightedType& option : options)
    {
        if (option.type == NodeType::Shop && nearBoss)
        {
            ItsOk = false;
        }

        for(int i = 0; i < guaranteedCampfireFloors.size(); ++i)
        {
            if(option.type == NodeType::Campfire && (floorIndex == guaranteedCampfireFloors[i] + 1 || floorIndex == guaranteedCampfireFloors[i] - 1))
            {
                ItsOk = false;
            }
        }
        if(ItsOk)
        {
            filtered.append(option);
            totalWeight += option.weight;
        }
    }

    int roll = QRandomGenerator::global()->bounded(totalWeight);

    for (const WeightedType& option : filtered)
    {
        if (roll < option.weight)
            return option.type;

        roll -= option.weight;
    }

    return NodeType::Monster; // Fallback, should not normally be reached.
}

bool Map::violatesAdjacencyRule(MapNode* node, NodeType candidate) const
{
    static const QVector<NodeType> restrictedGroup = {
        NodeType::Elite, NodeType::Campfire, NodeType::Treasure
    };

    if (!restrictedGroup.contains(candidate))
        return false;

    for (MapNode* parent : node->getParents())
    {
        if (restrictedGroup.contains(parent->getType()))
            return true;
    }

    const QVector<NodeType> repeatedGroup = {
        NodeType::Shop, NodeType::Campfire
    };

    if (!repeatedGroup.contains(candidate))
        return false;

    for(int i = 0; i < repeatedGroup.size(); ++i)
    {
        for (MapNode* parent : node->getParents())
        {
            if (repeatedGroup[i] == parent->getType())
                return true;
        }
    }

    return false;
}

NodeType Map::pickTypeAvoidingAdjacency(MapNode* node, int floorIndex) const
{
    const int maxAttempts = 15;

    for (int attempt = 0; attempt < maxAttempts; ++attempt)
    {
        const NodeType candidate = pickWeightedType(floorIndex);

        if (!violatesAdjacencyRule(node, candidate))
            return candidate;
    }

    return NodeType::Monster; // Safe fallback if every attempt collided.
}

void Map::assignNodeTypes()
{
    const int bossFloorIndex = floors.size() - 1;

    // Pick two distinct, non-fixed floors to be ENTIRELY Campfire floors.
    // Because every path must pass through exactly one node per floor,
    // making the whole floor a Campfire guarantees every possible path
    // crosses at least these two Campfires — satisfying the doc's rule
    // ("at least 2 Campfires per path from floor 1 to 11") deterministically
    // instead of relying on random luck.
    QVector<int> restFloorCandidates;
    for (int i = 2; i < bossFloorIndex - 2; ++i)
    {
        if (i != TREASURE_FLOOR_INDEX)
            restFloorCandidates.append(i);
    }

    for (int i = restFloorCandidates.size() - 1; i > 0; --i)
    {
        const int j = QRandomGenerator::global()->bounded(i + 1);
        restFloorCandidates.swapItemsAt(i, j);
    }

    guaranteedCampfireFloors.append(bossFloorIndex - 1);
    if (restFloorCandidates.size() >= 1)
    {
        guaranteedCampfireFloors.append(restFloorCandidates[0]);
    }

    for (int i = 0; i < floors.size(); ++i)
    {
        const QVector<MapNode*>& nodes = floors[i].getNodes();

        for (MapNode* node : nodes)
        {
            if (i == 0)
            {
                node->setType(NodeType::Monster); // Doc: Floor 1 is always Enemy.
            }
            else if (i == bossFloorIndex)
            {
                node->setType(NodeType::Boss);
            }
            else if (i == TREASURE_FLOOR_INDEX)
            {
                node->setType(NodeType::Treasure); // Doc: Floor 6 is always Treasure.
            }
            else if (guaranteedCampfireFloors.contains(i))
            {
                node->setType(NodeType::Campfire);
            }
            else
            {
                node->setType(pickTypeAvoidingAdjacency(node, i));
            }
        }
    }
}

bool Map::validateMap() const
{
    const int bossFloorIndex = floors.size() - 1;

    for (MapNode* node : allNodes)
    {
        const bool isBossFloor = (node->getFloor() == bossFloorIndex);
        const bool isFirstFloor = (node->getFloor() == 0);

        if (!isBossFloor && node->getChildren().isEmpty())
            return false;

        if (!isFirstFloor && node->getParents().isEmpty())
            return false;
    }

    return true;
}

int Map::getCurrentAct() const
{
    return currentAct;
}

int Map::getCurrentFloorIndex() const
{
    return currentFloorIndex;
}

bool Map::hasCurrentNode() const
{
    return currentNode != nullptr;
}

const MapNode* Map::getCurrentNode() const
{
    return currentNode;
}

NodeType Map::getCurrentNodeType() const
{
    // Callers should check hasCurrentNode() first; this fallback only
    // protects against misuse before the player has entered any node.
    if (currentNode == nullptr)
        return NodeType::Monster;

    return currentNode->getType();
}

QVector<const MapNode*> Map::getAvailableNodes() const
{
    QVector<const MapNode*> result;

    for (MapNode* node : allNodes)
    {
        if (node->isAvailable())
            result.append(node);
    }

    return result;
}

const QVector<MapFloor>& Map::getFloors() const
{
    return floors;
}

MapNode* Map::findNodeById(int id) const
{
    for (MapNode* node : allNodes)
    {
        if (node->getId() == id)
            return node;
    }

    return nullptr;
}

void Map::updateAvailableNodes()
{
    for (MapNode* node : std::as_const(allNodes))
    {
        node->setAvailable(false);
    }

    if (currentNode == nullptr)
    {
        // Game just started on this Act: only the first floor is selectable.
        for (MapNode* node : floors[0].getNodes())
        {
            node->setAvailable(true);
        }
    }
    else
    {
        for (MapNode* child : currentNode->getChildren())
        {
            child->setAvailable(true);
        }
    }
}

bool Map::moveToNode(int nodeId)
{
    MapNode* target = findNodeById(nodeId);

    if (target == nullptr || !target->isAvailable())
        return false;

    if (currentNode != nullptr)
        currentNode->markVisited();

    currentNode = target;
    currentNode->markVisited();
    currentFloorIndex = currentNode->getFloor();

    updateAvailableNodes();

    return true;
}

MapState Map::extractState() const
{
    MapState state;
    state.currentAct = currentAct;
    state.currentFloorIndex = currentFloorIndex;
    state.currentNodeId = (currentNode != nullptr) ? currentNode->getId() : -1;

    for (MapNode* node : allNodes)
    {
        MapNodeState nodeState;
        nodeState.id = node->getId();
        nodeState.type = node->getType();
        nodeState.floor = node->getFloor();
        nodeState.column = node->getColumn();
        nodeState.visited = node->isVisited();
        nodeState.available = node->isAvailable();

        for (MapNode* parent : node->getParents())
            nodeState.parentIds.append(parent->getId());

        for (MapNode* child : node->getChildren())
            nodeState.childIds.append(child->getId());

        state.nodes.append(nodeState);

        if (nodeState.visited)
            state.visitedNodeIds.append(nodeState.id);

        if (nodeState.available)
            state.availableNodeIds.append(nodeState.id);
    }

    return state;
}

void Map::restoreState(const MapState& state)
{
    clear();

    int maxFloorIndex = 0;
    for (const MapNodeState& nodeState : state.nodes)
        maxFloorIndex = qMax(maxFloorIndex, nodeState.floor);

    for (int i = 0; i <= maxFloorIndex; ++i)
        floors.append(MapFloor(i));

    // First pass: recreate every node with its own data (no links yet).
    QHash<int, MapNode*> nodeLookup;

    for (const MapNodeState& nodeState : state.nodes)
    {
        MapNode* node = new MapNode(nodeState.id, nodeState.type, nodeState.floor, nodeState.column);
        node->setAvailable(nodeState.available);

        if (nodeState.visited)
            node->markVisited();

        allNodes.append(node);
        floors[nodeState.floor].addNode(node);
        nodeLookup.insert(nodeState.id, node);

        nextNodeId = qMax(nextNodeId, nodeState.id + 1);
    }

    // Second pass: rebuild parent/child pointer relationships from saved IDs.
    for (const MapNodeState& nodeState : state.nodes)
    {
        MapNode* node = nodeLookup.value(nodeState.id, nullptr);

        if (node == nullptr)
            continue;

        for (int parentId : nodeState.parentIds)
        {
            if (MapNode* parent = nodeLookup.value(parentId, nullptr))
                node->addParent(parent);
        }

        for (int childId : nodeState.childIds)
        {
            if (MapNode* child = nodeLookup.value(childId, nullptr))
                node->addChild(child);
        }
    }

    currentAct = state.currentAct;
    currentFloorIndex = state.currentFloorIndex;
    currentNode = (state.currentNodeId >= 0) ? nodeLookup.value(state.currentNodeId, nullptr) : nullptr;
}