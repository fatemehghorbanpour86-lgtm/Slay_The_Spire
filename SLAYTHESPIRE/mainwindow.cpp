#include "mainwindow.h"
#include "ui_mainwindow.h"

#include "splashpage.h"


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
    stackedWidget->addWidget(splashPage);

    stackedWidget->setCurrentWidget(splashPage);


}

MainWindow::~MainWindow()
{
    delete ui;
}

