#ifndef LEADERBOARDMANAGER_H
#define LEADERBOARDMANAGER_H

#include <QString>
#include <QVector>
#include <QDateTime>
#include <QJsonObject>
#include <QJsonDocument>
#include <QJsonArray>
#include <QFile>
#include <QDir>

// Forward declarations to avoid heavy includes in the header
class Player;
class Map;

/**
 * @struct ScoreBreakdown
 * @brief Holds detailed scoring metrics for a single run.
 */
struct ScoreBreakdown
{
    int explorer       = 0;  // Based on Acts and Floors reached
    int survivor       = 0;  // Based on remaining HP (e.g., 2 points per HP)
    int treasureHunter = 0;  // Based on Gold collected (e.g., 1 point per 5 Gold)
    int collector      = 0;  // Based on number of Relics (e.g., 50 points per Relic)
    int deckBuilder    = 0;  // Based on number of cards (e.g., 10 points per Card)
    int victoryBonus   = 0;  // Bonus for successfully winning the run (1000 points)
    int totalScore     = 0;  // Sum of all the above metrics
};

/**
 * @struct LeaderboardEntry
 * @brief Represents a single player's record in the global ranking.
 */
struct LeaderboardEntry
{
    QString username;
    QString characterName;
    ScoreBreakdown breakdown;
    bool victory = false;
    QString dateTime; // ISO 8601 formatted string
};

/**
 * @class LeaderboardManager
 * @brief Handles scoring logic, persistence, and ranking of player records.
 */
class LeaderboardManager
{
public:
    // --- Scoring Logic ---

    /**
     * @brief Calculates the real-time score based on current player/map state.
     */
    static ScoreBreakdown calculateScore(Player* player, Map* map, bool won);

    // --- Persistence & Data Management ---

    /**
     * @brief Updates player score using "Upsert" logic (Only keeps the best personal record).
     */
    static void updatePlayerScore(const QString& username, Player* player, Map* map, bool won);

    /**
     * @brief Loads the entire leaderboard from the local JSON file.
     */
    static QVector<LeaderboardEntry> loadLeaderboard();

    /**
     * @brief Saves the given leaderboard entries to the JSON file.
     */
    static void saveLeaderboard(const QVector<LeaderboardEntry>& entries);

    // --- Utility Methods ---

    /**
     * @brief Returns the 1-based rank of a specific player. Returns -1 if not found.
     */
    static int getPlayerRank(const QString& username);

private:
    static const QString filepath; // Default: "leaderboard.json"

    // JSON Serialization Helpers
    static QJsonObject serializeBreakdown(const ScoreBreakdown& sb);
    static ScoreBreakdown deserializeBreakdown(const QJsonObject& json);
    static QJsonObject serializeEntry(const LeaderboardEntry& entry);
    static LeaderboardEntry deserializeEntry(const QJsonObject& json);
};

#endif // LEADERBOARDMANAGER_H
