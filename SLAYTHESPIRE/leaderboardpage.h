#ifndef LEADERBOARDPAGE_H
#define LEADERBOARDPAGE_H

#include <QWidget>
#include <QVector>
#include <QPixmap>

#include "leaderboardmanager.h"

class QPushButton;
class QLabel;
class QTableWidget;
class QFrame;

class LeaderboardPage : public QWidget
{
    Q_OBJECT

public:
    explicit LeaderboardPage(const QString& currentUsername, QWidget* parent = nullptr);

    void refreshData();

signals:
    void backRequested();

protected:
    void paintEvent(QPaintEvent* event) override;
    void resizeEvent(QResizeEvent* event) override;

private slots:
    void onTableSelectionChanged();

private:
    void setupUi();
    void setupStyles();
    void loadAssets();
    void populateTable();
    void updateCurrentPlayerPanel();
    void showEntryDetails(const LeaderboardEntry& entry);
    void clearDetails();

    QWidget* createBreakdownCard(const QString& title,
                                 const QString& iconPath,
                                 QLabel*& valueLabel);

    QString formatResultText(bool victory) const;
    QString formatDateText(const QString& rawDate) const;

private:
    QString currentUsername;
    QVector<LeaderboardEntry> entries;

    QPixmap backgroundPixmap;

    QPushButton* backButton = nullptr;
    QLabel* titleLabel = nullptr;
    QFrame* mainPanel = nullptr;

    QFrame* playerSummaryFrame = nullptr;
    QLabel* currentUserLabel = nullptr;
    QLabel* currentRankLabel = nullptr;
    QLabel* currentBestScoreLabel = nullptr;

    QTableWidget* leaderboardTable = nullptr;

    QFrame* detailsFrame = nullptr;
    QLabel* selectedPlayerLabel = nullptr;
    QLabel* selectedMetaLabel = nullptr;
    QLabel* selectedTotalScoreLabel = nullptr;

    QLabel* explorerValueLabel = nullptr;
    QLabel* survivorValueLabel = nullptr;
    QLabel* treasureValueLabel = nullptr;
    QLabel* collectorValueLabel = nullptr;
    QLabel* deckBuilderValueLabel = nullptr;
    QLabel* victoryBonusValueLabel = nullptr;
};

#endif // LEADERBOARDPAGE_H
