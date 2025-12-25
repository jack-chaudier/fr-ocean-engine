//
//  EngineException.hpp
//  FR-Ocean Engine
//
//  Exception hierarchy for engine error handling.
//  Provides specific exception types for different error categories.
//

#ifndef EngineException_hpp
#define EngineException_hpp

#include <stdexcept>
#include <string>

/**
 * @class EngineException
 * @brief Base exception class for all engine errors.
 *
 * All engine-specific exceptions derive from this class.
 * Catch this type to handle any engine error generically.
 */
class EngineException : public std::runtime_error {
public:
    explicit EngineException(const std::string& message)
        : std::runtime_error(message) {}

    virtual ~EngineException() = default;
};

/**
 * @class ConfigurationException
 * @brief Exception for configuration-related errors.
 *
 * Thrown when:
 * - Configuration files are missing or malformed
 * - Required configuration values are not present
 * - Configuration parsing fails
 */
class ConfigurationException : public EngineException {
public:
    explicit ConfigurationException(const std::string& message)
        : EngineException("Configuration error: " + message) {}
};

/**
 * @class ResourceNotFoundException
 * @brief Exception for missing resource errors.
 *
 * Thrown when a required resource (image, font, audio, etc.) cannot be found.
 */
class ResourceNotFoundException : public EngineException {
public:
    ResourceNotFoundException(const std::string& resourceType,
                              const std::string& resourceName)
        : EngineException("Resource not found: " + resourceType + " '" + resourceName + "'"),
          resource_type(resourceType),
          resource_name(resourceName) {}

    explicit ResourceNotFoundException(const std::string& message)
        : EngineException("Resource not found: " + message),
          resource_type(""),
          resource_name("") {}

    const std::string& GetResourceType() const { return resource_type; }
    const std::string& GetResourceName() const { return resource_name; }

private:
    std::string resource_type;
    std::string resource_name;
};

/**
 * @class ScriptException
 * @brief Exception for Lua scripting errors.
 *
 * Thrown when:
 * - Lua script files fail to load
 * - Lua execution errors occur
 * - Component type definitions are invalid
 */
class ScriptException : public EngineException {
public:
    explicit ScriptException(const std::string& message)
        : EngineException("Script error: " + message) {}
};

/**
 * @class RenderException
 * @brief Exception for rendering-related errors.
 *
 * Thrown when:
 * - SDL initialization fails
 * - Window creation fails
 * - Renderer creation fails
 * - Texture operations fail
 */
class RenderException : public EngineException {
public:
    explicit RenderException(const std::string& message)
        : EngineException("Render error: " + message) {}
};

/**
 * @class AudioException
 * @brief Exception for audio-related errors.
 *
 * Thrown when:
 * - SDL_mixer initialization fails
 * - Audio file loading fails
 */
class AudioException : public EngineException {
public:
    explicit AudioException(const std::string& message)
        : EngineException("Audio error: " + message) {}
};

/**
 * @class PhysicsException
 * @brief Exception for physics-related errors.
 *
 * Thrown when:
 * - Physics world initialization fails
 * - Invalid physics configuration
 */
class PhysicsException : public EngineException {
public:
    explicit PhysicsException(const std::string& message)
        : EngineException("Physics error: " + message) {}
};

#endif /* EngineException_hpp */
