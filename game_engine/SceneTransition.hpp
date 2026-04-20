//
//  SceneTransition.hpp
//  game_engine
//
//  Scene transition effects (fade to black, instant cut).
//

#pragma once

#include <string>

class SceneTransition {
public:
    enum class State {
        NONE,
        FADE_OUT,
        FADE_IN
    };

    static void Init();
    static void StartTransition(const std::string& scene_name, const std::string& type, float duration);
    static void Update(float dt);
    static void Render();
    static bool IsTransitioning();
    static bool ShouldLoadScene();
    static std::string GetTargetScene();
    static void ClearTargetScene();

private:
    inline static State state = State::NONE;
    inline static std::string target_scene;
    inline static std::string transition_type = "none";
    inline static float duration = 0.0f;
    inline static float elapsed = 0.0f;
    inline static bool scene_load_pending = false;
};

