// PhysicsQuery.hpp
// game_engine
//
//  Created by Jack Gaffney on 4/20/25.
//
#ifndef PhysicsQuery_hpp
#define PhysicsQuery_hpp

#include "box2d/box2d.h"
#include "Actor.hpp"
#include "ComponentDB.hpp"
#include <vector>
#include <lua/lua.hpp>

struct HitResult {
    Actor* actor;
    b2Vec2 point;
    b2Vec2 normal;
    bool   is_trigger;
    float  fraction;
};

class SingleRaycastCallback : public b2RayCastCallback {
public:
    float ReportFixture(b2Fixture* fixture,
                        const b2Vec2& point,
                        const b2Vec2& normal,
                        float fraction) override;

    HitResult GetHitResult() const { return m_closestHit; }
    bool      HasHit()       const { return m_hasHit; }

private:
    HitResult m_closestHit{};
    float     m_closestFraction = 1.0f;
    bool      m_hasHit          = false;
};

class MultiRaycastCallback : public b2RayCastCallback {
public:
    float ReportFixture(b2Fixture* fixture,
                        const b2Vec2& point,
                        const b2Vec2& normal,
                        float fraction) override;

    std::vector<HitResult> GetAllHitResults() const { return m_hitResults; }

private:
    std::vector<HitResult> m_hitResults;
};

class PhysicsQuery {
public:
    // Lua-facing bindings: (Vector2 start, Vector2 dir, float distance)
    static luabridge::LuaRef Raycast(const b2Vec2& start,
                                     const b2Vec2& direction,
                                     float distance);

    static luabridge::LuaRef RaycastAll(const b2Vec2& start,
                                        const b2Vec2& direction,
                                        float distance);

private:
    // Internal implementations grab the Lua state themselves
    static luabridge::LuaRef RaycastImpl(const b2Vec2& start,
                                         const b2Vec2& direction,
                                         float distance);
    static luabridge::LuaRef RaycastAllImpl(const b2Vec2& start,
                                            const b2Vec2& direction,
                                            float distance);
};

#endif /* PhysicsQuery_hpp */
