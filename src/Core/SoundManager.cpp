#include "SoundManager.hpp"
#include <iostream>

SoundManager::SoundManager() : isMusicPlaying(false), isInitialized(false) {
    currentMusic.ctxData = nullptr;
}

SoundManager::~SoundManager() {
    cleanup();
}

void SoundManager::init() {
    if (isInitialized) return;
    InitAudioDevice();
    if (!IsAudioDeviceReady()) {
        std::cerr << "Audio device failed to initialize!" << std::endl;
    }

    // Subscribe to gameplay events for automatic sound effect playing
    EventManager::getInstance().subscribe(EventType::PlayerJump, this);
    EventManager::getInstance().subscribe(EventType::PlayerHurt, this);
    EventManager::getInstance().subscribe(EventType::PlayerDied, this);
    EventManager::getInstance().subscribe(EventType::CoinCollected, this);
    EventManager::getInstance().subscribe(EventType::EnemyStomped, this);
    EventManager::getInstance().subscribe(EventType::BrickBroken, this);
    isInitialized = true;
}

void SoundManager::cleanup() {
    if (!isInitialized) return;
    // Unsubscribe from events
    EventManager::getInstance().unsubscribe(EventType::PlayerJump, this);
    EventManager::getInstance().unsubscribe(EventType::PlayerHurt, this);
    EventManager::getInstance().unsubscribe(EventType::PlayerDied, this);
    EventManager::getInstance().unsubscribe(EventType::CoinCollected, this);
    EventManager::getInstance().unsubscribe(EventType::EnemyStomped, this);
    EventManager::getInstance().unsubscribe(EventType::BrickBroken, this);

    stopMusic();

    // Unload all sounds
    for (auto const& [id, snd] : sounds) {
        UnloadSound(snd);
    }
    sounds.clear();

    // Unload all music
    for (auto const& [id, mus] : musicStreams) {
        UnloadMusicStream(mus);
    }
    musicStreams.clear();

    CloseAudioDevice();
    isInitialized = false;
}

bool SoundManager::loadSound(const std::string& id, const std::string& filePath) {
    if (sounds.find(id) != sounds.end()) {
        return true;
    }

    Sound snd = LoadSound(filePath.c_str());
    if (snd.frameCount == 0) {
        std::cerr << "Failed to load sound: " << filePath << std::endl;
        return false;
    }

    sounds[id] = snd;
    return true;
}

void SoundManager::playSound(const std::string& id) {
    auto it = sounds.find(id);
    if (it != sounds.end()) {
        PlaySound(it->second);
    }
}

bool SoundManager::loadMusic(const std::string& id, const std::string& filePath) {
    if (musicStreams.find(id) != musicStreams.end()) {
        return true;
    }

    Music mus = LoadMusicStream(filePath.c_str());
    if (mus.frameCount == 0) {
        std::cerr << "Failed to load music: " << filePath << std::endl;
        return false;
    }

    musicStreams[id] = mus;
    return true;
}

void SoundManager::playMusic(const std::string& id) {
    auto it = musicStreams.find(id);
    if (it != musicStreams.end()) {
        stopMusic();
        currentMusic = it->second;
        PlayMusicStream(currentMusic);
        isMusicPlaying = true;
    }
}

void SoundManager::stopMusic() {
    if (isMusicPlaying) {
        StopMusicStream(currentMusic);
        isMusicPlaying = false;
        currentMusic.ctxData = nullptr;
    }
}

void SoundManager::update() {
    if (isMusicPlaying && currentMusic.ctxData != nullptr) {
        UpdateMusicStream(currentMusic);
    }
}

void SoundManager::onEvent(EventType type, void* data) {
    (void)data; // Unused
    switch (type) {
        case EventType::PlayerJump:
            playSound("jump");
            break;
        case EventType::PlayerHurt:
            playSound("hurt");
            break;
        case EventType::PlayerDied:
            playSound("die");
            break;
        case EventType::CoinCollected:
            playSound("coin");
            break;
        case EventType::EnemyStomped:
            playSound("stomp");
            break;
        case EventType::BrickBroken:
            playSound("break");
            break;
        default:
            break;
    }
}
