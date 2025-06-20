//
//  Engine.hpp
//  game_engine
//
//  Created by Jack Gaffney on 1/22/25.
//

#ifndef Engine_hpp
#define Engine_hpp

#include <string>
#include <optional>
#include "Renderer.hpp"
#include "EngineUtils.h"
#include "SceneDB.hpp"
#include "Actor.hpp"
#include "rapidjson/document.h"
#include "Helper.h"
#include "ConfigManager.hpp"
#include "lua/lua.hpp"
#include "LuaBridge/LuaBridge.h"
#include "ApplicationAPI.h"


class Engine {
public:
    Engine();
    ~Engine();
    

private:
    inline static SceneDB scene;

    inline static glm::ivec3 cleanColor;
    
public:
    static void GameLoop();
    static void Update();
    static void Render();
    
};

#endif /* Engine_hpp */
