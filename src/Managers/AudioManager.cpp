#include "AudioManager.h"
#include "../Core/Config.h"
#include <iostream>
#include <stdexcept>
#include <algorithm> // <-- For std::shuffle
#include <random>    // <-- For randomization

AudioManager::AudioManager() : currentTrackIndex(0) {
    backgroundMusic = std::make_unique<sf::Music>();
}

void AudioManager::loadSound(const std::string& name, const std::string& filepath) {
    try {
        sf::SoundBuffer buffer(filepath);
        soundBuffers.emplace(name, std::move(buffer));
    }
    catch (const std::exception& e) {
        std::cerr << "Failed to load audio: " << filepath << " - " << e.what() << "\n";
    }
}

void AudioManager::playSound(const std::string& name) {
    auto it = soundBuffers.find(name);
    if (it != soundBuffers.end()) {
        activeSounds.emplace_back(it->second);
        float finalVol = Config::sfxVolume * (Config::masterVolume / 100.f);
        activeSounds.back().setVolume(finalVol);
        activeSounds.back().play();
    }
}

// --- NEW: START THE PLAYLIST ---
void AudioManager::playPlaylist(const std::vector<std::string>& filepaths, bool shuffle) {
    if (filepaths.empty()) return;

    playlist = filepaths;
    currentTrackIndex = 0;

    // Optional: Randomize the playlist order!
    if (shuffle) {
        std::random_device rd;
        std::mt19937 g(rd());
        std::shuffle(playlist.begin(), playlist.end(), g);
    }

    if (backgroundMusic->openFromFile(playlist[currentTrackIndex])) {
        backgroundMusic->setLooping(false); // CRITICAL: Turn OFF loop so it actually ends!
        updateVolumes();
        backgroundMusic->play();
    } else {
        std::cerr << "Failed to load playlist track: " << playlist[currentTrackIndex] << "\n";
    }
}

void AudioManager::updateVolumes() {
    float finalMusicVol = Config::musicVolume * (Config::masterVolume / 100.f);
    backgroundMusic->setVolume(finalMusicVol);

    float finalSfxVol = Config::sfxVolume * (Config::masterVolume / 100.f);
    for (auto& sound : activeSounds) {
        sound.setVolume(finalSfxVol);
    }
}

// --- NEW: THE UPDATE LOOP ---
void AudioManager::update() {
    // 1. Clean up finished sound effects
    activeSounds.remove_if([](const sf::Sound& s) {
        return s.getStatus() == sf::Sound::Status::Stopped;
    });

    // 2. Check if the current song finished, and play the next one!
    if (!playlist.empty() && backgroundMusic->getStatus() == sf::Sound::Status::Stopped) {
        currentTrackIndex++;

        // If we reached the end of the playlist, loop back to the first song
        if (currentTrackIndex >= playlist.size()) {
            currentTrackIndex = 0;
        }

        if (backgroundMusic->openFromFile(playlist[currentTrackIndex])) {
            updateVolumes();
            backgroundMusic->play();
        }
    }
}