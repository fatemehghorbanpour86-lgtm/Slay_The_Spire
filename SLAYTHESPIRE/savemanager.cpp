#include "savemanager.h"
#include "player.h"
#include "map.h"
#include "character.h"
#include "effect.h"
#include "card.h"
#include "masterdeck.h"
#include "relicsystem.h"
#include "relic.h"

#include <QFile>
#include <QJsonDocument>
#include <QJsonParseError>

// ============================================================
//  Paths
// ============================================================

QString SaveManager::usersFilePath()
{
    return QStringLiteral("users.json");
}

QString SaveManager::saveFilePath(const QString& username)
{
    return QStringLiteral("save_%1.json").arg(username);
}

// ============================================================
//  Low-level JSON file IO
// ============================================================

bool SaveManager::readJsonFile(const QString& path, QJsonObject& outObject)
{
    QFile file(path);

    if (!file.open(QIODevice::ReadOnly))
        return false;

    QByteArray raw = file.readAll();
    file.close();

    QJsonParseError parseError;
    QJsonDocument doc = QJsonDocument::fromJson(raw, &parseError);

    if (parseError.error != QJsonParseError::NoError || !doc.isObject())
        return false;

    outObject = doc.object();
    return true;
}

bool SaveManager::writeJsonFile(const QString& path, const QJsonObject& object)
{
    QFile file(path);

    if (!file.open(QIODevice::WriteOnly | QIODevice::Truncate))
        return false;

    QJsonDocument doc(object);
    qint64 written = file.write(doc.toJson(QJsonDocument::Indented));
    file.close();

    return written >= 0;
}

// ============================================================
//  Login / Register
// ============================================================

SaveManager::AuthResult SaveManager::authenticate(const QString& username, const QString& password)
{
    if (username.isEmpty())
        return AuthResult::Error;

    QJsonObject usersRoot;

    if (QFile::exists(usersFilePath()))
    {
        if (!readJsonFile(usersFilePath(), usersRoot))
            return AuthResult::Error;
    }

    QJsonObject usersObj = usersRoot.value("users").toObject();

    if (usersObj.contains(username))
    {
        const QString storedPassword = usersObj.value(username).toString();

        if (storedPassword == password)
            return AuthResult::LoggedIn;

        return AuthResult::WrongPassword;
    }

    // کاربر جدید: ثبت‌نام (پسورد به صورت متن ساده ذخیره می‌شود)
    usersObj.insert(username, password);
    usersRoot.insert("users", usersObj);

    if (!writeJsonFile(usersFilePath(), usersRoot))
        return AuthResult::Error;

    return AuthResult::Registered;
}

bool SaveManager::hasSaveFile(const QString& username)
{
    return QFile::exists(saveFilePath(username));
}

bool SaveManager::deleteSaveFile(const QString& username)
{
    if (username.isEmpty())
        return false;

    QString path = saveFilePath(username);

    if (!QFile::exists(path))
        return true; // چیزی برای پاک کردن نبود، خطا نیست

    return QFile::remove(path);
}

// ============================================================
//  Effect <-> JSON
// ============================================================

QJsonObject SaveManager::effectToJson(const EffectSaveData& effect)
{
    QJsonObject obj;
    obj["type"] = static_cast<int>(effect.type);
    obj["category"] = static_cast<int>(effect.category);
    obj["amount"] = effect.amount;
    obj["duration"] = effect.duration;
    return obj;
}

EffectSaveData SaveManager::effectFromJson(const QJsonObject& obj)
{
    EffectSaveData effect;
    effect.type = static_cast<Effect::Type>(obj.value("type").toInt());
    effect.category = static_cast<Effect::Category>(obj.value("category").toInt());
    effect.amount = obj.value("amount").toInt();
    effect.duration = obj.value("duration").toInt(-1);
    return effect;
}

// ============================================================
//  Card <-> JSON
// ============================================================

QJsonObject SaveManager::cardToJson(const CardSaveData& card)
{
    QJsonObject obj;
    obj["cardId"] = card.cardId;
    obj["type"] = static_cast<int>(card.type);
    obj["isUpgraded"] = card.isUpgraded;
    return obj;
}

CardSaveData SaveManager::cardFromJson(const QJsonObject& obj)
{
    CardSaveData card;
    card.cardId = obj.value("cardId").toString();
    card.type = static_cast<CardType>(obj.value("type").toInt());
    card.isUpgraded = obj.value("isUpgraded").toBool();
    return card;
}

QJsonArray SaveManager::cardListToJson(const QVector<CardSaveData>& cards)
{
    QJsonArray arr;
    for (const CardSaveData& c : cards)
        arr.append(cardToJson(c));
    return arr;
}

QVector<CardSaveData> SaveManager::cardListFromJson(const QJsonArray& arr)
{
    QVector<CardSaveData> cards;
    for (const QJsonValue& v : arr)
        cards.append(cardFromJson(v.toObject()));
    return cards;
}

// ============================================================
//  MasterDeck <-> JSON
// ============================================================

QJsonObject SaveManager::masterDeckToJson(const MasterDeckSaveData& deck)
{
    QJsonObject obj;
    obj["cards"] = cardListToJson(deck.cards);
    return obj;
}

MasterDeckSaveData SaveManager::masterDeckFromJson(const QJsonObject& obj)
{
    MasterDeckSaveData deck;
    deck.cards = cardListFromJson(obj.value("cards").toArray());
    return deck;
}

// ============================================================
//  Relic <-> JSON
// ============================================================

QJsonObject SaveManager::relicToJson(const RelicSaveData& relic)
{
    QJsonObject obj;
    obj["id"] = static_cast<int>(relic.id);
    obj["counter"] = relic.counter;
    return obj;
}

RelicSaveData SaveManager::relicFromJson(const QJsonObject& obj)
{
    RelicSaveData relic;
    relic.id = static_cast<RelicId>(obj.value("id").toInt());
    relic.counter = obj.value("counter").toInt(-1);
    return relic;
}

QJsonArray SaveManager::relicListToJson(const QVector<RelicSaveData>& relics)
{
    QJsonArray arr;
    for (const RelicSaveData& r : relics)
        arr.append(relicToJson(r));
    return arr;
}

QVector<RelicSaveData> SaveManager::relicListFromJson(const QJsonArray& arr)
{
    QVector<RelicSaveData> relics;
    for (const QJsonValue& v : arr)
        relics.append(relicFromJson(v.toObject()));
    return relics;
}

// ============================================================
//  Character / Player <-> JSON
// ============================================================

QJsonObject SaveManager::characterToJson(const CharacterSaveData& data)
{
    QJsonObject obj;
    obj["currentHealth"] = data.currentHealth;
    obj["maxHealth"] = data.maxHealth;

    QJsonArray effectsArr;
    for (const EffectSaveData& e : data.effects)
        effectsArr.append(effectToJson(e));
    obj["effects"] = effectsArr;

    return obj;
}

CharacterSaveData SaveManager::characterFromJson(const QJsonObject& obj)
{
    CharacterSaveData data;
    data.currentHealth = obj.value("currentHealth").toInt();
    data.maxHealth = obj.value("maxHealth").toInt();

    for (const QJsonValue& v : obj.value("effects").toArray())
        data.effects.append(effectFromJson(v.toObject()));

    return data;
}

QJsonObject SaveManager::playerToJson(const PlayerSaveData& data)
{
    QJsonObject obj;
    obj["character"] = characterToJson(data.characterData);
    obj["gold"] = data.gold;
    obj["maxEnergy"] = data.maxEnergy;
    obj["cardRemovalCost"] = data.cardRemovalCost;
    obj["relics"] = relicListToJson(data.relics);
    obj["masterDeck"] = masterDeckToJson(data.masterDeckData);
    return obj;
}

PlayerSaveData SaveManager::playerFromJson(const QJsonObject& obj)
{
    PlayerSaveData data;
    data.characterData = characterFromJson(obj.value("character").toObject());
    data.gold = obj.value("gold").toInt();
//  data.currentEnergy = obj.value("currentEnergy").toInt();
    data.maxEnergy = obj.value("maxEnergy").toInt(3);
    data.cardRemovalCost = obj.value("cardRemovalCost").toInt(50);
    data.relics = relicListFromJson(obj.value("relics").toArray());
    data.masterDeckData = masterDeckFromJson(obj.value("masterDeck").toObject());
    return data;
}

// ============================================================
//  MapState <-> JSON
// ============================================================

QJsonObject SaveManager::mapStateToJson(const MapState& state)
{
    QJsonObject obj;
    obj["currentAct"] = state.currentAct;
    obj["currentFloorIndex"] = state.currentFloorIndex;
    obj["currentNodeId"] = state.currentNodeId;

    QJsonArray visitedArr;
    for (int id : state.visitedNodeIds)
        visitedArr.append(id);
    obj["visitedNodeIds"] = visitedArr;

    QJsonArray availableArr;
    for (int id : state.availableNodeIds)
        availableArr.append(id);
    obj["availableNodeIds"] = availableArr;

    QJsonArray nodesArr;
    for (const MapNodeState& node : state.nodes)
    {
        QJsonObject nodeObj;
        nodeObj["id"] = node.id;
        nodeObj["type"] = static_cast<int>(node.type);
        nodeObj["floor"] = node.floor;
        nodeObj["column"] = node.column;
        nodeObj["visited"] = node.visited;
        nodeObj["available"] = node.available;

        QJsonArray parentArr;
        for (int pid : node.parentIds)
            parentArr.append(pid);
        nodeObj["parentIds"] = parentArr;

        QJsonArray childArr;
        for (int cid : node.childIds)
            childArr.append(cid);
        nodeObj["childIds"] = childArr;

        nodesArr.append(nodeObj);
    }
    obj["nodes"] = nodesArr;

    QJsonArray usedFirstArr;
    for (int id : state.usedFirstEncounterIds)
        usedFirstArr.append(id);
    obj["usedFirstEncounterIds"] = usedFirstArr;

    obj["normalEncounterCount"] = state.normalEncounterCount;

    return obj;
}

MapState SaveManager::mapStateFromJson(const QJsonObject& obj)
{
    MapState state;
    state.currentAct = obj.value("currentAct").toInt(1);
    state.currentFloorIndex = obj.value("currentFloorIndex").toInt(0);
    state.currentNodeId = obj.value("currentNodeId").toInt(-1);

    for (const QJsonValue& v : obj.value("visitedNodeIds").toArray())
        state.visitedNodeIds.append(v.toInt());

    for (const QJsonValue& v : obj.value("availableNodeIds").toArray())
        state.availableNodeIds.append(v.toInt());

    for (const QJsonValue& v : obj.value("nodes").toArray())
    {
        QJsonObject nodeObj = v.toObject();
        MapNodeState node;
        node.id = nodeObj.value("id").toInt();
        node.type = static_cast<NodeType>(nodeObj.value("type").toInt());
        node.floor = nodeObj.value("floor").toInt();
        node.column = nodeObj.value("column").toInt();
        node.visited = nodeObj.value("visited").toBool();
        node.available = nodeObj.value("available").toBool();

        for (const QJsonValue& pv : nodeObj.value("parentIds").toArray())
            node.parentIds.append(pv.toInt());

        for (const QJsonValue& cv : nodeObj.value("childIds").toArray())
            node.childIds.append(cv.toInt());

        state.nodes.append(node);
    }

    for (const QJsonValue& v : obj.value("usedFirstEncounterIds").toArray())
        state.usedFirstEncounterIds.append(v.toInt());

    state.normalEncounterCount = obj.value("normalEncounterCount").toInt(0);

    return state;
}

// ============================================================
//  Save / Load Game
// ============================================================

bool SaveManager::saveGame(const QString& username, const Player* player, const Map* map)
{
    if (username.isEmpty() || !player || !map)
        return false;

    QJsonObject root;
    root["version"] = CURRENT_SAVE_VERSION;
    root["player"] = playerToJson(player->extractState());
    root["map"] = mapStateToJson(map->extractState());

    return writeJsonFile(saveFilePath(username), root);
}

bool SaveManager::loadGame(const QString& username, Player* player, Map* map)
{
    if (username.isEmpty() || !player || !map)
        return false;

    QJsonObject root;

    if (!readJsonFile(saveFilePath(username), root))
        return false;

    if (!root.contains("player") || !root.contains("map"))
        return false;

    player->restoreState(playerFromJson(root.value("player").toObject()));
    map->restoreState(mapStateFromJson(root.value("map").toObject()));

    return true;
}