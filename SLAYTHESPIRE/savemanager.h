#ifndef SAVEMANAGER_H
#define SAVEMANAGER_H

#include <QString>
#include <QJsonObject>
#include <QJsonArray>
#include <QVector>

class Player;
class Map;

struct MapState;
struct EffectSaveData;
struct CardSaveData;
struct MasterDeckSaveData;
struct CharacterSaveData;
struct PlayerSaveData;
struct RelicSaveData;

class SaveManager
{
public:
    enum class AuthResult
    {
        LoggedIn,      // Username exist, Password true
        Registered,    // New user registered
        WrongPassword, // Username exist, Password false
        Error          // File error
    };

    static AuthResult authenticate(const QString& username, const QString& password);

    static bool saveGame(const QString& username, const Player* player, const Map* map);
    static bool loadGame(const QString& username, Player* player, Map* map);

    static bool hasSaveFile(const QString& username);

    static bool deleteSaveFile(const QString& username);

private:
    static QString usersFilePath();
    static QString saveFilePath(const QString& username);

    static bool readJsonFile(const QString& path, QJsonObject& outObject);
    static bool writeJsonFile(const QString& path, const QJsonObject& object);

    static QJsonObject effectToJson(const EffectSaveData& effect);
    static EffectSaveData effectFromJson(const QJsonObject& obj);

    static QJsonObject cardToJson(const CardSaveData& card);
    static CardSaveData cardFromJson(const QJsonObject& obj);

    static QJsonArray cardListToJson(const QVector<CardSaveData>& cards);
    static QVector<CardSaveData> cardListFromJson(const QJsonArray& arr);

    static QJsonObject masterDeckToJson(const MasterDeckSaveData& deck);
    static MasterDeckSaveData masterDeckFromJson(const QJsonObject& obj);

    static QJsonObject relicToJson(const RelicSaveData& relic);
    static RelicSaveData relicFromJson(const QJsonObject& obj);

    static QJsonArray relicListToJson(const QVector<RelicSaveData>& relics);
    static QVector<RelicSaveData> relicListFromJson(const QJsonArray& arr);

    static QJsonObject characterToJson(const CharacterSaveData& data);
    static CharacterSaveData characterFromJson(const QJsonObject& obj);

    static QJsonObject playerToJson(const PlayerSaveData& data);
    static PlayerSaveData playerFromJson(const QJsonObject& obj);

    static QJsonObject mapStateToJson(const MapState& state);
    static MapState mapStateFromJson(const QJsonObject& obj);

    static const int CURRENT_SAVE_VERSION = 1;
};

#endif // SAVEMANAGER_H
