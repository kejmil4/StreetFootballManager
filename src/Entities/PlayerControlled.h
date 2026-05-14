#pragma once
#include "Entity.h"

class PlayerControlled : public Entity {
private:
    void handleInput();

public:
    PlayerControlled(float x, float y, const EntityStats& baseStats);

    ~PlayerControlled() override = default;

    void update(float dt) override;
};