#include "mainwindow.h"
#include "ui_mainwindow.h"

#include "splashpage.h"
#include "loginpage.h"
#include "mainpage.h"

void MainWindow::showLoginPAge()
{
    stackedWidget->setCurrentWidget(loginPage);
}

void MainWindow::showMainMenuPage()
{
    stackedWidget->setCurrentWidget(mainMenuPage);
}

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    qDebug() << "1";
    setFixedSize(1280,720);
    setWindowTitle("Slay The Spire");

    stackedWidget = new QStackedWidget(this);
    qDebug() << "2";
    setCentralWidget(stackedWidget);
    qDebug() << "3";

    splashPage = new SplashPage(this);
    qDebug() << "4";
    loginPage = new loginpage(this);
    qDebug() << "5";
    mainMenuPage = new mainpage(this);
    qDebug() << "6";

    stackedWidget->addWidget(splashPage);
    qDebug() << "7";
    stackedWidget->addWidget(loginPage);
    qDebug() << "8";
    stackedWidget->addWidget(mainMenuPage);
    qDebug() << "9";

    connect(splashPage, &SplashPage::loadingFinished, this, &MainWindow::showLoginPAge);
    qDebug() << "10";
    connect(loginPage, &loginpage::loginSuccess,this, &MainWindow::showMainMenuPage );
    qDebug() << "11";

    stackedWidget->setCurrentWidget(splashPage);
    qDebug() << "12";
}

MainWindow::~MainWindow()
{
    delete ui;
}

