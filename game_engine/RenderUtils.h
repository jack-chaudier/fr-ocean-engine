//
//  RenderUtil.h
//  game_engine
//
//  Created by Jack Gaffney on 2/5/25.
//

#ifndef RenderUtil_h
#define RenderUtil_h

#include <filesystem>
#include <string>
#include "Helper.h"
#include "SDL2/SDL.h"
#include "SceneDB.hpp"
#include "glm/glm.hpp"

class RenderUtils
{
private:
    inline static SDL_Window *window = nullptr;
    inline static glm::ivec2 resolution;
    inline static std::string name;
    
    static EngineUtils utility;
    static rapidjson::Document render_json;
public:
    inline static SDL_Renderer *renderer = nullptr;
    
    static void CreateWindow()
    {
        window = Helper::SDL_CreateWindow(name.c_str(), 100, 100, resolution.x, resolution.y, SDL_WINDOW_SHOWN);
    }
    static void RenderWindow()
    {
        renderer = Helper::SDL_CreateRenderer(window, -1, SDL_RENDERER_PRESENTVSYNC | SDL_RENDERER_ACCELERATED);
    }
    static void DrawColor(int color)
    {
        SDL_SetRenderDrawColor(renderer, color, color, color, 0);
        SDL_RenderClear(renderer);
        Helper::SDL_RenderPresent(renderer);
    }
    
    static void set_name(std::string name_path)
    {
        utility.ReadJsonFile(name_path, render_json);
        name = render_json.HasMember("game_title") ? render_json["game_title"].GetString() : "";
    }
    
    static void set_render(std::string render_path)
    {
        if (std::filesystem::exists(render_path))
        {
            utility.ReadJsonFile(render_path, render_json);
            int x = render_json.HasMember("x_resolution") ? render_json["x_resolution"].GetInt() : 640;
            int y = render_json.HasMember("y_resolution") ? render_json["y_resolution"].GetInt() : 360;
            resolution = glm::ivec2(x, y);
        }
        
        else resolution = glm::ivec2(640, 360);
    }
    
    static glm::ivec2 get_render()
    {
        return resolution;
    }
};

#endif /* RenderUtil_h */
