//
//  AnimationDB.hpp
//  FR-Ocean Engine
//
//  Sprite sheet animation system with named animation definitions
//  and per-actor playback state tracking.
//

#pragma once

#include <string>
#include <unordered_map>
#include "SDL2/SDL.h"

struct AnimationDef {
    std::string spritesheet;
    int frame_width = 0;
    int frame_height = 0;
    int frame_count = 1;
    float fps = 12.0f;
};

struct AnimationState {
    std::string current_anim;
    float elapsed = 0.0f;
    int current_frame = 0;
    bool playing = false;
    bool loop = true;
};

class AnimationDB {
public:
    static void Init();
    static void Clear();
    static void Update(float dt);

    // Define a named animation from a spritesheet
    static void DefineAnimation(const std::string& name, const std::string& spritesheet,
                                int frame_width, int frame_height, int frame_count, float fps);

    // Control playback for a given key (typically actor component key)
    static void Play(const std::string& key, const std::string& anim_name, bool loop = true);
    static void Stop(const std::string& key);
    static void SetFrame(const std::string& key, int frame);

    // Query state
    static bool IsPlaying(const std::string& key);
    static int GetCurrentFrame(const std::string& key);
    static std::string GetCurrentAnimation(const std::string& key);

    // Get source rect for current animation frame (for rendering)
    static SDL_Rect GetSourceRect(const std::string& key);

    // Get the spritesheet name for a playing animation
    static std::string GetSpritesheet(const std::string& key);

    static bool HasAnimation(const std::string& name);

private:
    inline static std::unordered_map<std::string, AnimationDef> animations;
    inline static std::unordered_map<std::string, AnimationState> active_animations;
};

