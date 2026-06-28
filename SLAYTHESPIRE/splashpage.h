#ifndef SPLASHPAGE_H
#define SPLASHPAGE_H

#include <QWidget>
#include <QProgressBar>
#include <QTimer>
#include <QStyleOption>
#include <QPainter>

class SplashPage:public QWidget
{
    Q_OBJECT
public:
    explicit SplashPage(QWidget *parent = nullptr);
signals:
    void loadingFinished();

private slots:
    void updateLoadingProgress();
private:
    QProgressBar *loadingBar;
    QTimer *loadingTimer;
    int currentProgress;

protected:
    void paintEvent(QPaintEvent *) override{
        QStyleOption opt;
        opt.initFrom(this);
        QPainter p(this);
        style()->drawPrimitive(QStyle::PE_Widget, &opt, &p, this);
    }
};

#endif // SPLASHPAGE_H
