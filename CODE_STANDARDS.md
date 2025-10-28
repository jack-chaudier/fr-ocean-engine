# FR-Ocean Engine Code Standards

This document outlines the coding standards and conventions used in FR-Ocean Engine.

## Table of Contents
- [General Principles](#general-principles)
- [C++ Language Features](#c-language-features)
- [Naming Conventions](#naming-conventions)
- [Code Formatting](#code-formatting)
- [Documentation](#documentation)
- [Error Handling](#error-handling)
- [Performance Guidelines](#performance-guidelines)
- [Platform Considerations](#platform-considerations)

## General Principles

### Code Quality

1. **Readability First**: Code is read more often than written
2. **KISS**: Keep It Simple, Stupid - avoid unnecessary complexity
3. **DRY**: Don't Repeat Yourself - abstract common patterns
4. **YAGNI**: You Aren't Gonna Need It - don't over-engineer
5. **RAII**: Resource Acquisition Is Initialization - manage resources via constructors/destructors

### Design Principles

- **Single Responsibility**: Each class should have one clear purpose
- **Open/Closed**: Open for extension, closed for modification
- **Dependency Inversion**: Depend on abstractions, not concretions
- **Composition Over Inheritance**: Prefer component-based design

## C++ Language Features

### Language Standard

- **C++17** is the minimum required standard
- Prefer modern C++ features over C-style alternatives
- Avoid compiler-specific extensions when possible

### Allowed Features

**Encouraged**:
- `auto` for type deduction (when type is obvious)
- Range-based `for` loops
- `nullptr` instead of `NULL`
- `std::unique_ptr` and `std::shared_ptr`
- `constexpr` for compile-time constants
- `enum class` instead of `enum`
- Structured bindings (C++17)
- `std::optional` for optional values

**Use with Caution**:
- Templates (prefer simple, well-documented templates)
- Operator overloading (only when natural and intuitive)
- Multiple inheritance (prefer composition)
- Exceptions (currently disabled in engine)

**Avoid**:
- Raw `new`/`delete` (use smart pointers)
- C-style casts (use `static_cast`, `reinterpret_cast`)
- `goto` statements
- `#define` for constants (use `constexpr`)
- `using namespace` in headers

### Example

```cpp
// Good ✓
class ImageDB {
public:
    static SDL_Texture* GetTexture(const std::string& imageName);

private:
    inline static std::unordered_map<std::string, SDL_Texture*> textureMap;
};

// Bad ✗
class ImageDB {
public:
    static SDL_Texture* get_texture(char* image_name);  // No const, C-string

private:
    static map<string, SDL_Texture*> texture_map;  // using namespace std
};
```

## Naming Conventions

### Classes and Structs

- **PascalCase** for class and struct names
- Descriptive names that clearly indicate purpose

```cpp
class Engine { };
class RigidbodyWorld { };
struct ImageDrawRequest { };
```

### Functions and Methods

- **PascalCase** for public methods
- **snake_case** for private helper functions (optional)
- Verbs for actions (Get, Set, Add, Remove, Create, Destroy)

```cpp
class Actor {
public:
    std::string GetName();
    void AddComponent(const std::string& type);

private:
    void processComponents();  // Private helper
};
```

### Variables

- **snake_case** for local variables and parameters
- **snake_case** for member variables
- No Hungarian notation

```cpp
class SceneDB {
private:
    std::vector<uint64_t> actor_id_vec;         // Member variable
    std::string current_scene_name;             // Member variable
    inline static uint64_t id_ctr = 0;          // Static member
};

void ProcessEvent(const SDL_Event& event) {
    int button_id = event.button.button;        // Local variable
    glm::vec2 mouse_pos = GetMousePosition();   // Local variable
}
```

### Constants

- **UPPER_SNAKE_CASE** for constants
- Use `constexpr` or `const` instead of `#define`

```cpp
constexpr int MAX_ACTORS = 1000;
constexpr float DEFAULT_GRAVITY = 9.8f;

const std::string SCENE_PATH = "resources/scenes/";
```

### Enums

- **PascalCase** for enum class names
- **UPPER_SNAKE_CASE** for enum values

```cpp
enum class INPUT_STATE {
    INPUT_STATE_UP,
    INPUT_STATE_JUST_BECAME_DOWN,
    INPUT_STATE_DOWN,
    INPUT_STATE_JUST_BECAME_UP
};
```

### Files

- Match class name: `ImageDB.hpp` / `ImageDB.cpp`
- Use `.hpp` for C++ headers, `.cpp` for sources
- Use `.h` only for C headers

## Code Formatting

### Indentation

- **4 spaces** for indentation (no tabs)
- Configure your editor to convert tabs to spaces

### Braces

- **K&R style** for braces (opening brace on same line)
- Always use braces for control structures, even single-line

```cpp
// Good ✓
if (condition) {
    DoSomething();
}

for (int i = 0; i < 10; i++) {
    Process(i);
}

// Bad ✗
if (condition)
    DoSomething();  // Missing braces

if (condition) { DoSomething(); }  // One-liner with braces
```

### Line Length

- **Aim for 100 characters** per line
- Break long lines for readability

```cpp
// Good ✓
static void QueueImageDrawEx(const std::string& imageName, float x, float y,
                      float rotationDegrees, float scaleX, float scaleY,
                      float pivotX, float pivotY,
                      float r, float g, float b, float a,
                      float sortingOrder);

// Okay (if unavoidable)
static void QueueImageDrawEx(const std::string& imageName, float x, float y, float rotationDegrees, float scaleX, float scaleY, float pivotX, float pivotY, float r, float g, float b, float a, float sortingOrder);  // 237 chars
```

### Spacing

- Space after keywords: `if (`, `for (`, `while (`
- No space before function call parentheses: `Function(arg)`
- Space around operators: `a + b`, `x = 5`
- No space inside parentheses: `(value)` not `( value )`

```cpp
// Good ✓
if (x > 0) {
    int result = Calculate(x, y);
    value = result + 10;
}

// Bad ✗
if(x>0){
    int result=Calculate( x , y );
    value=result+10;
}
```

### Pointer and Reference Alignment

- Attach `*` and `&` to the type, not the variable

```cpp
// Good ✓
SDL_Texture* texture;
const std::string& name;
Actor* InstantiateActor(const std::string& temp);

// Bad ✗
SDL_Texture *texture;
const std::string &name;
Actor *InstantiateActor(const std::string &temp);
```

### Header Organization

```cpp
// 1. Header comment
//
//  ClassName.hpp
//  FR-Ocean Engine
//
//  Brief description of what this file does.
//
//  Created by Author on Date.
//

// 2. Include guard
#ifndef ClassName_hpp
#define ClassName_hpp

// 3. System includes
#include <string>
#include <vector>

// 4. Third-party includes
#include "SDL2/SDL.h"
#include "glm/glm.hpp"

// 5. Project includes
#include "OtherClass.hpp"

// 6. Forward declarations
class ForwardDeclaredClass;

// 7. Class definition
class ClassName {
public:
    // Public interface

private:
    // Private members
};

#endif /* ClassName_hpp */
```

## Documentation

### Public API Documentation

All public APIs must have Doxygen-style documentation:

```cpp
/**
 * @brief Brief one-line description.
 *
 * Detailed description of what this function does, how it works,
 * and any important notes or caveats.
 *
 * @param param1 Description of first parameter
 * @param param2 Description of second parameter
 * @return Description of return value
 *
 * @throws std::runtime_error if something fails
 *
 * @note Additional notes or warnings
 * @see RelatedFunction() for related functionality
 *
 * @example
 * ```cpp
 * ClassName obj;
 * obj.Function(arg1, arg2);
 * ```
 */
ReturnType Function(Type1 param1, Type2 param2);
```

### Inline Comments

- Explain **why**, not **what** (the code explains what)
- Use `//` for single-line comments
- Use `/* */` for multi-line comments
- Comment complex algorithms and non-obvious logic

```cpp
// Good ✓ - Explains WHY
// Sort by z-order first, then submission order for stable sorting
std::stable_sort(queue.begin(), queue.end(), ...);

// Bad ✗ - States the obvious
// Sort the queue
std::stable_sort(queue.begin(), queue.end(), ...);
```

### TODO Comments

```cpp
// TODO(author): Description of what needs to be done
// FIXME(author): Description of what needs fixing
// HACK(author): Description of workaround
```

## Error Handling

### Assertions

- Use `assert()` for debug-only checks
- Use static assertions for compile-time checks

```cpp
#include <cassert>

void Process(Actor* actor) {
    assert(actor != nullptr && "Actor must not be null");
    // ...
}

static_assert(sizeof(int) == 4, "int must be 4 bytes");
```

### Error Reporting

- Log errors to console with `std::cerr`
- Use `Debug.LogError()` for Lua-visible errors
- Fail gracefully when possible (don't crash)

```cpp
if (!file.is_open()) {
    std::cerr << "Error: Failed to open file: " << filename << std::endl;
    return false;  // or return default value
}
```

### Exceptions

- Exceptions are currently **disabled** in the engine
- Use return codes or `std::optional` for error cases
- Document error conditions in function comments

```cpp
// Returns texture or nullptr if loading fails
static SDL_Texture* GetTexture(const std::string& imageName);

// Returns optional actor (nullopt if not found)
static std::optional<Actor*> FindActor(const std::string& name);
```

## Performance Guidelines

### Memory Management

- Use **RAII** for automatic resource cleanup
- Prefer `std::unique_ptr` for ownership
- Use `std::shared_ptr` sparingly (only when shared ownership needed)
- Avoid unnecessary copies (use `const&` for parameters)

```cpp
// Good ✓
std::unique_ptr<Actor> actor = std::make_unique<Actor>();

void Process(const std::string& name) {  // Pass by const reference
    // ...
}

// Bad ✗
Actor* actor = new Actor();  // Manual memory management

void Process(std::string name) {  // Unnecessary copy
    // ...
}
```

### Optimization

- **Premature optimization is the root of all evil** - profile first!
- Use `inline` for small, frequently called functions
- Prefer `reserve()` for vectors with known size
- Use `emplace_back()` instead of `push_back()` for complex types
- Cache function results when appropriate

```cpp
// Reserve capacity to avoid reallocations
std::vector<uint64_t> actor_ids;
actor_ids.reserve(1000);

// Emplace instead of push_back (avoids copy)
actors.emplace_back(std::make_unique<Actor>());

// Cache expensive lookups
auto cached_component = actor->GetComponent("Rigidbody");
// Use cached_component instead of repeated GetComponent calls
```

### Algorithmic Complexity

- **O(1)** for critical path operations
- **O(n)** acceptable for per-frame iteration
- **O(n log n)** okay for sorting (once per frame)
- **O(n²)** avoid in hot paths (consider spatial partitioning)

```cpp
// Good ✓ - O(1) lookup with unordered_map
std::unordered_map<std::string, SDL_Texture*> textureCache;
auto it = textureCache.find(name);

// Bad ✗ - O(n) lookup with vector
std::vector<std::pair<std::string, SDL_Texture*>> textureCache;
auto it = std::find_if(textureCache.begin(), textureCache.end(), ...);
```

## Platform Considerations

### Cross-Platform Code

- Use SDL2 for cross-platform functionality (window, events, graphics)
- Avoid platform-specific APIs when possible
- Use `#ifdef` sparingly, only when necessary

```cpp
#ifdef _WIN32
    #include <windows.h>
    // Windows-specific code
#elif defined(__APPLE__)
    #include <TargetConditionals.h>
    // macOS-specific code
#elif defined(__linux__)
    // Linux-specific code
#endif
```

### File Paths

- Use forward slashes `/` in paths (works on all platforms)
- Use `std::filesystem` for path manipulation (C++17)

```cpp
#include <filesystem>

namespace fs = std::filesystem;
fs::path scene_path = fs::path("resources") / "scenes" / "level1.scene";
```

### Endianness

- Be aware of endianness when reading/writing binary files
- Use network byte order for serialization

### Compiler Warnings

- Build with warnings enabled (`-Wall -Wextra` or `/W4`)
- Treat warnings as errors (`-Werror` or `/WX`)
- Fix all warnings before committing

```cmake
# CMake example
if(MSVC)
    add_compile_options(/W4 /WX)
else()
    add_compile_options(-Wall -Wextra -Werror)
endif()
```

## Lua Scripting Standards

### Lua Naming Conventions

- **PascalCase** for component tables
- **snake_case** for local variables
- **PascalCase** for lifecycle methods (OnStart, OnUpdate)

```lua
PlayerController = {
    speed = 5.0,
    is_grounded = false
}

function PlayerController:OnStart()
    local rigidbody = self.actor:GetComponent("Rigidbody")
end
```

### Lua Code Style

- Use 4 spaces for indentation
- Always use `local` for variables (avoid global pollution)
- Comment complex logic

```lua
-- Good ✓
function Component:OnUpdate()
    local velocity = self.rigidbody:GetVelocity()

    if Input.GetKey("left") then
        velocity.x = -self.speed
    end

    self.rigidbody:SetVelocity(velocity)
end

-- Bad ✗
function Component:OnUpdate()
velocity=self.rigidbody:GetVelocity() -- No local, no spacing
if Input.GetKey("left") then velocity.x=-self.speed end -- One-liner
self.rigidbody:SetVelocity(velocity)
end
```

## Tools and IDE Configuration

### Recommended Tools

- **IDE**: Visual Studio Code, Visual Studio, CLion, Xcode
- **Formatter**: clang-format (config provided in repo)
- **Linter**: clang-tidy
- **Debugger**: GDB (Linux), LLDB (macOS), MSVC Debugger (Windows)
- **Profiler**: Valgrind, Instruments, Visual Studio Profiler

### clang-format Configuration

Save as `.clang-format` in project root:

```yaml
BasedOnStyle: Google
IndentWidth: 4
ColumnLimit: 100
PointerAlignment: Left
```

## Code Review Checklist

Before submitting code, verify:

- [ ] Code follows naming conventions
- [ ] All public APIs are documented
- [ ] No compiler warnings
- [ ] No memory leaks (checked with Valgrind or similar)
- [ ] Code is readable and self-documenting
- [ ] Complex logic has explanatory comments
- [ ] Performance is acceptable (profile if unsure)
- [ ] Works on all supported platforms
- [ ] Follows RAII principles
- [ ] Uses appropriate data structures (complexity)

## Additional Resources

- [C++ Core Guidelines](https://isocpp.github.io/CppCoreGuidelines/CppCoreGuidelines)
- [Google C++ Style Guide](https://google.github.io/styleguide/cppguide.html)
- [Effective Modern C++](https://www.oreilly.com/library/view/effective-modern-c/9781491908419/)

---

**Last Updated**: 2025-10-28
**Engine Version**: 1.0
**Author**: Jack Gaffney
