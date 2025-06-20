//
//  SceneDB.hpp
//  game_engine
//
//  Created by Jack Gaffney on 1/29/25.
//

#ifndef SceneDB_hpp
#define SceneDB_hpp

#include <string>
#include <unordered_map>
#include <memory>
#include <vector>
#include <array>
#include <unordered_set>
#include <set>
#include <filesystem>
#include <algorithm>
#include "EngineUtils.h"
#include "rapidjson/document.h"
#include "Actor.hpp"
#include "ConfigManager.hpp"
#include "lua/lua.hpp"
#include "LuaBridge/LuaBridge.h"
#include "Helper.h"
#include "ComponentDB.hpp"


class SceneDB {
public:
    SceneDB() {
        actor_id_vec.reserve(1000);
        actors_to_destroy.reserve(100);
        actors_to_add.reserve(100);
    }

    ~SceneDB();
    
    static SceneDB* currentScene;

    struct ComponentKey {
        uint64_t actorId;
        std::string componentKey;

        bool operator<(const ComponentKey& other) const {
            if (actorId != other.actorId)
                return actorId < other.actorId;
            return componentKey < other.componentKey;
        }
    };
    
    struct RigidbodyInitInfo {
        uint64_t actorId;
        std::string componentKey;
        bool isNew;
        int frameAdded;
    };
    
    void loadScene();
    static void loadTemplate(const std::string & template_name, Actor * actor);
    void clearLuaRefs();
    void UpdateScene();
    
    static luabridge::LuaRef FindActor(const std::string & name);
    static luabridge::LuaRef FindAllActor(const std::string & name);
    
    static Actor * InstantiateActor(const std::string & temp);
    
    static void DestroyActor(Actor * actor);
    
    inline static std::unordered_map<uint64_t, std::unique_ptr<Actor>> actors;
    inline static std::vector<uint64_t> actor_id_vec;
    
    inline static std::vector<uint64_t> actors_to_destroy;
    inline static std::vector<Actor*> actors_to_add;
    
    inline static std::string current_scene_name;
    inline static std::string next_scene_to_load;

    static void Load(const std::string& scene_name);
    static std::string GetCurrent();
    static void DontDestroy(Actor* actor);
    
    inline static std::unordered_map<std::string, rapidjson::Document> templateCache;

    inline static std::map<ComponentKey, std::shared_ptr<luabridge::LuaRef>> on_start_cache;
    inline static std::map<ComponentKey, std::shared_ptr<luabridge::LuaRef>> on_update_cache;
    inline static std::map<ComponentKey, std::shared_ptr<luabridge::LuaRef>> on_late_update_cache;
    
    inline static std::vector<RigidbodyInitInfo> rigidbodies_to_init;

    inline static bool onstart_new = false;


    static void ReportError(const std::string& actor_name, const luabridge::LuaException& e);
    static void addComponentToCaches(uint64_t actorId, const std::string& key, std::shared_ptr<luabridge::LuaRef> compRef);
    static void removeComponentFromCaches(uint64_t actorId, const std::string& key);
    static void rebuildComponentCaches();
private:
    inline static std::string scene_path = "resources/scenes/";

    inline static uint64_t id_ctr = 0;
    
private:
    
    void ProcessSceneOnStart();
    void ProcessSceneUpdate();
    void ProcessSceneLateUpdate();
    void RemoveActorComponents();
    
    void ActorsPendingDestruction();
};

#endif /* SceneDB_hpp */
