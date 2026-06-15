#include "EnvironmentManager.h"
#include "../Core/Config.h" // For WINDOW_WIDTH and WINDOW_HEIGHT
#include <cstdlib> // For rand()

EnvironmentManager::EnvironmentManager(PitchType pitch, WeatherType weather)
    : currentPitch(pitch), currentWeather(weather), particleSpawnTimer(0.f)
{
    calculateModifiers();

    /// Establish how frequently new particles are injected into the scene.
    // Lower rate = faster spawning = denser weather.
    if (currentWeather == WeatherType::Rain) {
        particleSpawnRate = 0.005f; // rain
    } else if (currentWeather == WeatherType::Snow) {
        particleSpawnRate = 0.02f;  // snow
    } else {
        particleSpawnRate = 999.f;  // clear
    }
}

void EnvironmentManager::calculateModifiers() {
    // --- Phase 1: Base Pitch Modifiers ---
    // Establish the baseline physics of the playing surface.
    switch (currentPitch) {
        case PitchType::Grass:
            frictionMultiplier = 1.0f;
            bounceMultiplier = 1.0f;
            staminaDrainMultiplier = 1.0f;
            speedMultiplier = 1.0f;
            break;
        case PitchType::Asphalt:
            frictionMultiplier = 0.8f;     // Smooth surface: ball rolls further
            bounceMultiplier = 1.3f;       // Hard surface: higher ball bounces
            staminaDrainMultiplier = 0.9f; // Solid footing: players exhaust slower
            speedMultiplier = 1.1f;        // Solid footing: players run slightly faster
            break;
        case PitchType::Mud:
            frictionMultiplier = 1.5f;     // Sticky surface: ball stops quickly
            bounceMultiplier = 0.5f;       // Soft surface: kills ball bounces
            staminaDrainMultiplier = 1.4f; // Difficult footing: players exhaust quickly
            speedMultiplier = 0.8f;        // Difficult footing: players move slower
            break;
    }

    // --- Phase 2: Weather Overrides ---
    // Stack weather conditions on top of the base pitch conditions.
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
    // Spawn particles anywhere across the top of the screen
    float startX = static_cast<float>(rand() % (Config::WINDOW_WIDTH + 400)) - 200.f;
    p.position = {startX, -50.f};

    if (currentWeather == WeatherType::Rain) {
        p.velocity = {-200.f, 600.f}; // Rains falls fast and slightly left (wind)
        p.shape.setSize({2.f, 15.f});
        p.shape.setFillColor(sf::Color(150, 150, 255, 150)); // Semi-transparent blue
        p.shape.setRotation(sf::degrees(0.f));
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