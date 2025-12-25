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

/**
 * @class ConfigManager
 * @brief Manages game and rendering configuration.
 *
 * Loads and provides access to configuration values from JSON files.
 * Configuration is loaded once at startup and cached for efficient access.
 */
class ConfigManager {
public:
    ConfigManager(const std::string& gameConfigPath, const std::string& renderConfigPath);

    /**
     * @brief Load all configuration files.
     * @throws ConfigurationException if required files are missing or invalid.
     */
    static void Load();

    static std::string GetGameTitle();
    static glm::ivec2 GetResolution();
    static glm::ivec3 GetClearColor();
    static std::string GetInitialScene();

private:
    inline static glm::ivec3 color = {255, 255, 255};
    inline static glm::ivec2 resolution = {640, 360};
    inline static std::string gameConfigPath;
    inline static std::string renderConfigPath;
    inline static std::string gameTitle = "";
    inline static std::string initialScene = "";

    inline static rapidjson::Document gameDoc;
    inline static rapidjson::Document renderDoc;

    static void LoadRender();
    static void LoadGame();
};

#endif /* ConfigManager_hpp */
