#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "gamemanager.h"


MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    setFixedSize(1280,720);
    setWindowTitle("Slay The Spire");

    stackedWidget = new QStackedWidget(this);
    setCentralWidget(stackedWidget);

    // GameManager is the ONLY place that opens/closes pages from here on.
    gameManager = new GameManager(stackedWidget, this);
    gameManager->start();
}

MainWindow::~MainWindow()
{
    delete ui;
}

