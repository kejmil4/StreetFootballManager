#pragma once
#include <SFML/Graphics.hpp>

/**
 * @class GameObject
 * @brief The absolute base class for every physical object in the game world.
 * * Whether it's a dynamic Footballer, the physics-driven Ball, or a static
 * Pitch marker, everything inherits from this. It binds a logical 2D coordinate
 * to a graphical SFML Sprite and enforces the standard game loop contract.
 */

class GameObject {
protected:
    sf::Vector2f position;
    sf::Texture texture;
    sf::Sprite sprite;

public:
    GameObject(float x, float y);
    virtual ~GameObject() = default;

    /**
     * Pure virtual update function. Forces every child class to explicitly define
     * how it behaves over time (dt = delta time).
     */
    virtual void update(float dt) = 0; 

    /**
     * Pure virtual render function. Forces child classes to explicitly draw
     * themselves to the provided SFML target (usually the main game window).
     */
    virtual void render(sf::RenderTarget& target) = 0;

    /**
     * Safely teleports the object and immediately syncs the sprite's graphics.
     * Made virtual so complex objects (like the Ball) can override this to also
     * reset their 3D physics states (Z-axis, velocity, etc.).
     */
    virtual void resetPosition(float x, float y);

    sf::Vector2f getPosition() const;

    /**
     * Returns the Axis-Aligned Bounding Box (AABB) of the graphical sprite.
     * Extremely useful for UI mouse-clicking or rudimentary rectangle collision.
     */
    sf::FloatRect getBounds() const;
};