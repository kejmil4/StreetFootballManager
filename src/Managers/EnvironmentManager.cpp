#include "EnvironmentManager.h"
#include "../Core/Config.h" // For WINDOW_WIDTH and WINDOW_HEIGHT
#include <cstdlib> // For rand()

EnvironmentManager::EnvironmentManager(PitchType pitch, WeatherType weather)
    : currentPitch(pitch), currentWeather(weather), particleSpawnTimer(0.f)
{
    calculateModifiers();

    // Set particle spawn rates based on weather
    if (currentWeather == WeatherType::Rain) {
        particleSpawnRate = 0.005f; // Lots of rain!
    } else if (currentWeather == WeatherType::Snow) {
        particleSpawnRate = 0.02f;  // Slower, thicker snow
    } else {
        particleSpawnRate = 999.f;  // Clear weather, no particles
    }
}

void EnvironmentManager::calculateModifiers() {
    // 1. BASE PITCH MODIFIERS
    switch (currentPitch) {
        case PitchType::Grass:
            frictionMultiplier = 1.0f;
            bounceMultiplier = 1.0f;
            staminaDrainMultiplier = 1.0f;
            speedMultiplier = 1.0f;
            break;
        case PitchType::Asphalt:
            frictionMultiplier = 0.8f; // Less friction, ball rolls further
            bounceMultiplier = 1.3f;   // Hard surface, higher bounce
            staminaDrainMultiplier = 0.9f; // Easy to run on
            speedMultiplier = 1.1f;    // Players run slightly faster
            break;
        case PitchType::Mud:
            frictionMultiplier = 1.6f; // High friction, ball gets stuck
            bounceMultiplier = 0.3f;   // Mud absorbs the bounce
            staminaDrainMultiplier = 1.5f; // Exhausting to run through
            speedMultiplier = 0.8f;    // Players are slowed down
            break;
    }

    // 2. STACK WEATHER EFFECTS ON TOP
    switch (currentWeather) {
        case WeatherType::Clear:
            break; // No changes
        case WeatherType::Rain:
            frictionMultiplier *= 0.8f; // Everything becomes slick
            bounceMultiplier *= 0.9f;   // Slightly dampens bounce
            speedMultiplier *= 0.95f;   // Slight slipping slows players
            break;
        case WeatherType::Snow:
            frictionMultiplier *= 1.3f; // Snow drag slows the ball
            bounceMultiplier *= 0.6f;   // Snow absorbs bounce heavily
            staminaDrainMultiplier *= 1.2f; // Cold and trudging depletes stamina
            speedMultiplier *= 0.85f;   // Snow slows movement
            break;
    }
}

void EnvironmentManager::spawnParticle() {
    WeatherParticle p;
    // Spawn randomly across the top (and a bit off-screen to the right for wind)
    float startX = static_cast<float>(rand() % (Config::WINDOW_WIDTH + 400)) - 200.f;
    p.position = {startX, -50.f};

    if (currentWeather == WeatherType::Rain) {
        p.velocity = {-200.f, 600.f}; // Rains falls fast and slightly left (wind)
        p.shape.setSize({2.f, 15.f});
        p.shape.setFillColor(sf::Color(150, 150, 255, 150)); // Semi-transparent blue
        p.shape.setRotation(sf::degrees(0.f)); // Optional: angle it to match velocity if desired
    }
    else if (currentWeather == WeatherType::Snow) {
        // Random drift for snow
        float driftX = static_cast<float>((rand() % 100) - 50);
        p.velocity = {driftX, 150.f}; // Falls slower

        float size = static_cast<float>((rand() % 3) + 3); // 3 to 5 pixels
        p.shape.setSize({size, size});
        p.shape.setFillColor(sf::Color(255, 255, 255, 200));
    }

    p.shape.setPosition(p.position);
    particles.push_back(p);
}

void EnvironmentManager::update(float dt) {
    if (currentWeather == WeatherType::Clear) return;

    // 1. Spawn new particles
    particleSpawnTimer += dt;
    while (particleSpawnTimer >= particleSpawnRate) {
        spawnParticle();
        particleSpawnTimer -= particleSpawnRate;
    }

    // 2. Update existing particles and remove dead ones
    for (auto it = particles.begin(); it != particles.end(); ) {
        it->position += it->velocity * dt;
        it->shape.setPosition(it->position);

        // If particle goes below the screen, delete it
        if (it->position.y > Config::WINDOW_HEIGHT) {
            it = particles.erase(it);
        } else {
            ++it;
        }
    }
}

void EnvironmentManager::render(sf::RenderTarget& target) {
    if (currentWeather == WeatherType::Clear) return;

    for (const auto& p : particles) {
        target.draw(p.shape);
    }
}