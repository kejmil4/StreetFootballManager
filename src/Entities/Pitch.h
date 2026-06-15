#pragma once
#include "GameObject.h"
#include "../States/MatchState.h"
#include <SFML/Graphics/Texture.hpp>
#include <SFML/Graphics/Sprite.hpp>

/**
 * @class Pitch
 * @brief The static environment where the match takes place.
 * Inherits from GameObject to seamlessly fit into the main render loop.
 * Handles displaying the playing surface (Asphalt, Mud, Grass) and visually
 * branding the field with the home team's central logo.
 */
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

    /**
     * Required by the GameObject contract, but intentionally left empty
     * since the pitch is a static background object.
     */
    void update(float dt) override;
    void render(sf::RenderTarget& target) override;
};