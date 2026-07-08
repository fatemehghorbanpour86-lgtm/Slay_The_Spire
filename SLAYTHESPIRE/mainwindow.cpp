#include "mainwindow.h"
#include "ui_mainwindow.h"

#include "splashpage.h"
#include "loginpage.h"
#include "mainpage.h"

#include "map.h"
#include "player.h"
#include "mappage.h"

void MainWindow::showLoginPAge()
{
    stackedWidget->setCurrentWidget(loginPage);
}

void MainWindow::showMainMenuPage()
{
    stackedWidget->setCurrentWidget(mainMenuPage);
}

void MainWindow::showMapPage()
{
    Map* currentMap = new Map();
    Player* currentPlayer = new Player("Ironclad", 80);

    MapPage* mapPage = new MapPage(currentMap, currentPlayer);

    stackedWidget->addWidget(mapPage);
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

    stackedWidget->addWidget(splashPage);
    stackedWidget->addWidget(loginPage);
    stackedWidget->addWidget(mainMenuPage);

    connect(splashPage, &SplashPage::loadingFinished, this, &MainWindow::showLoginPAge);
    connect(loginPage, &loginpage::loginSuccess,this, &MainWindow::showMainMenuPage );
    connect(mainMenuPage, &mainpage::startGame, this, &MainWindow::showMapPage);

    stackedWidget->setCurrentWidget(splashPage);
}

MainWindow::~MainWindow()
{
    delete ui;
}

