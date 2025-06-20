// PhysicsQuery.cpp
// game_engine
//
//  Created by Jack Gaffney on 4/20/25.
//
#include "PhysicsQuery.hpp"
#include "RigidbodyWorld.hpp"
#include "ComponentDB.hpp"
#include <algorithm>

//
// Single‐hit callback
//
float SingleRaycastCallback::ReportFixture(b2Fixture* fixture,
                                           const b2Vec2& point,
                                           const b2Vec2& normal,
                                           float fraction)
{
    Actor* actor = reinterpret_cast<Actor*>(fixture->GetUserData().pointer);
    if (!actor) return -1.0f;           // skip phantom/unowned

    // skip fixtures with no contacts (phantoms masked out)
    if (fixture->GetFilterData().maskBits == 0)
        return -1.0f;

    if (fraction < m_closestFraction) {
        m_closestHit = { actor, point, normal,
                         fixture->IsSensor(), fraction };
        m_closestFraction = fraction;
        m_hasHit = true;
    }
    return m_closestFraction;           // truncate ray here
}

//
// Multi‐hit callback
//
float MultiRaycastCallback::ReportFixture(b2Fixture* fixture,
                                          const b2Vec2& point,
                                          const b2Vec2& normal,
                                          float fraction)
{
    Actor* actor = reinterpret_cast<Actor*>(fixture->GetUserData().pointer);
    if (!actor) return -1.0f;           // skip phantom

    if (fixture->GetFilterData().maskBits == 0)
        return -1.0f;

    m_hitResults.push_back({ actor, point, normal,
                             fixture->IsSensor(), fraction });
    return 1.0f;                        // continue ray
}

//
// Lua‐binding entry points
//
luabridge::LuaRef PhysicsQuery::Raycast(const b2Vec2& start,
                                        const b2Vec2& direction,
                                        float distance)
{
    return RaycastImpl(start, direction, distance);
}

luabridge::LuaRef PhysicsQuery::RaycastAll(const b2Vec2& start,
                                           const b2Vec2& direction,
                                           float distance)
{
    return RaycastAllImpl(start, direction, distance);
}

//
// Internal implementations
//
luabridge::LuaRef PhysicsQuery::RaycastImpl(const b2Vec2& start,
                                            const b2Vec2& direction,
                                            float distance)
{
    lua_State* L = ComponentDB::GetLuaState();
    if (distance <= 0.0f || !RigidbodyWorld::GetWorld())
        return luabridge::LuaRef(L);  // nil

    b2Vec2 dir = direction;
    dir.Normalize();
    b2Vec2 end = start + distance * dir;

    SingleRaycastCallback cb;
    RigidbodyWorld::GetWorld()->RayCast(&cb, start, end);

    if (!cb.HasHit())
        return luabridge::LuaRef(L);  // nil

    auto hit = cb.GetHitResult();
    auto tbl = luabridge::newTable(L);
    tbl["actor"]      = hit.actor;
    tbl["point"]      = hit.point;
    tbl["normal"]     = hit.normal;
    tbl["is_trigger"] = hit.is_trigger;
    return tbl;
}

luabridge::LuaRef PhysicsQuery::RaycastAllImpl(const b2Vec2& start,
                                               const b2Vec2& direction,
                                               float distance)
{
    lua_State* L = ComponentDB::GetLuaState();
    auto resultTbl = luabridge::newTable(L);

    if (distance <= 0.0f || !RigidbodyWorld::GetWorld())
        return resultTbl;

    b2Vec2 dir = direction;
    dir.Normalize();
    b2Vec2 end = start + distance * dir;

    MultiRaycastCallback cb;
    RigidbodyWorld::GetWorld()->RayCast(&cb, start, end);

    auto hits = cb.GetAllHitResults();
    std::sort(hits.begin(), hits.end(),
              [](auto &a, auto &b){ return a.fraction < b.fraction; });

    for (size_t i = 0; i < hits.size(); ++i) {
        auto &h = hits[i];
        auto tbl = luabridge::newTable(L);
        tbl["actor"]      = h.actor;
        tbl["point"]      = h.point;
        tbl["normal"]     = h.normal;
        tbl["is_trigger"] = h.is_trigger;
        resultTbl[i+1]    = tbl;  // Lua 1‑indexed
    }
    return resultTbl;
}
