#ifndef GAMEMANAGER_H
#define GAMEMANAGER_H

#include <QObject>
#include <QStackedWidget>
#include <QVector>
#include <QString>

#include "mapnode.h"        // NodeType
#include "enemyfactory.h"   // EnemyFactory::EncounterId

class SplashPage;
class loginpage;
class mainpage;
class MapPage;
class BattlePage;
class CampfirePage;
class RewardPage;
class RewardSystem;
class Player;
class Map;
class Enemy;
class PauseDialog;
class TreasurePage;


class GameManager : public QObject
{
    Q_OBJECT

public:
    explicit GameManager(QStackedWidget* stackedWidget, QObject* parent = nullptr);
    ~GameManager() override;

    // Call once, right after construction, to show the Splash page.
    void start();

private:

    enum class EncounterKind
    {
        Normal,
        Elite,
        Boss
    };

    // ---- Core ----
    QStackedWidget* stackedWidget;

    // ---- Pages (owned & shown/hidden only by GameManager) ----
    SplashPage*   splashPage   = nullptr;
    loginpage*    loginPage    = nullptr;
    mainpage*     mainMenuPage = nullptr;
    MapPage*      mapPage      = nullptr;
    BattlePage*   battlePage   = nullptr;
    CampfirePage* campfirePage = nullptr;
    RewardPage*   rewardPage   = nullptr;
    TreasurePage* treasurePage = nullptr;

    // ---- Run state ----
    QString currentUsername;
    Player* player = nullptr;
    Map*    map    = nullptr;

    RewardSystem* pendingRewardSystem = nullptr;
    EncounterKind currentEncounterKind = EncounterKind::Normal;

    // ---- Encounter selection bookkeeping (reset every Act) ----
    QVector<EnemyFactory::EncounterId> usedFirstEncounters;
    int normalEncounterCount = 0;

    // ---- Setup ----
    void createStaticPages();
    void connectStaticPages();

    // ---- Page navigation (the only place pages are shown) ----
    void showSplashPage();
    void showLoginPage();
    void showMainMenuPage();
    void showMapPage();
    void showCampfirePage();
    void showRewardPage(RewardSystem* rewardSystem);
    void showBattlePage(const QVector<Enemy*>& enemies);

    void cleanupTransientPages(); // battlePage / rewardPage / campfirePage / TreasurePage

    // ---- Hooks for pages that don't exist yet ----
    // (Not implemented on purpose - wire the real UI to these once it exists.)
    void showShopPage();
    void showTreasurePage();
    void showEventPage();
    void showVictoryPage();
    void showDefeatPage();
    void updateLeaderboard();

    // ---- Run lifecycle ----
    void createNewRun(const QString& username);
    void loadRun(const QString& username);
    void cleanupRun();
    void autoSave();

    // ---- Encounter selection (GameManager decides WHICH, EnemyFactory builds) ----
    void setupActEncounterPools();
    QVector<Enemy*> selectNormalEncounter();
    QVector<Enemy*> selectEliteEncounter();
    QVector<Enemy*> selectBossEncounter();

    void startBattle(const QVector<Enemy*>& enemies, EncounterKind kind);
    void returnToMapAndAutosave();

    // ---- Slots (connected to existing pages' existing signals) ----
    void handleAuthAttempt();
    void handleRegisterAttempt();
    void onStartGameRequested();
    void onSettingsRequested();
    void onMapNodeEntered(NodeType type);
    void onCombatResult(bool playerWon);
    void handlePlayerDefeat();
    void onRewardContinue();
    void onBossDefeated();
    void onCampfireLeft();
    void onTreasureFinished();
    void onMapPauseRequested();

    // Hook: connect this to the (future) Defeat/Victory page's
    // "Return to Main Menu" button.
    void onReturnToMainMenuRequested();
};

#endif // GAMEMANAGER_H