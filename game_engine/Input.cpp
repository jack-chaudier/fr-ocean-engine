#include "Input.hpp"
#include <algorithm>
#include <iostream>


const std::unordered_map<std::string, SDL_Scancode> __keycode_to_scancode = {
    // Directional (arrow) Keys
    {"up", SDL_SCANCODE_UP},
    {"down", SDL_SCANCODE_DOWN},
    {"right", SDL_SCANCODE_RIGHT},
    {"left", SDL_SCANCODE_LEFT},

    // Misc Keys
    {"escape", SDL_SCANCODE_ESCAPE},

    // Modifier Keys
    {"lshift", SDL_SCANCODE_LSHIFT},
    {"rshift", SDL_SCANCODE_RSHIFT},
    {"lctrl", SDL_SCANCODE_LCTRL},
    {"rctrl", SDL_SCANCODE_RCTRL},
    {"lalt", SDL_SCANCODE_LALT},
    {"ralt", SDL_SCANCODE_RALT},

    // Editing Keys
    {"tab", SDL_SCANCODE_TAB},
    {"return", SDL_SCANCODE_RETURN},
    {"enter", SDL_SCANCODE_RETURN},
    {"backspace", SDL_SCANCODE_BACKSPACE},
    {"delete", SDL_SCANCODE_DELETE},
    {"insert", SDL_SCANCODE_INSERT},

    // Character Keys
    {"space", SDL_SCANCODE_SPACE},
    {"a", SDL_SCANCODE_A},
    {"b", SDL_SCANCODE_B},
    {"c", SDL_SCANCODE_C},
    {"d", SDL_SCANCODE_D},
    {"e", SDL_SCANCODE_E},
    {"f", SDL_SCANCODE_F},
    {"g", SDL_SCANCODE_G},
    {"h", SDL_SCANCODE_H},
    {"i", SDL_SCANCODE_I},
    {"j", SDL_SCANCODE_J},
    {"k", SDL_SCANCODE_K},
    {"l", SDL_SCANCODE_L},
    {"m", SDL_SCANCODE_M},
    {"n", SDL_SCANCODE_N},
    {"o", SDL_SCANCODE_O},
    {"p", SDL_SCANCODE_P},
    {"q", SDL_SCANCODE_Q},
    {"r", SDL_SCANCODE_R},
    {"s", SDL_SCANCODE_S},
    {"t", SDL_SCANCODE_T},
    {"u", SDL_SCANCODE_U},
    {"v", SDL_SCANCODE_V},
    {"w", SDL_SCANCODE_W},
    {"x", SDL_SCANCODE_X},
    {"y", SDL_SCANCODE_Y},
    {"z", SDL_SCANCODE_Z},
    {"0", SDL_SCANCODE_0},
    {"1", SDL_SCANCODE_1},
    {"2", SDL_SCANCODE_2},
    {"3", SDL_SCANCODE_3},
    {"4", SDL_SCANCODE_4},
    {"5", SDL_SCANCODE_5},
    {"6", SDL_SCANCODE_6},
    {"7", SDL_SCANCODE_7},
    {"8", SDL_SCANCODE_8},
    {"9", SDL_SCANCODE_9},
    {"/", SDL_SCANCODE_SLASH},
    {";", SDL_SCANCODE_SEMICOLON},
    {"=", SDL_SCANCODE_EQUALS},
    {"-", SDL_SCANCODE_MINUS},
    {".", SDL_SCANCODE_PERIOD},
    {",", SDL_SCANCODE_COMMA},
    {"[", SDL_SCANCODE_LEFTBRACKET},
    {"]", SDL_SCANCODE_RIGHTBRACKET},
    {"\\", SDL_SCANCODE_BACKSLASH},
    {"'", SDL_SCANCODE_APOSTROPHE}
};

SDL_Scancode Input::StringToScancode(const std::string& key) {
    auto it = __keycode_to_scancode.find(key);
    if (it != __keycode_to_scancode.end()) {
        return it->second;
    }
    return SDL_SCANCODE_UNKNOWN;
}

void Input::Init() {
    keyStates.clear();
    justBecameDown.clear();
    justBecameUp.clear();

    for (int code = SDL_SCANCODE_UNKNOWN; code < SDL_NUM_SCANCODES; ++code) {
        keyStates[static_cast<SDL_Scancode>(code)] = INPUT_STATE_UP;
    }

    mouseButtonStates[1] = INPUT_STATE_UP;
    mouseButtonStates[2] = INPUT_STATE_UP;
    mouseButtonStates[3] = INPUT_STATE_UP;

    mouse_scroll_this_frame = 0.0f;
    mouse_position = {0.0f, 0.0f};
}

void Input::ProcessEvent(const SDL_Event& event) {
    switch (event.type) {
        case SDL_KEYDOWN: {
            keyStates[event.key.keysym.scancode] = INPUT_STATE_JUST_BECAME_DOWN;
            justBecameDown.push_back(event.key.keysym.scancode);
            break;
        }
        
        case SDL_KEYUP: {
            keyStates[event.key.keysym.scancode] = INPUT_STATE_JUST_BECAME_UP;
            justBecameUp.push_back(event.key.keysym.scancode);
            break;
        }
            
        case SDL_MOUSEMOTION: {
            mouse_position.x = static_cast<float>(event.motion.x);
            mouse_position.y = static_cast<float>(event.motion.y);
            break;
        }

        case SDL_MOUSEBUTTONDOWN: {
            int button = event.button.button;
            mouseButtonStates[button] = INPUT_STATE_JUST_BECAME_DOWN;
            mouseButtonsJustDown.push_back(button);
            break;
        }
        case SDL_MOUSEBUTTONUP: {
            int button = event.button.button;
            mouseButtonStates[button] = INPUT_STATE_JUST_BECAME_UP;
            mouseButtonsJustUp.push_back(button);
            break;
        }
        case SDL_MOUSEWHEEL: {
            mouse_scroll_this_frame = static_cast<float>(event.wheel.preciseY);
            break;
        }

    default:
        break;
    }
}


void Input::LateUpdate() {
    for (auto code : justBecameDown) {
        keyStates[code] = INPUT_STATE_DOWN;
    }
    justBecameDown.clear();

    for (auto b : mouseButtonsJustDown) {
        mouseButtonStates[b] = INPUT_STATE_DOWN;
    }
    mouseButtonsJustDown.clear();

    for (auto b : mouseButtonsJustUp) {
        mouseButtonStates[b] = INPUT_STATE_UP;
    }
    mouseButtonsJustUp.clear();

    mouse_scroll_this_frame = 0.0f;
}


bool Input::GetKey(SDL_Scancode code) {
    auto it = keyStates.find(code);
    if (it == keyStates.end()) return false;
    return (it->second == INPUT_STATE_DOWN || it->second == INPUT_STATE_JUST_BECAME_DOWN);
}

bool Input::GetKeyDown(SDL_Scancode code) {
    auto it = keyStates.find(code);
    if (it == keyStates.end()) return false;
    return (it->second == INPUT_STATE_JUST_BECAME_DOWN);
}

bool Input::GetKeyUp(SDL_Scancode code) {
    auto it = keyStates.find(code);
    if (it == keyStates.end()) return false;
    return (it->second == INPUT_STATE_JUST_BECAME_UP);
}

bool Input::GetKey(const std::string& key) {
    SDL_Scancode scancode = StringToScancode(key);
    return GetKey(scancode);
}

bool Input::GetKeyDown(const std::string& key) {
    SDL_Scancode scancode = StringToScancode(key);
    return GetKeyDown(scancode);
}

bool Input::GetKeyUp(const std::string& key) {
    SDL_Scancode scancode = StringToScancode(key);
    return GetKeyUp(scancode);
}

glm::vec2 Input::GetMousePosition() {
    return mouse_position;
}

bool Input::GetMouseButton(int button) {
    auto it = mouseButtonStates.find(button);
    if (it == mouseButtonStates.end()) return false;
    return (it->second == INPUT_STATE_DOWN || it->second == INPUT_STATE_JUST_BECAME_DOWN);
}

bool Input::GetMouseButtonDown(int button) {
    auto it = mouseButtonStates.find(button);
    if (it == mouseButtonStates.end()) return false;
    return (it->second == INPUT_STATE_JUST_BECAME_DOWN);
}

bool Input::GetMouseButtonUp(int button) {
    auto it = mouseButtonStates.find(button);
    if (it == mouseButtonStates.end()) return false;
    return (it->second == INPUT_STATE_JUST_BECAME_UP);
}

float Input::GetMouseScrollDelta() {
    return mouse_scroll_this_frame;
}

void Input::HideCursor() {
    SDL_ShowCursor(SDL_DISABLE);
}

void Input::ShowCursor()
{
    SDL_ShowCursor(SDL_ENABLE);
}

void Input::BeginFrame() {
    for (auto code : justBecameDown) {
        keyStates[code] = INPUT_STATE_DOWN;
    }
    justBecameDown.clear();

    for (auto code : justBecameUp) {
        keyStates[code] = INPUT_STATE_UP;
    }
    justBecameUp.clear();

    for (auto b : mouseButtonsJustDown) {
        mouseButtonStates[b] = INPUT_STATE_DOWN;
    }
    mouseButtonsJustDown.clear();

    for (auto b : mouseButtonsJustUp) {
        mouseButtonStates[b] = INPUT_STATE_UP;
    }
    mouseButtonsJustUp.clear();

    mouse_scroll_this_frame = 0.0f;
}
