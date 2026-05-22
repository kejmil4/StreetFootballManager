#pragma once
#include "GameObject.h"
#include "../States/MatchState.h"

class Pitch : public GameObject {
private:
    sf::Texture texture;
    sf::Sprite sprite;
public:
    Pitch(PitchType type = PitchType::Grass);
    ~Pitch() override = default;

    void update(float dt) override;
    void render(sf::RenderTarget& target) override;
};