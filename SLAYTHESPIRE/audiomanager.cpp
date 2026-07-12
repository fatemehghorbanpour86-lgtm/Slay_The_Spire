#include "audiomanager.h"
#include <QCoreApplication>
#include <QDir>

AudioManager& AudioManager::instance()
{
    static AudioManager manager;
    return manager;
}

AudioManager::AudioManager(QObject *parent)
    : QObject{parent}
{
}

void AudioManager::initialize()
{
    QString baseDir = QCoreApplication::applicationDirPath();
    auto addSound = [&](Sound sound, const QString &relativePath)
    {
        QSoundEffect *effect = new QSoundEffect(this);

        QString fullPath = QDir(baseDir).filePath(relativePath);

        effect->setSource(QUrl::fromLocalFile(fullPath));
        effect->setVolume(volume);

        effects.insert(sound, effect);
    };

    addSound(Sound::ButtonClick, "assets/audio/ButtonClicked.wav");
    addSound(Sound::Attack, "assets/audio/Attack.wav");
    addSound(Sound::Campfire, "assets/audio/Campfire.wav");
    addSound(Sound::Death, "assets/audio/DeathEnemyOrPlayer.wav");
    addSound(Sound::Drink, "assets/audio/DrinkPotion.wav");
    addSound(Sound::Raven, "assets/audio/RavenRelic.wav");
    addSound(Sound::Enemy, "assets/audio/IntentEnemy.wav");
    addSound(Sound::Reward, "assets/audio/Reward.wav");
    addSound(Sound::TakeDamage, "assets/audio/TakeDamage.wav");
    addSound(Sound::Defeat, "assets/audio/Defeat.wav");
}

void AudioManager::play(Sound sound)
{
    if (muted)
        return;

    if(effects.contains(sound))
        effects[sound]->play();
}

void AudioManager::setVolume(float volume)
{
    this->volume = volume;

    for(QSoundEffect* effect : std::as_const(effects))
        effect->setVolume(volume);
}

bool AudioManager::isMuted() const
{
    return muted;
}

void AudioManager::setMuted(bool muted)
{
    this->muted = muted;
}