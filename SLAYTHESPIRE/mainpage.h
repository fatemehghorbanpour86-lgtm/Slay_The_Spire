#ifndef MAINPAGE_H
#define MAINPAGE_H

#include <QWidget>
#include <QPushButton>
#include <QStyleOption>
#include <QPainter>


class mainpage : public QWidget
{
    Q_OBJECT
public:
    explicit mainpage(QWidget *parent = nullptr);
signals:
    void startGame();
private slots:
    void singlePlayerClicked();
    // void multiPlayerClicked();
    // void leaderBoardClicked();
    // void settingClicked();
    // void exitClicked();
private:
    QPushButton *singlePlayerBtn;
    QPushButton *multiPlayerBtn;
    QPushButton *leaderBoardBtn;
    QPushButton *settingBtn;
    QPushButton *exitBtn;
protected:
    void paintEvent(QPaintEvent *) override{
        QStyleOption opt;
        opt.initFrom(this);
        QPainter p(this);
        style()->drawPrimitive(QStyle::PE_Widget, &opt, &p, this);
    }
};

#endif // MAINPAGE_H
