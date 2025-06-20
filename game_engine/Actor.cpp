//
//  Actor.cpp
//  game_engine
//
//  Created by Jack Gaffney on 2/1/25.
//

#include "Actor.hpp"
#include "Helper.h"
#include "SceneDB.hpp"


luabridge::LuaRef Actor::GetComponentByKey(const std::string & key) {
    auto it = components.find(key);
    
    const auto& compRef = *(it->second);
    
    if (compRef.isUserdata()) return compRef;
    
    if (it != components.end() && (*it->second)["enabled"])
        return *(components[key]);
    else
        return luabridge::LuaRef(ComponentDB::GetLuaState());
}

luabridge::LuaRef Actor::GetComponent(const std::string & type) {
    for (const auto & key : component_keys) {
        auto it = components.find(key);
        if (it == components.end()) continue;
        
        const auto& compRef = *(it->second);
        if (type == "Rigidbody" && compRef.isUserdata()) return compRef;
        
        if (compRef["type"] == type && compRef["enabled"])
            return compRef;
    }
    
    return luabridge::LuaRef(ComponentDB::GetLuaState());
}

luabridge::LuaRef Actor::GetComponents(const std::string & type) {
    int idx = 1;
    luabridge::LuaRef ret = luabridge::newTable(ComponentDB::GetLuaState());
    
    for (const auto & key : component_keys) {
        auto it = components.find(key);
        if (it == components.end()) continue;
        
        const auto& compRef = *(it->second);
        if (type == "Rigidbody" && compRef.isUserdata())
            ret[idx++] = compRef;
        if (compRef["type"] == type && compRef["enabled"])
            ret[idx++] = compRef;
    }
    
    return ret;
}

luabridge::LuaRef Actor::AddComponent(const std::string& type) {
    std::string comp_key = "r" + std::to_string(ComponentDB::runtime_comp_add++);
    std::shared_ptr<luabridge::LuaRef> component_ref;
    component_ref = ComponentDB::CreateComponent(type, comp_key);
    
    components[comp_key] = component_ref;
    component_keys.insert(comp_key);
    InjectReference(component_ref);
    ComponentDB::CDB[comp_key] = component_ref;

    // Special handling for different component types
    if (type == "Rigidbody") {
        // Add to rigidbodies_to_init with current frame information
        SceneDB::rigidbodies_to_init.push_back({
            this->id,
            comp_key,
            true,  // isNew
            Helper::GetFrameNumber()
        });
    } else if ((*component_ref).isTable()) {
        // For Lua table components, add frame_added property
        (*component_ref)["frame_added"] = Helper::GetFrameNumber();
        (*component_ref)["new_addition"] = true;
        
        if ((*component_ref)["OnStart"].isFunction()) {
            SceneDB::onstart_new = true;
        }
    }

    SceneDB::addComponentToCaches(this->GetID(), comp_key, component_ref);
    return *component_ref;
}

void Actor::RemoveComponent(const luabridge::LuaRef& to_remove) {
    if (!to_remove.isTable()) return;
    std::string key = to_remove["key"];

    auto it = components.find(key);
    if (it != components.end()) {
        (*it->second)["enabled"] = false;
        components_to_remove.push_back(key);

        SceneDB::removeComponentFromCaches(this->GetID(), key);
    }
}

void Actor::InjectReference(std::shared_ptr<luabridge::LuaRef> comp_ref) {
    luabridge::LuaRef comp = *comp_ref;
    if (comp.isTable())
        comp["actor"] = this;
}
