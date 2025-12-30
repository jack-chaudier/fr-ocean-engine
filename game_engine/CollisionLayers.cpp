//
//  CollisionLayers.cpp
//  game_engine
//
//  Created for FR-Ocean Engine.
//

#include "CollisionLayers.hpp"
#include "Logger.hpp"

void CollisionLayers::Init() {
    layer_bits.clear();
    collision_matrix.clear();

    // Define default layers
    DefineLayer("default", 0);
    DefineLayer("player", 1);
    DefineLayer("enemy", 2);
    DefineLayer("player_bullet", 3);
    DefineLayer("enemy_bullet", 4);
    DefineLayer("wall", 5);
    DefineLayer("item", 6);
    DefineLayer("trigger", 7);

    // Default collision settings - everything collides with default and wall
    SetLayerCollision("default", "default", true);
    SetLayerCollision("default", "wall", true);
    SetLayerCollision("player", "wall", true);
    SetLayerCollision("enemy", "wall", true);
    SetLayerCollision("player", "enemy", true);
    SetLayerCollision("player", "enemy_bullet", true);
    SetLayerCollision("enemy", "player_bullet", true);
    SetLayerCollision("player", "item", true);
    SetLayerCollision("player_bullet", "wall", true);
    SetLayerCollision("enemy_bullet", "wall", true);
}

void CollisionLayers::DefineLayer(const std::string& name, int bit_index) {
    if (bit_index < 0 || bit_index > 15) {
        LOG_WARNING("CollisionLayers: bit_index must be 0-15, got " + std::to_string(bit_index));
        return;
    }
    layer_bits[name] = bit_index;
}

void CollisionLayers::SetLayerCollision(const std::string& layer1, const std::string& layer2, bool collide) {
    std::string key = MakePairKey(layer1, layer2);

    if (collide) {
        collision_matrix.insert(key);
    } else {
        collision_matrix.erase(key);
    }
}

uint16_t CollisionLayers::GetCategoryBits(const std::string& layer_name) {
    auto it = layer_bits.find(layer_name);
    if (it == layer_bits.end()) {
        return 0x0001;  // Default layer
    }
    return static_cast<uint16_t>(1 << it->second);
}

uint16_t CollisionLayers::GetMaskBits(const std::string& layer_name) {
    uint16_t mask = 0;

    for (const auto& [other_layer, bit_index] : layer_bits) {
        if (DoLayersCollide(layer_name, other_layer)) {
            mask |= static_cast<uint16_t>(1 << bit_index);
        }
    }

    return mask;
}

void CollisionLayers::ApplyToFixture(b2FixtureDef& fixture_def, const std::string& layer_name) {
    fixture_def.filter.categoryBits = GetCategoryBits(layer_name);
    fixture_def.filter.maskBits = GetMaskBits(layer_name);
}

bool CollisionLayers::DoLayersCollide(const std::string& layer1, const std::string& layer2) {
    std::string key = MakePairKey(layer1, layer2);
    return collision_matrix.count(key) > 0;
}

std::string CollisionLayers::MakePairKey(const std::string& a, const std::string& b) {
    // Always store in alphabetical order for consistent lookup
    if (a < b) {
        return a + "," + b;
    }
    return b + "," + a;
}
