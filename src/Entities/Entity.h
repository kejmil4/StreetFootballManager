#pragma once
#include "GameObject.h"

/**
 * @enum Team
 * @brief Identifies which side an entity belongs to for collision and AI targeting.
 */
enum class Team {
    None,
    Home,
    Away
};

/**
 * @struct EntityStats
 * @brief RPG-style core attributes that govern an entity's physical capabilities.
 * These are used throughout the simulation for physics calculations (like shot power)
 * and AI decision-making (like lottery-based goal scoring).
 */
struct EntityStats {
    float speed;
    float shooting;
    float passing;
    float tackling;
    float maxStamina;
};

/**
 * @class Entity
 * @brief Abstract base class for any dynamic, living object on the pitch (e.g., Footballers).
 * Inherits from GameObject for basic 2D positioning, but adds complex state management
 * like RPG stats, stamina tracking, team affiliation, and cooldowns.
 */
class Entity : public GameObject {
protected:
    // --- Physics & Attributes ---
    sf::Vector2f velocity;
    EntityStats stats;

    // --- State Management ---
    float currentStamina;
    bool hasPossession = false;
    float tackleCooldown = 0.f;
    float stunTimer = 0.f;
    Team team;

    /**
     * Translates intended velocity into actual positional changes, factoring in
     * stamina drain and pitch boundaries. Intended to be called by child classes
     * during their specific update loops.
     */
    void applyMovement(float dt);

public:
    Entity(float x, float y, const EntityStats& baseStats);
    virtual ~Entity() = default;

    virtual void render(sf::RenderTarget& target) override;

    // Forces child classes to implement their own AI or Input logic
    virtual void update(float dt) = 0;

    float getStamina() const;
    const EntityStats& getStats() const;

    bool getPossession() const;
    void setPossession(bool state);

    bool canTackle() const;
    void resetTackleCooldown();
    void updateCooldowns(float dt);

    void stun(float duration);
    bool isStunned() const;

    Team getTeam() const;
    void setTeam(Team t);
};
