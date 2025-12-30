//
//  SceneDB.cpp
//  game_engine
//
//  Created by Jack Gaffney on 1/29/25.
//

#include "SceneDB.hpp"
#include "EngineUtils.h"
#include "rapidjson/document.h"
#include "Actor.hpp"
#include "RigidbodyWorld.hpp"
#include "Rigidbody.hpp"
#include "Logger.hpp"
#include "EngineException.hpp"
#include <iostream>
#include <filesystem>
#include <algorithm>
#include <unordered_set>

SceneDB::~SceneDB() { }


void SceneDB::ReportError(const std::string& actor_name, const luabridge::LuaException& e) {
    std::string error_message = e.what();
    std::replace(error_message.begin(), error_message.end(), '\\', '/');
    LOG_ERROR(actor_name + " : " + error_message);
}

void SceneDB::Load(const std::string& scene_name) {
    next_scene_to_load = scene_name;
}

std::string SceneDB::GetCurrent() {
    return current_scene_name;
}

void SceneDB::DontDestroy(Actor* actor) {
    if (actor) {
        actor->dont_destroy = true;
    }
}

void SceneDB::loadScene() {
    std::string scene_to_load = ConfigManager::GetInitialScene();
    if (!next_scene_to_load.empty()) {
        scene_to_load = next_scene_to_load;
        next_scene_to_load = "";
    }
    
    current_scene_name = scene_to_load;
    
    std::string full_path = scene_path + scene_to_load + ".scene";

    if (!std::filesystem::exists(full_path)) {
        LOG_FATAL("Scene missing: " + scene_to_load);
        throw ResourceNotFoundException("scene", scene_to_load);
    }
    
    std::vector<std::unique_ptr<Actor>> persistent_actors;
        
    // Process actors - call OnDestroy for non-persistent ones
    for (auto it = actors.begin(); it != actors.end();) {
        if (it->second->dont_destroy && !it->second->destroyed) {
            persistent_actors.push_back(std::move(it->second));
            it = actors.erase(it);
        } else {
            // For non-persistent actors, call OnDestroy on components
            auto& actor = it->second;
            
            // Sort component keys to ensure consistent order
            std::vector<std::string> keys;
            for (const auto& key : actor->component_keys) {
                keys.push_back(key);
            }
            std::sort(keys.begin(), keys.end());
            
            // Call OnDestroy in sorted order
            for (const auto& key : keys) {
                auto comp_it = actor->components.find(key);
                if (comp_it != actor->components.end()) {
                    auto& comp = comp_it->second;
                    if ((*comp)["OnDestroy"].isFunction()) {
                        try {
                            (*comp)["OnDestroy"](*comp);
                        }
                        catch (luabridge::LuaException& e) {
                            ReportError(actor->GetName(), e);
                        }
                    } else if ((*comp).isUserdata() && (*comp).isInstance<Rigidbody>()) {
                        // Special case for Rigidbody
                        Rigidbody* rb = (*comp).cast<Rigidbody*>();
                        rb->OnDestroy();
                    }
                }
            }
            
            ++it; // Move to next actor
        }
    }
    
    actors.clear();
    actor_id_vec.clear();
    
    // Re-add persistent actors
    for (auto& actor : persistent_actors) {
        uint64_t id = actor->id;
        actors[id] = std::move(actor);
        actor_id_vec.push_back(id);
    }
    
    rapidjson::Document scene_doc;
    EngineUtils::ReadJsonFile(full_path, scene_doc);

    const rapidjson::Value &jsonActors = scene_doc["actors"];
    
    uint64_t numActors = jsonActors.Size();
    actors.reserve(numActors + persistent_actors.size());
    actor_id_vec.reserve(numActors + persistent_actors.size());
    
    for (rapidjson::SizeType i = 0; i < jsonActors.Size(); i++) {
        const auto& actor_json = jsonActors[i];
        auto actor = std::make_unique<Actor>();
        if (actor_json.HasMember("name"))
            actor->name = actor_json["name"].GetString();
        
        if (actor_json.HasMember("template")) {
            std::string template_name = actor_json["template"].GetString();
            loadTemplate(template_name, actor.get());
        }
        
        if (actor_json.HasMember("components")) {
            ComponentDB::loadComponents(actor.get(), actor_json["components"]);
        }
        
        actor->id = id_ctr;
        this->actors[id_ctr] = std::move(actor);
        this->actor_id_vec.push_back(id_ctr);
        id_ctr++;
    }

    rebuildComponentCaches();

    onstart_new = true;
}

void SceneDB::loadTemplate(const std::string &template_name, Actor * actor) {
    if (templateCache.find(template_name) == templateCache.end()) {
        std::string template_path = "resources/actor_templates/" + template_name + ".template";
        if (!std::filesystem::exists(template_path)) {
            LOG_FATAL("Actor template missing: " + template_name);
            throw ResourceNotFoundException("actor template", template_name);
        }

        templateCache[template_name] = rapidjson::Document();
        EngineUtils::ReadJsonFile(template_path, templateCache[template_name]);
    }
    
    const auto& template_doc = templateCache[template_name];
    
    if (template_doc.HasMember("name"))
        actor->name = template_doc["name"].GetString();
    
    if (template_doc.HasMember("components"))
        ComponentDB::loadComponents(actor, template_doc["components"]);
}

void SceneDB::UpdateScene() {
    if (onstart_new) {
        onstart_new = false;
        ProcessSceneOnStart();
    }

    // Process Rigidbodies that need initialization
    auto currentFrame = Helper::GetFrameNumber();
    for (auto it = rigidbodies_to_init.begin(); it != rigidbodies_to_init.end();) {
        // Skip initialization if it was added this frame
        if (it->isNew && it->frameAdded == currentFrame) {
            it->isNew = false;  // Mark as not new for next frame
            ++it;
            continue;
        }
        
        auto actorIt = actors.find(it->actorId);
        if (actorIt == actors.end()) {
            it = rigidbodies_to_init.erase(it);
            continue;
        }
        
        auto& actor = actorIt->second;
        auto compIt = actor->components.find(it->componentKey);
        if (compIt == actor->components.end()) {
            it = rigidbodies_to_init.erase(it);
            continue;
        }
        
        auto& compRef = *(compIt->second);
        if (compRef.isInstance<Rigidbody>()) {
            Rigidbody* rb = compRef.cast<Rigidbody*>();
            rb->Init(actor.get());
        }
        
        it = rigidbodies_to_init.erase(it);
    }

    ProcessSceneUpdate();
    ProcessSceneLateUpdate();
    RemoveActorComponents();
    ActorsPendingDestruction();

    for (Actor* actor : actors_to_add) {
        actor_id_vec.push_back(actor->id);
    }
    actors_to_add.clear();
    
    RigidbodyWorld::UpdateWorld();
}

void SceneDB::ProcessSceneOnStart() {
    if (on_start_cache.empty())
        return;

    // Use std::move since we clear the cache at the end anyway
    auto cache_to_process = std::move(on_start_cache);
    on_start_cache.clear();

    for (const auto& [cacheKey, compRef] : cache_to_process) {
        auto itt = actors.find(cacheKey.actorId);
        if (itt == actors.end()) continue;
        const auto& actor = itt->second;
        if (actor->destroyed) continue;

        luabridge::LuaRef comp = *compRef;
        if (!comp["enabled"] || comp["on_start"]) continue;

        if (comp["frame_added"] == Helper::GetFrameNumber() && comp["new_addition"]) continue;

        try {
            comp["OnStart"](comp);
        }
        catch (luabridge::LuaException& e) {
            ReportError(actor->GetName(), e);
        }
        comp["on_start"] = true;
    }
}

void SceneDB::ProcessSceneUpdate() {
    if (on_update_cache.empty()) return;

    // Collect keys first to avoid issues if cache is modified during iteration
    std::vector<ComponentKey> keys_to_process;
    keys_to_process.reserve(on_update_cache.size());
    for (const auto& [key, _] : on_update_cache) {
        keys_to_process.push_back(key);
    }

    for (const auto& cacheKey : keys_to_process) {
        auto cache_it = on_update_cache.find(cacheKey);
        if (cache_it == on_update_cache.end()) continue;

        auto itt = actors.find(cacheKey.actorId);
        if (itt == actors.end()) continue;
        const auto& actor = itt->second;
        if (actor->destroyed) continue;

        luabridge::LuaRef comp = *(cache_it->second);

        if (comp.isUserdata()) {
            continue;
        }

        if (!comp["enabled"]) continue;

        if (comp["frame_added"] == Helper::GetFrameNumber() && comp["new_addition"]) continue;

        try {
            comp["OnUpdate"](comp);
        }
        catch (luabridge::LuaException& e) {
            ReportError(actor->GetName(), e);
        }
    }
}

void SceneDB::ProcessSceneLateUpdate() {
    if (on_late_update_cache.empty()) return;

    // Collect keys first to avoid issues if cache is modified during iteration
    std::vector<ComponentKey> keys_to_process;
    keys_to_process.reserve(on_late_update_cache.size());
    for (const auto& [key, _] : on_late_update_cache) {
        keys_to_process.push_back(key);
    }

    for (const auto& cacheKey : keys_to_process) {
        auto cache_it = on_late_update_cache.find(cacheKey);
        if (cache_it == on_late_update_cache.end()) continue;

        auto itt = actors.find(cacheKey.actorId);
        if (itt == actors.end()) continue;
        const auto& actor = itt->second;
        if (actor->destroyed) continue;

        luabridge::LuaRef comp = *(cache_it->second);
        if (!comp["enabled"]) continue;

        if (comp["frame_added"] == Helper::GetFrameNumber() && comp["new_addition"]) continue;

        try {
            comp["OnLateUpdate"](comp);
        }
        catch (luabridge::LuaException& e) {
            ReportError(actor->GetName(), e);
        }
    }
}


void SceneDB::RemoveActorComponents() {
    for (const auto & actor_pair : actors) {
        auto & actor = actor_pair.second;
        auto & remove_vec = actor->components_to_remove;
        if (remove_vec.empty()) continue;
        
        std::sort(remove_vec.begin(), remove_vec.end());
        
        auto & comp = actor->components;
        auto & comp_keys = actor->component_keys;
        
        for (auto & key : remove_vec) {
            auto comp_it = comp.find(key);
            if (comp_it != comp.end()) {
                // Call OnDestroy if it exists
                auto& component = comp_it->second;
                if ((*component)["OnDestroy"].isFunction()) {
                    try {
                        (*component)["OnDestroy"](*component);
                    }
                    catch (luabridge::LuaException& e) {
                        ReportError(actor->GetName(), e);
                    }
                } else if ((*component).isUserdata() && (*component).isInstance<Rigidbody>()) {
                    Rigidbody* rb = (*component).cast<Rigidbody*>();
                    rb->OnDestroy();
                }
            }
            
            comp_keys.erase(key);
            comp.erase(key);
        }
        remove_vec.clear();
    }
}

void SceneDB::addComponentToCaches(uint64_t actorId, const std::string& key, std::shared_ptr<luabridge::LuaRef> compRef) {
    luabridge::LuaRef component = *compRef;
    ComponentKey cacheKey{ actorId, key };

    if (component.isUserdata()) {
        if (component.isInstance<Rigidbody>()) {
            on_update_cache[cacheKey] = compRef;
            on_late_update_cache[cacheKey] = compRef;
        }
        return;
    }

    if (!component["enabled"]) return;

    if (component["OnStart"].isFunction() && !component["on_start"]) {
        on_start_cache[cacheKey] = compRef;
    }

    if (component["OnUpdate"].isFunction()) {
        on_update_cache[cacheKey] = compRef;
    }

    if (component["OnLateUpdate"].isFunction()) {
        on_late_update_cache[cacheKey] = compRef;
    }
}

void SceneDB::removeComponentFromCaches(uint64_t actorId, const std::string& key) {
    ComponentKey cacheKey{ actorId, key };
    on_start_cache.erase(cacheKey);
    on_update_cache.erase(cacheKey);
    on_late_update_cache.erase(cacheKey);
}

void SceneDB::rebuildComponentCaches() {
    on_start_cache.clear();
    on_update_cache.clear();
    on_late_update_cache.clear();

    for (const auto& [actorId, actor] : actors) {
        if (actor->destroyed) continue;

        for (const auto& key : actor->component_keys) {
            auto it = actor->components.find(key);
            if (it == actor->components.end()) continue;

            addComponentToCaches(actorId, key, it->second);
        }
    }
}

Actor* SceneDB::InstantiateActor(const std::string& temp) {
    uint64_t newId = id_ctr++;
    actors[newId] = std::make_unique<Actor>();
    Actor* actor = actors[newId].get();

    loadTemplate(temp, actor);
    actor->id = newId;

    for (auto& key : actor->component_keys) {
        auto& comp = actor->components[key];
        
        if ((*comp).isTable()) {
            (*comp)["frame_added"] = Helper::GetFrameNumber();
            (*comp)["new_addition"] = true;
        } else if ((*comp).isUserdata() && (*comp).isInstance<Rigidbody>()) {
            rigidbodies_to_init.push_back({
                actor->id,
                key,
                true,
                Helper::GetFrameNumber()
            });
        }

        addComponentToCaches(newId, key, comp);
    }

    actors_to_add.push_back(actor);
    onstart_new = true;

    return actor;
}

void SceneDB::DestroyActor(Actor* actor) {
    actor->destroyed = true;
    actors_to_destroy.push_back(actor->GetID());
    
    std::vector<std::string> keys;
    for (const auto& key : actor->component_keys) {
        keys.push_back(key);
    }
    std::sort(keys.begin(), keys.end());
    
    for (const auto& key : keys) {
        auto comp_it = actor->components.find(key);
        if (comp_it != actor->components.end()) {
            auto& comp = comp_it->second;
            if ((*comp)["OnDestroy"].isFunction()) {
                try {
                    (*comp)["OnDestroy"](*comp);
                }
                catch (luabridge::LuaException& e) {
                    ReportError(actor->GetName(), e);
                }
            } else if ((*comp).isUserdata() && (*comp).isInstance<Rigidbody>()) {
                Rigidbody* rb = (*comp).cast<Rigidbody*>();
                rb->OnDestroy();
            }
            
            (*comp)["enabled"] = false;
            removeComponentFromCaches(actor->GetID(), key);
        }
    }
}

void SceneDB::ActorsPendingDestruction() {
    if (actors_to_destroy.empty()) return;

    // Build set of IDs to destroy for O(1) lookup
    std::unordered_set<uint64_t> destroy_set(
        actors_to_destroy.begin(),
        actors_to_destroy.end()
    );

    // Erase from actors map
    for (uint64_t id : actors_to_destroy) {
        actors.erase(id);
    }

    // Remove from actor_id_vec using single-pass O(n) approach
    size_t write_idx = 0;
    for (size_t read_idx = 0; read_idx < actor_id_vec.size(); ++read_idx) {
        if (destroy_set.count(actor_id_vec[read_idx]) == 0) {
            actor_id_vec[write_idx++] = actor_id_vec[read_idx];
        }
    }
    actor_id_vec.resize(write_idx);

    actors_to_destroy.clear();
}

luabridge::LuaRef SceneDB::FindActor(const std::string & name) {
    luabridge::LuaRef ret = luabridge::LuaRef(ComponentDB::GetLuaState());
    for (const auto & id : actor_id_vec) {
        auto it = actors.find(id);
        if (it == actors.end()) continue;
        const auto& actor = it->second;
        
        if (actor->name == name && !actor->destroyed) {
            ret = actor.get();
            return ret;
        }
    }
    
    for (auto & actor : actors_to_add) {
        if (actor->GetName() == name){
            ret = actor;
            return ret;
        }
    }
    
    return ret;
}

luabridge::LuaRef SceneDB::FindAllActor(const std::string & name) {
    luabridge::LuaRef ret_table = luabridge::newTable(ComponentDB::GetLuaState());
    int idx = 1;
    
    for (const auto & id : actor_id_vec) {
        auto it = actors.find(id);
        if (it == actors.end()) continue;
        const auto& actor = it->second;
        
        if (actor->GetName() == name && !actor->destroyed)
            ret_table[idx++] = actor.get();
    }
    
    for (Actor* actor : actors_to_add) {
        if (actor->GetName() == name && !actor->destroyed) {
            ret_table[idx++] = actor;
        }
    }
    
    return ret_table;
}

void SceneDB::clearLuaRefs() {
    actors.clear();
    actor_id_vec.clear();
    ComponentDB::CDB.clear();
    templateCache.clear();
}
