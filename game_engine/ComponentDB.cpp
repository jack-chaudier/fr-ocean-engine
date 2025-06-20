//
//  ComponentDB.cpp
//  game_engine
//
//  Created by Jack Gaffney on 3/31/25.
//

#include "ComponentDB.hpp"
#include "Actor.hpp"
#include "SceneDB.hpp"
#include "ApplicationAPI.h"
#include "Input.hpp"
#include "TextDB.hpp"
#include "AudioDB.hpp"
#include "ImageDB.hpp"
#include "Rigidbody.hpp"
#include "box2d/box2d.h"
#include "PhysicsQuery.hpp"

void ComponentDB::Init() {
    using namespace luabridge;
    L = luaL_newstate();
    luaL_openlibs(L);
    getGlobalNamespace(L)
    
        // DEBUG
        .beginNamespace("Debug")
            .addFunction("Log", ComponentDB::CPPLog)
            .addFunction("LogError", ComponentDB::CPPLog)
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
        .endNamespace()
        .beginNamespace("Camera")
            .addFunction("SetPosition", static_cast<void (*)(float, float)>(&Renderer::SetCameraPosition))
            .addFunction("GetPositionX", &Renderer::GetCameraPositionX)
            .addFunction("GetPositionY", &Renderer::GetCameraPositionY)
            .addFunction("SetZoom", &Renderer::SetCameraZoomFactor)
            .addFunction("GetZoom", &Renderer::GetCameraZoomFactor)
        .endNamespace()
        .beginNamespace("Scene")
            .addFunction("Load", &SceneDB::Load)
            .addFunction("GetCurrent", &SceneDB::GetCurrent)
            .addFunction("DontDestroy", &SceneDB::DontDestroy)
        .endNamespace();
        
    CDB.reserve(1000);
    componentTypeCache.reserve(50);
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
            
            for (auto it_2 = comp_data.MemberBegin(); it_2 != comp_data.MemberEnd(); ++it_2) {
                std::string prop = it_2->name.GetString();
                if (prop == "type")
                    continue;
                
                if (comp_data["type"].GetString() == std::string("Rigidbody")) {
                    overrideRigidbodyfValue(existing_table, prop, it_2->value);
                }
                else
                    overrideLuaRefValue(existing_table, prop, it_2->value);
            }
        }
        else {
            std::string comp_type = comp_data["type"].GetString();
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
            
            CDB[comp_key] = component_ref;
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
        std::string lua_path = "resources/component_types/" + type + ".lua";
        if (!std::filesystem::exists(lua_path)) {
            std::cout << "error: failed to locate component " << type;
            exit(0);
        }
        
        if (luaL_dofile(L, lua_path.c_str()) != LUA_OK) {
            std::cout << "problem with lua file " << type;
            exit(0);
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

void ComponentDB::CPPLog(std::string message) {
    std::cout << message << std::endl;
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

void ComponentDB::overrideRigidbodyfValue(luabridge::LuaRef &table, const std::string &name, const rapidjson::Value &prop_value) {
    Rigidbody* rb = table.cast<Rigidbody*>();

    if (name == "x")
        rb->x = prop_value.GetFloat();
    else if (name == "y")
        rb->y = prop_value.GetFloat();
    else if (name == "width")
        rb->width = prop_value.GetFloat();
    else if (name == "height")
        rb->height = prop_value.GetFloat();
    else if (name == "gravity_scale")
        rb->gravity_scale = prop_value.GetFloat();
    else if (name == "density")
        rb->density = prop_value.GetFloat();
    else if (name == "angular_friction")
        rb->angular_friction = prop_value.GetFloat();
    else if (name == "rotation")
        rb->rotation = prop_value.GetFloat();
    else if (name == "body_type")
        rb->body_type = prop_value.GetString();
    else if (name == "precise")
        rb->precise = prop_value.GetBool();
    else if (name == "has_collider")
        rb->has_collider = prop_value.GetBool();
    else if (name == "has_trigger")
        rb->has_trigger = prop_value.GetBool();
    else if (name == "friction")
        rb->friction = prop_value.GetFloat();
    else if (name == "bounciness")
        rb->bounciness = prop_value.GetFloat();
    else if (name == "radius")
        rb->radius = prop_value.GetFloat();
    else if (name == "collider_type")
        rb->collider_type = prop_value.GetString();
    else if (name == "trigger_type")
        rb->trigger_type = prop_value.GetString();
    else if (name == "trigger_width")
        rb->trigger_width = prop_value.GetFloat();
    else if (name == "trigger_height")
        rb->trigger_height = prop_value.GetFloat();
    else if (name == "trigger_radius")
        rb->trigger_radius = prop_value.GetFloat();
}
void ComponentDB::EstablishInheritance(luabridge::LuaRef& instance_table, luabridge::LuaRef& parent_table) {
    luabridge::LuaRef new_metatable = luabridge::newTable(L);
    new_metatable["__index"] = parent_table;

    instance_table.push(L);
    new_metatable.push(L);
    lua_setmetatable(L, -2);
    lua_pop(L, 1);
}
