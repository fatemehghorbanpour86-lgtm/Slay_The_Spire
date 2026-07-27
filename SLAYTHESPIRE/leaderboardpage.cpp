#include "leaderboardpage.h"

#include <QAbstractItemView>
#include <QBrush>
#include <QColor>
#include <QDateTime>
#include <QDialog>
#include <QFrame>
#include <QGraphicsDropShadowEffect>
#include <QGridLayout>
#include <QHeaderView>
#include <QHBoxLayout>
#include <QLabel>
#include <QPainter>
#include <QPushButton>
#include <QScrollBar>
#include <QTableWidget>
#include <QTableWidgetItem>
#include <QVBoxLayout>
#include <QCoreApplication>

#include <utility>

LeaderboardPage::LeaderboardPage(const QString& currentUsername, QWidget* parent)
    : QWidget(parent),
    currentUsername(currentUsername)
{
    setFixedSize(1280, 720);

    loadAssets();
    setupUi();
    setupStyles();
    refreshData();
}

void LeaderboardPage::loadAssets()
{
    QString baseDir = QCoreApplication::applicationDirPath();
    QString BackgroundPath = QDir(baseDir).filePath("assets/leaderboard/LeaderBackground.png");

    backgroundPixmap.load(BackgroundPath);
}

void LeaderboardPage::setupUi()
{
    mainPanel = new QFrame(this);
    mainPanel->setGeometry(55, 35, 1170, 650);

    QVBoxLayout* rootLayout = new QVBoxLayout(mainPanel);
    rootLayout->setContentsMargins(30, 25, 30, 25);
    rootLayout->setSpacing(20);

    // =========================================================
    // Top bar
    // =========================================================
    QHBoxLayout* topBarLayout = new QHBoxLayout();

    backButton = new QPushButton("Back", this);
    backButton->setFixedSize(110, 42);
    connect(backButton, &QPushButton::clicked, this, &LeaderboardPage::backRequested);

    titleLabel = new QLabel("GLOBAL RANKING", this);
    titleLabel->setAlignment(Qt::AlignCenter);

    // Add a subtle shadow to make the title more readable and premium-looking.
    QGraphicsDropShadowEffect* titleShadow = new QGraphicsDropShadowEffect(this);
    titleShadow->setBlurRadius(12);
    titleShadow->setColor(QColor(0, 0, 0, 200));
    titleShadow->setOffset(2, 3);
    titleLabel->setGraphicsEffect(titleShadow);

    playerSummaryFrame = new QFrame(this);
    playerSummaryFrame->setFixedWidth(270);

    QVBoxLayout* playerSummaryLayout = new QVBoxLayout(playerSummaryFrame);
    playerSummaryLayout->setContentsMargins(14, 10, 14, 10);
    playerSummaryLayout->setSpacing(4);

    currentUserLabel = new QLabel("Player: -", this);
    currentRankLabel = new QLabel("Rank: -", this);
    currentBestScoreLabel = new QLabel("Best Score: -", this);

    playerSummaryLayout->addWidget(currentUserLabel);
    playerSummaryLayout->addWidget(currentRankLabel);
    playerSummaryLayout->addWidget(currentBestScoreLabel);

    topBarLayout->addWidget(backButton, 0, Qt::AlignLeft);
    topBarLayout->addStretch();
    topBarLayout->addWidget(titleLabel, 0, Qt::AlignCenter);
    topBarLayout->addStretch();
    topBarLayout->addWidget(playerSummaryFrame, 0, Qt::AlignRight);

    rootLayout->addLayout(topBarLayout);

    // =========================================================
    // Leaderboard table
    // =========================================================
    leaderboardTable = new QTableWidget(this);
    leaderboardTable->setColumnCount(7);
    leaderboardTable->setHorizontalHeaderLabels(
        QStringList() << "Rank"
                      << "Player"
                      << "Character"
                      << "Score"
                      << "Result"
                      << "Date"
                      << "Details"
        );

    leaderboardTable->verticalHeader()->setVisible(false);
    leaderboardTable->setSelectionMode(QAbstractItemView::NoSelection);
    leaderboardTable->setEditTriggers(QAbstractItemView::NoEditTriggers);
    leaderboardTable->setFocusPolicy(Qt::NoFocus);
    leaderboardTable->setShowGrid(false);
    leaderboardTable->setAlternatingRowColors(true);

    leaderboardTable->horizontalHeader()->setStretchLastSection(false);
    leaderboardTable->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
    leaderboardTable->horizontalHeader()->setSectionResizeMode(0, QHeaderView::ResizeToContents);
    leaderboardTable->horizontalHeader()->setSectionResizeMode(3, QHeaderView::ResizeToContents);
    leaderboardTable->horizontalHeader()->setSectionResizeMode(4, QHeaderView::ResizeToContents);
    leaderboardTable->horizontalHeader()->setSectionResizeMode(6, QHeaderView::ResizeToContents);

    leaderboardTable->setVerticalScrollBarPolicy(Qt::ScrollBarAsNeeded);
    leaderboardTable->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    leaderboardTable->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    leaderboardTable->setMinimumHeight(480);

    rootLayout->addWidget(leaderboardTable, 1);
}

QWidget* LeaderboardPage::createBreakdownCard(const QString& title,
                                              const QString& iconPath,
                                              int value,
                                              const QString& formula)
{
    QFrame* card = new QFrame;
    card->setMinimumSize(360, 120);
    card->setToolTip(formula);

    // Golden fantasy card style with brighter highlight and cleaner contrast.
    card->setStyleSheet(
        "QFrame {"
        "  background-color: rgba(42, 32, 20, 215);"
        "  border: 1px solid rgba(255, 215, 120, 170);"
        "  border-radius: 14px;"
        "}"
        "QFrame:hover {"
        "  background-color: rgba(58, 44, 26, 235);"
        "  border: 1px solid rgba(255, 235, 160, 255);"
        "}"
        "QToolTip {"
        "  background-color: rgba(32, 24, 16, 245);"
        "  color: rgb(255, 232, 170);"
        "  border: 1px solid rgb(255, 215, 120);"
        "  padding: 8px;"
        "  font-size: 13px;"
        "  border-radius: 6px;"
        "}"
        );

    QHBoxLayout* layout = new QHBoxLayout(card);
    layout->setContentsMargins(12, 10, 15, 10);
    layout->setSpacing(15);

    // Icon area
    QLabel* iconLabel = new QLabel(card);
    iconLabel->setFixedSize(100, 100);
    iconLabel->setStyleSheet("border: none; background: transparent;");

    QPixmap pix(iconPath);
    if (!pix.isNull()) {
        iconLabel->setPixmap(pix.scaled(100, 100, Qt::KeepAspectRatio, Qt::SmoothTransformation));
    }

    // Text area
    QVBoxLayout* textLayout = new QVBoxLayout();
    textLayout->setSpacing(2);
    textLayout->setContentsMargins(0, 0, 0, 0);

    QLabel* titleLabel = new QLabel(title, card);
    QLabel* valueLabel = new QLabel(QString::number(value), card);

    titleLabel->setStyleSheet(
        "font-size: 15px;"
        "font-weight: bold;"
        "color: rgb(250, 232, 190);"
        "border: none;"
        "background: transparent;"
        );

    valueLabel->setStyleSheet(
        "font-size: 30px;"
        "font-weight: bold;"
        "color: rgb(255, 226, 125);"
        "border: none;"
        "background: transparent;"
        );

    textLayout->addStretch();
    textLayout->addWidget(titleLabel);
    textLayout->addWidget(valueLabel);
    textLayout->addStretch();

    layout->addWidget(iconLabel, 0, Qt::AlignVCenter);
    layout->addLayout(textLayout, 1);

    return card;
}

void LeaderboardPage::setupStyles()
{
    setAttribute(Qt::WA_StyledBackground, true);

    // Main title style
    titleLabel->setStyleSheet(
        "color: rgb(255, 225, 140);"
        "font-size: 36px;"
        "font-weight: bold;"
        "letter-spacing: 2px;"
        );

    // Back button style
    backButton->setStyleSheet(
        "QPushButton {"
        "  background-color: rgba(110, 78, 28, 210);"
        "  color: rgb(255, 239, 190);"
        "  border: 2px solid rgb(255, 210, 110);"
        "  border-radius: 10px;"
        "  font-size: 16px;"
        "  font-weight: bold;"
        "}"
        "QPushButton:hover {"
        "  background-color: rgba(145, 102, 34, 240);"
        "  color: rgb(255, 248, 220);"
        "  border-color: rgb(255, 232, 150);"
        "}"
        "QPushButton:pressed {"
        "  background-color: rgba(88, 62, 21, 255);"
        "}"
        );

    // Main panel style
    mainPanel->setStyleSheet(
        "QFrame {"
        "  background-color: rgba(18, 14, 10, 120);"
        "  border: 2px solid rgba(255, 210, 110, 160);"
        "  border-radius: 18px;"
        "}"
        );

    // Current player summary box style
    playerSummaryFrame->setStyleSheet(
        "QFrame {"
        "  background-color: rgba(45, 34, 20, 190);"
        "  border: 1px solid rgba(255, 214, 120, 180);"
        "  border-radius: 12px;"
        "}"
        "QLabel {"
        "  color: rgb(255, 236, 190);"
        "  font-size: 14px;"
        "  font-weight: 600;"
        "  border: none;"
        "  background: transparent;"
        "}"
        );

    // Table style
    leaderboardTable->setStyleSheet(
        "QTableWidget {"
        "  background-color: rgba(26, 20, 14, 70);"
        "  alternate-background-color: rgba(40, 30, 20, 90);"
        "  color: rgb(248, 235, 205);"
        "  border: 1px solid rgba(255, 210, 120, 110);"
        "  border-radius: 12px;"
        "  font-size: 15px;"
        "  gridline-color: transparent;"
        "}"
        "QHeaderView::section {"
        "  background-color: rgba(120, 84, 30, 230);"
        "  color: rgb(255, 240, 190);"
        "  border: none;"
        "  border-bottom: 2px solid rgba(255, 220, 130, 220);"
        "  padding: 12px;"
        "  font-weight: bold;"
        "  font-size: 15px;"
        "}"
        "QScrollBar:vertical {"
        "  background: rgba(35, 28, 20, 120);"
        "  width: 10px;"
        "  border-radius: 5px;"
        "}"
        "QScrollBar::handle:vertical {"
        "  background: rgba(255, 196, 85, 190);"
        "  border-radius: 5px;"
        "  min-height: 24px;"
        "}"
        "QScrollBar::handle:vertical:hover {"
        "  background: rgba(255, 220, 120, 230);"
        "}"
        "QScrollBar::add-line:vertical, QScrollBar::sub-line:vertical {"
        "  height: 0px;"
        "}"
        );
}

void LeaderboardPage::refreshData()
{
    entries = LeaderboardManager::loadLeaderboard();
    populateTable();
    updateCurrentPlayerPanel();
}

void LeaderboardPage::populateTable()
{
    leaderboardTable->clearContents();
    leaderboardTable->setRowCount(entries.size());

    for (int row = 0; row < entries.size(); ++row) {
        const LeaderboardEntry& entry = entries[row];

        // Show special symbols for top 3 ranks.
        QString rankText;
        if (row == 0) {
            rankText = "🏆 1";
        } else if (row == 1) {
            rankText = "🥈 2";
        } else if (row == 2) {
            rankText = "🥉 3";
        } else {
            rankText = QString::number(row + 1);
        }

        leaderboardTable->setItem(row, 0, new QTableWidgetItem(rankText));
        leaderboardTable->setItem(row, 1, new QTableWidgetItem(entry.username));
        leaderboardTable->setItem(row, 2, new QTableWidgetItem(entry.characterName));
        leaderboardTable->setItem(row, 3, new QTableWidgetItem(QString::number(entry.breakdown.totalScore)));
        leaderboardTable->setItem(row, 4, new QTableWidgetItem(formatResultText(entry.status)));
        leaderboardTable->setItem(row, 5, new QTableWidgetItem(formatDateText(entry.dateTime)));

        const bool isCurrentUser =
            (entry.username == currentUsername && !currentUsername.isEmpty());

        for (int col = 0; col < 6; ++col) {
            QTableWidgetItem* item = leaderboardTable->item(row, col);
            if (!item) {
                continue;
            }

            item->setTextAlignment(Qt::AlignCenter);

            // Rank-based coloring
            if (row == 0) {
                item->setForeground(QColor(255, 232, 130));   // Gold
            } else if (row == 1) {
                item->setForeground(QColor(230, 235, 245));   // Silver
            } else if (row == 2) {
                item->setForeground(QColor(212, 165, 110));   // Bronze
            } else {
                item->setForeground(QColor(245, 232, 205));   // Regular row text
            }

            // Highlight the current player's row
            if (isCurrentUser) {
                item->setBackground(QBrush(QColor(255, 215, 110, 55)));
                item->setFont(QFont("Arial", -1, QFont::Bold));
            }
        }

        // Details button
        QPushButton* detailsButton = new QPushButton("Details");
        detailsButton->setFixedSize(90, 30);
        detailsButton->setCursor(Qt::PointingHandCursor);
        detailsButton->setStyleSheet(
            "QPushButton {"
            "  background-color: rgba(125, 88, 28, 220);"
            "  color: rgb(255, 241, 200);"
            "  border: 1px solid rgb(255, 210, 110);"
            "  border-radius: 7px;"
            "  font-weight: bold;"
            "  font-size: 13px;"
            "  padding: 4px 10px;"
            "}"
            "QPushButton:hover {"
            "  background-color: rgba(160, 112, 36, 245);"
            "  color: rgb(255, 250, 230);"
            "  border-color: rgb(255, 232, 155);"
            "}"
            "QPushButton:pressed {"
            "  background-color: rgba(102, 72, 22, 255);"
            "}"
            );

        connect(detailsButton, &QPushButton::clicked, this, [this, row]() {
            openEntryDetailsDialog(row);
        });

        QWidget* container = new QWidget();
        QHBoxLayout* containerLayout = new QHBoxLayout(container);
        containerLayout->addWidget(detailsButton);
        containerLayout->setContentsMargins(0, 0, 0, 0);
        containerLayout->setAlignment(Qt::AlignCenter);

        if (isCurrentUser) {
            container->setStyleSheet("background-color: rgba(255, 215, 110, 55);");
        }

        leaderboardTable->setCellWidget(row, 6, container);
        leaderboardTable->setRowHeight(row, 48);
    }
}

void LeaderboardPage::updateCurrentPlayerPanel()
{
    currentUserLabel->setText(
        "Player: " + (currentUsername.isEmpty() ? "-" : currentUsername)
        );

    const int rank = LeaderboardManager::getPlayerRank(currentUsername);
    currentRankLabel->setText(
        "Rank: " + (rank == -1 ? "Unranked" : "#" + QString::number(rank))
        );

    currentBestScoreLabel->setText("Best Score: -");

    for (const auto& entry : std::as_const(entries)) {
        if (entry.username == currentUsername) {
            currentBestScoreLabel->setText(
                "Best Score: " + QString::number(entry.breakdown.totalScore)
                );
            break;
        }
    }
}

void LeaderboardPage::openEntryDetailsDialog(int row)
{
    if (row < 0 || row >= entries.size()) {
        return;
    }

    QDialog dialog(this);
    dialog.setWindowTitle("Score Details");
    dialog.setFixedSize(900, 620);
    dialog.setStyleSheet(
        "background-color: rgb(24, 18, 12);"
        "border: 2px solid rgb(255, 210, 110);"
        "border-radius: 14px;"
        );

    QVBoxLayout* layout = new QVBoxLayout(&dialog);
    layout->setContentsMargins(30, 25, 30, 25);
    layout->setSpacing(15);

    // =========================================================
    // Dialog header
    // =========================================================
    QHBoxLayout* headerLayout = new QHBoxLayout();

    QLabel* playerNameLabel = new QLabel("Player: " + entries[row].username, &dialog);
    playerNameLabel->setStyleSheet(
        "font-size: 26px;"
        "font-weight: bold;"
        "color: rgb(255, 229, 155);"
        "border: none;"
        "background: transparent;"
        );

    QPushButton* closeButton = new QPushButton("Close", &dialog);
    closeButton->setFixedSize(90, 36);
    closeButton->setCursor(Qt::PointingHandCursor);
    closeButton->setStyleSheet(
        "QPushButton {"
        "  background-color: rgba(120, 84, 28, 220);"
        "  color: rgb(255, 239, 195);"
        "  border: 1px solid rgb(255, 210, 110);"
        "  border-radius: 7px;"
        "  font-weight: bold;"
        "}"
        "QPushButton:hover {"
        "  background-color: rgba(155, 108, 36, 245);"
        "  border-color: rgb(255, 232, 150);"
        "}"
        );
    connect(closeButton, &QPushButton::clicked, &dialog, &QDialog::accept);

    headerLayout->addWidget(playerNameLabel);
    headerLayout->addStretch();
    headerLayout->addWidget(closeButton);

    layout->addLayout(headerLayout);

    QLabel* totalScoreLabel = new QLabel(
        "Total Score: " + QString::number(entries[row].breakdown.totalScore),
        &dialog
        );
    totalScoreLabel->setStyleSheet(
        "font-size: 34px;"
        "font-weight: bold;"
        "color: rgb(255, 223, 120);"
        "border: none;"
        "background: transparent;"
        "margin-bottom: 5px;"
        );

    layout->addWidget(totalScoreLabel);

    // =========================================================
    // Score breakdown cards
    // =========================================================
    QGridLayout* grid = new QGridLayout();
    grid->setSpacing(15);

    const auto& b = entries[row].breakdown;

    // Tooltip formulas
    const QString explorerFormula =
        QString("Explorer Score\n"
                "Formula:\n"
                "((Floor Index + 1) × 10) + ((Act - 1) × 500)\n\n"
                "This entry: %1")
            .arg(b.explorer);

    const QString survivorFormula =
        QString("Survivor Score\n"
                "Formula:\n"
                "Current Health × 2\n\n"
                "This entry: %1")
            .arg(b.survivor);

    const QString treasureFormula =
        QString("Treasure Hunter Score\n"
                "Formula:\n"
                "Gold / 5\n\n"
                "This entry: %1")
            .arg(b.treasureHunter);

    const QString collectorFormula =
        QString("Collector Score\n"
                "Formula:\n"
                "Number of Relics × 50\n\n"
                "This entry: %1")
            .arg(b.collector);

    const QString deckFormula =
        QString("Deck Builder Score\n"
                "Formula:\n"
                "Master Deck Card Count × 10\n\n"
                "This entry: %1")
            .arg(b.deckBuilder);

    const QString victoryFormula =
        QString("Victory Bonus\n"
                "Formula:\n"
                "1000 points if won\n\n"
                "This entry: %1")
            .arg(b.victoryBonus);


    QString baseDir = QCoreApplication::applicationDirPath();

    QString ExplorerPath = QDir(baseDir).filePath("assets/leaderboard/Explorer.png");
    grid->addWidget(
        createBreakdownCard("Explorer", ExplorerPath, b.explorer, explorerFormula),
        0, 0
        );

    QString SurvivorPath = QDir(baseDir).filePath("assets/leaderboard/survivor.png");
    grid->addWidget(
        createBreakdownCard("Survivor", SurvivorPath, b.survivor, survivorFormula),
        0, 1
        );

    QString TreasureHunterPath = QDir(baseDir).filePath("assets/leaderboard/treasureHunter.png");
    grid->addWidget(
        createBreakdownCard("Treasure Hunter", TreasureHunterPath, b.treasureHunter, treasureFormula),
        1, 0
        );

    QString CollectorPath = QDir(baseDir).filePath("assets/leaderboard/collector.png");
    grid->addWidget(
        createBreakdownCard("Collector", CollectorPath, b.collector, collectorFormula),
        1, 1
        );

    QString DeckBuilderPath = QDir(baseDir).filePath("assets/leaderboard/deckBuilder.png");
    grid->addWidget(
        createBreakdownCard("Deck Builder", DeckBuilderPath, b.deckBuilder, deckFormula),
        2, 0
        );

    QString VictoryBonusPath = QDir(baseDir).filePath("assets/leaderboard/victoryBonus.png");
    grid->addWidget(
        createBreakdownCard("Victory Bonus", VictoryBonusPath, b.victoryBonus, victoryFormula),
        2, 1
        );

    layout->addLayout(grid);
    layout->addStretch();

    dialog.exec();
}

QString LeaderboardPage::formatResultText(RunStatus status) const
{
    switch (status)
    {
    case RunStatus::Victory:
        return "Victory";
    case RunStatus::InProgress:
        return "In Progress";
    case RunStatus::Defeat:
        return "Defeat";
    }

    return "Unknown";
}


QString LeaderboardPage::formatDateText(const QString& rawDate) const
{
    return rawDate;
}

void LeaderboardPage::paintEvent(QPaintEvent*)
{
    QPainter painter(this);
    painter.drawPixmap(rect(), backgroundPixmap);
}

void LeaderboardPage::resizeEvent(QResizeEvent* event)
{
    QWidget::resizeEvent(event);

    if (mainPanel)
    {
        mainPanel->setGeometry((width() - 1170) / 2, (height() - 650) / 2, 1170, 650);
    }
}
