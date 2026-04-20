//
//  ParticleSystem.hpp
//  FR-Ocean Engine
//
//  Configurable particle emitter system with object pooling,
//  color/size interpolation, and gravity support.
//

#pragma once

#include <string>
#include <vector>
#include "glm/glm.hpp"

struct Particle {
    float x, y;
    float vx, vy;
    float lifetime;
    float max_lifetime;
    float size;
    float start_size;
    float end_size;
    float gravity;
    glm::vec4 start_color;
    glm::vec4 end_color;
    std::string image_name;
    int sorting_order = 999;
    bool active = false;
};

struct ParticleConfig {
    float lifetime_min = 0.5f;
    float lifetime_max = 1.5f;
    float speed_min = 50.0f;
    float speed_max = 150.0f;
    float spread_angle = 360.0f;  // degrees
    float direction = 270.0f;     // degrees (270 = up)
    float gravity = 0.0f;
    float start_size = 8.0f;
    float end_size = 0.0f;
    glm::vec4 start_color = {255, 255, 255, 255};
    glm::vec4 end_color = {255, 255, 255, 0};
    std::string image_name = "";  // empty = use default particle texture
    int sorting_order = 999;

    void SetStartColor(float r, float g, float b, float a) {
        start_color = {r, g, b, a};
    }
    void SetEndColor(float r, float g, float b, float a) {
        end_color = {r, g, b, a};
    }
};

struct ParticleDrawData {
    std::string image_name;
    float x, y;
    float size;
    int r, g, b, a;
    int sorting_order;
};

class ParticleSystem {
public:
    static void Init();
    static void Clear();
    static void Update(float dt);

    // Emit particles at position with config
    static void Emit(float x, float y, int count, const ParticleConfig& config);

    // Get current particles for rendering
    static const std::vector<ParticleDrawData>& GetDrawData();

    static int GetActiveCount();

private:
    static constexpr int MAX_PARTICLES = 2000;
    inline static std::vector<Particle> particles;
    inline static std::vector<ParticleDrawData> draw_data;
    inline static int active_count = 0;

    static int FindFreeParticle();
};

