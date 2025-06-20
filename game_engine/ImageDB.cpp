//
//  ImageDB.cpp
//  game_engine
//
//  Created by Jack Gaffney on 2/9/25.
//

#include "ImageDB.hpp"
#include "SDL2_image/SDL_image.h"
#include <iostream>
#include <filesystem>
#include <cstdlib>
#include <algorithm>


bool compare_image_requests(const ImageDrawRequest& a, const ImageDrawRequest& b) {
    if (a.is_ui != b.is_ui) return !a.is_ui;
    
    if (a.sorting_order != b.sorting_order) return a.sorting_order < b.sorting_order;
    
    return a.order_index < b.order_index;
}

ImageDB::~ImageDB() {
    for (auto& pair : textureMap) {
        if (pair.second) {
            SDL_DestroyTexture(pair.second);
        }
    }
}

SDL_Texture* ImageDB::GetTexture(const std::string& imageName) {
    auto it = textureMap.find(imageName);
    if (it != textureMap.end()) return it->second;
    
    std::string path = "resources/images/" + imageName + ".png";
    if (!std::filesystem::exists(path)) {
        std::cout << "error: missing image " << imageName << "\n";
        exit(0);
    }
    
    SDL_Texture* tex = IMG_LoadTexture(Renderer::getSDLRenderer(), path.c_str());
    if (!tex) {
        std::cout << "error: failed to load image " << imageName << "\n";
        exit(0);
    }
    
    textureMap[imageName] = tex;
    return tex;
}

// Image API methods

void ImageDB::QueueImageDraw(const std::string& imageName, float x, float y) {
    ImageDrawRequest request;
    request.image_name = imageName;
    request.x = x;
    request.y = y;
    request.rotation_degrees = 0;
    request.scale_x = 1.0f;
    request.scale_y = 1.0f;
    request.pivot_x = 0.5f;
    request.pivot_y = 0.5f;
    request.r = 255;
    request.g = 255;
    request.b = 255;
    request.a = 255;
    request.sorting_order = 0;
    request.is_ui = false;
    request.order_index = request_counter++;
    
    image_draw_request_queue.push_back(request);
}

void ImageDB::QueueImageDrawEx(const std::string& imageName, float x, float y,
                              float rotationDegrees, float scaleX, float scaleY,
                              float pivotX, float pivotY,
                              float r, float g, float b, float a,
                              float sortingOrder) {
    ImageDrawRequest request;
    request.image_name = imageName;
    request.x = x;
    request.y = y;
    request.rotation_degrees = static_cast<int>(rotationDegrees);
    request.scale_x = scaleX;
    request.scale_y = scaleY;
    request.pivot_x = pivotX;
    request.pivot_y = pivotY;
    request.r = static_cast<int>(r);
    request.g = static_cast<int>(g);
    request.b = static_cast<int>(b);
    request.a = static_cast<int>(a);
    request.sorting_order = static_cast<int>(sortingOrder);
    request.is_ui = false;
    request.order_index = request_counter++;
    
    image_draw_request_queue.push_back(request);
}

void ImageDB::QueueImageDrawUI(const std::string& imageName, float x, float y) {
    ImageDrawRequest request;
    request.image_name = imageName;
    
    request.x = static_cast<int>(x);
    request.y = static_cast<int>(y);
    request.rotation_degrees = 0;
    request.scale_x = 1.0f;
    request.scale_y = 1.0f;
    
    request.pivot_x = 0.0f;
    request.pivot_y = 0.0f;
    request.r = 255;
    request.g = 255;
    request.b = 255;
    request.a = 255;
    
    request.sorting_order = 0;
    request.is_ui = true;
    request.order_index = request_counter++;

    image_draw_request_queue.push_back(request);
}


void ImageDB::QueueImageDrawUIEx(const std::string& imageName, float x, float y,
    float r, float g, float b, float a,
    float sortingOrder) {
    ImageDrawRequest request;
    request.image_name = imageName;
    
    request.x = static_cast<int>(x);
    request.y = static_cast<int>(y);
    request.rotation_degrees = 0;
    request.scale_x = 1.0f;
    request.scale_y = 1.0f;
    
    request.pivot_x = 0.0f;
    request.pivot_y = 0.0f;
    request.r = static_cast<int>(r);
    request.g = static_cast<int>(g);
    request.b = static_cast<int>(b);
    request.a = static_cast<int>(a);
    request.sorting_order = static_cast<int>(sortingOrder);
    request.is_ui = true;
    request.order_index = request_counter++;

    image_draw_request_queue.push_back(request);
}

void ImageDB::QueueDrawPixel(float x, float y, float r, float g, float b, float a) {
    PixelDrawRequest request;
    request.x = static_cast<int>(x);
    request.y = static_cast<int>(y);
    request.r = static_cast<int>(r);
    request.g = static_cast<int>(g);
    request.b = static_cast<int>(b);
    request.a = static_cast<int>(a);
    
    pixel_draw_request_queue.push_back(request);
}

void ImageDB::RenderAndClearAllImages() {
    std::stable_sort(image_draw_request_queue.begin(), image_draw_request_queue.end(), compare_image_requests);

    float zoom_factor = Renderer::GetCameraZoomFactor();
    glm::ivec2 cam_dimensions = Renderer::GetCameraDimensions();
    const int pixels_per_meter = 100;

    for (auto& request : image_draw_request_queue) {
        SDL_Texture* tex = GetTexture(request.image_name);
        SDL_FRect tex_rect;

        float texture_width, texture_height;
        Helper::SDL_QueryTexture(tex, &texture_width, &texture_height);

        // Apply scale
        float x_scale = glm::abs(request.scale_x);
        float y_scale = glm::abs(request.scale_y);
        tex_rect.w = texture_width * x_scale;
        tex_rect.h = texture_height * y_scale;

        // Calculate pivot point
        SDL_FPoint pivot_point = { request.pivot_x * tex_rect.w, request.pivot_y * tex_rect.h };

        // Handle flip
        int flip_mode = SDL_FLIP_NONE;
        if (request.scale_x < 0)
            flip_mode |= SDL_FLIP_HORIZONTAL;
        if (request.scale_y < 0)
            flip_mode |= SDL_FLIP_VERTICAL;

        if (request.is_ui) {
            SDL_RenderSetScale(Renderer::getSDLRenderer(), 1, 1);

            tex_rect.x = request.x;
            tex_rect.y = request.y;
        }
        else {
            SDL_RenderSetScale(Renderer::getSDLRenderer(), zoom_factor, zoom_factor);

            glm::vec2 final_rendering_position = glm::vec2(request.x, request.y) - Renderer::GetCameraPosition();

            tex_rect.x = final_rendering_position.x * pixels_per_meter +
                cam_dimensions.x * 0.5f * (1.0f / zoom_factor) -
                pivot_point.x;
            tex_rect.y = final_rendering_position.y * pixels_per_meter +
                cam_dimensions.y * 0.5f * (1.0f / zoom_factor) -
                pivot_point.y;
        }

        SDL_SetTextureColorMod(tex, request.r, request.g, request.b);
        SDL_SetTextureAlphaMod(tex, request.a);

        Helper::SDL_RenderCopyEx(-1, "", Renderer::getSDLRenderer(), tex, NULL, &tex_rect,
            request.rotation_degrees, &pivot_point,
            static_cast<SDL_RendererFlip>(flip_mode));

        SDL_SetTextureColorMod(tex, 255, 255, 255);
        SDL_SetTextureAlphaMod(tex, 255);
    }

    SDL_RenderSetScale(Renderer::getSDLRenderer(), 1, 1);

    image_draw_request_queue.clear();
}

void ImageDB::RenderAndClearAllPixels() {
    SDL_Renderer* renderer = Renderer::getSDLRenderer();
    
    SDL_SetRenderDrawBlendMode(renderer, SDL_BLENDMODE_BLEND);
    
    for (const auto& request : pixel_draw_request_queue) {
        SDL_SetRenderDrawColor(renderer, request.r, request.g, request.b, request.a);
        SDL_RenderDrawPoint(renderer, request.x, request.y);
    }
    
    SDL_SetRenderDrawBlendMode(renderer, SDL_BLENDMODE_NONE);
    
    pixel_draw_request_queue.clear();
}



void ImageDB::ClearQueues() {
    image_draw_request_queue.clear();
    pixel_draw_request_queue.clear();
    request_counter = 0;
}

void ImageDB::CreateDefaultParticleTextureWithName(const std::string& name) {
    // Have we already cached this default texture?
    if (textureMap.find(name) != textureMap.end())
        return;
        
    // Create an SDL_Surface (a cpu-side texture) with no special flags, 8 width, 8 height, 32 bits of color depth (RGBA) and no masking.
    SDL_Surface* surface = SDL_CreateRGBSurfaceWithFormat(0, 8, 8, 32, SDL_PIXELFORMAT_RGBA8888);
    
    // Ensure color set to white (255, 255, 255, 255)
    Uint32 white_color = SDL_MapRGBA(surface->format, 255, 255, 255, 255);
    SDL_FillRect(surface, NULL, white_color);
    
    // Create a gpu-side texture from the cpu-side surface now that we're done editing it.
    SDL_Texture* texture = SDL_CreateTextureFromSurface(Renderer::getSDLRenderer(), surface);
    
    // Clean up the surface and cache this default texture for future use (we'll probably spawn many particles with it).
    SDL_FreeSurface(surface);
    textureMap[name] = texture;
}
