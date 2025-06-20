//
//  ImageDB.hpp
//  game_engine
//
//  Created by Jack Gaffney on 2/9/25.
//

#ifndef ImageDB_hpp
#define ImageDB_hpp

#include "SDL2/SDL.h"
#include "SDL2_image/SDL_image.h"
#include "Helper.h"
#include "glm/glm.hpp"
#include "Renderer.hpp"
#include "ConfigManager.hpp"
#include <vector>
#include <string>
#include <unordered_map>
#include <queue>
#include <algorithm>

// Struct to hold image draw requests
struct ImageDrawRequest {
    std::string image_name;
    float x;
    float y;
    int rotation_degrees;
    float scale_x;
    float scale_y;
    float pivot_x;
    float pivot_y;
    int r;
    int g;
    int b;
    int a;
    int sorting_order;
    bool is_ui;
    size_t order_index;
};

// Struct to hold pixel draw requests
struct PixelDrawRequest {
    int x, y;
    int r, g, b, a;
    size_t order_index;
};

class ImageDB {
public:
    ~ImageDB();

    static SDL_Texture* GetTexture(const std::string& imageName);
    
    // Image API methods
    static void QueueImageDraw(const std::string& imageName, float x, float y);
    static void QueueImageDrawEx(const std::string& imageName, float x, float y,
                          float rotationDegrees, float scaleX, float scaleY,
                          float pivotX, float pivotY,
                          float r, float g, float b, float a,
                          float sortingOrder);
    static void QueueImageDrawUI(const std::string& imageName, float x, float y);
    static void QueueImageDrawUIEx(const std::string& imageName, float x, float y,
                            float r, float g, float b, float a,
                            float sortingOrder);
    static void QueueDrawPixel(float x, float y, float r, float g, float b, float a);
    
    static void RenderAndClearAllImages();
    static void RenderAndClearAllPixels();
    
    static void ClearQueues();
    
    void CreateDefaultParticleTextureWithName(const std::string& name);
    
private:
    inline static std::unordered_map<std::string, SDL_Texture*> textureMap;
    
    inline static std::vector<ImageDrawRequest> image_draw_request_queue;
    inline static std::vector<PixelDrawRequest> pixel_draw_request_queue;
    inline static size_t request_counter = 0;
    inline static  size_t pixel_reuest_counter = 0;
};

#endif /* ImageDB_hpp */
