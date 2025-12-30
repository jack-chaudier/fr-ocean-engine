//
//  RigidbodyWorld.cpp
//  game_engine
//
//  Created by Jack Gaffney on 4/14/25.
//

#include "RigidbodyWorld.hpp"
#include "Helper.h"
#include "CollisionListener.hpp"


void RigidbodyWorld::Init() {
    if (!world) {
        world = std::make_unique<b2World>(gravity);

        static CollisionListener listener;
        world->SetContactListener(&listener);
    }
}

void RigidbodyWorld::UpdateWorld() {
    if (world)
        world->Step(physics_timestep, velocity_iterations, position_iterations);
}

b2Body* RigidbodyWorld::AddRigidbody(const b2BodyDef& bodyDef) {
    if (world)
        return world->CreateBody(&bodyDef);
    
    return nullptr;
}

void RigidbodyWorld::Shutdown() {
    world.reset();
}
