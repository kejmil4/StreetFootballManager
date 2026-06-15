#pragma once
#include <SFML/Graphics.hpp>
#include <vector>
#include "../States/MatchState.h" // Needed for PitchType and WeatherType enums

/**
 * @struct WeatherParticle
 * @brief A lightweight container for individual rain drops or snowflakes.
 * We use simple SFML Rectangles instead of textures to save memory and processing power
 * when rendering hundreds of particles simultaneously.
 */
struct WeatherParticle {
    sf::Vector2f position;
    sf::Vector2f velocity;
    sf::RectangleShape shape;
};

/**
 * @class EnvironmentManager
 * @brief Controls the ambient conditions of the match.
 * Serves two distinct purposes:
 * 1. Visuals: Spawns, updates, and culls weather particles (rain/snow).
 * 2. Physics: Calculates global gameplay modifiers (friction, bounce, speed)
 * based on the current surface and weather combination, which the Ball
 * and Footballer classes query during their update loops.
 */
class EnvironmentManager {
private:
    PitchType currentPitch;
    WeatherType currentWeather;

    // --- PHYSICS MODIFIERS ---
    // These act as percentage multipliers (1.0 = standard) applied to base entity stats
    float frictionMultiplier;
    float bounceMultiplier;
    float staminaDrainMultiplier;
    float speedMultiplier;

    // --- VISUAL EFFECTS ---
    std::vector<WeatherParticle> particles;
    float particleSpawnTimer;
    float particleSpawnRate;

    /**
     * Stacks the pitch type modifiers with the weather modifiers to generate
     * the final multipliers for the match.
     */
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