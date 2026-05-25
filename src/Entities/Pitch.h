#pragma once
#include "GameObject.h"
#include "../States/MatchState.h"
#include <SFML/Graphics/Texture.hpp>
#include <SFML/Graphics/Sprite.hpp>

class Pitch : public GameObject {
private:
    sf::Texture texture;
    sf::Sprite sprite;

    sf::Texture logoTexture;
    sf::Sprite logoSprite;
    bool hasLogo;

public:
    Pitch(PitchType type = PitchType::Grass, int logoId = -1);
    ~Pitch() override = default;

    void update(float dt) override;
    void render(sf::RenderTarget& target) override;
};