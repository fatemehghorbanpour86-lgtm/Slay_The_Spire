#ifndef OUTLINEDLABEL_H
#define OUTLINEDLABEL_H

#include <QWidget>
#include <QPainter>
#include <QPainterPath>

class OutlinedLabel : public QWidget
{
public:
    explicit OutlinedLabel(const QString &text, QWidget *parent = nullptr)
        : QWidget(parent), m_text(text)
    {
        setAttribute(Qt::WA_TransparentForMouseEvents);
    }

    void setText(const QString &text)
    {
        m_text = text;
        update();
    }

protected:
    void paintEvent(QPaintEvent *) override
    {
        QPainter painter(this);
        painter.setRenderHint(QPainter::Antialiasing);

        QFont font("Arial", 18, QFont::Bold);
        painter.setFont(font);

        QPainterPath path;
        path.addText(0, 0, font, m_text);

        QRectF bounds = path.boundingRect();
        QPointF center = rect().center() - bounds.center();
        path.translate(center);

        QPen outerPen(QColor(0x7a, 0x1f, 0x1f));
        outerPen.setWidth(5);
        outerPen.setJoinStyle(Qt::RoundJoin);
        outerPen.setCapStyle(Qt::RoundCap);
        painter.setPen(outerPen);
        painter.setBrush(Qt::white);
        painter.drawPath(path);

        painter.setPen(Qt::NoPen);
        painter.setBrush(Qt::white);
        painter.drawPath(path);
    }


private:
    QString m_text;
};


#endif // OUTLINEDLABEL_H
