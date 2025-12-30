//
//  Rigidbody.hpp
//  FR-Ocean Engine
//
//  Box2D physics wrapper providing 2D rigid body dynamics, collision detection,
//  and trigger volumes for actors.
//
//  Created by Jack Gaffney on 4/13/25.
//

#ifndef Rigidbody_hpp
#define Rigidbody_hpp

#include <string>
#include "box2d/box2d.h"
#include "Actor.hpp"

/**
 * @class Rigidbody
 * @brief Box2D physics wrapper for 2D rigid body dynamics and collision detection.
 *
 * Rigidbody provides a Lua-friendly interface to Box2D physics. It supports:
 * - Three body types: dynamic, kinematic, static
 * - Collision shapes: box, circle
 * - Trigger volumes (sensors) for overlap detection without physical collision
 * - Physics properties: friction, bounciness, density, gravity scale
 * - Forces and velocity control
 *
 * Body Types:
 * - "dynamic": Affected by forces and gravity, has mass
 * - "kinematic": Moves via velocity, not affected by forces
 * - "static": Immovable, infinite mass (e.g., walls, floors)
 *
 * Collision Callbacks (Lua):
 * - OnCollisionEnter(other_actor): Called when collision starts
 * - OnCollisionStay(other_actor): Called every frame while colliding
 * - OnCollisionExit(other_actor): Called when collision ends
 * - OnTriggerEnter(other_actor): Called when trigger overlap starts
 * - OnTriggerStay(other_actor): Called every frame while overlapping
 * - OnTriggerExit(other_actor): Called when trigger overlap ends
 *
 * Usage (from Lua component):
 * ```lua
 * rb = actor:GetComponent("Rigidbody")
 * rb:SetVelocity({x = 5, y = 0})
 * rb:AddForce({x = 0, y = -10})
 * ```
 *
 * @note Rigidbody is a C++ component exposed to Lua via LuaBridge
 */
class Rigidbody {

public:
    /**
     * @brief Initializes the rigidbody and creates the Box2D body.
     *
     * @param owner Pointer to the owning Actor
     *
     * @note This is called automatically by SceneDB after component creation
     */
    void Init(Actor * owner);

    /**
     * @brief Gets the current position of the rigidbody.
     *
     * @return 2D vector containing world position
     */
    b2Vec2 GetPosition() const;

    /**
     * @brief Gets the current rotation in degrees.
     *
     * @return Rotation in degrees (clockwise)
     */
    float GetRotation() const;

    /**
     * @brief Applies a force to the rigidbody (affects dynamic bodies only).
     *
     * @param force Force vector to apply (world space)
     *
     * @note Forces accumulate and are reset after each physics step
     */
    void AddForce(const b2Vec2& force);

    /**
     * @brief Sets the linear velocity directly.
     *
     * @param velocity Velocity vector (units per second)
     *
     * @note This overrides the current velocity instantly
     */
    void SetVelocity(const b2Vec2& velocity);

    /**
     * @brief Teleports the rigidbody to a new position.
     *
     * @param position New world position
     *
     * @note This is an instant teleport, not a smooth movement
     */
    void SetPosition(const b2Vec2& position);

    /**
     * @brief Sets the rotation directly.
     *
     * @param degrees_clockwise New rotation in degrees (clockwise)
     */
    void SetRotation(float degrees_clockwise);

    /**
     * @brief Sets the angular velocity (rotation speed).
     *
     * @param degrees_clockwise Angular velocity in degrees per second (clockwise)
     */
    void SetAngularVelocity(float degrees_clockwise);

    /**
     * @brief Sets the gravity scale multiplier.
     *
     * @param scale Gravity scale (1.0 = normal, 0.0 = no gravity, -1.0 = inverted)
     */
    void SetGravityScale(float scale);

    /**
     * @brief Sets the "up" direction by rotating the body.
     *
     * @param direction Normalized direction vector
     *
     * @note This rotates the body to align its local "up" with the direction
     */
    void SetUpDirection(b2Vec2 direction);

    /**
     * @brief Sets the "right" direction by rotating the body.
     *
     * @param direction Normalized direction vector
     *
     * @note This rotates the body to align its local "right" with the direction
     */
    void SetRightDirection(b2Vec2 direction);

    /**
     * @brief Gets the current linear velocity.
     *
     * @return Velocity vector (units per second)
     */
    b2Vec2 GetVelocity() const;

    /**
     * @brief Gets the current angular velocity.
     *
     * @return Angular velocity in degrees per second (clockwise)
     */
    float GetAngularVelocity() const;

    /**
     * @brief Gets the current gravity scale.
     *
     * @return Gravity scale multiplier
     */
    float GetGravityScale() const;

    /**
     * @brief Gets the current "up" direction vector.
     *
     * @return Normalized up direction in world space
     */
    b2Vec2 GetUpDirection() const;

    /**
     * @brief Gets the current "right" direction vector.
     *
     * @return Normalized right direction in world space
     */
    b2Vec2 GetRightDirection() const;

    /**
     * @brief Cleans up the Box2D body when the component is destroyed.
     *
     * @note Called automatically when the actor is destroyed
     */
    void OnDestroy();

    /**
     * @brief Recreates all fixtures with current dimension properties.
     *
     * @param owner Pointer to the owning Actor (for fixture user data)
     *
     * @note Call this after changing width/height/radius if body already exists
     */
    void RecreateFixtures(Actor* owner);

    // Physics configuration properties (set before Init() or from Lua):

    float x = 0.0f;                             ///< Initial X position
    float y = 0.0f;                             ///< Initial Y position

    float width = 1.0f;                         ///< Collider box width
    float height = 1.0f;                        ///< Collider box height
    float radius = 0.5f;                        ///< Collider circle radius
    float friction = 0.3f;                      ///< Surface friction (0.0 = ice, 1.0 = rubber)
    float bounciness = 0.3f;                    ///< Restitution/elasticity (0.0 = no bounce, 1.0 = perfect bounce)

    float gravity_scale = 1.0f;                 ///< Gravity multiplier (1.0 = normal)
    float density = 1.0f;                       ///< Mass density (affects mass calculation)
    float angular_friction = 0.3f;              ///< Rotational damping (0.0 = no friction, 1.0 = high friction)
    float rotation = 0.0f;                      ///< Initial rotation in degrees

    float trigger_width = 1.0f;                 ///< Trigger box width
    float trigger_height = 1.0f;                ///< Trigger box height
    float trigger_radius = 0.5f;                ///< Trigger circle radius

    std::string body_type = "dynamic";          ///< Body type: "dynamic", "kinematic", "static"
    std::string collider_type = "box";          ///< Collider shape: "box", "circle"
    std::string trigger_type = "box";           ///< Trigger shape: "box", "circle"

    bool precise = true;                        ///< Use continuous collision detection (prevents tunneling)
    bool has_collider = true;                   ///< Enable collision shape
    bool has_trigger = true;                    ///< Enable trigger/sensor shape

private:
    b2Body* body = nullptr;                     ///< Box2D body handle
};

#endif /* Rigidbody_hpp */
