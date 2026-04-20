//
//  CollisionListener.cpp
//  game_engine
//
//  Created by Jack Gaffney on 4/16/25.
//

#include "CollisionListener.hpp"

void CollisionListener::dispatch(b2Contact* c, bool isEnter)
{
    auto* A = c->GetFixtureA();
    auto* B = c->GetFixtureB();
    auto* actorA = reinterpret_cast<Actor*>(A->GetUserData().pointer);
    auto* actorB = reinterpret_cast<Actor*>(B->GetUserData().pointer);
    if (!actorA || !actorB) return;

    const bool sensorA = A->IsSensor();
    const bool sensorB = B->IsSensor();

    // Compute manifold (point + normal) once
    b2WorldManifold wm;
    c->GetWorldManifold(&wm);

    const b2Vec2 rawPoint  = wm.points[0];
    const b2Vec2 rawNormal = wm.normal;
    // relative velocity = vel(A) - vel(B)
    const b2Vec2 rawRelVel = A->GetBody()->GetLinearVelocity()
                           - B->GetBody()->GetLinearVelocity();

    const b2Vec2 sentinel(-999.0f, -999.0f);

    // ──────── COLLISION LIFECYCLE ─────────────────────
    // Only between *two* non‑sensor fixtures
    if (!sensorA && !sensorB) {
        // point & normal valid on enter; sentinel on exit
        b2Vec2 pt = isEnter ? rawPoint : sentinel;
        b2Vec2 n  = isEnter ? rawNormal : sentinel;
        const char* fn = isEnter ? "OnCollisionEnter" : "OnCollisionExit";
        callLua(actorA, actorB, pt, rawRelVel, n, fn);
        callLua(actorB, actorA, pt, rawRelVel, n, fn);
    }
    // ──────── TRIGGER LIFECYCLE ───────────────────────
    // Between any fixtures where at least one is a sensor
    else if (sensorA || sensorB) {
        // point & normal always sentinel for triggers
        const char* fn = isEnter ? "OnTriggerEnter" : "OnTriggerExit";
        callLua(actorA, actorB, sentinel, rawRelVel, sentinel, fn);
        callLua(actorB, actorA, sentinel, rawRelVel, sentinel, fn);
    }
}

void CollisionListener::dispatchStay(b2Contact* c)
{
    if (!c->IsTouching()) return;
    auto* A = c->GetFixtureA();
    auto* B = c->GetFixtureB();
    auto* actorA = reinterpret_cast<Actor*>(A->GetUserData().pointer);
    auto* actorB = reinterpret_cast<Actor*>(B->GetUserData().pointer);
    if (!actorA || !actorB) return;

    const bool sensorA = A->IsSensor();
    const bool sensorB = B->IsSensor();

    b2WorldManifold wm;
    c->GetWorldManifold(&wm);

    const b2Vec2 rawPoint  = wm.points[0];
    const b2Vec2 rawNormal = wm.normal;
    const b2Vec2 rawRelVel = A->GetBody()->GetLinearVelocity()
                           - B->GetBody()->GetLinearVelocity();

    const b2Vec2 sentinel(-999.0f, -999.0f);

    if (!sensorA && !sensorB) {
        callLua(actorA, actorB, rawPoint, rawRelVel, rawNormal, "OnCollisionStay");
        callLua(actorB, actorA, rawPoint, rawRelVel, rawNormal, "OnCollisionStay");
    } else if (sensorA || sensorB) {
        callLua(actorA, actorB, sentinel, rawRelVel, sentinel, "OnTriggerStay");
        callLua(actorB, actorA, sentinel, rawRelVel, sentinel, "OnTriggerStay");
    }
}

void CollisionListener::callLua(Actor* self, Actor* other,
                                const b2Vec2& pt, const b2Vec2& rv,
                                const b2Vec2& n, const char* fn) {
    // Skip if either actor has been destroyed
    if (self->destroyed || other->destroyed) return;

    auto L = ComponentDB::GetLuaState();
    
    for (auto const& key : self->component_keys) {
        auto it = self->components.find(key);
        if (it == self->components.end()) continue;
        luabridge::LuaRef comp = *it->second;

        // Skip C++ userdata components (like Rigidbody) - they don't have Lua callbacks
        if (comp.isUserdata()) continue;

        if (!comp[fn].isFunction()) continue;

        luabridge::LuaRef col = luabridge::newTable(L);
        col["other"] = other;
        col["point"] = pt;
        col["relative_velocity"] = rv;
        col["normal"] = n;

        try {
            comp[fn](comp, col);
        }
        catch (luabridge::LuaException& e) {
            SceneDB::ReportError(self->GetName(), e);
        }
    }
}
