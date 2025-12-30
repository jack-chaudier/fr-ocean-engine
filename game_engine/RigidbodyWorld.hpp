//
//  RigidbodyWorld.hpp
//  game_engine
//
//  Created by Jack Gaffney on 4/14/25.
//

#ifndef RigidbodyWorld_hpp
#define RigidbodyWorld_hpp

#include <memory>
#include "box2d/box2d.h"

class RigidbodyWorld {
public:
    static void Init();
    static void UpdateWorld();

    static b2Body* AddRigidbody(const b2BodyDef& bodyDef);

    static b2World* GetWorld() { return world.get(); }

    static void Shutdown();

    // Configurable physics settings
    static void SetPhysicsTimestep(float timestep) { physics_timestep = timestep; }
    static float GetPhysicsTimestep() { return physics_timestep; }
    static void SetIterations(int velocity, int position) {
        velocity_iterations = velocity;
        position_iterations = position;
    }

private:
    inline static b2Vec2 gravity = b2Vec2(0.0f, 9.8f);

    inline static std::unique_ptr<b2World> world = nullptr;

    // Configurable physics parameters
    inline static float physics_timestep = 1.0f / 60.0f;
    inline static int velocity_iterations = 8;
    inline static int position_iterations = 3;
};

#endif /* RigidbodyWorld_hpp */
