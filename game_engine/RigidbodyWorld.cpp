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
        world = new b2World(gravity);

        static CollisionListener listener;
        world->SetContactListener(&listener);
    }
}

void RigidbodyWorld::UpdateWorld() {
    if (world)
        world->Step(1.0f / 60.0f, 8, 3);
}

b2Body* RigidbodyWorld::AddRigidbody(const b2BodyDef& bodyDef) {
    if (world)
        return world->CreateBody(&bodyDef);
    
    return nullptr;
}

void RigidbodyWorld::Shutdown() {
    if (world) {
        delete world;
        world = nullptr;
    }
}
