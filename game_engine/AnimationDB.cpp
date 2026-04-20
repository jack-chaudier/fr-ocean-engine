//
//  AnimationDB.cpp
//  FR-Ocean Engine
//
//  Sprite sheet animation system implementation.
//

#include "AnimationDB.hpp"
#include "Logger.hpp"
#include <cmath>

void AnimationDB::Init() {
    animations.clear();
    active_animations.clear();
}

void AnimationDB::Clear() {
    active_animations.clear();
}

void AnimationDB::Update(float dt) {
    for (auto& [key, state] : active_animations) {
        if (!state.playing) {
            continue;
        }

        auto it = animations.find(state.current_anim);
        if (it == animations.end()) {
            LOG_WARNING("Animation not found for active state: " + state.current_anim);
            state.playing = false;
            continue;
        }

        const AnimationDef& def = it->second;
        state.elapsed += dt;

        int total_frames = def.frame_count;
        float frame_duration = 1.0f / def.fps;
        int frame = static_cast<int>(state.elapsed / frame_duration);

        if (state.loop) {
            state.current_frame = frame % total_frames;
        } else {
            if (frame >= total_frames - 1) {
                state.current_frame = total_frames - 1;
                state.playing = false;
            } else {
                state.current_frame = frame;
            }
        }
    }
}

void AnimationDB::DefineAnimation(const std::string& name, const std::string& spritesheet,
                                  int frame_width, int frame_height, int frame_count, float fps) {
    AnimationDef def;
    def.spritesheet = spritesheet;
    def.frame_width = frame_width;
    def.frame_height = frame_height;
    def.frame_count = frame_count;
    def.fps = fps;
    animations[name] = def;
}

void AnimationDB::Play(const std::string& key, const std::string& anim_name, bool loop) {
    if (animations.find(anim_name) == animations.end()) {
        LOG_WARNING("Cannot play undefined animation: " + anim_name);
        return;
    }

    AnimationState& state = active_animations[key];
    // Reset if switching to a different animation
    if (state.current_anim != anim_name) {
        state.current_anim = anim_name;
        state.elapsed = 0.0f;
        state.current_frame = 0;
    }
    state.playing = true;
    state.loop = loop;
}

void AnimationDB::Stop(const std::string& key) {
    auto it = active_animations.find(key);
    if (it != active_animations.end()) {
        it->second.playing = false;
    }
}

void AnimationDB::SetFrame(const std::string& key, int frame) {
    auto it = active_animations.find(key);
    if (it == active_animations.end()) {
        LOG_WARNING("SetFrame called on unknown animation key: " + key);
        return;
    }

    AnimationState& state = it->second;
    auto anim_it = animations.find(state.current_anim);
    if (anim_it == animations.end()) {
        return;
    }

    const AnimationDef& def = anim_it->second;
    if (frame < 0) {
        frame = 0;
    } else if (frame >= def.frame_count) {
        frame = def.frame_count - 1;
    }

    state.current_frame = frame;
    state.elapsed = static_cast<float>(frame) / def.fps;
}

bool AnimationDB::IsPlaying(const std::string& key) {
    auto it = active_animations.find(key);
    if (it == active_animations.end()) {
        return false;
    }
    return it->second.playing;
}

int AnimationDB::GetCurrentFrame(const std::string& key) {
    auto it = active_animations.find(key);
    if (it == active_animations.end()) {
        return 0;
    }
    return it->second.current_frame;
}

std::string AnimationDB::GetCurrentAnimation(const std::string& key) {
    auto it = active_animations.find(key);
    if (it == active_animations.end()) {
        return "";
    }
    return it->second.current_anim;
}

SDL_Rect AnimationDB::GetSourceRect(const std::string& key) {
    SDL_Rect rect = {0, 0, 0, 0};

    auto state_it = active_animations.find(key);
    if (state_it == active_animations.end()) {
        return rect;
    }

    const AnimationState& state = state_it->second;
    auto anim_it = animations.find(state.current_anim);
    if (anim_it == animations.end()) {
        return rect;
    }

    const AnimationDef& def = anim_it->second;

    // Single-row spritesheet layout: frames are arranged left-to-right
    rect.x = state.current_frame * def.frame_width;
    rect.y = 0;
    rect.w = def.frame_width;
    rect.h = def.frame_height;

    return rect;
}

std::string AnimationDB::GetSpritesheet(const std::string& key) {
    auto state_it = active_animations.find(key);
    if (state_it == active_animations.end()) {
        return "";
    }

    const AnimationState& state = state_it->second;
    auto anim_it = animations.find(state.current_anim);
    if (anim_it == animations.end()) {
        return "";
    }

    return anim_it->second.spritesheet;
}

bool AnimationDB::HasAnimation(const std::string& name) {
    return animations.find(name) != animations.end();
}
