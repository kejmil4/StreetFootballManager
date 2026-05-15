#pragma once
#include "GameObject.h"

class Pitch : public GameObject {
public:
    Pitch();
    ~Pitch() override = default;

    void update(float dt) override;
    void render(sf::RenderTarget& target) override;
};