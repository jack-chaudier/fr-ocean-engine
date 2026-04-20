//
//  ComponentDB.cpp
//  game_engine
//
//  Created by Jack Gaffney on 3/31/25.
//

#include "ComponentDB.hpp"
#include "Actor.hpp"
#include "SceneDB.hpp"
#include "ApplicationAPI.hpp"
#include "Input.hpp"
#include "TextDB.hpp"
#include "AudioDB.hpp"
#include "ImageDB.hpp"
#include "Rigidbody.hpp"
#include "box2d/box2d.h"
#include "PhysicsQuery.hpp"
#include "Logger.hpp"
#include "EngineException.hpp"
#include "Time.hpp"
#include "EventSystem.hpp"
#include "Scheduler.hpp"
#include "Tween.hpp"
#include "Transform.hpp"
#include "CollisionLayers.hpp"
#include "ConfigManager.hpp"
#include "AnimationDB.hpp"
#include "ParticleSystem.hpp"
#include "DebugDraw.hpp"
#include "SceneTransition.hpp"

void ComponentDB::Init() {
    using namespace luabridge;
    L = luaL_newstate();
    luaL_openlibs(L);
    getGlobalNamespace(L)
    
        // DEBUG
        .beginNamespace("Debug")
            .addFunction("Log", &ComponentDB::CPPLog)
            .addFunction("LogError", &ComponentDB::CPPLogError)
        .endNamespace()
    
        // RIGIDBODY
        .beginClass<Rigidbody>("Rigidbody")
            .addConstructor<void (*) (void)>()
            .addProperty("x", &Rigidbody::x)
            .addProperty("y", &Rigidbody::y)
            .addProperty("width", &Rigidbody::width)
            .addProperty("height", &Rigidbody::height)
            .addProperty("radius", &Rigidbody::radius)
            .addProperty("friction", &Rigidbody::friction)
            .addProperty("bounciness", &Rigidbody::bounciness)
            .addProperty("gravity_scale", &Rigidbody::gravity_scale)
            .addProperty("density", &Rigidbody::density)
            .addProperty("angular_friction", &Rigidbody::angular_friction)
            .addProperty("rotation", &Rigidbody::rotation)
            .addProperty("trigger_width", &Rigidbody::trigger_width)
            .addProperty("trigger_height", &Rigidbody::trigger_height)
            .addProperty("trigger_radius", &Rigidbody::trigger_radius)
            .addProperty("body_type", &Rigidbody::body_type)
            .addProperty("collider_type", &Rigidbody::collider_type)
            .addProperty("trigger_type", &Rigidbody::trigger_type)
            .addProperty("precise", &Rigidbody::precise)
            .addProperty("has_collider", &Rigidbody::has_collider)
            .addProperty("has_trigger", &Rigidbody::has_trigger)
            .addProperty("collision_layer", &Rigidbody::collision_layer)
            .addFunction("GetPosition", &Rigidbody::GetPosition)
            .addFunction("GetRotation", &Rigidbody::GetRotation)
            .addFunction("AddForce", &Rigidbody::AddForce)
            .addFunction("SetVelocity", &Rigidbody::SetVelocity)
            .addFunction("SetPosition", &Rigidbody::SetPosition)
            .addFunction("SetRotation", &Rigidbody::SetRotation)
            .addFunction("SetAngularVelocity", &Rigidbody::SetAngularVelocity)
            .addFunction("SetGravityScale", &Rigidbody::SetGravityScale)
            .addFunction("SetUpDirection", &Rigidbody::SetUpDirection)
            .addFunction("SetRightDirection", &Rigidbody::SetRightDirection)
            .addFunction("GetVelocity", &Rigidbody::GetVelocity)
            .addFunction("GetAngularVelocity", &Rigidbody::GetAngularVelocity)
            .addFunction("GetGravityScale", &Rigidbody::GetGravityScale)
            .addFunction("GetUpDirection", &Rigidbody::GetUpDirection)
            .addFunction("GetRightDirection", &Rigidbody::GetRightDirection)
        .endClass()
    
        // Physics
        .beginNamespace("Physics")
            .addFunction("Raycast", &PhysicsQuery::Raycast)
            .addFunction("RaycastAll", &PhysicsQuery::RaycastAll)
        .endNamespace()
    
        // VECTOR2
        .beginClass<b2Vec2>("Vector2")
            .addConstructor<void(*) (float, float)>()
            .addProperty("x", &b2Vec2::x)
            .addProperty("y", &b2Vec2::y)
            .addFunction("Normalize", &b2Vec2::Normalize)
            .addFunction("Length", &b2Vec2::Length)
            .addFunction("__add", &b2Vec2::operator_add)
            .addFunction("__sub", &b2Vec2::operator_sub)
            .addFunction("__mul", &b2Vec2::operator_mul)
            .addStaticFunction("Distance", &b2Distance)
            .addStaticFunction("Dot", static_cast<float(*)(const b2Vec2&, const b2Vec2&)>(&b2Dot))
        .endClass()
        .beginClass<Actor>("Actor")
            .addFunction("GetName", &Actor::GetName)
            .addFunction("GetID", &Actor::GetID)
            .addFunction("GetComponentByKey", &Actor::GetComponentByKey)
            .addFunction("GetComponent", &Actor::GetComponent)
            .addFunction("GetComponents", &Actor::GetComponents)
            .addFunction("AddComponent", &Actor::AddComponent)
            .addFunction("RemoveComponent", &Actor::RemoveComponent)
        .endClass()
        .beginNamespace("Actor")
            .addFunction("Find", &SceneDB::FindActor)
            .addFunction("FindAll", &SceneDB::FindAllActor)
            .addFunction("Instantiate", &SceneDB::InstantiateActor)
            .addFunction("Destroy", &SceneDB::DestroyActor)
        .endNamespace()
        .beginNamespace("Application")
            .addFunction("Quit", &ApplicationAPI::Quit)
            .addFunction("Sleep", &ApplicationAPI::Sleep)
            .addFunction("OpenURL", &ApplicationAPI::OpenURL)
            .addFunction("GetFrame", &ApplicationAPI::GetFrame)
        .endNamespace()
        .beginClass<glm::vec2>("vec2")
            .addProperty("x", &glm::vec2::x)
            .addProperty("y", &glm::vec2::y)
        .endClass()
        .beginNamespace("Input")
            .addFunction("GetKey", static_cast<bool (*)(const std::string&)>(&Input::GetKey))
            .addFunction("GetKeyDown", static_cast<bool (*)(const std::string&)>(&Input::GetKeyDown))
            .addFunction("GetKeyUp", static_cast<bool (*)(const std::string&)>(&Input::GetKeyUp))
            .addFunction("GetMousePosition", &Input::GetMousePosition)
            .addFunction("GetMouseButton", &Input::GetMouseButton)
            .addFunction("GetMouseButtonDown", &Input::GetMouseButtonDown)
            .addFunction("GetMouseButtonUp", &Input::GetMouseButtonUp)
            .addFunction("GetMouseScrollDelta", &Input::GetMouseScrollDelta)
            .addFunction("HideCursor", &Input::HideCursor)
            .addFunction("ShowCursor", &Input::ShowCursor)
        .endNamespace()
        .beginNamespace("Text")
            .addFunction("Draw", &TextDB::QueueTextDraw)
        .endNamespace()
        .beginNamespace("Audio")
            .addFunction("Play", &AudioDB::PlayChannel)
            .addFunction("Halt", &AudioDB::HaltChannel)
            .addFunction("SetVolume", &AudioDB::SetVolume)
        .endNamespace()
        .beginNamespace("Image")
            .addFunction("DrawUI", &ImageDB::QueueImageDrawUI)
            .addFunction("DrawUIEx", &ImageDB::QueueImageDrawUIEx)
            .addFunction("Draw", &ImageDB::QueueImageDraw)
            .addFunction("DrawEx", &ImageDB::QueueImageDrawEx)
            .addFunction("DrawPixel", &ImageDB::QueueDrawPixel)
            .addFunction("DrawRect", &ImageDB::QueueDrawRect)
        .endNamespace()
        .beginNamespace("Camera")
            .addFunction("SetPosition", static_cast<void (*)(float, float)>(&Renderer::SetCameraPosition))
            .addFunction("GetPositionX", &Renderer::GetCameraPositionX)
            .addFunction("GetPositionY", &Renderer::GetCameraPositionY)
            .addFunction("SetZoom", &Renderer::SetCameraZoomFactor)
            .addFunction("GetZoom", &Renderer::GetCameraZoomFactor)
            .addFunction("Follow", &Renderer::Follow)
            .addFunction("StopFollow", &Renderer::StopFollow)
            .addFunction("Shake", &Renderer::Shake)
            .addFunction("SetBounds", &Renderer::SetBounds)
            .addFunction("ClearBounds", &Renderer::ClearBounds)
        .endNamespace()
        .beginNamespace("Scene")
            .addFunction("Load", &SceneDB::Load)
            .addFunction("GetCurrent", &SceneDB::GetCurrent)
            .addFunction("DontDestroy", &SceneDB::DontDestroy)
            .addFunction("LoadWithTransition", &SceneTransition::StartTransition)
        .endNamespace()

        // TIME API
        .beginNamespace("Time")
            .addFunction("GetDeltaTime", &Time::GetDeltaTime)
            .addFunction("GetUnscaledDeltaTime", &Time::GetUnscaledDeltaTime)
            .addFunction("GetTimeScale", &Time::GetTimeScale)
            .addFunction("SetTimeScale", &Time::SetTimeScale)
            .addFunction("GetTotalTime", &Time::GetTotalTime)
            .addFunction("GetUnscaledTotalTime", &Time::GetUnscaledTotalTime)
            .addFunction("GetFrameCount", &Time::GetFrameCount)
        .endNamespace()

        // EVENT SYSTEM
        .beginNamespace("Event")
            .addFunction("Emit", &EventSystem::Emit)
            .addFunction("Subscribe", &EventSystem::Subscribe)
            .addFunction("SubscribeOnce", &EventSystem::SubscribeOnce)
            .addFunction("Unsubscribe", &EventSystem::Unsubscribe)
            .addFunction("UnsubscribeAll", &EventSystem::UnsubscribeAll)
        .endNamespace()

        // TIMER/SCHEDULER
        .beginNamespace("Timer")
            .addFunction("After", &Scheduler::After)
            .addFunction("Every", &Scheduler::Every)
            .addFunction("Cancel", &Scheduler::Cancel)
            .addFunction("CancelAll", &Scheduler::CancelAll)
        .endNamespace()

        // TWEEN SYSTEM
        .beginNamespace("Tween")
            .addFunction("To", &Tween::To)
            .addFunction("Cancel", &Tween::Cancel)
            .addFunction("CancelAll", &Tween::CancelAll)
        .endNamespace()

        // TRANSFORM COMPONENT
        .beginClass<Transform>("Transform")
            .addConstructor<void (*) (void)>()
            .addProperty("x", &Transform::x)
            .addProperty("y", &Transform::y)
            .addProperty("rotation", &Transform::rotation)
            .addProperty("scale_x", &Transform::scale_x)
            .addProperty("scale_y", &Transform::scale_y)
            .addFunction("GetPosition", &Transform::GetPosition)
            .addFunction("SetPosition", &Transform::SetPosition)
            .addFunction("Translate", &Transform::Translate)
            .addFunction("GetRotation", &Transform::GetRotation)
            .addFunction("SetRotation", &Transform::SetRotation)
            .addFunction("Rotate", &Transform::Rotate)
            .addFunction("GetScale", &Transform::GetScale)
            .addFunction("SetScale", &Transform::SetScale)
            .addFunction("SetUniformScale", &Transform::SetUniformScale)
            .addFunction("GetUpDirection", &Transform::GetUpDirection)
            .addFunction("GetRightDirection", &Transform::GetRightDirection)
        .endClass()

        // ANIMATION SYSTEM
        .beginNamespace("Animation")
            .addFunction("Define", &AnimationDB::DefineAnimation)
            .addFunction("Play", &AnimationDB::Play)
            .addFunction("Stop", &AnimationDB::Stop)
            .addFunction("SetFrame", &AnimationDB::SetFrame)
            .addFunction("IsPlaying", &AnimationDB::IsPlaying)
            .addFunction("GetCurrentFrame", &AnimationDB::GetCurrentFrame)
            .addFunction("GetCurrentAnimation", &AnimationDB::GetCurrentAnimation)
        .endNamespace()

        // PARTICLE SYSTEM
        .beginClass<ParticleConfig>("ParticleConfig")
            .addConstructor<void (*) (void)>()
            .addProperty("lifetime_min", &ParticleConfig::lifetime_min)
            .addProperty("lifetime_max", &ParticleConfig::lifetime_max)
            .addProperty("speed_min", &ParticleConfig::speed_min)
            .addProperty("speed_max", &ParticleConfig::speed_max)
            .addProperty("spread_angle", &ParticleConfig::spread_angle)
            .addProperty("direction", &ParticleConfig::direction)
            .addProperty("gravity", &ParticleConfig::gravity)
            .addProperty("start_size", &ParticleConfig::start_size)
            .addProperty("end_size", &ParticleConfig::end_size)
            .addProperty("image_name", &ParticleConfig::image_name)
            .addProperty("sorting_order", &ParticleConfig::sorting_order)
            .addFunction("SetStartColor", &ParticleConfig::SetStartColor)
            .addFunction("SetEndColor", &ParticleConfig::SetEndColor)
        .endClass()
        .beginNamespace("Particles")
            .addFunction("Emit", &ParticleSystem::Emit)
            .addFunction("GetActiveCount", &ParticleSystem::GetActiveCount)
        .endNamespace()

        // COLLISION LAYERS
        .beginNamespace("CollisionLayers")
            .addFunction("DefineLayer", &CollisionLayers::DefineLayer)
            .addFunction("SetLayerCollision", &CollisionLayers::SetLayerCollision)
            .addFunction("DoLayersCollide", &CollisionLayers::DoLayersCollide)
        .endNamespace()

        // DEBUG OVERLAY
        .beginNamespace("DebugOverlay")
            .addFunction("Toggle", &DebugDraw::ToggleEnabled)
            .addFunction("SetEnabled", &DebugDraw::SetEnabled)
            .addFunction("IsEnabled", &DebugDraw::IsEnabled)
        .endNamespace();

    componentTypeCache.reserve(50);
}

void ComponentDB::Shutdown() {
    // Caches of LuaRef must be released BEFORE lua_close, otherwise their
    // destructors call luaL_unref on a closed state (crash).
    componentTypeCache.clear();
    runtime_comp_add = 0;
    if (L) {
        lua_close(L);
        L = nullptr;
    }
}

void ComponentDB::loadComponents(Actor * a, const rapidjson::Value &doc) {
    size_t num_components = doc.MemberCount();
    if (num_components > 0) {
        a->components.reserve(num_components);
    }
    
    for (auto it = doc.MemberBegin(); it != doc.MemberEnd(); ++it) {
        std::string comp_key = it->name.GetString();
        const rapidjson::Value& comp_data = it->value;
        
        bool rigidbody = false;
        
        if (a->components.find(comp_key) != a->components.end()) {
            luabridge::LuaRef& existing_table = *(a->components[comp_key]);

            // Determine component type: use "type" field if present, otherwise use the key name
            std::string comp_type = comp_key;
            if (comp_data.HasMember("type") && comp_data["type"].IsString()) {
                comp_type = comp_data["type"].GetString();
            }

            bool position_overridden = false;
            bool dimensions_overridden = false;
            for (auto it_2 = comp_data.MemberBegin(); it_2 != comp_data.MemberEnd(); ++it_2) {
                std::string prop = it_2->name.GetString();
                if (prop == "type")
                    continue;

                if (comp_type == "Rigidbody") {
                    overrideRigidbodyfValue(existing_table, prop, it_2->value);
                    if (prop == "x" || prop == "y") {
                        position_overridden = true;
                    }
                    if (prop == "width" || prop == "height" || prop == "radius" ||
                        prop == "trigger_width" || prop == "trigger_height" || prop == "trigger_radius") {
                        dimensions_overridden = true;
                    }
                }
                else
                    overrideLuaRefValue(existing_table, prop, it_2->value);
            }

            // If rigidbody was overridden, update the physics body
            if (comp_type == "Rigidbody") {
                Rigidbody* rb = existing_table.cast<Rigidbody*>();
                if (rb) {
                    if (dimensions_overridden) {
                        rb->RecreateFixtures(a);
                    }
                    if (position_overridden) {
                        rb->SetPosition(b2Vec2(rb->x, rb->y));
                    }
                }
            }
        }
        else {
            // Use "type" field if present, otherwise use the key name as component type
            std::string comp_type = comp_key;
            if (comp_data.HasMember("type") && comp_data["type"].IsString()) {
                comp_type = comp_data["type"].GetString();
            }
            if (comp_type == "Rigidbody") rigidbody = true;
            
            std::shared_ptr<luabridge::LuaRef> component_ref = CreateComponent(comp_type, comp_key);
            
            for (auto it_2 = comp_data.MemberBegin(); it_2 != comp_data.MemberEnd(); ++it_2) {
                std::string prop_name = it_2->name.GetString();
                if (prop_name == "type") continue;
                if (rigidbody)
                    overrideRigidbodyfValue(*component_ref, prop_name, it_2->value);
                else
                    overrideLuaRefValue(*component_ref, prop_name, it_2->value);
            }
            
            if (rigidbody) {
                Rigidbody* rb = (*component_ref).cast<Rigidbody*>();
                rb->Init(a);
            }
            a->components[comp_key] = component_ref;
            a->component_keys.insert(comp_key);
            a->InjectReference(component_ref);
        }
    }
}

std::shared_ptr<luabridge::LuaRef> ComponentDB::CreateComponent(const std::string& type, const std::string& comp_key) {
    if (type == "Rigidbody") {
        Rigidbody* new_component = nullptr;
        
        if (componentTypeCache.find(type) != componentTypeCache.end()) {
            luabridge::LuaRef& component_type_instance = *componentTypeCache[type];
            Rigidbody* parent_component = component_type_instance.cast<Rigidbody*>();
            new_component = new Rigidbody(*parent_component);
        }
        
        if (new_component == nullptr) {
            new_component = new Rigidbody();
        }
        
        luabridge::LuaRef ref(L, new_component);
                
        return std::make_shared<luabridge::LuaRef>(ref);
    }
    
    if (componentTypeCache.find(type) == componentTypeCache.end()) {
        std::string lua_path = ConfigManager::GetResourcesPath() + "component_types/" + type + ".lua";
        if (!std::filesystem::exists(lua_path)) {
            LOG_FATAL("Component type missing: " + type);
            throw ScriptException("Component type not found: " + type);
        }

        if (luaL_dofile(L, lua_path.c_str()) != LUA_OK) {
            std::string error = lua_tostring(L, -1);
            LOG_FATAL("Lua error in component " + type + ": " + error);
            throw ScriptException("Lua error in component " + type + ": " + error);
        }

        componentTypeCache[type] = std::make_shared<luabridge::LuaRef>(luabridge::getGlobal(L, type.c_str()));
    }
    
    luabridge::LuaRef instance_table = luabridge::newTable(L);
    EstablishInheritance(instance_table, *componentTypeCache[type]);
    
    instance_table["key"] = comp_key;
    instance_table["type"] = type;
    instance_table["enabled"] = true;
    instance_table["on_start"] = false;

    if (instance_table["OnStart"].isFunction())
        SceneDB::onstart_new = true;
    
    return std::make_shared<luabridge::LuaRef>(instance_table);
}

void ComponentDB::CPPLog(const std::string& message) {
    LOG_INFO(message);
}

void ComponentDB::CPPLogError(const std::string& message) {
    LOG_ERROR(message);
}

void ComponentDB::overrideLuaRefValue(luabridge::LuaRef& table, const std::string & name, const rapidjson::Value& prop_value) {
    if (prop_value.IsBool())
        table[name] = prop_value.GetBool();
    else if (prop_value.IsString())
        table[name] = prop_value.GetString();
    else if (prop_value.IsInt())
        table[name] = prop_value.GetInt();
    else if (prop_value.IsFloat())
        table[name] = prop_value.GetFloat();
}

namespace {
    const std::unordered_map<std::string, float Rigidbody::*> kRigidbodyFloatFields = {
        {"x", &Rigidbody::x},
        {"y", &Rigidbody::y},
        {"width", &Rigidbody::width},
        {"height", &Rigidbody::height},
        {"radius", &Rigidbody::radius},
        {"friction", &Rigidbody::friction},
        {"bounciness", &Rigidbody::bounciness},
        {"gravity_scale", &Rigidbody::gravity_scale},
        {"density", &Rigidbody::density},
        {"angular_friction", &Rigidbody::angular_friction},
        {"rotation", &Rigidbody::rotation},
        {"trigger_width", &Rigidbody::trigger_width},
        {"trigger_height", &Rigidbody::trigger_height},
        {"trigger_radius", &Rigidbody::trigger_radius},
    };
    const std::unordered_map<std::string, std::string Rigidbody::*> kRigidbodyStringFields = {
        {"body_type", &Rigidbody::body_type},
        {"collider_type", &Rigidbody::collider_type},
        {"trigger_type", &Rigidbody::trigger_type},
        {"collision_layer", &Rigidbody::collision_layer},
    };
    const std::unordered_map<std::string, bool Rigidbody::*> kRigidbodyBoolFields = {
        {"precise", &Rigidbody::precise},
        {"has_collider", &Rigidbody::has_collider},
        {"has_trigger", &Rigidbody::has_trigger},
    };
}

void ComponentDB::overrideRigidbodyfValue(luabridge::LuaRef& table, const std::string& name, const rapidjson::Value& prop_value) {
    Rigidbody* rb = table.cast<Rigidbody*>();
    if (!rb) return;

    if (auto it = kRigidbodyFloatFields.find(name); it != kRigidbodyFloatFields.end()) {
        if (prop_value.IsNumber()) rb->*(it->second) = prop_value.GetFloat();
        else LOG_WARNING("Rigidbody property '" + name + "' expects a number, skipping");
        return;
    }
    if (auto it = kRigidbodyStringFields.find(name); it != kRigidbodyStringFields.end()) {
        if (prop_value.IsString()) rb->*(it->second) = prop_value.GetString();
        else LOG_WARNING("Rigidbody property '" + name + "' expects a string, skipping");
        return;
    }
    if (auto it = kRigidbodyBoolFields.find(name); it != kRigidbodyBoolFields.end()) {
        if (prop_value.IsBool()) rb->*(it->second) = prop_value.GetBool();
        else LOG_WARNING("Rigidbody property '" + name + "' expects a bool, skipping");
        return;
    }
}
void ComponentDB::EstablishInheritance(luabridge::LuaRef& instance_table, luabridge::LuaRef& parent_table) {
    luabridge::LuaRef new_metatable = luabridge::newTable(L);
    new_metatable["__index"] = parent_table;

    instance_table.push(L);
    new_metatable.push(L);
    lua_setmetatable(L, -2);
    lua_pop(L, 1);
}
