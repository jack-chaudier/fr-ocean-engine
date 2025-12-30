//
//  CollisionLayers.hpp
//  game_engine
//
//  Named collision layer system for physics filtering.
//

#ifndef CollisionLayers_hpp
#define CollisionLayers_hpp

#include <string>
#include <unordered_map>
#include <unordered_set>
#include <cstdint>
#include "box2d/box2d.h"

/**
 * @class CollisionLayers
 * @brief Static collision layer management for physics filtering.
 *
 * Allows defining named layers and configuring which layers collide with each other.
 */
class CollisionLayers {
public:
    /**
     * @brief Initialize with default layers.
     */
    static void Init();

    /**
     * @brief Define a named layer with a specific bit index.
     * @param name Layer name (e.g., "player", "enemy").
     * @param bit_index Bit position (0-15 for Box2D's 16-bit mask).
     */
    static void DefineLayer(const std::string& name, int bit_index);

    /**
     * @brief Configure whether two layers should collide.
     * @param layer1 First layer name.
     * @param layer2 Second layer name.
     * @param collide Whether collision should occur.
     */
    static void SetLayerCollision(const std::string& layer1, const std::string& layer2, bool collide);

    /**
     * @brief Get the category bits for a layer.
     * @param layer_name Layer name.
     * @return Category bits for Box2D fixture.
     */
    static uint16_t GetCategoryBits(const std::string& layer_name);

    /**
     * @brief Get the mask bits for a layer.
     * @param layer_name Layer name.
     * @return Mask bits for Box2D fixture.
     */
    static uint16_t GetMaskBits(const std::string& layer_name);

    /**
     * @brief Apply layer settings to a fixture definition.
     * @param fixture_def Fixture definition to modify.
     * @param layer_name Layer name.
     */
    static void ApplyToFixture(b2FixtureDef& fixture_def, const std::string& layer_name);

    /**
     * @brief Check if two layers collide.
     * @param layer1 First layer name.
     * @param layer2 Second layer name.
     * @return True if layers collide.
     */
    static bool DoLayersCollide(const std::string& layer1, const std::string& layer2);

private:
    // Layer name -> bit index (0-15)
    inline static std::unordered_map<std::string, int> layer_bits;

    // Set of layer pairs that should collide (stored as "layer1,layer2" with alphabetical order)
    inline static std::unordered_set<std::string> collision_matrix;

    static std::string MakePairKey(const std::string& a, const std::string& b);
};

#endif /* CollisionLayers_hpp */
