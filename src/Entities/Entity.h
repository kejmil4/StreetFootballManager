#pragma once
#include "GameObject.h"

enum class Team {
    Home,
    Away
};

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
    bool hasPossession = false;
    float tackleCooldown = 0.f;
    float stunTimer = 0.f;
    Team team;

    void applyMovement(float dt);

public:
    Entity(float x, float y, const EntityStats& baseStats);
    virtual ~Entity() = default;

    virtual void render(sf::RenderTarget& target) override;


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
