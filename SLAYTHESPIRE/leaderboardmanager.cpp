#include "leaderboardmanager.h"

#include <QDir>
#include <QStandardPaths>

#include <algorithm>

#include "player.h"
#include "map.h"

const QString LeaderboardManager::filepath =
    QStandardPaths::writableLocation(QStandardPaths::AppDataLocation) + "/leaderboard.json";

ScoreBreakdown LeaderboardManager::calculateScore(Player* player, Map* map, RunStatus status)
{
    ScoreBreakdown sb;
    if (player == nullptr || map == nullptr)
    {
        return sb;
    }

    // 1. Explorer score:
    int effectiveFloor = map->getCurrentFloorIndex() + 1;
    int completedActs = map->getCurrentAct() - 1;
    sb.explorer = (effectiveFloor * 10) + (completedActs * 500);

    // 2. Survivor score:
    sb.survivor = player->getCurrentHealth() * 2;

    // 3. Treasure Hunter score:
    sb.treasureHunter = player->getGold() / 5;

    // 4. Collector score:
    sb.collector = player->getAllRelics().size() * 50;

    // 5. Deck Builder score:
    if (player->getMasterDeck())
    {
        sb.deckBuilder = player->getMasterDeck()->getCardCount() * 10;
    }
    else
    {
        sb.deckBuilder = 0;
    }

    // 6. Victory Bonus
    if (status == RunStatus::Victory)
    {
        sb.victoryBonus = 1000;
    }
    else
    {
        sb.victoryBonus = 0;
    }


    // Total Calculation
    sb.totalScore = sb.explorer + sb.survivor + sb.treasureHunter +
                    sb.collector + sb.deckBuilder + sb.victoryBonus;

    return sb;
}

void LeaderboardManager::updatePlayerScore(const QString& username, Player* player, Map* map, RunStatus status)
{
    if (username.trimmed().isEmpty() || player == nullptr || map == nullptr)
    {
        return;
    }

    const ScoreBreakdown currentScore = calculateScore(player, map, status);

    QVector<LeaderboardEntry> entries = loadLeaderboard();

    bool found = false;

    for (LeaderboardEntry& entry : entries)
    {
        if (entry.username == username)
        {
            found = true;

            bool shouldUpdate = false;

            if (status == RunStatus::Victory && entry.status != RunStatus::Victory)
            {
                shouldUpdate = true;
            }

            else if (status == entry.status && currentScore.totalScore > entry.breakdown.totalScore)
            {
                shouldUpdate = true;
            }

            else if (status == RunStatus::InProgress && entry.status == RunStatus::Defeat)
            {
                shouldUpdate = true;
            }

            if (shouldUpdate)
            {
                entry.characterName = player->getName();
                entry.breakdown = currentScore;
                entry.status = status;
                entry.dateTime = QDateTime::currentDateTime().toString("yyyy-MM-dd HH:mm:ss");
            }
            break;
        }
    }

    if (!found)
    {
        LeaderboardEntry newEntry;
        newEntry.username = username;
        newEntry.characterName = player->getName();
        newEntry.breakdown = currentScore;
        newEntry.status = status;
        newEntry.dateTime = QDateTime::currentDateTime().toString("yyyy-MM-dd HH:mm:ss");

        entries.push_back(newEntry);
    }

    std::sort(
        entries.begin(),
        entries.end(),
        [](const LeaderboardEntry& a, const LeaderboardEntry& b)
        {
            //  (Victory > InProgress > Defeat)
            if (a.status != b.status)
            {
                return static_cast<int>(a.status) > static_cast<int>(b.status);
            }

            if (a.breakdown.totalScore != b.breakdown.totalScore)
            {
                return a.breakdown.totalScore > b.breakdown.totalScore;
            }
            return a.dateTime <  b.dateTime;
        }
        );

    saveLeaderboard(entries);
}


int LeaderboardManager::getPlayerRank(const QString& username)
{
    if (username.trimmed().isEmpty())
    {
        return -1;
    }

    const QVector<LeaderboardEntry> entries = loadLeaderboard();

    for (int i = 0; i < entries.size(); ++i)
    {
        if (entries[i].username == username)
        {
            return i + 1;
        }
    }

    return -1;
}


QVector<LeaderboardEntry> LeaderboardManager::loadLeaderboard()
{
    QVector<LeaderboardEntry> entries;

    QFile file(filepath);

    if (!file.exists())
    {
        return entries;
    }

    if (!file.open(QIODevice::ReadOnly))
    {
        return entries;
    }

    const QByteArray data = file.readAll();
    file.close();

    const QJsonDocument doc = QJsonDocument::fromJson(data);

    if (!doc.isArray())
    {
        return entries;
    }

    const QJsonArray array = doc.array();

    for (const QJsonValue& value : array)
    {
        if (!value.isObject())
        {
            continue;
        }

        entries.push_back(deserializeEntry(value.toObject()));
    }

    std::sort(
        entries.begin(),
        entries.end(),
        [](const LeaderboardEntry& a, const LeaderboardEntry& b)
        {
            //  (Victory > InProgress > Defeat)
            if (a.status != b.status)
            {
                return static_cast<int>(a.status) > static_cast<int>(b.status);
            }

            if (a.breakdown.totalScore != b.breakdown.totalScore)
            {
                return a.breakdown.totalScore > b.breakdown.totalScore;
            }

            return a.dateTime <  b.dateTime;
        }
        );

    return entries;
}


void LeaderboardManager::saveLeaderboard(const QVector<LeaderboardEntry>& entries)
{
    const QFileInfo fileInfo(filepath);
    QDir dir = fileInfo.dir();

    if (!dir.exists())
    {
        dir.mkpath(".");
    }

    QJsonArray array;

    for (const LeaderboardEntry& entry : entries)
    {
        array.append(serializeEntry(entry));
    }

    const QJsonDocument doc(array);

    QFile file(filepath);

    if (!file.open(QIODevice::WriteOnly | QIODevice::Truncate))
    {
        return;
    }

    file.write(doc.toJson(QJsonDocument::Indented));
    file.close();
}


QJsonObject LeaderboardManager::serializeBreakdown(const ScoreBreakdown& sb)
{
    QJsonObject json;
    json["explorer"] = sb.explorer;
    json["survivor"] = sb.survivor;
    json["treasureHunter"] = sb.treasureHunter;
    json["collector"] = sb.collector;
    json["deckBuilder"] = sb.deckBuilder;
    json["victoryBonus"] = sb.victoryBonus;
    json["totalScore"] = sb.totalScore;
    return json;
}


ScoreBreakdown LeaderboardManager::deserializeBreakdown(const QJsonObject& json)
{
    ScoreBreakdown sb;
    sb.explorer = json["explorer"].toInt();
    sb.survivor = json["survivor"].toInt();
    sb.treasureHunter = json["treasureHunter"].toInt();
    sb.collector = json["collector"].toInt();
    sb.deckBuilder = json["deckBuilder"].toInt();
    sb.victoryBonus = json["victoryBonus"].toInt();
    sb.totalScore = json["totalScore"].toInt();
    return sb;
}


QJsonObject LeaderboardManager::serializeEntry(const LeaderboardEntry& entry)
{
    QJsonObject json;
    json["username"] = entry.username;
    json["characterName"] = entry.characterName;
    json["breakdown"] = serializeBreakdown(entry.breakdown);
    json["status"] = static_cast<int>(entry.status);
    json["dateTime"] = entry.dateTime;
    return json;
}


LeaderboardEntry LeaderboardManager::deserializeEntry(const QJsonObject& json)
{
    LeaderboardEntry entry;
    entry.username = json["username"].toString();
    entry.characterName = json["characterName"].toString();
    entry.status = static_cast<RunStatus>(json["status"].toInt(0));
    entry.dateTime = json["dateTime"].toString();

    if (json.contains("breakdown") && json["breakdown"].isObject())
    {
        entry.breakdown = deserializeBreakdown(json["breakdown"].toObject());
    }

    return entry;
}
