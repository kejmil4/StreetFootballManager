#pragma once
#include <SFML/Graphics.hpp>
#include <vector>
#include "../States/MatchState.h" // Needed for PitchType and WeatherType enums

// A simple struct to hold individual raindrops or snowflakes
struct WeatherParticle {
    sf::Vector2f position;
    sf::Vector2f velocity;
    sf::RectangleShape shape;
};

class EnvironmentManager {
private:
    PitchType currentPitch;
    WeatherType currentWeather;

    // --- PHYSICS MODIFIERS ---
    float frictionMultiplier;
    float bounceMultiplier;
    float staminaDrainMultiplier;
    float speedMultiplier;

    // --- VISUAL EFFECTS ---
    std::vector<WeatherParticle> particles;
    float particleSpawnTimer;
    float particleSpawnRate; // How fast particles spawn

    void calculateModifiers();
    void spawnParticle();

public:
    EnvironmentManager(PitchType pitch, WeatherType weather);
    ~EnvironmentManager() = default;

    void update(float dt);
    void render(sf::RenderTarget& target);

    // --- GETTERS FOR ENTITIES AND BALL ---
    float getFrictionMultiplier() const { return frictionMultiplier; }
    float getBounceMultiplier() const { return bounceMultiplier; }
    float getStaminaDrainMultiplier() const { return staminaDrainMultiplier; }
    float getSpeedMultiplier() const { return speedMultiplier; }
};