#ifndef INPUT_H
#define INPUT_H

#include <unordered_map>
#include <vector>
#include <string>
#include "SDL.h"
#include <glm/vec2.hpp>

enum INPUT_STATE {
    INPUT_STATE_UP,
    INPUT_STATE_JUST_BECAME_DOWN,
    INPUT_STATE_DOWN,
    INPUT_STATE_JUST_BECAME_UP
};

class Input {
public:
    static void Init();
    static void BeginFrame();
    static void ProcessEvent(const SDL_Event& event);
    static void LateUpdate();

    static bool GetKey(SDL_Scancode code);
    static bool GetKeyDown(SDL_Scancode code);
    static bool GetKeyUp(SDL_Scancode code);
    
    static bool GetKey(const std::string & code);
    static bool GetKeyDown(const std::string & code);
    static bool GetKeyUp(const std::string & code);

    static glm::vec2 GetMousePosition();

    static bool GetMouseButton(int button);
    static bool GetMouseButtonDown(int button);
    static bool GetMouseButtonUp(int button);
    static float GetMouseScrollDelta();

    static void HideCursor();
    static void ShowCursor();

private:
    static inline std::unordered_map<SDL_Scancode, INPUT_STATE> keyStates;
    static inline std::vector<SDL_Scancode> justBecameDown;
    static inline std::vector<SDL_Scancode> justBecameUp;

    static inline glm::vec2 mouse_position;
    static inline std::unordered_map<int, INPUT_STATE> mouseButtonStates;
    static inline std::vector<int> mouseButtonsJustDown;
    static inline std::vector<int> mouseButtonsJustUp;
    
    static inline float mouse_scroll_this_frame = 0;

    static SDL_Scancode StringToScancode(const std::string& key);
};

#endif // INPUT_H
