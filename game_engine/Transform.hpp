//
//  Transform.hpp
//  game_engine
//
//  Non-physics transform component for position, rotation, and scale.
//

#ifndef Transform_hpp
#define Transform_hpp

#include "box2d/box2d.h"
#include <cmath>

/**
 * @class Transform
 * @brief Non-physics transform component for UI and decorative objects.
 *
 * Provides position, rotation, and scale without Box2D physics simulation.
 * Use this for objects that don't need collision detection.
 */
class Transform {
public:
    Transform() = default;

    // Position
    float x = 0.0f;
    float y = 0.0f;

    // Rotation in degrees
    float rotation = 0.0f;

    // Scale
    float scale_x = 1.0f;
    float scale_y = 1.0f;

    /**
     * @brief Get position as Vector2.
     */
    b2Vec2 GetPosition() const { return b2Vec2(x, y); }

    /**
     * @brief Set position from Vector2.
     */
    void SetPosition(const b2Vec2& pos) { x = pos.x; y = pos.y; }

    /**
     * @brief Move by delta.
     */
    void Translate(const b2Vec2& delta) { x += delta.x; y += delta.y; }

    /**
     * @brief Get rotation in degrees.
     */
    float GetRotation() const { return rotation; }

    /**
     * @brief Set rotation in degrees.
     */
    void SetRotation(float degrees) { rotation = degrees; }

    /**
     * @brief Add to rotation.
     */
    void Rotate(float degrees) { rotation += degrees; }

    /**
     * @brief Get scale as Vector2.
     */
    b2Vec2 GetScale() const { return b2Vec2(scale_x, scale_y); }

    /**
     * @brief Set scale from Vector2.
     */
    void SetScale(const b2Vec2& scale) { scale_x = scale.x; scale_y = scale.y; }

    /**
     * @brief Set uniform scale.
     */
    void SetUniformScale(float scale) { scale_x = scale; scale_y = scale; }

    /**
     * @brief Get the up direction based on current rotation.
     */
    b2Vec2 GetUpDirection() const {
        float rad = rotation * (b2_pi / 180.0f);
        return b2Vec2(-std::sin(rad), -std::cos(rad));
    }

    /**
     * @brief Get the right direction based on current rotation.
     */
    b2Vec2 GetRightDirection() const {
        float rad = rotation * (b2_pi / 180.0f);
        return b2Vec2(std::cos(rad), -std::sin(rad));
    }
};

#endif /* Transform_hpp */
