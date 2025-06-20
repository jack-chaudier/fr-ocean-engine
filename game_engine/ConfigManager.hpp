//
//  ConfigManager.hpp
//  game_engine
//
//  Created by Jack Gaffney on 2/8/25.
//

#ifndef ConfigManager_hpp
#define ConfigManager_hpp

#include <string>
#include <vector>
#include <memory>
#include <optional>
#include "glm/glm.hpp"
#include "EngineUtils.h"
#include "rapidjson/document.h"

class Actor;
class ConfigManager {
public:
    ConfigManager(const std::string &gameConfigPath, const std::string &renderConfigPath);
    static void load();
    
    static std::string getGameTitle();
    static glm::ivec2 getResolution();
    static glm::ivec3 getClearColor();
    static std::string getInitialScene();
private:
    inline static glm::ivec3 color = {255, 255, 255};
    inline static glm::ivec2 resolution = {640, 360};
    inline static glm::vec2 camOffset = { 0, 0 };
    inline static std::string gameConfigPath;
    inline static std::string renderConfigPath;
    inline static std::string gameTitle = "";
    inline static std::string initialScene = "";
    
    inline static rapidjson::Document gameDoc;
    inline static rapidjson::Document renderDoc;
    
private:
    static void loadRender();
    static void loadGame();
    static uint64_t create_composite_key(int x, int y);
};

#endif /* ConfigManager_hpp */
