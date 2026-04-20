//
//  DebugDraw.cpp
//  game_engine
//
//  Physics debug visualization implementing Box2D's b2Draw interface.
//

#include "DebugDraw.hpp"
#include "Renderer.hpp"
#include "RigidbodyWorld.hpp"
#include <cmath>

void DebugDraw::Init() {
    static DebugDraw inst;
    instance = &inst;
    instance->SetFlags(e_shapeBit | e_jointBit | e_aabbBit);
}

void DebugDraw::SetEnabled(bool value) {
    enabled = value;
}

bool DebugDraw::IsEnabled() {
    return enabled;
}

void DebugDraw::ToggleEnabled() {
    enabled = !enabled;
}

void DebugDraw::Render() {
    if (!enabled || !instance) {
        return;
    }

    b2World* world = RigidbodyWorld::GetWorld();
    if (!world) {
        return;
    }

    // Temporarily reset render scale so debug lines are drawn in pixel coords
    SDL_Renderer* sdl_renderer = Renderer::getSDLRenderer();
    float zoom = Renderer::GetCameraZoomFactor();
    SDL_RenderSetScale(sdl_renderer, 1.0f, 1.0f);

    world->SetDebugDraw(instance);
    world->DebugDraw();

    // Restore render scale
    SDL_RenderSetScale(sdl_renderer, zoom, zoom);
}

glm::ivec2 DebugDraw::WorldToScreen(const b2Vec2& worldPos) const {
    float zoom = Renderer::GetCameraZoomFactor();
    glm::vec2 cam = Renderer::GetCameraPosition();
    glm::ivec2 dims = Renderer::GetCameraDimensions();
    int sx = (int)((worldPos.x - cam.x) * 100.0f * zoom + dims.x * 0.5f);
    int sy = (int)((worldPos.y - cam.y) * 100.0f * zoom + dims.y * 0.5f);
    return {sx, sy};
}

void DebugDraw::DrawPolygon(const b2Vec2* vertices, int32 vertexCount, const b2Color& color) {
    SDL_Renderer* sdl_renderer = Renderer::getSDLRenderer();
    SDL_SetRenderDrawColor(sdl_renderer,
        (Uint8)(color.r * 255),
        (Uint8)(color.g * 255),
        (Uint8)(color.b * 255),
        128);

    for (int32 i = 0; i < vertexCount; ++i) {
        glm::ivec2 p1 = WorldToScreen(vertices[i]);
        glm::ivec2 p2 = WorldToScreen(vertices[(i + 1) % vertexCount]);
        SDL_RenderDrawLine(sdl_renderer, p1.x, p1.y, p2.x, p2.y);
    }
}

void DebugDraw::DrawSolidPolygon(const b2Vec2* vertices, int32 vertexCount, const b2Color& color) {
    // Draw outline with the provided color
    DrawPolygon(vertices, vertexCount, color);
}

void DebugDraw::DrawCircle(const b2Vec2& center, float radius, const b2Color& color) {
    SDL_Renderer* sdl_renderer = Renderer::getSDLRenderer();
    SDL_SetRenderDrawColor(sdl_renderer,
        (Uint8)(color.r * 255),
        (Uint8)(color.g * 255),
        (Uint8)(color.b * 255),
        128);

    float zoom = Renderer::GetCameraZoomFactor();
    glm::ivec2 c = WorldToScreen(center);
    int r = (int)(radius * 100.0f * zoom);

    // Midpoint circle algorithm
    int x = r;
    int y = 0;
    int err = 1 - r;

    while (x >= y) {
        SDL_RenderDrawPoint(sdl_renderer, c.x + x, c.y + y);
        SDL_RenderDrawPoint(sdl_renderer, c.x + y, c.y + x);
        SDL_RenderDrawPoint(sdl_renderer, c.x - y, c.y + x);
        SDL_RenderDrawPoint(sdl_renderer, c.x - x, c.y + y);
        SDL_RenderDrawPoint(sdl_renderer, c.x - x, c.y - y);
        SDL_RenderDrawPoint(sdl_renderer, c.x - y, c.y - x);
        SDL_RenderDrawPoint(sdl_renderer, c.x + y, c.y - x);
        SDL_RenderDrawPoint(sdl_renderer, c.x + x, c.y - y);

        y++;
        if (err < 0) {
            err += 2 * y + 1;
        } else {
            x--;
            err += 2 * (y - x) + 1;
        }
    }
}

void DebugDraw::DrawSolidCircle(const b2Vec2& center, float radius, const b2Vec2& axis, const b2Color& color) {
    DrawCircle(center, radius, color);

    // Draw axis line from center
    b2Vec2 endpoint = center + radius * axis;
    DrawSegment(center, endpoint, color);
}

void DebugDraw::DrawSegment(const b2Vec2& p1, const b2Vec2& p2, const b2Color& color) {
    SDL_Renderer* sdl_renderer = Renderer::getSDLRenderer();
    SDL_SetRenderDrawColor(sdl_renderer,
        (Uint8)(color.r * 255),
        (Uint8)(color.g * 255),
        (Uint8)(color.b * 255),
        128);

    glm::ivec2 sp1 = WorldToScreen(p1);
    glm::ivec2 sp2 = WorldToScreen(p2);
    SDL_RenderDrawLine(sdl_renderer, sp1.x, sp1.y, sp2.x, sp2.y);
}

void DebugDraw::DrawTransform(const b2Transform& xf) {
    const float axis_length = 0.4f;

    b2Vec2 p = xf.p;
    b2Vec2 px = p + axis_length * xf.q.GetXAxis();
    b2Vec2 py = p + axis_length * xf.q.GetYAxis();

    // X axis in red
    DrawSegment(p, px, b2Color(1.0f, 0.0f, 0.0f));
    // Y axis in green
    DrawSegment(p, py, b2Color(0.0f, 1.0f, 0.0f));
}

void DebugDraw::DrawPoint(const b2Vec2& p, float size, const b2Color& color) {
    SDL_Renderer* sdl_renderer = Renderer::getSDLRenderer();
    SDL_SetRenderDrawColor(sdl_renderer,
        (Uint8)(color.r * 255),
        (Uint8)(color.g * 255),
        (Uint8)(color.b * 255),
        255);

    glm::ivec2 sp = WorldToScreen(p);
    int half = (int)(size * 0.5f);

    SDL_Rect rect = {sp.x - half, sp.y - half, (int)size, (int)size};
    SDL_RenderFillRect(sdl_renderer, &rect);
}
