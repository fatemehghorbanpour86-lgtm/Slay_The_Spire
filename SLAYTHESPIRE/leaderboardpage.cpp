#include "leaderboardpage.h"

#include <QPushButton>
#include <QLabel>
#include <QTableWidget>
#include <QHeaderView>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QGridLayout>
#include <QFrame>
#include <QPainter>
#include <QTableWidgetItem>
#include <QAbstractItemView>
#include <QBrush>
#include <QColor>
#include <QDateTime>
#include<utility>

LeaderboardPage::LeaderboardPage(const QString& currentUsername, QWidget* parent)
    : QWidget(parent), currentUsername(currentUsername)
{
    setFixedSize(1280, 720);

    loadAssets();
    setupUi();
    setupStyles();
    refreshData();
}

void LeaderboardPage::loadAssets()
{
    backgroundPixmap.load(":/leaderboard/LeaderBackground.png");
}

void LeaderboardPage::setupUi()
{
    mainPanel = new QFrame(this);
    mainPanel->setGeometry(55, 35, 1170, 650);

    QVBoxLayout* rootLayout = new QVBoxLayout(mainPanel);
    rootLayout->setContentsMargins(24, 20, 24, 20);
    rootLayout->setSpacing(16);

    // =========================
    // Top Bar
    // =========================
    QHBoxLayout* topBarLayout = new QHBoxLayout();

    backButton = new QPushButton("Back", this);
    backButton->setFixedSize(110, 42);
    connect(backButton, &QPushButton::clicked, this, &LeaderboardPage::backRequested);

    titleLabel = new QLabel("GLOBAL RANKING", this);
    titleLabel->setAlignment(Qt::AlignCenter);

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

    // =========================
    // Table
    // =========================
    leaderboardTable = new QTableWidget(this);
    leaderboardTable->setColumnCount(6);
    leaderboardTable->setHorizontalHeaderLabels(
        QStringList() << "Rank" << "Player" << "Character" << "Score" << "Result" << "Date"
        );

    leaderboardTable->verticalHeader()->setVisible(false);
    leaderboardTable->setSelectionBehavior(QAbstractItemView::SelectRows);
    leaderboardTable->setSelectionMode(QAbstractItemView::SingleSelection);
    leaderboardTable->setEditTriggers(QAbstractItemView::NoEditTriggers);
    leaderboardTable->setFocusPolicy(Qt::NoFocus);
    leaderboardTable->setShowGrid(false);
    leaderboardTable->setAlternatingRowColors(true);
    leaderboardTable->horizontalHeader()->setStretchLastSection(true);
    leaderboardTable->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
    leaderboardTable->horizontalHeader()->setSectionResizeMode(0, QHeaderView::ResizeToContents);
    leaderboardTable->horizontalHeader()->setSectionResizeMode(3, QHeaderView::ResizeToContents);
    leaderboardTable->horizontalHeader()->setSectionResizeMode(4, QHeaderView::ResizeToContents);
    leaderboardTable->setMinimumHeight(290);

    connect(leaderboardTable, &QTableWidget::itemSelectionChanged,
            this, &LeaderboardPage::onTableSelectionChanged);

    rootLayout->addWidget(leaderboardTable, 1);

    // =========================
    // Details
    // =========================
    detailsFrame = new QFrame(this);

    QVBoxLayout* detailsLayout = new QVBoxLayout(detailsFrame);
    detailsLayout->setContentsMargins(16, 14, 16, 14);
    detailsLayout->setSpacing(10);

    QHBoxLayout* detailsHeaderLayout = new QHBoxLayout();

    selectedPlayerLabel = new QLabel("Selected: -", this);
    selectedMetaLabel = new QLabel("-", this);
    selectedTotalScoreLabel = new QLabel("Total Score: -", this);

    detailsHeaderLayout->addWidget(selectedPlayerLabel);
    detailsHeaderLayout->addStretch();
    detailsHeaderLayout->addWidget(selectedMetaLabel);
    detailsHeaderLayout->addStretch();
    detailsHeaderLayout->addWidget(selectedTotalScoreLabel);

    detailsLayout->addLayout(detailsHeaderLayout);

    QGridLayout* cardsLayout = new QGridLayout();
    cardsLayout->setHorizontalSpacing(12);
    cardsLayout->setVerticalSpacing(12);

    cardsLayout->addWidget(createBreakdownCard("Explorer",        ":/leaderboard/Explorer.png",       explorerValueLabel),    0, 0);
    cardsLayout->addWidget(createBreakdownCard("Survivor",        ":/leaderboard/survivor.png",       survivorValueLabel),    0, 1);
    cardsLayout->addWidget(createBreakdownCard("Treasure Hunter", ":/leaderboard/treasureHunter.png", treasureValueLabel),    0, 2);
    cardsLayout->addWidget(createBreakdownCard("Collector",       ":/leaderboard/collector.png",      collectorValueLabel),   1, 0);
    cardsLayout->addWidget(createBreakdownCard("Deck Builder",    ":/leaderboard/deckBuilder.png",    deckBuilderValueLabel), 1, 1);
    cardsLayout->addWidget(createBreakdownCard("Victory Bonus",   ":/leaderboard/victoryBonus.png",   victoryBonusValueLabel),1, 2);

    detailsLayout->addLayout(cardsLayout);

    rootLayout->addWidget(detailsFrame);
}

QWidget* LeaderboardPage::createBreakdownCard(const QString& title,
                                              const QString& iconPath,
                                              QLabel*& valueLabel)
{
    QFrame* card = new QFrame(this);
    card->setMinimumHeight(105);

    QHBoxLayout* layout = new QHBoxLayout(card);
    layout->setContentsMargins(10, 10, 10, 10);
    layout->setSpacing(10);

    QLabel* iconLabel = new QLabel(this);
    iconLabel->setFixedSize(72, 72);

    QPixmap pix(iconPath);
    if (!pix.isNull())
    {
        iconLabel->setPixmap(pix.scaled(72, 72, Qt::KeepAspectRatio, Qt::SmoothTransformation));
    }

    QVBoxLayout* textLayout = new QVBoxLayout();
    textLayout->setSpacing(4);

    QLabel* localTitleLabel = new QLabel(title, this);
    valueLabel = new QLabel("0", this);

    localTitleLabel->setStyleSheet(
        "font-size: 15px;"
        "font-weight: 700;"
        "color: rgb(232, 216, 188);"
        );

    valueLabel->setStyleSheet(
        "font-size: 22px;"
        "font-weight: bold;"
        "color: rgb(255, 214, 120);"
        );

    textLayout->addStretch();
    textLayout->addWidget(localTitleLabel);
    textLayout->addWidget(valueLabel);
    textLayout->addStretch();

    layout->addWidget(iconLabel);
    layout->addLayout(textLayout, 1);

    return card;
}

void LeaderboardPage::setupStyles()
{
    setAttribute(Qt::WA_StyledBackground, true);

    titleLabel->setStyleSheet(
        "QLabel {"
        " color: rgb(245, 214, 140);"
        " font-size: 30px;"
        " font-weight: bold;"
        " letter-spacing: 1px;"
        "}"
        );

    backButton->setStyleSheet(
        "QPushButton {"
        " background-color: rgba(65, 38, 18, 210);"
        " color: rgb(241, 220, 170);"
        " border: 2px solid rgb(156, 113, 58);"
        " border-radius: 10px;"
        " font-size: 18px;"
        " font-weight: bold;"
        "}"
        "QPushButton:hover {"
        " background-color: rgba(92, 54, 27, 220);"
        "}"
        "QPushButton:pressed {"
        " background-color: rgba(45, 26, 12, 220);"
        "}"
        );

    mainPanel->setStyleSheet(
        "QFrame {"
        " background-color: rgba(10, 10, 12, 175);"
        " border: 2px solid rgba(180, 135, 70, 180);"
        " border-radius: 18px;"
        "}"
        );

    playerSummaryFrame->setStyleSheet(
        "QFrame {"
        " background-color: rgba(25, 18, 16, 180);"
        " border: 1px solid rgba(190, 150, 90, 160);"
        " border-radius: 12px;"
        "}"
        "QLabel {"
        " color: rgb(238, 226, 200);"
        " font-size: 15px;"
        " font-weight: 600;"
        " background: transparent;"
        " border: none;"
        "}"
        );

    leaderboardTable->setStyleSheet(
        "QTableWidget {"
        " background-color: rgba(18, 15, 14, 185);"
        " alternate-background-color: rgba(34, 28, 25, 190);"
        " color: rgb(235, 225, 205);"
        " border: 1px solid rgba(170, 130, 75, 140);"
        " border-radius: 12px;"
        " font-size: 16px;"
        " selection-background-color: rgba(176, 119, 47, 140);"
        " selection-color: white;"
        " gridline-color: transparent;"
        "}"
        "QHeaderView::section {"
        " background-color: rgba(84, 54, 29, 230);"
        " color: rgb(247, 226, 178);"
        " border: none;"
        " border-bottom: 1px solid rgba(200, 160, 90, 180);"
        " padding: 8px;"
        " font-size: 16px;"
        " font-weight: bold;"
        "}"
        );

    detailsFrame->setStyleSheet(
        "QFrame {"
        " background-color: rgba(20, 16, 15, 190);"
        " border: 1px solid rgba(180, 140, 80, 150);"
        " border-radius: 14px;"
        "}"
        "QLabel {"
        " color: rgb(236, 225, 204);"
        " background: transparent;"
        " border: none;"
        "}"
        );

    selectedPlayerLabel->setStyleSheet("font-size: 18px; font-weight: bold;");
    selectedMetaLabel->setStyleSheet("font-size: 14px; color: rgb(200, 190, 175);");
    selectedTotalScoreLabel->setStyleSheet("font-size: 20px; font-weight: bold; color: rgb(255, 218, 130);");
}

void LeaderboardPage::refreshData()
{
    entries = LeaderboardManager::loadLeaderboard();
    populateTable();
    updateCurrentPlayerPanel();

    if (!entries.isEmpty() && leaderboardTable->rowCount() > 0)
    {
        leaderboardTable->selectRow(0);
        showEntryDetails(entries[0]);
    }
    else
    {
        clearDetails();
    }
}

void LeaderboardPage::populateTable()
{
    leaderboardTable->clearContents();

    const int topCount = qMin(10, entries.size());
    leaderboardTable->setRowCount(topCount);

    for (int row = 0; row < topCount; ++row)
    {
        const LeaderboardEntry& entry = entries[row];

        QTableWidgetItem* rankItem   = new QTableWidgetItem(QString::number(row + 1));
        QTableWidgetItem* userItem   = new QTableWidgetItem(entry.username);
        QTableWidgetItem* charItem   = new QTableWidgetItem(entry.characterName);
        QTableWidgetItem* scoreItem  = new QTableWidgetItem(QString::number(entry.breakdown.totalScore));
        QTableWidgetItem* resultItem = new QTableWidgetItem(formatResultText(entry.victory));
        QTableWidgetItem* dateItem   = new QTableWidgetItem(formatDateText(entry.dateTime));

        rankItem->setTextAlignment(Qt::AlignCenter);
        scoreItem->setTextAlignment(Qt::AlignCenter);
        resultItem->setTextAlignment(Qt::AlignCenter);

        QString tooltip =
            "Explorer: " + QString::number(entry.breakdown.explorer) + "\n" +
            "Survivor: " + QString::number(entry.breakdown.survivor) + "\n" +
            "Treasure Hunter: " + QString::number(entry.breakdown.treasureHunter) + "\n" +
            "Collector: " + QString::number(entry.breakdown.collector) + "\n" +
            "Deck Builder: " + QString::number(entry.breakdown.deckBuilder) + "\n" +
            "Victory Bonus: " + QString::number(entry.breakdown.victoryBonus) + "\n\n" +
            "Total: " + QString::number(entry.breakdown.totalScore);

        rankItem->setToolTip(tooltip);
        userItem->setToolTip(tooltip);
        charItem->setToolTip(tooltip);
        scoreItem->setToolTip(tooltip);
        resultItem->setToolTip(tooltip);

        if (row == 0)
        {
            const QBrush goldBrush(QColor(255, 215, 90));
            rankItem->setForeground(goldBrush);
            userItem->setForeground(goldBrush);
            charItem->setForeground(goldBrush);
            scoreItem->setForeground(goldBrush);
            resultItem->setForeground(goldBrush);
            dateItem->setForeground(goldBrush);
        }
        else if (row == 1)
        {
            const QBrush silverBrush(QColor(220, 220, 230));
            rankItem->setForeground(silverBrush);
            userItem->setForeground(silverBrush);
            charItem->setForeground(silverBrush);
            scoreItem->setForeground(silverBrush);
            resultItem->setForeground(silverBrush);
            dateItem->setForeground(silverBrush);
        }
        else if (row == 2)
        {
            const QBrush bronzeBrush(QColor(205, 145, 85));
            rankItem->setForeground(bronzeBrush);
            userItem->setForeground(bronzeBrush);
            charItem->setForeground(bronzeBrush);
            scoreItem->setForeground(bronzeBrush);
            resultItem->setForeground(bronzeBrush);
            dateItem->setForeground(bronzeBrush);
        }

        leaderboardTable->setItem(row, 0, rankItem);
        leaderboardTable->setItem(row, 1, userItem);
        leaderboardTable->setItem(row, 2, charItem);
        leaderboardTable->setItem(row, 3, scoreItem);
        leaderboardTable->setItem(row, 4, resultItem);
        leaderboardTable->setItem(row, 5, dateItem);
    }

    leaderboardTable->resizeRowsToContents();
}

void LeaderboardPage::updateCurrentPlayerPanel()
{
    currentUserLabel->setText("Player: " + (currentUsername.isEmpty() ? "-" : currentUsername));

    const int rank = LeaderboardManager::getPlayerRank(currentUsername);
    if (rank == -1)
    {
        currentRankLabel->setText("Rank: Unranked");
        currentBestScoreLabel->setText("Best Score: -");
        return;
    }

    currentRankLabel->setText("Rank: #" + QString::number(rank));

    for (const LeaderboardEntry& entry : std::as_const(entries))
    {
        if (entry.username == currentUsername)
        {
            currentBestScoreLabel->setText("Best Score: " + QString::number(entry.breakdown.totalScore));
            return;
        }
    }

    currentBestScoreLabel->setText("Best Score: -");
}

void LeaderboardPage::showEntryDetails(const LeaderboardEntry& entry)
{
    selectedPlayerLabel->setText("Selected: " + entry.username);
    selectedMetaLabel->setText(
        entry.characterName + " | " +
        formatResultText(entry.victory) + " | " +
        formatDateText(entry.dateTime)
        );

    selectedTotalScoreLabel->setText("Total Score: " + QString::number(entry.breakdown.totalScore));

    explorerValueLabel->setText(QString::number(entry.breakdown.explorer));
    survivorValueLabel->setText(QString::number(entry.breakdown.survivor));
    treasureValueLabel->setText(QString::number(entry.breakdown.treasureHunter));
    collectorValueLabel->setText(QString::number(entry.breakdown.collector));
    deckBuilderValueLabel->setText(QString::number(entry.breakdown.deckBuilder));
    victoryBonusValueLabel->setText(QString::number(entry.breakdown.victoryBonus));
}

void LeaderboardPage::clearDetails()
{
    selectedPlayerLabel->setText("Selected: -");
    selectedMetaLabel->setText("-");
    selectedTotalScoreLabel->setText("Total Score: -");

    explorerValueLabel->setText("0");
    survivorValueLabel->setText("0");
    treasureValueLabel->setText("0");
    collectorValueLabel->setText("0");
    deckBuilderValueLabel->setText("0");
    victoryBonusValueLabel->setText("0");
}

void LeaderboardPage::onTableSelectionChanged()
{
    const int row = leaderboardTable->currentRow();

    if (row < 0 || row >= leaderboardTable->rowCount() || row >= entries.size())
    {
        clearDetails();
        return;
    }

    showEntryDetails(entries[row]);
}

QString LeaderboardPage::formatResultText(bool victory) const
{
    return victory ? "Victory" : "Defeat";
}

QString LeaderboardPage::formatDateText(const QString& rawDate) const
{
    return rawDate;
}

void LeaderboardPage::paintEvent(QPaintEvent* event)
{
    Q_UNUSED(event);

    QPainter painter(this);
    painter.setRenderHint(QPainter::SmoothPixmapTransform, true);

    if (!backgroundPixmap.isNull())
    {
        painter.drawPixmap(rect(), backgroundPixmap);
    }
    else
    {
        painter.fillRect(rect(), QColor(18, 18, 24));
    }
}

void LeaderboardPage::resizeEvent(QResizeEvent* event)
{
    QWidget::resizeEvent(event);

    if (mainPanel)
    {
        const int panelW = qMin(width() - 60, 1170);
        const int panelH = qMin(height() - 50, 650);
        mainPanel->setGeometry((width() - panelW) / 2, (height() - panelH) / 2, panelW, panelH);
    }
}
