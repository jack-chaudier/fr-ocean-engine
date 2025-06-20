//
//  CollisionListener.hpp
//  game_engine
//
//  Created by Jack Gaffney on 4/16/25.
//

#ifndef CollisionListener_hpp
#define CollisionListener_hpp

#include "box2d/box2d.h"
#include "Actor.hpp"
#include "SceneDB.hpp"
#include "lua/lua.h"
#include "LuaBridge/LuaBridge.h"

class CollisionListener : public b2ContactListener {
public:
    void BeginContact(b2Contact* contact) override {
        dispatch(contact, true);
    }
    void EndContact(b2Contact* contact) override {
        dispatch(contact, false);
    }
    
private:
    void dispatch(b2Contact* c, bool isEnter);
    
    void callLua(Actor* self, Actor* other,
                 const b2Vec2& pt, const b2Vec2& rv,
                 const b2Vec2& n, bool isEnter, bool isTrigger);
};

#endif /* CollisionListener_hpp */
