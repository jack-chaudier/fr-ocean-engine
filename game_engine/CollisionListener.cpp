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
        callLua(actorA, actorB, pt, rawRelVel, n, isEnter, /*isTrigger=*/false);
        callLua(actorB, actorA, pt, rawRelVel, n, isEnter, /*isTrigger=*/false);
    }
    // ──────── TRIGGER LIFECYCLE ───────────────────────
    // Only between *two* sensor fixtures
    else if (sensorA && sensorB) {
        // point & normal always sentinel for triggers
        callLua(actorA, actorB, sentinel, rawRelVel, sentinel, isEnter, /*isTrigger=*/true);
        callLua(actorB, actorA, sentinel, rawRelVel, sentinel, isEnter, /*isTrigger=*/true);
    }
    // ──────── mixed sensor/collider ────────────────────
    // else: one sensor + one collider → ignore entirely
}

void CollisionListener::callLua(Actor* self, Actor* other,
                                const b2Vec2& pt, const b2Vec2& rv,
                                const b2Vec2& n, bool isEnter, bool isTrigger) {
    
    auto L = ComponentDB::GetLuaState();
    
    const char* fn;
    if (isTrigger)
        fn = isEnter ? "OnTriggerEnter" : "OnTriggerExit";
    else
        fn = isEnter ? "OnCollisionEnter" : "OnCollisionExit";

    for (auto const& key : self->component_keys) {
        auto it = self->components.find(key);
        if (it == self->components.end()) continue;
        luabridge::LuaRef comp = *it->second;
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
