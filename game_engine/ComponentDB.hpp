//
//  ComponentDB.hpp
//  FR-Ocean Engine
//
//  Lua component system managing component instantiation, inheritance, and
//  C++-to-Lua API bindings via LuaBridge.
//
//  Created by Jack Gaffney
//

#ifndef COMPONENTDB_HPP
#define COMPONENTDB_HPP

#include <string>
#include <unordered_map>
#include <memory>
#include <set>
#include "lua.hpp"
#include "LuaBridge.h"
#include "rapidjson/document.h"
#include "Actor.hpp"

class Actor;

/**
 * @class ComponentDB
 * @brief Lua component system managing component types, instantiation, and C++ API bindings.
 *
 * ComponentDB is the bridge between C++ engine systems and Lua game logic. It manages:
 * - Lua state initialization and cleanup
 * - Component type loading from Lua files (resources/component_types/)
 * - Component instantiation with prototype-based inheritance
 * - C++ API bindings exposed to Lua (Input, Image, Text, Audio, Rigidbody, etc.)
 * - Property overrides from JSON scene files
 *
 * Component Lifecycle:
 * 1. Init(): Create Lua state, bind C++ APIs, load all component type files
 * 2. CreateComponent(): Instantiate a component from a type template
 * 3. EstablishInheritance(): Set up Lua metatable inheritance (prototype chain)
 * 4. Lua OnStart/OnUpdate/OnLateUpdate/OnDestroy called by SceneDB
 *
 * C++ APIs Exposed to Lua:
 * - Actor: GetName(), GetID(), GetComponent(), AddComponent(), RemoveComponent()
 * - Input: GetKey(), GetMousePosition(), etc.
 * - Image: Draw(), DrawUI(), DrawPixel()
 * - Text: Draw()
 * - Audio: PlayChannel(), HaltChannel(), SetVolume()
 * - Rigidbody: GetPosition(), SetVelocity(), AddForce(), etc.
 * - Application: Quit(), Sleep(), GetFrame(), OpenURL()
 * - Debug: Log(), LogError()
 * - Scene: Load(), GetCurrent(), DontDestroy(), Find(), FindAll(), Instantiate(), Destroy()
 * - Camera: SetPosition(), SetZoom(), GetPosition()
 * - Physics: Raycast(), RaycastAll()
 *
 * @note ComponentDB uses a single global Lua state for all components
 */
class ComponentDB {
public:
    /**
     * @brief Destructor that closes the Lua state.
     */
    ~ComponentDB() {
        if (L) {
            lua_close(L);
            L = nullptr;
        }
    }

    /**
     * @brief Initializes Lua state and binds all C++ APIs to Lua.
     *
     * Initialization steps:
     * 1. Create new Lua state
     * 2. Open standard Lua libraries (math, string, table, etc.)
     * 3. Bind all C++ classes and functions via LuaBridge
     * 4. Load all component type files from resources/component_types/
     * 5. Cache component types for fast instantiation
     *
     * @throws std::runtime_error if Lua state creation fails
     */
    static void Init();

    /**
     * @brief Loads components from a JSON definition and attaches them to an actor.
     *
     * @param a Pointer to the Actor to attach components to
     * @param doc RapidJSON document containing component definitions
     *
     * @note Called during scene loading and actor template instantiation
     */
    static void loadComponents(Actor * a, const rapidjson::Value &doc);

    /**
     * @brief Creates a new component instance from a type template.
     *
     * @param type Component type name (e.g., "PlayerController", must exist in component_types/)
     * @param comp_key Unique key for this component instance (e.g., "PlayerController_0")
     * @return Shared pointer to LuaRef wrapping the component table
     *
     * @throws std::runtime_error if component type doesn't exist
     */
    static std::shared_ptr<luabridge::LuaRef> CreateComponent(const std::string& type, const std::string& comp_key);

    /// Global component database: component_key -> LuaRef (currently unused?)
    inline static std::unordered_map<std::string, std::shared_ptr<luabridge::LuaRef>> CDB;

    /// Component type cache: type_name -> LuaRef to prototype table
    inline static std::unordered_map<std::string, std::shared_ptr<luabridge::LuaRef>> componentTypeCache;

    /**
     * @brief Logs a message from C++ to stdout (exposed to Lua as Debug.Log()).
     *
     * @param message Message to log
     */
    static void CPPLog(std::string message);

    /**
     * @brief Gets the global Lua state.
     *
     * @return Pointer to lua_State
     */
    static lua_State * GetLuaState() {return L;};

    /// Counter for runtime component additions (used for unique key generation)
    inline static int runtime_comp_add = 0;

    /**
     * @brief Establishes prototype-based inheritance for a component instance.
     *
     * @param instance_table Lua table for the component instance
     * @param parent_table Lua table for the component type (prototype)
     *
     * @note Sets up metatable __index to implement inheritance
     */
    static void EstablishInheritance(luabridge::LuaRef& instance_table, luabridge::LuaRef& parent_table);

private:
    /// Global Lua state (shared by all components)
    inline static lua_State* L;

    /**
     * @brief Overrides a Lua component property with a JSON value.
     *
     * @param table Lua component table
     * @param name Property name
     * @param prop_value JSON value to assign
     */
    static void overrideLuaRefValue(luabridge::LuaRef& table, const std::string & name, const rapidjson::Value& prop_value);

    /**
     * @brief Overrides a Rigidbody property with a JSON value (special handling).
     *
     * @param table Lua component table
     * @param name Property name
     * @param prop_value JSON value to assign
     */
    static void overrideRigidbodyfValue(luabridge::LuaRef& table, const std::string & name, const rapidjson::Value& prop_value);
};

#endif // COMPONENTDB_HPP
