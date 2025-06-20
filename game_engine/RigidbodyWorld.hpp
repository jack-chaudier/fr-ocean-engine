//
//  RigidbodyWorld.hpp
//  game_engine
//
//  Created by Jack Gaffney on 4/14/25.
//

#ifndef RigidbodyWorld_hpp
#define RigidbodyWorld_hpp

#include "box2d/box2d.h"

class RigidbodyWorld {
public:
    static void Init();
    static void UpdateWorld();
    
    static b2Body* AddRigidbody(const b2BodyDef& bodyDef);
    
    static b2World* GetWorld() { return world; }
    
    static void Shutdown();

private:
    inline static b2Vec2 gravity = b2Vec2(0.0f, 9.8f);
    
    inline static b2World* world = nullptr;
};

#endif /* RigidbodyWorld_hpp */
