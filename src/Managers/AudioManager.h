#pragma once
#include <SFML/Audio.hpp>
#include <map>
#include <list>
#include <string>
#include <memory>
#include <vector>

/**
 * @class AudioManager
 * @brief Centralized subsystem for handling all game audio.
 * Separates audio into two distinct streams:
 * 1. SFX (Short, overlapping sounds like whistles and tackles) loaded entirely into RAM.
 * 2. Music (Long background tracks) streamed directly from the hard drive to save memory.
 */

class AudioManager {
private:
    // --- Sound Effects (SFX) ---
    // Maps string names (e.g., "whistle") to their heavy audio data in memory
    std::map<std::string, sf::SoundBuffer> soundBuffers;

    // std::list is used here instead of std::vector because we frequently
    // insert and delete sounds from the middle of the container as they finish playing.
    std::list<sf::Sound> activeSounds;

    // --- Background Music ---
    // sf::Music streams from the disk, so we only ever need one active instance
    std::unique_ptr<sf::Music> backgroundMusic;

    std::vector<std::string> playlist;
    size_t currentTrackIndex;

public:
    AudioManager();
    ~AudioManager() = default;

    /**
     * Loads a short audio file into RAM. Must be called before playSound.
     */
    void loadSound(const std::string& name, const std::string& filepath);

    /**
     * Spawns a new independent sound instance and plays it immediately.
     */
    void playSound(const std::string& name);

    /**
     * Recalculates all active audio volumes based on the global Config settings.
     */
    void updateVolumes();

    /**
     * Initiates a background music playlist, with an option to shuffle the track order.
     */
    void playPlaylist(const std::vector<std::string>& filepaths, bool shuffle = true);

    void update();
};