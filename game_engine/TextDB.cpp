//
//  TextDB.cpp
//  game_engine
//
//  Created by Jack Gaffney on 2/9/25.
//

#include <iostream>
#include "TextDB.hpp"
#include "Logger.hpp"
#include "EngineException.hpp"

void TextDB::Init() {
    if (!initialized) {
        if (TTF_Init() != 0) {
            std::string error = TTF_GetError();
            LOG_FATAL("SDL_ttf initialization failed: " + error);
            throw RenderException("SDL_ttf initialization failed: " + error);
        }
        initialized = true;

        // Make sure fonts directory exists
        if (!std::filesystem::exists(fontPath)) {
            std::filesystem::create_directories(fontPath);
        }
    }
}

void TextDB::Shutdown() {
    // Clean up all loaded fonts
    for (auto& fontPair : fonts) {
        for (auto& sizePair : fontPair.second) {
            if (sizePair.second) {
                TTF_CloseFont(sizePair.second);
            }
        }
    }
    fonts.clear();
    
    // Clean up any textures
    for (SDL_Texture* tex : texts) {
        if (tex) {
            SDL_DestroyTexture(tex);
        }
    }
    texts.clear();
    
    // Clear any pending draw requests
    while (!drawRequests.empty()) {
        drawRequests.pop();
    }
    
    if (initialized) {
        TTF_Quit();
        initialized = false;
    }
}

void TextDB::LoadFont(const std::string& fontName) {
    // This method now just checks if the font file exists
    // Actual font loading happens on-demand in GetFont
    std::string fp = fontPath + fontName + ".ttf";
    if (!std::filesystem::exists(fp)) {
        LOG_FATAL("Font missing: " + fontName);
        throw ResourceNotFoundException("font", fontName);
    }

    // Load the default size font for backward compatibility
    GetFont(fontName, 16);
}

TTF_Font* TextDB::GetFont(const std::string& fontName, int fontSize) {
    // Check if we already have this font in this size
    auto fontIt = fonts.find(fontName);
    if (fontIt != fonts.end()) {
        auto sizeIt = fontIt->second.find(fontSize);
        if (sizeIt != fontIt->second.end()) {
            return sizeIt->second; // Return existing font
        }
    }

    // Load the font
    std::string fp = fontPath + fontName + ".ttf";
    if (!std::filesystem::exists(fp)) {
        LOG_FATAL("Font missing: " + fontName);
        throw ResourceNotFoundException("font", fontName);
    }

    TTF_Font* font = TTF_OpenFont(fp.c_str(), fontSize);
    if (!font) {
        std::string error = "Cannot load font " + fontName + " size " + std::to_string(fontSize);
        LOG_FATAL(error);
        throw RenderException(error);
    }

    // Store and return the font
    fonts[fontName][fontSize] = font;
    return font;
}


void TextDB::QueueTextDraw(const std::string& content, float x, float y,
                          const std::string& fontName, float fontSize,
                          float r, float g, float b, float a) {
    TextDrawRequest request;
    request.content = content;
    request.x = static_cast<int>(x);
    request.y = static_cast<int>(y);
    request.fontName = fontName;
    request.fontSize = static_cast<int>(fontSize);
    request.color = {static_cast<Uint8>(r), static_cast<Uint8>(g),
                     static_cast<Uint8>(b), static_cast<Uint8>(a)};
    
    drawRequests.push(request);
}

void TextDB::RenderQueuedTexts() {
    while (!drawRequests.empty()) {
        const TextDrawRequest& req = drawRequests.front();
        
        // Get (or load) the requested font
        TTF_Font* font = GetFont(req.fontName, req.fontSize);
        
        // Render the text with anti-aliasing for better quality
        SDL_Surface* surface = TTF_RenderText_Blended(font, req.content.c_str(), req.color);
        if (surface) {
            SDL_Texture* texture = SDL_CreateTextureFromSurface(Renderer::getSDLRenderer(), surface);
            
            SDL_FRect destRect = {static_cast<float>(req.x), static_cast<float>(req.y), 0, 0};
            Helper::SDL_QueryTexture(texture, &destRect.w, &destRect.h);
            // Use dummy values for actor_id and actor_name as per requirements
            Helper::SDL_RenderCopy(Renderer::getSDLRenderer(), texture, nullptr, &destRect);
            
            SDL_DestroyTexture(texture);
            SDL_FreeSurface(surface);
        }
        
        drawRequests.pop();
    }
}
