#pragma once
#include <SFML/Audio.hpp>
#include <map>
#include <list>
#include <string>
#include <memory>
#include <vector> // <-- NEW

class AudioManager {
private:
    std::map<std::string, sf::SoundBuffer> soundBuffers;
    std::list<sf::Sound> activeSounds;

    std::unique_ptr<sf::Music> backgroundMusic;

    std::vector<std::string> playlist;
    size_t currentTrackIndex;

public:
    AudioManager();
    ~AudioManager() = default;

    void loadSound(const std::string& name, const std::string& filepath);
    void playSound(const std::string& name);
    void updateVolumes();

    void playPlaylist(const std::vector<std::string>& filepaths, bool shuffle = true);

    void update();
};