#ifndef AUDIOMANAGER_H
#define AUDIOMANAGER_H

#include <QObject>
#include <QSoundEffect>
#include <QHash>

class AudioManager : public QObject
{
    Q_OBJECT
public:
    enum class Sound
    {
        ButtonClick,
        Attack,
        Campfire,
        Raven,
        Reward,
        Death,
        Drink,
        Enemy,
        TakeDamage,
        Defeat
    };

    static AudioManager& instance();

    void initialize();

    void play(Sound sound);

    void setVolume(float volume);

    bool isMuted() const;
    void setMuted(bool muted);

private:
    explicit AudioManager(QObject *parent = nullptr);

    QHash<Sound, QSoundEffect*> effects;

    float volume = 0.8f;
    bool muted = false;
};

#endif // AUDIOMANAGER_H
