#include "mainwindow.h"
#include "ui_mainwindow.h"

#include "splashpage.h"
#include "loginpage.h"

void MainWindow::showLoginPAge()
{
    stackedWidget->setCurrentWidget(loginPage);
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

    stackedWidget->addWidget(splashPage);
    stackedWidget->addWidget(loginPage);

    connect(splashPage, &SplashPage::loadingFinished, this, &MainWindow::showLoginPAge);
    stackedWidget->setCurrentWidget(splashPage);
}

MainWindow::~MainWindow()
{
    delete ui;
}

