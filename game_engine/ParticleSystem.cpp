//
//  ParticleSystem.cpp
//  FR-Ocean Engine
//
//  Particle emitter system implementation with object pooling.
//

#include "ParticleSystem.hpp"
#include "Logger.hpp"
#include <cmath>
#include <cstdlib>

#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif

static float RandomRange(float min_val, float max_val) {
    float t = static_cast<float>(rand()) / static_cast<float>(RAND_MAX);
    return min_val + t * (max_val - min_val);
}

void ParticleSystem::Init() {
    particles.clear();
    particles.resize(MAX_PARTICLES);
    for (auto& p : particles) {
        p.active = false;
    }
    draw_data.clear();
    draw_data.reserve(MAX_PARTICLES);
    active_count = 0;
}

void ParticleSystem::Clear() {
    for (auto& p : particles) {
        p.active = false;
    }
    draw_data.clear();
    active_count = 0;
}

void ParticleSystem::Update(float dt) {
    active_count = 0;
    draw_data.clear();

    for (auto& p : particles) {
        if (!p.active) {
            continue;
        }

        p.lifetime -= dt;
        if (p.lifetime <= 0.0f) {
            p.active = false;
            continue;
        }

        // Apply gravity as downward acceleration
        p.vy += p.gravity * dt;

        // Move by velocity
        p.x += p.vx * dt;
        p.y += p.vy * dt;

        // Interpolate based on life progress (0 = just spawned, 1 = about to die)
        float life_ratio = 1.0f - (p.lifetime / p.max_lifetime);

        // Interpolate size
        p.size = p.start_size + (p.end_size - p.start_size) * life_ratio;

        // Interpolate color
        glm::vec4 color = p.start_color + (p.end_color - p.start_color) * life_ratio;

        ParticleDrawData dd;
        dd.image_name = p.image_name;
        dd.x = p.x;
        dd.y = p.y;
        dd.size = p.size;
        dd.r = static_cast<int>(color.x);
        dd.g = static_cast<int>(color.y);
        dd.b = static_cast<int>(color.z);
        dd.a = static_cast<int>(color.w);
        dd.sorting_order = p.sorting_order;
        draw_data.push_back(dd);

        active_count++;
    }
}

void ParticleSystem::Emit(float x, float y, int count, const ParticleConfig& config) {
    float dir_rad = static_cast<float>(config.direction * M_PI / 180.0);
    float spread_rad = static_cast<float>(config.spread_angle * M_PI / 180.0);
    float half_spread = spread_rad * 0.5f;

    for (int i = 0; i < count; i++) {
        int idx = FindFreeParticle();
        if (idx < 0) {
            LOG_WARNING("ParticleSystem: pool exhausted, cannot emit more particles");
            break;
        }

        Particle& p = particles[idx];
        p.active = true;
        p.x = x;
        p.y = y;

        // Randomize direction within spread cone
        float angle = dir_rad + RandomRange(-half_spread, half_spread);
        float speed = RandomRange(config.speed_min, config.speed_max);
        p.vx = std::cos(angle) * speed;
        p.vy = std::sin(angle) * speed;

        p.gravity = config.gravity;
        p.lifetime = RandomRange(config.lifetime_min, config.lifetime_max);
        p.max_lifetime = p.lifetime;
        p.start_size = config.start_size;
        p.end_size = config.end_size;
        p.size = config.start_size;
        p.start_color = config.start_color;
        p.end_color = config.end_color;
        p.image_name = config.image_name;
        p.sorting_order = config.sorting_order;
    }
}

const std::vector<ParticleDrawData>& ParticleSystem::GetDrawData() {
    return draw_data;
}

int ParticleSystem::GetActiveCount() {
    return active_count;
}

int ParticleSystem::FindFreeParticle() {
    for (int i = 0; i < MAX_PARTICLES; i++) {
        if (!particles[i].active) {
            return i;
        }
    }
    return -1;
}
