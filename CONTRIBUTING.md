# Contributing to FR-Ocean Engine

Thank you for your interest in contributing to FR-Ocean Engine! This document provides guidelines and instructions for contributing to the project.

## Table of Contents
- [Code of Conduct](#code-of-conduct)
- [How to Contribute](#how-to-contribute)
- [Development Setup](#development-setup)
- [Coding Standards](#coding-standards)
- [Pull Request Process](#pull-request-process)
- [Reporting Bugs](#reporting-bugs)
- [Requesting Features](#requesting-features)

## Code of Conduct

### Our Pledge

We are committed to providing a welcoming and inspiring community for all. Please be respectful and constructive in all interactions.

### Expected Behavior

- Be respectful and inclusive
- Accept constructive criticism gracefully
- Focus on what is best for the community and the project
- Show empathy towards other community members

### Unacceptable Behavior

- Trolling, insulting comments, or personal attacks
- Public or private harassment
- Publishing others' private information without permission
- Other conduct which could reasonably be considered inappropriate

## How to Contribute

### Types of Contributions

We welcome many types of contributions:

1. **Bug Reports**: Help us identify and fix bugs
2. **Feature Requests**: Suggest new features or enhancements
3. **Code Contributions**: Submit bug fixes or new features
4. **Documentation**: Improve or add documentation
5. **Examples**: Create example games or tutorials
6. **Testing**: Test the engine and report issues

### Getting Started

1. Fork the repository on GitHub
2. Clone your fork locally:
   ```bash
   git clone https://github.com/YOUR_USERNAME/fr-ocean-engine.git
   cd fr-ocean-engine
   ```
3. Create a new branch for your changes:
   ```bash
   git checkout -b feature/your-feature-name
   ```
4. Make your changes and commit them
5. Push to your fork and submit a pull request

## Development Setup

### Prerequisites

**All Platforms**:
- CMake 3.16 or higher
- Git

**Windows**:
- Visual Studio 2019 or higher with C++ desktop development
- Windows SDK

**macOS**:
- Xcode 12 or higher
- Xcode Command Line Tools

**Linux**:
- GCC 9+ or Clang 9+
- Development packages:
  ```bash
  sudo apt install build-essential cmake libsdl2-dev libsdl2-image-dev \
                   libsdl2-mixer-dev libsdl2-ttf-dev liblua5.4-dev
  ```

### Building the Engine

1. **Generate build files**:
   ```bash
   mkdir build
   cd build
   cmake ..
   ```

2. **Compile**:
   ```bash
   cmake --build . --config Release
   ```

3. **Run tests** (if available):
   ```bash
   ctest --config Release
   ```

### Project Structure

```
fr-ocean-engine/
├── game_engine/          # Core engine source code (.cpp/.hpp)
├── vendor/               # Third-party dependencies
│   ├── SDL2/            # SDL2 framework/libraries
│   ├── box2d/           # Box2D physics engine
│   ├── lua/             # Lua 5.4 interpreter
│   ├── LuaBridge/       # Lua-C++ binding library
│   ├── glm/             # Math library
│   └── rapidjson/       # JSON parser
├── resources/           # Game assets (not in repo)
│   ├── images/
│   ├── fonts/
│   ├── audio/
│   ├── scenes/
│   └── component_types/
├── docs/                # Additional documentation
├── CMakeLists.txt       # CMake build configuration
└── README.md
```

## Coding Standards

Please follow the coding standards outlined in [CODE_STANDARDS.md](CODE_STANDARDS.md).

### Quick Summary

- **Style**: Follow existing code style (similar to Google C++ Style Guide)
- **Naming**:
  - Classes: `PascalCase` (e.g., `ImageDB`)
  - Functions: `PascalCase` (e.g., `GetPosition()`)
  - Variables: `snake_case` (e.g., `actor_id`)
  - Constants: `UPPER_SNAKE_CASE` (e.g., `MAX_ACTORS`)
- **Comments**: Use Doxygen-style documentation for all public APIs
- **Language**: C++17 standard

### Code Quality

- Write clear, self-documenting code
- Add comments for complex logic
- Follow RAII principles for resource management
- Prefer `std::unique_ptr` and `std::shared_ptr` over raw pointers
- Use `const` where appropriate
- Avoid `using namespace` in headers

## Pull Request Process

### Before Submitting

1. **Ensure code builds** on your platform without warnings
2. **Follow coding standards** outlined in CODE_STANDARDS.md
3. **Add tests** if applicable (unit tests, integration tests)
4. **Update documentation** (README, API docs, architecture docs)
5. **Check for memory leaks** (use Valgrind on Linux, Instruments on macOS)

### PR Guidelines

1. **One feature per PR**: Keep PRs focused on a single feature or bug fix
2. **Descriptive title**: Use a clear, concise title (e.g., "Add particle system", "Fix memory leak in ImageDB")
3. **Detailed description**:
   - What changes were made
   - Why these changes were necessary
   - How to test the changes
   - Screenshots/videos if UI changes
4. **Link issues**: Reference related issues (e.g., "Fixes #123")
5. **Commit messages**: Use clear, descriptive commit messages

### PR Template

```markdown
## Description
Brief description of changes

## Motivation
Why is this change needed?

## Changes Made
- Change 1
- Change 2
- Change 3

## Testing
How to test these changes:
1. Step 1
2. Step 2
3. Expected result

## Screenshots (if applicable)
Add screenshots or videos

## Checklist
- [ ] Code builds without warnings
- [ ] Follows coding standards
- [ ] Documentation updated
- [ ] Tests added (if applicable)
- [ ] No memory leaks
```

### Review Process

1. Maintainers will review your PR within 1-2 weeks
2. Address any feedback or requested changes
3. Once approved, maintainers will merge your PR

## Reporting Bugs

### Before Reporting

1. **Check existing issues**: Search to see if the bug has already been reported
2. **Verify on latest version**: Ensure you're using the latest version
3. **Minimal reproduction**: Create the smallest possible example that reproduces the bug

### Bug Report Template

```markdown
**Describe the bug**
A clear description of what the bug is.

**To Reproduce**
Steps to reproduce:
1. Go to '...'
2. Click on '...'
3. See error

**Expected behavior**
What you expected to happen.

**Actual behavior**
What actually happened.

**Screenshots/Error Messages**
Add screenshots or paste error messages.

**Environment:**
- OS: [e.g., Windows 11, macOS 14, Ubuntu 22.04]
- Engine Version: [e.g., 1.0]
- Compiler: [e.g., MSVC 2022, GCC 11]
- Build Type: [e.g., Debug, Release]

**Additional context**
Any other relevant information.
```

## Requesting Features

### Feature Request Template

```markdown
**Is your feature request related to a problem?**
A clear description of the problem.

**Describe the solution you'd like**
What you want to happen.

**Describe alternatives you've considered**
Other solutions you've thought about.

**Use case**
How would this feature be used?

**Example code/API**
Example of how the feature might be used:
```lua
-- Example Lua code
```

**Additional context**
Screenshots, mockups, or other relevant information.
```

## Areas for Contribution

### High Priority

- **Unit Tests**: Add test coverage for engine systems
- **Particle System**: Implement GPU-accelerated particle effects
- **Tilemap Support**: Add tilemap rendering and collision
- **Animation System**: Sprite sheet animation with keyframes
- **Documentation**: Improve API docs, add tutorials
- **Examples**: Create demo games showcasing engine features

### Good First Issues

Look for issues labeled `good-first-issue` on GitHub. These are beginner-friendly tasks perfect for first-time contributors.

### Platform Support

- **Linux ARM**: Test and fix issues on ARM Linux (Raspberry Pi)
- **Web (Emscripten)**: Port engine to WebAssembly
- **Mobile**: Explore Android/iOS support

## Development Workflow

### Branching Strategy

- `main`: Stable, production-ready code
- `develop`: Integration branch for features
- `feature/*`: Feature branches (e.g., `feature/particle-system`)
- `bugfix/*`: Bug fix branches (e.g., `bugfix/memory-leak`)
- `hotfix/*`: Urgent fixes for production (e.g., `hotfix/crash-on-quit`)

### Commit Message Format

```
<type>: <subject>

<body>

<footer>
```

**Types**:
- `feat`: New feature
- `fix`: Bug fix
- `docs`: Documentation changes
- `style`: Code style changes (formatting, no logic change)
- `refactor`: Code refactoring
- `perf`: Performance improvements
- `test`: Adding or updating tests
- `chore`: Build system, dependencies, etc.

**Example**:
```
feat: Add particle system with GPU acceleration

- Implement ParticleEmitter component
- Add particle rendering to ImageDB
- Expose particle API to Lua
- Add example particle effects

Closes #42
```

## Testing

### Running Tests

```bash
cd build
ctest --output-on-failure
```

### Writing Tests

- Place tests in `tests/` directory
- Use Google Test framework (to be added)
- Test public APIs and edge cases
- Aim for >80% code coverage

### Manual Testing

- Test on multiple platforms (Windows, macOS, Linux)
- Test with different compilers (MSVC, GCC, Clang)
- Test in Debug and Release modes
- Check for memory leaks with sanitizers

## Documentation

### Documenting Code

Use Doxygen-style comments for all public APIs:

```cpp
/**
 * @brief Brief description.
 *
 * Detailed description of what this function does.
 *
 * @param param1 Description of param1
 * @param param2 Description of param2
 * @return Description of return value
 *
 * @note Additional notes
 * @see Related function
 */
void MyFunction(int param1, float param2);
```

### Updating Documentation

When adding features, update:
- API_REFERENCE.md (for Lua APIs)
- ARCHITECTURE.md (for engine design changes)
- README.md (if user-facing changes)
- Code comments (for implementation details)

## Community

### Getting Help

- **GitHub Discussions**: Ask questions and discuss ideas
- **Discord** (coming soon): Real-time chat with the community
- **GitHub Issues**: Report bugs and request features

### Staying Updated

- **Watch the repository**: Get notifications for new releases
- **Follow the project**: Star the repo to show support
- **Join discussions**: Participate in feature discussions

## License

By contributing to FR-Ocean Engine, you agree that your contributions will be licensed under the MIT License.

## Recognition

Contributors will be acknowledged in:
- CONTRIBUTORS.md file
- Release notes
- Annual contributor highlights

Thank you for contributing to FR-Ocean Engine!

---

**Questions?** Open a GitHub Discussion or contact the maintainers.
