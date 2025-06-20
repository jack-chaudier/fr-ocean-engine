//
//  TextDB.hpp
//  game_engine
//
//  Created by Jack Gaffney on 2/9/25.
//

#ifndef TextDB_hpp
#define TextDB_hpp

#include <string>
#include <vector>
#include <queue>
#include <filesystem>
#include <unordered_map>
#include "SDL2/SDL.h"
#include "SDL2_ttf/SDL_ttf.h"
#include "Renderer.hpp"
#include "Helper.h"

struct TextDrawRequest {
    std::string content;
    int x, y;
    std::string fontName;
    int fontSize;
    SDL_Color color;
};

class TextDB {
public:
    static void Init();
    static void Shutdown();
    
    static void LoadFont(const std::string& fontName);
    static TTF_Font* GetFont(const std::string& fontName, int fontSize);
    
    static void QueueTextDraw(const std::string& content, float x, float y,
                      const std::string& fontName, float fontSize,
                      float r, float g, float b, float a);
    static void RenderQueuedTexts();
    
private:
    inline static std::unordered_map<std::string, std::unordered_map<int, TTF_Font*>> fonts;
    inline static std::vector<SDL_Texture*> texts;
    inline static std::string fontPath = "resources/fonts/";
    inline static std::queue<TextDrawRequest> drawRequests;
    inline static SDL_Color textColor = {255, 255, 255, 255};
    inline static bool initialized = false;
};

#endif /* TextDB_hpp */
