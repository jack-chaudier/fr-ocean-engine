//
//  Rigidbody.hpp
//  game_engine
//
//  Created by Jack Gaffney on 4/13/25.
//

#ifndef Rigidbody_hpp
#define Rigidbody_hpp

#include <string>
#include "box2d/box2d.h"
#include "Actor.hpp"

class Rigidbody {
    
public:
    void Init(Actor * owner);
    b2Vec2 GetPosition() const;
    float GetRotation() const;
    void AddForce(const b2Vec2& force);
    void SetVelocity(const b2Vec2& velocity);
    void SetPosition(const b2Vec2& position);
    void SetRotation(float degrees_clockwise);
    void SetAngularVelocity(float degrees_clockwise);
    void SetGravityScale(float scale);
    void SetUpDirection(b2Vec2 direction);
    void SetRightDirection(b2Vec2 direction);
    b2Vec2 GetVelocity() const;
    float GetAngularVelocity() const;
    float GetGravityScale() const;
    b2Vec2 GetUpDirection() const;
    b2Vec2 GetRightDirection() const;
    
    void OnDestroy();
    
    float x = 0.0f;
    float y = 0.0f;
    
    float width = 1.0f;
    float height = 1.0f;
    float radius = 0.5f;
    float friction = 0.3f;
    float bounciness = 0.3f;

    
    float gravity_scale = 1.0f;
    float density = 1.0f;
    float angular_friction = 0.3f;
    float rotation = 0.0f;
    
    float trigger_width = 1.0f;
    float trigger_height = 1.0f;
    float trigger_radius = 0.5f;
    
    std::string body_type = "dynamic";
    std::string collider_type = "box";
    std::string trigger_type = "box";
    
    bool precise = true;
    bool has_collider = true;
    bool has_trigger = true;

private:
    b2Body* body = nullptr;
};

#endif /* Rigidbody_hpp */
