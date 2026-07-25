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
class QDialog;

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
    void openEntryDetailsDialog(int row);

private:
    void setupUi();
    void setupStyles();
    void loadAssets();
    void populateTable();
    void updateCurrentPlayerPanel();

    QDialog* createDetailsDialog(const LeaderboardEntry& entry, QWidget* parent = nullptr);
    QWidget* createBreakdownCard(const QString& title,
                                 const QString& iconPath,
                                 int value,const QString& formula);

    QString formatResultText(RunStatus status) const;
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
};

#endif // LEADERBOARDPAGE_H
