//
//  Input.hpp
//  FR-Ocean Engine
//
//  Cross-platform input system handling keyboard, mouse, and scroll events with
//  frame-accurate state tracking (pressed, held, released).
//
//  Created by Jack Gaffney
//

#ifndef INPUT_H
#define INPUT_H

#include <unordered_map>
#include <vector>
#include <string>
#include "SDL.h"
#include <glm/vec2.hpp>

/**
 * @enum INPUT_STATE
 * @brief Tracks the state of input keys/buttons across frames.
 *
 * State transitions:
 * - UP -> JUST_BECAME_DOWN (first frame pressed)
 * - JUST_BECAME_DOWN -> DOWN (held after first frame)
 * - DOWN -> JUST_BECAME_UP (first frame released)
 * - JUST_BECAME_UP -> UP (released after first frame)
 */
enum INPUT_STATE {
    INPUT_STATE_UP,                 ///< Not pressed
    INPUT_STATE_JUST_BECAME_DOWN,   ///< Pressed this frame
    INPUT_STATE_DOWN,               ///< Held down (pressed for multiple frames)
    INPUT_STATE_JUST_BECAME_UP      ///< Released this frame
};

/**
 * @class Input
 * @brief Cross-platform input system for keyboard, mouse, and scroll events.
 *
 * The Input class provides a clean API for querying input state with frame-accurate
 * tracking of pressed, held, and released events. It integrates with SDL2 for
 * cross-platform input handling.
 *
 * Features:
 * - Keyboard input with SDL scancodes or string keys (e.g., "space", "a")
 * - Mouse button tracking (left, right, middle)
 * - Mouse position in screen space
 * - Mouse scroll delta (vertical scroll wheel)
 * - Cursor visibility control
 * - Frame-accurate state transitions (GetKeyDown/GetKeyUp for single-frame events)
 *
 * Usage Pattern:
 * ```cpp
 * // In game loop:
 * Input::BeginFrame();              // Clear previous frame's transient state
 * // ... SDL_PollEvent loop ...
 * Input::ProcessEvent(event);       // Update state from SDL events
 * // ... game update ...
 * Input::LateUpdate();              // Transition states for next frame
 * ```
 *
 * @note Input uses static members for global access from Lua scripts and components
 */
class Input {
public:
    /**
     * @brief Initializes the input system.
     *
     * @note Currently a no-op; reserved for future initialization logic
     */
    static void Init();

    /**
     * @brief Resets transient input state at the beginning of each frame.
     *
     * Clears:
     * - justBecameDown and justBecameUp keyboard lists
     * - mouseButtonsJustDown and mouseButtonsJustUp lists
     * - mouse_scroll_this_frame delta
     *
     * @note Call this once per frame before processing SDL events
     */
    static void BeginFrame();

    /**
     * @brief Processes an SDL event and updates input state.
     *
     * Handles:
     * - SDL_KEYDOWN / SDL_KEYUP: Keyboard events
     * - SDL_MOUSEBUTTONDOWN / SDL_MOUSEBUTTONUP: Mouse button events
     * - SDL_MOUSEMOTION: Mouse cursor position
     * - SDL_MOUSEWHEEL: Mouse scroll wheel
     *
     * @param event SDL event to process
     *
     * @note Call this for each event in your SDL_PollEvent loop
     */
    static void ProcessEvent(const SDL_Event& event);

    /**
     * @brief Transitions input states for the next frame.
     *
     * State transitions:
     * - JUST_BECAME_DOWN -> DOWN
     * - JUST_BECAME_UP -> UP
     *
     * @note Call this once per frame after all game logic but before next BeginFrame()
     */
    static void LateUpdate();

    /**
     * @brief Checks if a key is currently held down.
     *
     * @param code SDL scancode for the key
     * @return true if key is DOWN or JUST_BECAME_DOWN, false otherwise
     */
    static bool GetKey(SDL_Scancode code);

    /**
     * @brief Checks if a key was pressed this frame (single-frame event).
     *
     * @param code SDL scancode for the key
     * @return true only on the first frame the key was pressed
     */
    static bool GetKeyDown(SDL_Scancode code);

    /**
     * @brief Checks if a key was released this frame (single-frame event).
     *
     * @param code SDL scancode for the key
     * @return true only on the first frame the key was released
     */
    static bool GetKeyUp(SDL_Scancode code);

    /**
     * @brief Checks if a key is currently held down (string version).
     *
     * @param code Key name as string (e.g., "space", "a", "return", "left")
     * @return true if key is DOWN or JUST_BECAME_DOWN, false otherwise
     *
     * @note Uses StringToScancode() internally to convert string to SDL_Scancode
     */
    static bool GetKey(const std::string & code);

    /**
     * @brief Checks if a key was pressed this frame (string version).
     *
     * @param code Key name as string
     * @return true only on the first frame the key was pressed
     */
    static bool GetKeyDown(const std::string & code);

    /**
     * @brief Checks if a key was released this frame (string version).
     *
     * @param code Key name as string
     * @return true only on the first frame the key was released
     */
    static bool GetKeyUp(const std::string & code);

    /**
     * @brief Gets the current mouse cursor position in screen space.
     *
     * @return 2D vector with mouse position (pixels from top-left corner)
     */
    static glm::vec2 GetMousePosition();

    /**
     * @brief Checks if a mouse button is currently held down.
     *
     * @param button SDL mouse button (1 = left, 2 = middle, 3 = right)
     * @return true if button is DOWN or JUST_BECAME_DOWN, false otherwise
     */
    static bool GetMouseButton(int button);

    /**
     * @brief Checks if a mouse button was pressed this frame.
     *
     * @param button SDL mouse button (1 = left, 2 = middle, 3 = right)
     * @return true only on the first frame the button was pressed
     */
    static bool GetMouseButtonDown(int button);

    /**
     * @brief Checks if a mouse button was released this frame.
     *
     * @param button SDL mouse button (1 = left, 2 = middle, 3 = right)
     * @return true only on the first frame the button was released
     */
    static bool GetMouseButtonUp(int button);

    /**
     * @brief Gets the mouse scroll delta for this frame.
     *
     * @return Vertical scroll delta (positive = up, negative = down, 0 = no scroll)
     *
     * @note This value is reset to 0 every frame in BeginFrame()
     */
    static float GetMouseScrollDelta();

    /**
     * @brief Hides the mouse cursor.
     *
     * @note Uses SDL_ShowCursor(SDL_DISABLE) internally
     */
    static void HideCursor();

    /**
     * @brief Shows the mouse cursor.
     *
     * @note Uses SDL_ShowCursor(SDL_ENABLE) internally
     */
    static void ShowCursor();

private:
    /// Map of SDL scancodes to their current input state
    static inline std::unordered_map<SDL_Scancode, INPUT_STATE> keyStates;

    /// List of keys that transitioned to JUST_BECAME_DOWN this frame
    static inline std::vector<SDL_Scancode> justBecameDown;

    /// List of keys that transitioned to JUST_BECAME_UP this frame
    static inline std::vector<SDL_Scancode> justBecameUp;

    /// Current mouse cursor position in screen space (pixels)
    static inline glm::vec2 mouse_position;

    /// Map of mouse buttons to their current input state
    static inline std::unordered_map<int, INPUT_STATE> mouseButtonStates;

    /// List of mouse buttons that transitioned to JUST_BECAME_DOWN this frame
    static inline std::vector<int> mouseButtonsJustDown;

    /// List of mouse buttons that transitioned to JUST_BECAME_UP this frame
    static inline std::vector<int> mouseButtonsJustUp;

    /// Mouse scroll delta for the current frame (reset each frame)
    static inline float mouse_scroll_this_frame = 0;

    /**
     * @brief Converts a string key name to SDL scancode.
     *
     * @param key Key name (e.g., "space", "return", "a", "escape")
     * @return Corresponding SDL_Scancode
     *
     * @note Uses SDL_GetScancodeFromName() internally
     */
    static SDL_Scancode StringToScancode(const std::string& key);
};

#endif // INPUT_H
