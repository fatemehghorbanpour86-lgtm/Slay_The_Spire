#include "mainwindow.h"
#include "ui_mainwindow.h"

void MainWindow::updateLoadingProgress()
{
    currentProgress++;

    if(currentProgress == 30 || currentProgress == 75)
    {
        loadingTimer->setInterval(1100);
    }
    else
    {
        loadingTimer->setInterval(30);
    }

    loadingBar->setValue(currentProgress);

    if(currentProgress >= 100)
    {
        loadingTimer->stop();
    }
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

    setupSplashPage();


    stackedWidget->setCurrentWidget(splashPage);

    currentProgress = 0;
    loadingTimer = new QTimer(this);
    connect(loadingTimer, &QTimer::timeout, this, &MainWindow::updateLoadingProgress);
    loadingTimer->start(30);
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::setupSplashPage()
{
    splashPage = new QWidget();
    splashPage->setObjectName("splashPage");

    splashPage->setStyleSheet("QWidget#splashPage { border-image: url(:/SplashPage.png); }");

    QVBoxLayout *layout = new QVBoxLayout(splashPage);
    layout->addStretch();

    loadingBar = new QProgressBar(splashPage);
    loadingBar->setRange(0, 100);
    loadingBar->setValue(0);
    loadingBar->setTextVisible(false);
    loadingBar->setFixedHeight(40);

    loadingBar->setStyleSheet(
        "QProgressBar { border: 2px solid #5a5a54; bprder-radius: 5px; background-color: #c7d4d9;}"
        "QProgressBar::chunk { background-color: #00cfc1; width: 10px; }"
        );

    layout->addWidget(loadingBar, 0 , Qt::AlignBottom);
    layout->setContentsMargins(320, 0 , 550, 120);

    stackedWidget->addWidget(splashPage);
}
