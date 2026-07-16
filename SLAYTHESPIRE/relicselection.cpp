#include "relicselection.h"
#include "relic.h"
#include "audiomanager.h"
#include "relicviewer.h"

#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QPushButton>

RelicSelectionDialog::RelicSelectionDialog(const QVector<Relic*>& relicChoices, QWidget* parent)
    : QDialog(parent), relicChoices(relicChoices), chosenRelic(nullptr)
{
    setupUI();
}

Relic* RelicSelectionDialog::getChosenRelic() const
{
    return chosenRelic;
}

void RelicSelectionDialog::setupUI()
{
    setWindowTitle("Choose a Relic");
    setStyleSheet("QDialog { border-image: url(:/Reward/SelectionViewer.png); border: none; background: transparent; }");

    QVBoxLayout* mainLayout = new QVBoxLayout(this);
    mainLayout->setContentsMargins(30, 30, 30, 30);

    QHBoxLayout* relicsLayout = new QHBoxLayout();
    relicsLayout->setSpacing(20);
    relicsLayout->setAlignment(Qt::AlignCenter);

    const int RELIC_SIZE = 140;

    for (Relic* relic : std::as_const(relicChoices))
    {
        if (!relic)
            continue;

        QPushButton* relicBtn = new QPushButton(this);
        relicBtn->setFixedSize(RELIC_SIZE, RELIC_SIZE);
        relicBtn->setCursor(Qt::PointingHandCursor);
        relicBtn->setToolTip(relic->getDescription());
        relicBtn->setStyleSheet(
            QString("QPushButton { border-image: url(%1); border: none; background: transparent; }"
                    "QPushButton:pressed { margin: 5px 5px 5px 5px; }" )
                .arg(RelicViewerDialog::relicIconPath(relic))
            );

        connect(relicBtn, &QPushButton::pressed, this, []() {
            AudioManager::instance().play(AudioManager::Sound::ButtonClick);
        });

        connect(relicBtn, &QPushButton::clicked, this, [this, relic]() {
            chosenRelic = relic;
            accept();
        });

        relicsLayout->addWidget(relicBtn);
    }

    mainLayout->addLayout(relicsLayout);
}