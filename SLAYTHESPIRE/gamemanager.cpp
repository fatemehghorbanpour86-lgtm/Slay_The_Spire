#include "gamemanager.h"

#include "splashpage.h"
#include "loginpage.h"
#include "mainpage.h"
#include "mappage.h"
#include "battlepage.h"
#include "campfirepage.h"
#include "rewardpage.h"
#include "rewardsystem.h"
#include "player.h"
#include "map.h"
#include "savemanager.h"
#include "enemy.h"

#include <QRandomGenerator>
#include <QMessageBox>

GameManager::GameManager(QStackedWidget* stackedWidget, QObject* parent)
    : QObject(parent), stackedWidget(stackedWidget)
{
    createStaticPages();
    connectStaticPages();
}

GameManager::~GameManager()
{
    cleanupRun();
}

void GameManager::start()
{
    showSplashPage();
}

// ============================================================
//  Setup
// ============================================================

void GameManager::createStaticPages()
{
    splashPage   = new SplashPage(nullptr);
    loginPage    = new loginpage(nullptr);
    mainMenuPage = new mainpage(nullptr);

    stackedWidget->addWidget(splashPage);
    stackedWidget->addWidget(loginPage);
    stackedWidget->addWidget(mainMenuPage);
}

void GameManager::connectStaticPages()
{
    connect(splashPage, &SplashPage::loadingFinished, this, &GameManager::showLoginPage);

    connect(loginPage, &loginpage::loginSuccess,     this, &GameManager::handleAuthAttempt);
    connect(loginPage, &loginpage::registerRequsted, this, &GameManager::handleRegisterAttempt);

    connect(mainMenuPage, &mainpage::startGame, this, &GameManager::onStartGameRequested);
}

// ============================================================
//  Page navigation
// ============================================================

void GameManager::showSplashPage()
{
    stackedWidget->setCurrentWidget(splashPage);
}

void GameManager::showLoginPage()
{
    stackedWidget->setCurrentWidget(loginPage);
}

void GameManager::showMainMenuPage()
{
    stackedWidget->setCurrentWidget(mainMenuPage);
}

void GameManager::showMapPage()
{
    if (!mapPage)
    {
        mapPage = new MapPage(map, player, nullptr);
        stackedWidget->addWidget(mapPage);
        connect(mapPage, &MapPage::nodeEntered, this, &GameManager::onMapNodeEntered);
    }
    else
    {
        mapPage->updateTopBarData();
    }

    cleanupTransientPages();

    stackedWidget->setCurrentWidget(mapPage);
}

void GameManager::showCampfirePage()
{
    if (campfirePage)
    {
        stackedWidget->removeWidget(campfirePage);
        campfirePage->deleteLater();
        campfirePage = nullptr;
    }

    campfirePage = new CampfirePage(player, map, nullptr);
    stackedWidget->addWidget(campfirePage);

    connect(campfirePage, &CampfirePage::leaveCampfire, this, &GameManager::onCampfireLeft);

    stackedWidget->setCurrentWidget(campfirePage);
}

void GameManager::showRewardPage(RewardSystem* rewardSystem)
{
    pendingRewardSystem = rewardSystem;

    if (rewardPage)
    {
        stackedWidget->removeWidget(rewardPage);
        rewardPage->deleteLater();
        rewardPage = nullptr;
    }

    rewardPage = new RewardPage(player, rewardSystem, nullptr);
    stackedWidget->addWidget(rewardPage);

    connect(rewardPage, &RewardPage::continueClicked, this, &GameManager::onRewardContinue);

    stackedWidget->setCurrentWidget(rewardPage);
}

void GameManager::showBattlePage(const QVector<Enemy*>& enemies)
{
    if (battlePage)
    {
        stackedWidget->removeWidget(battlePage);
        battlePage->deleteLater();
        battlePage = nullptr;
    }

    battlePage = new BattlePage(player, enemies, nullptr);
    stackedWidget->addWidget(battlePage);

    connect(battlePage, &BattlePage::combatResult, this, &GameManager::onCombatResult);

    stackedWidget->setCurrentWidget(battlePage);
}

void GameManager::cleanupTransientPages()
{
    if (battlePage)
    {
        stackedWidget->removeWidget(battlePage);
        battlePage->deleteLater();
        battlePage = nullptr;
    }

    if (rewardPage)
    {
        stackedWidget->removeWidget(rewardPage);
        rewardPage->deleteLater();
        rewardPage = nullptr;
    }

    if (campfirePage)
    {
        stackedWidget->removeWidget(campfirePage);
        campfirePage->deleteLater();
        campfirePage = nullptr;
    }
}

// ============================================================
//  Hooks for not-yet-implemented pages
//  (Shop UI / Event UI / Victory / Defeat / Leaderboard)
// ============================================================

void GameManager::showShopPage()
{
    // TODO: once ShopPage exists:
    //   - ShopPage* shopPage = new ShopPage(player, new Shop(), nullptr);
    //   - shopPage->generateStock() equivalent
    //   - connect(shopPage, &ShopPage::shoppingDone, this, &GameManager::returnToMapAndAutosave);
    //   - stackedWidget->addWidget(shopPage); stackedWidget->setCurrentWidget(shopPage);
    // Until then, don't block progression:
    returnToMapAndAutosave();
}

void GameManager::showEventPage()
{
    // TODO: once EventPage exists:
    //   - use EventSystem::getRandomEvent()/startRandomEvent(player) to pick an Event
    //   - display its EventOption list, call Event::chooseOption() on selection
    //   - connect(eventPage, &EventPage::eventResolved, this, &GameManager::returnToMapAndAutosave);
    returnToMapAndAutosave();
}

void GameManager::handleTreasureNode()
{
    // TODO: Treasure room reward/UI not implemented yet.
    returnToMapAndAutosave();
}

void GameManager::showVictoryPage()
{
    // TODO: once a Victory screen exists, show it here instead and connect
    // its "Return to Main Menu" action to onReturnToMainMenuRequested().
    showMainMenuPage();
}

void GameManager::showDefeatPage()
{
    // TODO: once a Defeat screen exists, show it here (with combat stats)
    // and connect its "Return to Main Menu" button to
    // onReturnToMainMenuRequested() instead of calling it directly.
    onReturnToMainMenuRequested();
}

void GameManager::updateLeaderboard()
{
    // TODO: hook point for writing this run's final score once the
    // Leaderboard storage/UI exists.
}

// ============================================================
//  Run lifecycle
// ============================================================

void GameManager::createNewRun(const QString& username)
{
    cleanupRun();

    // Player's constructor already grants the starter deck (5x Strike,
    // 4x Defend, 1x Carnage) and the starter relic (Burning Blood).
    player = new Player("Ironclad", 80);

    // Map's constructor generates Act 1 immediately.
    map = new Map();

    setupActEncounterPools();

    SaveManager::saveGame(username, player, map);
}

void GameManager::loadRun(const QString& username)
{
    cleanupRun();

    player = new Player("Ironclad", 80);
    map    = new Map();

    SaveManager::loadGame(username, player, map);

    usedFirstEncounters.clear();
    for (int raw : map->getUsedFirstEncounterIds())
        usedFirstEncounters.append(static_cast<EnemyFactory::EncounterId>(raw));

    normalEncounterCount = map->getNormalEncounterCount();
}

void GameManager::cleanupRun()
{
    if (mapPage)
    {
        stackedWidget->removeWidget(mapPage);
        mapPage->deleteLater();
        mapPage = nullptr;
    }

    cleanupTransientPages();

    delete player;
    player = nullptr;

    delete map;
    map = nullptr;
}

void GameManager::autoSave()
{
    if (currentUsername.isEmpty() || !player || !map)
        return;

    QVector<int> ids;
    for (EnemyFactory::EncounterId id : std::as_const(usedFirstEncounters))
        ids.append(static_cast<int>(id));

    map->setUsedFirstEncounterIds(ids);
    map->setNormalEncounterCount(normalEncounterCount);

    SaveManager::saveGame(currentUsername, player, map);
}

// ============================================================
//  Encounter selection
// ============================================================

void GameManager::setupActEncounterPools()
{
    usedFirstEncounters.clear();
    normalEncounterCount = 0;
}

QVector<Enemy*> GameManager::selectNormalEncounter()
{
    const int act = map ? map->getCurrentAct() : 1;

    if (normalEncounterCount < 2)
    {
        QVector<EnemyFactory::EncounterId> pool =
            (act == 1) ? EnemyFactory::act1FirstPool() : EnemyFactory::act2FirstPool();

        QVector<EnemyFactory::EncounterId> available;
        for (EnemyFactory::EncounterId id : pool)
        {
            if (!usedFirstEncounters.contains(id))
                available.append(id);
        }

        if (available.isEmpty())
            available = pool; // safety fallback, should not normally happen

        const int index = QRandomGenerator::global()->bounded(available.size());
        const EnemyFactory::EncounterId chosen = available[index];

        usedFirstEncounters.append(chosen);
        normalEncounterCount++;

        return EnemyFactory::createEncounter(chosen);
    }

    QVector<EnemyFactory::EncounterId> pool =
        (act == 1) ? EnemyFactory::act1LaterPool() : EnemyFactory::act2LaterPool();

    const int index = QRandomGenerator::global()->bounded(pool.size());
    normalEncounterCount++;

    return EnemyFactory::createEncounter(pool[index]);
}

QVector<Enemy*> GameManager::selectEliteEncounter()
{
    QVector<EnemyFactory::EncounterId> pool = EnemyFactory::elitePool();
    const int index = QRandomGenerator::global()->bounded(pool.size());
    return EnemyFactory::createEncounter(pool[index]);
}

QVector<Enemy*> GameManager::selectBossEncounter()
{
    const int act = map ? map->getCurrentAct() : 1;
    return EnemyFactory::createEncounter(EnemyFactory::randomBossForAct(act));
}

void GameManager::startBattle(const QVector<Enemy*>& enemies, EncounterKind kind)
{
    currentEncounterKind = kind;
    showBattlePage(enemies);
}

void GameManager::returnToMapAndAutosave()
{
    autoSave();
    showMapPage();
}

// ============================================================
//  Slots
// ============================================================

void GameManager::handleAuthAttempt()
{
    const QString username = loginPage->getUsername();
    const QString password = loginPage->getPassword();

    if (username.trimmed().isEmpty() || password.trimmed().isEmpty())
    {
        QMessageBox::warning(nullptr, "Login", "Fields cannot be empty.");
        return;
    }

    const SaveManager::AuthResult result = SaveManager::authenticate(username, password, SaveManager::AuthMode::Login);

    switch (result)
    {
    case SaveManager::AuthResult::LoggedIn:
        currentUsername = username;
        if (SaveManager::hasSaveFile(currentUsername)) loadRun(currentUsername);
        else createNewRun(currentUsername);
        showMainMenuPage();
        break;
    case SaveManager::AuthResult::UserNotFound:
        QMessageBox::warning(nullptr, "Login", "This username does not exist. Please register first.");
        break;

    case SaveManager::AuthResult::WrongPassword:
        QMessageBox::warning(nullptr, "Login", "Incorrect password.");
        break;

    case SaveManager::AuthResult::Error:
        QMessageBox::warning(nullptr, "Login", "Error accessing user file.");
        break;
    }
}

void GameManager::handleRegisterAttempt()
{
    const QString username = loginPage->getUsername();
    const QString password = loginPage->getPassword();

    if (username.trimmed().isEmpty()) {
        QMessageBox::warning(nullptr, "Register", "Username cannot be empty.");
        return;
    }


    bool hasUpper = false;
    bool hasLower = false;
    bool hasDigit = false;

    if (password.length() < 8)
    {
        QMessageBox::warning(nullptr, "Login", "Password must be at least 8 characters.");
        return;
    }

    for (const QChar& ch : password)
    {
        if (ch.isUpper()) hasUpper = true;
        else if (ch.isLower()) hasLower = true;
        else if (ch.isDigit()) hasDigit = true;
    }

    if (!hasUpper || !hasLower || !hasDigit)
    {
        QMessageBox::warning(nullptr, "Login",
                             "Password must contain at least one uppercase letter, one lowercase letter, and one number.");
        return;
    }


    const SaveManager::AuthResult result = SaveManager::authenticate(username, password, SaveManager::AuthMode::Register);

    switch (result)
    {
    case SaveManager::AuthResult::Registered:
        QMessageBox::information(nullptr, "Register", "Registration successful! Now you can login.");
        break;

    case SaveManager::AuthResult::Error:
        QMessageBox::warning(nullptr, "Register", "Username is already taken or file error occurred.");
        break;

    default: break;
    }
}

void GameManager::onStartGameRequested()
{
    // Only (re)create the run if none is currently in memory - e.g. right
    // after login this already exists; after a Defeat->MainMenu round trip
    // it was cleared and needs to be created fresh.
    if (!player || !map)
    {
        if (SaveManager::hasSaveFile(currentUsername))
            loadRun(currentUsername);
        else
            createNewRun(currentUsername);
    }

    showMapPage();
}

void GameManager::onMapNodeEntered(NodeType type)
{
    switch (type)
    {
    case NodeType::Monster:
        startBattle(selectNormalEncounter(), EncounterKind::Normal);
        break;

    case NodeType::Elite:
        startBattle(selectEliteEncounter(), EncounterKind::Elite);
        break;

    case NodeType::Boss:
        startBattle(selectBossEncounter(), EncounterKind::Boss);
        break;

    case NodeType::Campfire:
        showCampfirePage();
        break;

    case NodeType::Shop:
        showShopPage();
        break;

    case NodeType::Event:
        showEventPage();
        break;

    case NodeType::Treasure:
        handleTreasureNode();
        break;
    }
}

void GameManager::onCombatResult(bool playerWon)
{
    if (!playerWon)
    {
        handlePlayerDefeat();
        return;
    }

    RewardSystem* rewardSystem = new RewardSystem();

    switch (currentEncounterKind)
    {
    case EncounterKind::Normal:
        rewardSystem->generateNormalReward(player);
        break;
    case EncounterKind::Elite:
        rewardSystem->generateEliteReward(player);
        break;
    case EncounterKind::Boss:
        rewardSystem->generateBossReward(player);
        break;
    }

    showRewardPage(rewardSystem);
}

void GameManager::handlePlayerDefeat()
{
    // Battle lost: no reward is shown, straight to Defeat.
    showDefeatPage();
}

void GameManager::onRewardContinue()
{
    delete pendingRewardSystem;
    pendingRewardSystem = nullptr;

    if (currentEncounterKind == EncounterKind::Boss)
    {
        onBossDefeated();
        return;
    }

    returnToMapAndAutosave();
}

void GameManager::onBossDefeated()
{
    // Doc order: reward already claimed -> autosave -> advance Act (or finish run).
    autoSave();

    if (map->isFinalAct())
    {
        showVictoryPage();
        updateLeaderboard();

        // Run complete - keep the save file, just clear in-memory state so
        // a fresh "Start Game" begins a brand new run.
        cleanupRun();
        return;
    }

    map->goToNextAct();
    setupActEncounterPools();

    // MapWidget bakes its node layout in at construction time, so a new
    // Act needs a fresh MapPage to rebuild against the regenerated Map.
    if (mapPage)
    {
        stackedWidget->removeWidget(mapPage);
        mapPage->deleteLater();
        mapPage = nullptr;
    }

    autoSave();
    showMapPage();
}

void GameManager::onCampfireLeft()
{
    returnToMapAndAutosave();
}

void GameManager::onReturnToMainMenuRequested()
{
    if (!currentUsername.isEmpty())
        SaveManager::deleteSaveFile(currentUsername);

    cleanupRun();

    showMainMenuPage();
}
