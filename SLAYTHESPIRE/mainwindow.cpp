#include "mainwindow.h"
#include "normalenemies.h"
#include "ui_mainwindow.h"

#include "splashpage.h"
#include "loginpage.h"
#include "mainpage.h"
#include "battlepage.h"
#include "campfirepage.h"
#include <QDebug>

#include "map.h"
#include "player.h"
#include "mappage.h"
#include "card.h"
#include "attackcards.h"

void MainWindow::showLoginPAge()
{
    stackedWidget->setCurrentWidget(loginPage);
}

void MainWindow::showMainMenuPage()
{
    stackedWidget->setCurrentWidget(mainMenuPage);
}

void MainWindow::showBattlePage()
{
       if (battlePage) {
            stackedWidget->removeWidget(battlePage);
            delete battlePage;
            battlePage = nullptr;
        }
        battlePage = new BattlePage(new Player("Ironclad", 80), {new Cultist}, this);
        stackedWidget->addWidget(battlePage);
        stackedWidget->setCurrentWidget(battlePage);
}

void MainWindow::showMapPage()
{
    Map* currentMap = new Map();
    Player* currentPlayer = new Player("Ironclad", 80);

    mapPage = new MapPage(currentMap, currentPlayer);

    connect(mapPage, &MapPage::battleRequested, this, &MainWindow::showBattlePage);

    stackedWidget->addWidget(mapPage);
    stackedWidget->setCurrentWidget(mapPage);

    connect(mapPage, &MapPage::campfireEntered, this, &MainWindow::showCampfirePage);
}

void MainWindow::showCampfirePage()
{
    if (!mapPage)
        return;

    if (campfirePage)
    {
        stackedWidget->removeWidget(campfirePage);
        campfirePage->deleteLater();
        campfirePage = nullptr;

    }

    campfirePage = new CampfirePage(mapPage->getPlayer(), mapPage->getMap());

    stackedWidget->addWidget(campfirePage);
    stackedWidget->setCurrentWidget(campfirePage);

    connect(campfirePage, &CampfirePage::leaveCampfire, this, &MainWindow::returnFromCampfireToMap);
}

void MainWindow::returnFromCampfireToMap()
{
    if (!mapPage)
        return;

    if (campfirePage)
    {
        stackedWidget->removeWidget(campfirePage);
        campfirePage->deleteLater();
        campfirePage = nullptr;
    }


    mapPage->updateTopBarData();
    stackedWidget->setCurrentWidget(mapPage);
}

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    setFixedSize(1280,720);
    setWindowTitle("Slay The Spire");

    stackedWidget = new QStackedWidget(this);
    setCentralWidget(stackedWidget);

    splashPage = new SplashPage(this);
    loginPage = new loginpage(this);
    mainMenuPage = new mainpage(this);
    battlePage = new BattlePage(new Player("Ironclad", 80), {new Cultist}, this);


    stackedWidget->addWidget(splashPage);
    stackedWidget->addWidget(loginPage);
    stackedWidget->addWidget(mainMenuPage);
    stackedWidget->addWidget(battlePage);

    connect(splashPage, &SplashPage::loadingFinished, this, &MainWindow::showLoginPAge);
    connect(loginPage, &loginpage::loginSuccess,this, &MainWindow::showMainMenuPage );
    connect(mainMenuPage, &mainpage::startGame, this, &MainWindow::showMapPage);

    stackedWidget->setCurrentWidget(splashPage);
}

MainWindow::~MainWindow()
{
    delete ui;
}

