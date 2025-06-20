//
//  Actor.hpp
//  game_engine
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

class Actor {
public:
    std::string name;
    
    uint64_t id;
    bool destroyed = false;
    bool dont_destroy = false;
    
    std::string GetName() {return name;}
    uint64_t GetID() {return id;}
    
    std::unordered_map<std::string, std::shared_ptr<luabridge::LuaRef>> components;
    std::vector<std::string> components_to_remove;
    std::set<std::string> component_keys;
    
    luabridge::LuaRef GetComponentByKey(const std::string & key);
    luabridge::LuaRef GetComponent(const std::string & type);
    luabridge::LuaRef GetComponents(const std::string & type);
    
    luabridge::LuaRef AddComponent(const std::string & type);
    void RemoveComponent(const luabridge::LuaRef & to_remove);
    void InjectReference(std::shared_ptr<luabridge::LuaRef> comp_ref);
};

#endif // ACTOR_HPP
