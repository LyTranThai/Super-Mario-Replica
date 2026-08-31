#ifndef SOUND_MANAGER_H
#define SOUND_MANAGER_H

#include "raylib.h"
#include "EventSystem.h"
#include <map>
#include <string>

class SoundManager : public EventListener
{
private:
    std::map<std::string, Sound> sounds;
    std::map<std::string, Music> musicStreams;
    Music currentMusic;
    std::string currentMusicId; // Track which music is currently playing
    bool musicIsPlaying;        // Renamed to avoid conflict with isMusicPlaying(id) method
    bool isInitialized;

    SoundManager();

public:
    static SoundManager &getInstance()
    {
        static SoundManager instance;
        return instance;
    }

    SoundManager(const SoundManager &) = delete;
    SoundManager &operator=(const SoundManager &) = delete;

    ~SoundManager();

    void init();
    void cleanup();

    bool loadSound(const std::string &id, const std::string &filePath);
    void playSound(const std::string &id);

    bool loadMusic(const std::string &id, const std::string &filePath);
    void playMusic(const std::string &id);
    void stopMusic();
    bool isMusicPlaying(const std::string &id) const; // Check if a specific music is playing
    void update();                                    // Needs to be called every frame to update music buffer streams

    // Observer event callback
    void onEvent(EventType type, void *data = nullptr) override;
};

#endif // SOUND_MANAGER_H
