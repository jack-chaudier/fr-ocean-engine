//
//  Actor.hpp
//  FR-Ocean Engine
//
//  Game object class that holds dynamic Lua-based components and provides
//  component management API (add, remove, query).
//
//  Created by Jack Gaffney on 2/1/25.
//

#ifndef ACTOR_HPP
#define ACTOR_HPP

#include <string>
#include <unordered_map>
#include <cstdint>
#include <algorithm>
#include <filesystem>
#include <vector>
#include <memory>
#include <set>
#include "lua/lua.hpp"
#include "LuaBridge/LuaBridge.h"
#include "ComponentDB.hpp"

class ComponentDB;

/**
 * @class Actor
 * @brief Game object that holds dynamic Lua components and manages their lifecycle.
 *
 * The Actor class represents a game entity in the scene. Actors are containers for
 * components (written in Lua) that define behavior, rendering, physics, and game logic.
 *
 * Architecture:
 * - Component-based design: Actors have no inherent behavior, only components
 * - Lua scripting: All components are Lua tables with lifecycle methods
 * - Dynamic composition: Components can be added/removed at runtime
 * - Unique identification: Each actor has a globally unique 64-bit ID
 *
 * Component Lifecycle:
 * - OnStart(): Called once on the first frame after component creation
 * - OnUpdate(): Called every frame during the update phase
 * - OnLateUpdate(): Called every frame after physics simulation
 * - OnDestroy(): Called when the actor or component is destroyed
 *
 * Scene Persistence:
 * - By default, actors are destroyed when changing scenes
 * - Setting dont_destroy = true preserves the actor across scene transitions
 *
 * @note Actors are managed by SceneDB and should be created via SceneDB::InstantiateActor()
 */
class Actor {
public:
    /// Actor name (not necessarily unique, used for searching via SceneDB::FindActor)
    std::string name;

    /// Globally unique actor ID assigned by SceneDB
    uint64_t id;

    /// Flag marking actor for destruction (deferred until end of frame)
    bool destroyed = false;

    /// If true, actor persists across scene transitions (see SceneDB::DontDestroy)
    bool dont_destroy = false;

    /**
     * @brief Gets the actor's name.
     *
     * @return Actor name as string
     */
    std::string GetName() {return name;}

    /**
     * @brief Gets the actor's unique ID.
     *
     * @return 64-bit unique identifier
     */
    uint64_t GetID() {return id;}

    /// Map of component key -> Lua component reference (LuaBridge wrapper)
    std::unordered_map<std::string, std::shared_ptr<luabridge::LuaRef>> components;

    /// Deferred removal list (components marked for removal, processed at end of frame)
    std::vector<std::string> components_to_remove;

    /// Set of all component keys for fast lookup
    std::set<std::string> component_keys;

    /**
     * @brief Gets a component by its unique key.
     *
     * @param key Component key (unique identifier for this component instance)
     * @return LuaRef to the component, or nil if not found
     *
     * @note Component keys are generated as "typename_N" where N is a counter
     */
    luabridge::LuaRef GetComponentByKey(const std::string & key);

    /**
     * @brief Gets the first component of a given type.
     *
     * @param type Component type name (e.g., "PlayerController", "Rigidbody")
     * @return LuaRef to the component, or nil if no component of that type exists
     *
     * @note If multiple components of the same type exist, this returns the first one
     */
    luabridge::LuaRef GetComponent(const std::string & type);

    /**
     * @brief Gets all components of a given type.
     *
     * @param type Component type name
     * @return LuaRef to a Lua table containing all matching components
     *
     * @note The returned table is indexed from 1 (Lua convention)
     */
    luabridge::LuaRef GetComponents(const std::string & type);

    /**
     * @brief Adds a new component to this actor at runtime.
     *
     * @param type Component type name (must correspond to a file in resources/component_types/)
     * @return LuaRef to the newly created component
     *
     * @note The component's OnStart() will be called on the next frame
     * @throws std::runtime_error if component type doesn't exist
     */
    luabridge::LuaRef AddComponent(const std::string & type);

    /**
     * @brief Marks a component for removal (deferred deletion).
     *
     * @param to_remove LuaRef to the component to remove
     *
     * @note Removal is deferred until the end of the frame to avoid invalidating
     *       component iteration. OnDestroy() is called before removal.
     */
    void RemoveComponent(const luabridge::LuaRef & to_remove);

    /**
     * @brief Injects a component reference (used by SceneDB during actor instantiation).
     *
     * @param comp_ref Shared pointer to the Lua component reference
     *
     * @note This is an internal method used during scene loading and actor template instantiation
     */
    void InjectReference(std::shared_ptr<luabridge::LuaRef> comp_ref);
};

#endif // ACTOR_HPP
