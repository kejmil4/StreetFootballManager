#pragma once
#include "GameObject.h"

// A clean way to group all RPG-style stats together
struct EntityStats {
    float speed;
    float shooting;
    float passing;
    float tackling;
    float maxStamina;
};

class Entity : public GameObject {
protected:
    sf::Vector2f velocity;
    EntityStats stats;
    float currentStamina;

    void applyMovement(float dt);

public:
    Entity(float x, float y, const EntityStats& baseStats);
    virtual ~Entity() = default;

    virtual void render(sf::RenderTarget& target) override;


    virtual void update(float dt) = 0;

    float getStamina() const;
    const EntityStats& getStats() const;
};