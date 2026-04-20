//
//  DebugDraw.hpp
//  game_engine
//
//  Physics debug visualization implementing Box2D's b2Draw interface.
//

#pragma once

#include "box2d/box2d.h"
#include "SDL2/SDL.h"
#include "glm/glm.hpp"

class DebugDraw : public b2Draw {
public:
    static void Init();
    static void SetEnabled(bool enabled);
    static bool IsEnabled();
    static void ToggleEnabled();
    static void Render();

    // b2Draw interface implementation
    void DrawPolygon(const b2Vec2* vertices, int32 vertexCount, const b2Color& color) override;
    void DrawSolidPolygon(const b2Vec2* vertices, int32 vertexCount, const b2Color& color) override;
    void DrawCircle(const b2Vec2& center, float radius, const b2Color& color) override;
    void DrawSolidCircle(const b2Vec2& center, float radius, const b2Vec2& axis, const b2Color& color) override;
    void DrawSegment(const b2Vec2& p1, const b2Vec2& p2, const b2Color& color) override;
    void DrawTransform(const b2Transform& xf) override;
    void DrawPoint(const b2Vec2& p, float size, const b2Color& color) override;

private:
    inline static bool enabled = false;
    inline static DebugDraw* instance = nullptr;

    // Convert Box2D world coords to screen coords
    glm::ivec2 WorldToScreen(const b2Vec2& worldPos) const;
};

