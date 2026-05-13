#pragma once
#include <SFML/Graphics.hpp>

class GameObject {
protected:
    sf::Vector2f position;
    sf::Texture texture;
    sf::Sprite sprite;

public:
    GameObject(float x, float y);
    virtual ~GameObject() = default;

    virtual void update(float dt) = 0; 

    virtual void render(sf::RenderTarget& target) = 0; 

    sf::Vector2f getPosition() const;
    sf::FloatRect getBounds() const;
};