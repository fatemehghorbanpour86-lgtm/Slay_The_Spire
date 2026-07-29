#include "defeatpage.h"
#include "player.h"
#include "map.h"

#include <QCoreApplication>
#include <QDir>
#include <QFrame>
#include <QGraphicsDropShadowEffect>
#include <QHBoxLayout>
#include <QLabel>
#include <QPainter>
#include <QPushButton>
#include <QVBoxLayout>

DefeatPage::DefeatPage(Player* player, Map* map, const QString& username, QWidget* parent)
    : QWidget(parent), player(player), map(map), username(username)
{
    setFixedSize(1280, 720);

    // Score is finalized and persisted the moment the defeat screen is shown.
    if (this->player && this->map && !this->username.trimmed().isEmpty())
    {
        LeaderboardManager::updatePlayerScore(this->username, this->player, this->map, RunStatus::Defeat);
        breakdown = LeaderboardManager::calculateScore(this->player, this->map, RunStatus::Defeat);
        rank = LeaderboardManager::getPlayerRank(this->username);
    }

    loadAssets();
    setupUi();
    setupStyles();
}

void DefeatPage::loadAssets()
{
    QString baseDir = QCoreApplication::applicationDirPath();
    QString backgroundPath = QDir(baseDir).filePath(":/DefeatBackground.png");
    backgroundPixmap.load(backgroundPath);
}

void DefeatPage::setupUi()
{
    // -- Score panel, positioned right below the "Defeat" title in the art --
    scorePanel = new QFrame(this);
    scorePanel->setFixedSize(760, 250);

    QVBoxLayout* panelLayout = new QVBoxLayout(scorePanel);
    panelLayout->setContentsMargins(35, 22, 35, 22);
    panelLayout->setSpacing(14);

    // Total score, front and center
    totalScoreLabel = new QLabel(
        QString("Final Score: %1").arg(breakdown.totalScore), scorePanel
        );
    totalScoreLabel->setAlignment(Qt::AlignCenter);

    // Rank line
    rankLabel = new QLabel(
        rank == -1 ? "Unranked" : QString("Rank on Leaderboard: #%1").arg(rank),
        scorePanel
        );
    rankLabel->setAlignment(Qt::AlignCenter);

    // Compact breakdown row (no icons, no cards — just quiet numbers)
    QWidget* statsRow = new QWidget(scorePanel);
    QHBoxLayout* statsLayout = new QHBoxLayout(statsRow);
    statsLayout->setContentsMargins(0, 0, 0, 0);
    statsLayout->setSpacing(0);
    statsLayout->addWidget(createStatItem("Explorer", breakdown.explorer));
    statsLayout->addWidget(createStatItem("Survivor", breakdown.survivor));
    statsLayout->addWidget(createStatItem("Treasure", breakdown.treasureHunter));
    statsLayout->addWidget(createStatItem("Relics", breakdown.collector));
    statsLayout->addWidget(createStatItem("Deck", breakdown.deckBuilder));

    // Buttons
    QWidget* buttonRow = new QWidget(scorePanel);
    QHBoxLayout* buttonLayout = new QHBoxLayout(buttonRow);
    buttonLayout->setContentsMargins(0, 0, 0, 0);
    buttonLayout->setSpacing(16);

    leaderboardButton = new QPushButton("Leaderboard", scorePanel);
    leaderboardButton->setFixedSize(160, 42);
    leaderboardButton->setCursor(Qt::PointingHandCursor);
    connect(leaderboardButton, &QPushButton::clicked, this, &DefeatPage::leaderboardRequested);

    mainMenuButton = new QPushButton("Main Menu", scorePanel);
    mainMenuButton->setFixedSize(160, 42);
    mainMenuButton->setCursor(Qt::PointingHandCursor);
    connect(mainMenuButton, &QPushButton::clicked, this, &DefeatPage::mainMenuRequested);

    buttonLayout->addStretch();
    buttonLayout->addWidget(leaderboardButton);
    buttonLayout->addWidget(mainMenuButton);
    buttonLayout->addStretch();

    panelLayout->addWidget(totalScoreLabel);
    panelLayout->addWidget(rankLabel);
    panelLayout->addWidget(statsRow);
    panelLayout->addStretch();
    panelLayout->addWidget(buttonRow);

    // Subtle purple glow around the panel, echoing the magic haze in the art
    auto* glow = new QGraphicsDropShadowEffect(scorePanel);
    glow->setColor(QColor(140, 60, 190, 160));
    glow->setBlurRadius(45);
    glow->setOffset(0, 0);
    scorePanel->setGraphicsEffect(glow);
}

QWidget* DefeatPage::createStatItem(const QString& label, int value)
{
    QWidget* item = new QWidget();
    QVBoxLayout* layout = new QVBoxLayout(item);
    layout->setContentsMargins(10, 0, 10, 0);
    layout->setSpacing(2);

    QLabel* valueLabel = new QLabel(QString::number(value), item);
    valueLabel->setAlignment(Qt::AlignCenter);
    valueLabel->setStyleSheet(
        "color: rgb(225, 205, 235);"
        "font-size: 20px;"
        "font-weight: bold;"
        "background: transparent;"
        "border: none;"
        );

    QLabel* nameLabel = new QLabel(label, item);
    nameLabel->setAlignment(Qt::AlignCenter);
    nameLabel->setStyleSheet(
        "color: rgba(200, 180, 210, 190);"
        "font-size: 12px;"
        "background: transparent;"
        "border: none;"
        );

    layout->addWidget(valueLabel);
    layout->addWidget(nameLabel);
    return item;
}

void DefeatPage::setupStyles()
{
    scorePanel->setStyleSheet(
        "QFrame {"
        "  background-color: rgba(18, 12, 22, 170);"
        "  border: 1px solid rgba(150, 90, 190, 140);"
        "  border-radius: 16px;"
        "}"
        );

    totalScoreLabel->setStyleSheet(
        "color: rgb(235, 220, 245);"
        "font-size: 28px;"
        "font-weight: bold;"
        "background: transparent;"
        "border: none;"
        );

    rankLabel->setStyleSheet(
        "color: rgba(200, 180, 210, 210);"
        "font-size: 15px;"
        "background: transparent;"
        "border: none;"
        );

    const QString buttonStyle =
        "QPushButton {"
        "  background-color: rgba(60, 30, 70, 190);"
        "  color: rgb(230, 210, 240);"
        "  border: 1px solid rgba(160, 100, 200, 180);"
        "  border-radius: 8px;"
        "  font-size: 14px;"
        "  font-weight: bold;"
        "}"
        "QPushButton:hover {"
        "  background-color: rgba(85, 45, 100, 220);"
        "  border-color: rgb(190, 130, 230);"
        "}"
        "QPushButton:pressed {"
        "  background-color: rgba(40, 20, 48, 255);"
        "}";
    leaderboardButton->setStyleSheet(buttonStyle);
    mainMenuButton->setStyleSheet(buttonStyle);
}

void DefeatPage::paintEvent(QPaintEvent*)
{
    QPainter painter(this);
    painter.drawPixmap(rect(), backgroundPixmap);
}

void DefeatPage::resizeEvent(QResizeEvent* event)
{
    QWidget::resizeEvent(event);
    if (scorePanel)
    {
        // Centered horizontally, positioned just under the "Defeat" title area of the art.
        scorePanel->move((width() - scorePanel->width()) / 2, 380);
    }
}