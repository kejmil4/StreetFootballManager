#pragma once
#include "GameObject.h"

class Pitch : public GameObject {
private:
    sf::Texture texture;
    sf::Sprite sprite;
public:
    Pitch();
    ~Pitch() override = default;

    void update(float dt) override;
    void render(sf::RenderTarget& target) override;
};