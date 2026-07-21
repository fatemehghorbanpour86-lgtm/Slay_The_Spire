#include "eventpage.h"
#include "campfirepage.h"   // TopBarWidget
#include "event.h"
#include "map.h"
#include "player.h"
#include "campfire.h"       // for Ominous Forge's [Forge]
#include "upgradecards.h"   // Upgrade Dialog used by Campfire's Smith option
#include "relicviewer.h"
#include "deckviewer.h"
#include "audiomanager.h"

#include <QHBoxLayout>
#include <QVBoxLayout>

EventPage::EventPage(Player* playerPtr, Map* mapPtr, Event* eventPtr, QWidget* parent)
    : QWidget(parent), player(playerPtr), map(mapPtr), event(eventPtr)
{
    setupUI();

    if (topBar)
        topBar->updateData(player, map);

    setFixedSize(1280, 720);

    populateOptions();
}

QString EventPage::eventBackgroundPath() const
{
    if (!event)
        return QString(":/Event/EventDefault.png");

    QString cleanName = event->getName();
    cleanName.remove(' ');
    cleanName.remove('\'');
    cleanName.remove('.');

    return QString(":/Event/%1.png").arg(cleanName);
}

void EventPage::setupUI()
{
    setObjectName("EventPage");
    setStyleSheet(
        QString(
            "#EventPage {"
            "background-image: url(%1);"
            "background-repeat: no-repeat;"
            "background-position: center;"
            "background-color: #1a1410;"
            "}"
            "QToolTip { color: #facc15; background-color: #1f2937; border: 1px solid #b91c1c;"
            "border-radius: 4px; padding: 6px; font-weight: bold; font-family: Tahoma;"
            "}"
            ).arg(eventBackgroundPath())
        );

    QVBoxLayout* mainLayout = new QVBoxLayout(this);
    mainLayout->setContentsMargins(0, 0, 0, 0);
    mainLayout->setSpacing(0);

    // ---- Shared TopBar (same widget as Map/Campfire, nothing new) ----
    topBar = new TopBarWidget(this);

    connect(topBar, &TopBarWidget::relicClicked, this, [this]()
            {
                RelicViewerDialog dialog(player, this);
                dialog.exec();
            });

    connect(topBar, &TopBarWidget::deckClicked, this, [this]()
            {
                DeckViewerDialog dialog(player, this);
                dialog.exec();
            });

    // Settings hook left as a no-op, matching CampfirePage's current
    // placeholder behavior (its Settings dialog isn't wired up yet either).
    connect(topBar, &TopBarWidget::settingsClicked, this, []() {});

    mainLayout->addWidget(topBar);

    // ---- Content: background art (left, empty) + Event info (right) ----
    mainLayout->addStretch(1);

    QHBoxLayout* contentLayout = new QHBoxLayout();
    contentLayout->addStretch(1); // left side reserved for the background artwork

    QWidget* rightContainer = new QWidget(this);
    rightContainer->setFixedWidth(480);
    rightContainer->setStyleSheet("background: transparent;");

    QVBoxLayout* rightLayout = new QVBoxLayout(rightContainer);
    rightLayout->setSpacing(20);
    rightLayout->setAlignment(Qt::AlignTop);

    nameLabel = new QLabel(event ? event->getName() : QString(), this);
    nameLabel->setAlignment(Qt::AlignCenter);
    nameLabel->setWordWrap(true);
    nameLabel->setStyleSheet(
        "color: #facc15; font-size: 28px; font-weight: bold; background: transparent;");
    rightLayout->addWidget(nameLabel);

    descriptionLabel = new QLabel(event ? event->getDescription() : QString(), this);
    descriptionLabel->setAlignment(Qt::AlignCenter);
    descriptionLabel->setWordWrap(true);
    descriptionLabel->setStyleSheet(
        "color: #E8DCC0; font-size: 16px; background: transparent;");
    rightLayout->addWidget(descriptionLabel);

    optionsLayout = new QVBoxLayout();
    optionsLayout->setSpacing(14);
    optionsLayout->setAlignment(Qt::AlignCenter);
    rightLayout->addLayout(optionsLayout);

    contentLayout->addWidget(rightContainer);
    contentLayout->addSpacing(60);

    mainLayout->addLayout(contentLayout);
    mainLayout->addStretch(2);
}

void EventPage::populateOptions()
{
    if (!event || !optionsLayout)
        return;

    const QVector<EventOption>& options = event->getOptions();

    // Fully dynamic: however many options this Event defines is exactly
    // how many buttons are created - nothing hardcoded here.
    for (int i = 0; i < options.size(); ++i)
    {
        const EventOption& option = options[i];

        QPushButton* btn = new QPushButton(option.text, this);
        btn->setEnabled(option.enabled);
        btn->setMinimumHeight(52);
        btn->setCursor(Qt::PointingHandCursor);
        btn->setStyleSheet(
            "QPushButton { background-color: rgba(31,41,55,0.85); color: #facc15;"
            "border: 1px solid #b91c1c; border-radius: 8px; font-weight: bold;"
            "font-size: 14px; padding: 8px 16px; }"
            "QPushButton:hover { background-color: rgba(60,60,80,0.9); }"
            "QPushButton:pressed { margin: 3px 3px 3px 3px; }"
            "QPushButton:disabled { color: #6b7280; border-color: #4b5563; }"
            );

        connect(btn, &QPushButton::pressed, this, []()
                {
                    AudioManager::instance().play(AudioManager::Sound::ButtonClick);
                });

        connect(btn, &QPushButton::clicked, this, [this, i]()
                {
                    onOptionClicked(i);
                });

        optionsLayout->addWidget(btn);
    }
}

void EventPage::onOptionClicked(int optionIndex)
{
    if (!event || !player)
        return;

    bool isResolved = event->chooseOption(player, optionIndex, this);

    if (!isResolved)
        return;


    if (topBar)
        topBar->updateData(player, map);

    emit eventResolved();
}