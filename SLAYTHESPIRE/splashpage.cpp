#include "splashpage.h"
#include <QVBoxLayout>

SplashPage::SplashPage(QWidget *parent): QWidget(parent)
{
    setObjectName("splashPage");

    setStyleSheet("QWidget#splashPage { border-image: url(:/SplashPage.png); }");

    QVBoxLayout *layout = new QVBoxLayout(this);
    layout->addStretch();

    loadingBar = new QProgressBar(this);
    loadingBar->setRange(0, 100);
    loadingBar->setValue(0);
    loadingBar->setTextVisible(false);
    loadingBar->setFixedHeight(40);

    loadingBar->setStyleSheet(
        "QProgressBar {"
        "   border: 2px solid #00cfc1;"
        "   border-radius: 20px;"
        "   background-color: #2b2b2b;"
        "}"
        "QProgressBar::chunk {"
        "   background-color: #00cfc1;"
        "   border-radius: 16px;"
        "   margin: 2px; "
        "}"
        );

    layout->addWidget(loadingBar, 0 , Qt::AlignBottom);
    layout->setContentsMargins(320, 0 , 550, 120);

    currentProgress = 0;
    loadingTimer = new QTimer(this);
    connect(loadingTimer, &QTimer::timeout, this, &SplashPage::updateLoadingProgress);
    loadingTimer->start(30);
}

void SplashPage::updateLoadingProgress()
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
        emit loadingFinished();
    }
}
