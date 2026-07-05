#include "mainpage.h"
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QApplication>
#include <QLabel>
#include <QMovie>

mainpage::mainpage(QWidget *parent)
    : QWidget{parent}
{
    setObjectName("MainMenuPage");

    QLabel *bg = new QLabel(this);
    bg->setGeometry(0, 0, 1280, 720);
    bg->setScaledContents(true);
    bg->lower();
    QMovie *movie = new QMovie(":/MainMenu.gif");
    bg->setMovie(movie);
    movie->start();

    setStyleSheet(
        "QToolTip { color: #facc15; background-color: #1f2937; border: 1px solid #b91c1c;"
        "border-radius: 4px; padding: 6px; font-weight: bold; font-family: Tahoma;"
        "}"
        );

    QVBoxLayout *mainLayout = new QVBoxLayout(this);
    mainLayout->setContentsMargins(0, 0, 650 , 50);
    mainLayout->addStretch();

    QHBoxLayout *buttonsLayout = new QHBoxLayout;
    buttonsLayout->setAlignment(Qt::AlignCenter);

    singlePlayerBtn = new QPushButton(this);
    singlePlayerBtn->setFixedSize(100, 75);
    singlePlayerBtn->setStyleSheet("QPushButton { border-image: url(:/singlePlayerBtn.png); }");
    singlePlayerBtn->setToolTip("Start Battle!");

    multiPlayerBtn = new QPushButton(this);
    multiPlayerBtn->setFixedSize(100, 75);
    multiPlayerBtn->setStyleSheet("QPushButton { border-image: url(:/multiPalyerBtn.png); }");
    multiPlayerBtn->setToolTip("Battle in the Tower with Friends!");

    leaderBoardBtn = new QPushButton(this);
    leaderBoardBtn->setFixedSize(100, 75);
    leaderBoardBtn->setStyleSheet("QPushButton { border-image: url(:/leaderBoardBtn.png); }");
    leaderBoardBtn->setToolTip("Top Tower Slayers!");

    settingBtn = new QPushButton(this);
    settingBtn->setFixedSize(100, 75);
    settingBtn->setStyleSheet("QPushButton { border-image: url(:/settingBtn.png); }");
    settingBtn->setToolTip("Game Settings");

    exitBtn = new QPushButton(this);
    exitBtn->setFixedSize(100, 75);
    exitBtn->setStyleSheet("QPushButton { border-image: url(:/ExitBtn.png); }");
    exitBtn->setToolTip("Leave the Tower");

    buttonsLayout->addWidget(singlePlayerBtn);
    buttonsLayout->addWidget(multiPlayerBtn);
    buttonsLayout->addWidget(leaderBoardBtn);
    buttonsLayout->addWidget(settingBtn);
    buttonsLayout->addWidget(exitBtn);
    buttonsLayout->setSpacing(0);

    mainLayout->addLayout(buttonsLayout);
    mainLayout->addStretch();

    connect(singlePlayerBtn, &QPushButton::clicked, this, &mainpage::singlePlayerClicked);

    connect(exitBtn, &QPushButton::clicked, this, [](){
        QApplication::quit();}
            );
}
