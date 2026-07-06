#include "audiomanager.h"

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
    auto addSound = [&](Sound sound, const QString &path)
    {
        QSoundEffect *effect = new QSoundEffect(this);
        effect->setSource(QUrl(path));
        effect->setVolume(volume);

        effects.insert(sound, effect);
    };

    addSound(Sound::ButtonClick, "qrc:/Audio/ButtonClicked.wav");
    addSound(Sound::Attack, "qrc:/Audio/Attack.wav");
    addSound(Sound::Campfire, "qrc:/Audio/Campfire.wav");
    addSound(Sound::Death, "qrc:/Audio/DeathEnemyOrPlayer.wav");
    addSound(Sound::Drink, "qrc:/Audio/DrinkPotion.wav");
    addSound(Sound::Raven, "qrc:/Audio/RavenRelic.wav");
    addSound(Sound::Enemy, "qrc:/Audio/IntentEnemy.wav");
    addSound(Sound::Reward, "qrc:/Audio/Reward.wav");
    addSound(Sound::TakeDamage, "qrc:/Audio/TakeDamage.wav");
    addSound(Sound::Defeat, "qrc:/Audio/Defeat.wav");
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