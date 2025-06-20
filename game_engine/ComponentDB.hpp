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

class ComponentDB {
public:
    ~ComponentDB() {
        if (L) {
            lua_close(L);
            L = nullptr;
        }
    }
    
    static void Init();

    static void loadComponents(Actor * a, const rapidjson::Value &doc);
    
    static std::shared_ptr<luabridge::LuaRef> CreateComponent(const std::string& type, const std::string& comp_key);
    
    inline static std::unordered_map<std::string, std::shared_ptr<luabridge::LuaRef>> CDB;
    
    inline static std::unordered_map<std::string, std::shared_ptr<luabridge::LuaRef>> componentTypeCache;
    
    static void CPPLog(std::string message);
    
    static lua_State * GetLuaState() {return L;};
    
    inline static int runtime_comp_add = 0;
    
    static void EstablishInheritance(luabridge::LuaRef& instance_table, luabridge::LuaRef& parent_table);

private:
    inline static lua_State* L;
    static void overrideLuaRefValue(luabridge::LuaRef& table, const std::string & name, const rapidjson::Value& prop_value);
    static void overrideRigidbodyfValue(luabridge::LuaRef& table, const std::string & name, const rapidjson::Value& prop_value);
};

#endif // COMPONENTDB_HPP
