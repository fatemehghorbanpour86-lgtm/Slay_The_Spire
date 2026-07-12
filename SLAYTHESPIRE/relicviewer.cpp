#include "relicviewer.h"
#include <QVBoxLayout>
#include <QGridLayout>
#include <QScrollArea>
#include <QLabel>
#include <QWidget>

RelicViewerDialog::RelicViewerDialog(Player* player, QWidget* parent)
    : QDialog(parent), player(player)
{
    setWindowTitle("Relics");
    setFixedSize(460, 520);
    setStyleSheet(
        "QDialog {"
        "border-image: url(:/Relic/RelicViewer.png);"
        "}"
        "QToolTip {"
        "color: #facc15;"
        "background-color: #1f2937;"
        "border: 1px solid #b91c1c;"
        "border-radius: 4px;"
        "padding: 6px;"
        "font-weight: bold;"
        "}"
        );

    setupUI();
}

QString RelicViewerDialog::relicIconPath(const Relic* relic)
{
    if (!relic)
        return QString();

    QString cleanName = relic->getName();
    cleanName.remove(' ');
    cleanName.remove('\'');
    cleanName.remove('.');

    return QString(":/Relic/%1.png").arg(cleanName);
}

void RelicViewerDialog::setupUI()
{
    QVBoxLayout* mainLayout = new QVBoxLayout(this);
    mainLayout->setContentsMargins(20, 20, 20, 20);

    QScrollArea* scrollArea = new QScrollArea(this);
    scrollArea->setWidgetResizable(true);
    scrollArea->setFrameShape(QFrame::NoFrame);
    scrollArea->setStyleSheet("QScrollArea { background: transparent; border: none; }");
    scrollArea->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    scrollArea->viewport()->setAutoFillBackground(false);
    scrollArea->viewport()->setStyleSheet("background: transparent;");

    QWidget* container = new QWidget();
    container->setStyleSheet("background: transparent;");

    QGridLayout* gridLayout = new QGridLayout(container);
    gridLayout->setSpacing(14);
    gridLayout->setAlignment(Qt::AlignTop | Qt::AlignLeft);

    const int columns = 4;

    if (player)
    {
        const QVector<Relic*>& relics = player->getAllRelics();

        for (int i = 0; i < relics.size(); ++i)
        {
            Relic* relic = relics[i];
            if (!relic)
                continue;

            QWidget* itemWidget = new QWidget();
            QVBoxLayout* itemLayout = new QVBoxLayout(itemWidget);
            itemLayout->setContentsMargins(0, 0, 0, 0);
            itemLayout->setAlignment(Qt::AlignCenter);

            QLabel* iconLabel = new QLabel();

            QPixmap pix(relicIconPath(relic));

            iconLabel->setPixmap(
                pix.scaled(64, 64,
                           Qt::KeepAspectRatio,
                           Qt::SmoothTransformation));

            iconLabel->setAlignment(Qt::AlignCenter);
            iconLabel->setFixedSize(64,64);
            iconLabel->setStyleSheet("background: transparent;");

            iconLabel->setToolTip(relic->getDescription());

            QLabel* nameLabel = new QLabel(relic->getName());
            nameLabel->setStyleSheet("color: #E8DCC0; font-size: 11px; font-weight: bold;");
            nameLabel->setAlignment(Qt::AlignCenter);
            nameLabel->setWordWrap(true);
            nameLabel->setFixedWidth(90);
            nameLabel->setToolTip(relic->getDescription());

            itemLayout->addWidget(iconLabel, 0, Qt::AlignCenter);
            itemLayout->addWidget(nameLabel);

            gridLayout->addWidget(itemWidget, i / columns, i % columns);
        }
    }

    scrollArea->setWidget(container);
    mainLayout->addWidget(scrollArea);
}