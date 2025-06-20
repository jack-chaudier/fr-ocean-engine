cmake_minimum_required(VERSION 3.16)
project(game_engine VERSION 1.0 LANGUAGES C CXX)

#—— C++ setup ——
set(CMAKE_CXX_STANDARD        17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS      OFF)

# on macOS, point at your vendored .framework bundles
if(APPLE)
  set(CMAKE_FRAMEWORK_PATH
    ${CMAKE_SOURCE_DIR}/vendor/SDL2/lib
    ${CMAKE_SOURCE_DIR}/vendor/SDL2_image/lib
    ${CMAKE_SOURCE_DIR}/vendor/SDL2_mixer/lib
    ${CMAKE_SOURCE_DIR}/vendor/SDL2_ttf/lib
    ${CMAKE_FRAMEWORK_PATH}
  )
  # Enable RPATH support
  set(CMAKE_MACOSX_RPATH ON)
endif()

# default to Release if no build type was specified
if(NOT CMAKE_BUILD_TYPE)
  set(CMAKE_BUILD_TYPE Release CACHE STRING "" FORCE)
endif()

# warnings + debug/release flags
add_compile_options(
  -Wall -Wextra
  $<$<CONFIG:Debug>:-g>
  $<$<CONFIG:Release>:-O3>
)

#—— Engine executable ——
file(GLOB_RECURSE ENGINE_SOURCES
  ${CMAKE_SOURCE_DIR}/game_engine/*.cpp
)
add_executable(${PROJECT_NAME} ${ENGINE_SOURCES})

# include engine headers + all vendored headers
target_include_directories(${PROJECT_NAME} PRIVATE
  ${CMAKE_SOURCE_DIR}/game_engine
  ${CMAKE_SOURCE_DIR}/vendor          # box2d, rapidjson, glm, etc.
  ${CMAKE_SOURCE_DIR}/vendor/LuaBridge # for LuaBridge.h
  ${CMAKE_SOURCE_DIR}/vendor/lua       # for lua.hpp
)

#—— Vendored Box2D ——
set(BOX2D_ROOT ${CMAKE_SOURCE_DIR}/vendor/box2d)
file(GLOB_RECURSE BOX2D_SOURCES
  ${BOX2D_ROOT}/src/*.cpp
)
add_library(Box2D STATIC ${BOX2D_SOURCES})
target_include_directories(Box2D PUBLIC
  ${CMAKE_SOURCE_DIR}/vendor
  ${BOX2D_ROOT}/src
)
# Ensure Box2D uses the same C++ standard
target_compile_features(Box2D PUBLIC cxx_std_17)

#—— Vendored Lua ——
file(GLOB LUA_SOURCES "${CMAKE_SOURCE_DIR}/vendor/lua/*.c")
add_library(lua_static STATIC ${LUA_SOURCES})
target_include_directories(lua_static PUBLIC
  ${CMAKE_SOURCE_DIR}/vendor/lua
)

#—— Platform‑specific SDL + Lua linkage ——
if(UNIX AND NOT APPLE)
  find_package(PkgConfig REQUIRED)
  pkg_check_modules(SDL2       REQUIRED sdl2)
  pkg_check_modules(SDL2_IMAGE REQUIRED SDL2_image)
  pkg_check_modules(SDL2_MIXER REQUIRED SDL2_mixer)
  pkg_check_modules(SDL2_TTF   REQUIRED SDL2_ttf)
  find_package(Lua 5.4 REQUIRED)

  target_include_directories(${PROJECT_NAME} PRIVATE
    ${SDL2_INCLUDE_DIRS}
    ${SDL2_IMAGE_INCLUDE_DIRS}
    ${SDL2_MIXER_INCLUDE_DIRS}
    ${SDL2_TTF_INCLUDE_DIRS}
    ${LUA_INCLUDE_DIR}
  )
  target_link_directories(${PROJECT_NAME} PRIVATE
    ${SDL2_LIBRARY_DIRS}
    ${SDL2_IMAGE_LIBRARY_DIRS}
    ${SDL2_MIXER_LIBRARY_DIRS}
    ${SDL2_TTF_LIBRARY_DIRS}
  )
  # Link the Box2D library first
  target_link_libraries(${PROJECT_NAME} PRIVATE
    Box2D
    lua_static
    ${SDL2_LIBRARIES}
    ${SDL2_IMAGE_LIBRARIES}
    ${SDL2_MIXER_LIBRARIES}
    ${SDL2_TTF_LIBRARIES}
    ${LUA_LIBRARIES}
  )

elseif(APPLE)
  # macOS: vendored .frameworks + lua_static
  target_include_directories(${PROJECT_NAME} PRIVATE
    ${CMAKE_SOURCE_DIR}/vendor/SDL2
    ${CMAKE_SOURCE_DIR}/vendor/SDL2_image
    ${CMAKE_SOURCE_DIR}/vendor/SDL2_mixer
    ${CMAKE_SOURCE_DIR}/vendor/SDL2_ttf
  )

  # Find frameworks with REQUIRED flag to error early if they can't be found
  find_library(SDL2_FRAMEWORK       SDL2       REQUIRED PATHS ${CMAKE_SOURCE_DIR}/vendor/SDL2/lib)
  find_library(SDL2_IMAGE_FRAMEWORK SDL2_image REQUIRED PATHS ${CMAKE_SOURCE_DIR}/vendor/SDL2_image/lib)
  find_library(SDL2_MIXER_FRAMEWORK SDL2_mixer REQUIRED PATHS ${CMAKE_SOURCE_DIR}/vendor/SDL2_mixer/lib)
  find_library(SDL2_TTF_FRAMEWORK   SDL2_ttf   REQUIRED PATHS ${CMAKE_SOURCE_DIR}/vendor/SDL2_ttf/lib)

  # Link in specific order: frameworks, Box2D, Lua
  target_link_libraries(${PROJECT_NAME} PRIVATE
    ${SDL2_FRAMEWORK}
    ${SDL2_IMAGE_FRAMEWORK}
    ${SDL2_MIXER_FRAMEWORK}
    ${SDL2_TTF_FRAMEWORK}
    Box2D
    lua_static
  )
  
  # Create a frameworks directory in the binary output
  set(FRAMEWORKS_DIR "${CMAKE_BINARY_DIR}/bin/Frameworks")
  file(MAKE_DIRECTORY ${FRAMEWORKS_DIR})
  
  # Copy frameworks to the build output directory
  add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_directory
      "${CMAKE_SOURCE_DIR}/vendor/SDL2/lib/SDL2.framework" 
      "${FRAMEWORKS_DIR}/SDL2.framework"
    COMMAND ${CMAKE_COMMAND} -E copy_directory
      "${CMAKE_SOURCE_DIR}/vendor/SDL2_image/lib/SDL2_image.framework" 
      "${FRAMEWORKS_DIR}/SDL2_image.framework"
    COMMAND ${CMAKE_COMMAND} -E copy_directory
      "${CMAKE_SOURCE_DIR}/vendor/SDL2_mixer/lib/SDL2_mixer.framework" 
      "${FRAMEWORKS_DIR}/SDL2_mixer.framework"
    COMMAND ${CMAKE_COMMAND} -E copy_directory
      "${CMAKE_SOURCE_DIR}/vendor/SDL2_ttf/lib/SDL2_ttf.framework" 
      "${FRAMEWORKS_DIR}/SDL2_ttf.framework"
  )
  
  # Fix RPATH to look in the Frameworks directory relative to the executable
  set_target_properties(${PROJECT_NAME} PROPERTIES
    BUILD_WITH_INSTALL_RPATH TRUE
    INSTALL_RPATH "@executable_path/Frameworks"
  )

elseif(WIN32)
  # Windows: vendored .lib + lua_static
  target_include_directories(${PROJECT_NAME} PRIVATE
    ${CMAKE_SOURCE_DIR}/vendor/SDL2
    ${CMAKE_SOURCE_DIR}/vendor/SDL2_image
    ${CMAKE_SOURCE_DIR}/vendor/SDL2_mixer
    ${CMAKE_SOURCE_DIR}/vendor/SDL2_ttf
  )
  target_link_libraries(${PROJECT_NAME} PRIVATE
    Box2D
    lua_static
    "${CMAKE_SOURCE_DIR}/vendor/SDL2/lib/SDL2.lib"
    "${CMAKE_SOURCE_DIR}/vendor/SDL2/lib/SDL2main.lib"
    "${CMAKE_SOURCE_DIR}/vendor/SDL2_image/lib/SDL2_image.lib"
    "${CMAKE_SOURCE_DIR}/vendor/SDL2_mixer/lib/SDL2_mixer.lib"
    "${CMAKE_SOURCE_DIR}/vendor/SDL2_ttf/lib/SDL2_ttf.lib"
  )
  
  # Set the subsystem to Windows on Windows
  if(MSVC)
    set_target_properties(${PROJECT_NAME} PROPERTIES
      WIN32_EXECUTABLE TRUE
    )
  endif()
endif()

#—— Put the binary in build/bin ——
set_target_properties(${PROJECT_NAME} PROPERTIES
  RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/bin"
  RUNTIME_OUTPUT_DIRECTORY_DEBUG "${CMAKE_BINARY_DIR}/bin"
  RUNTIME_OUTPUT_DIRECTORY_RELEASE "${CMAKE_BINARY_DIR}/bin"
  RUNTIME_OUTPUT_DIRECTORY_MINSIZEREL "${CMAKE_BINARY_DIR}/bin"
  RUNTIME_OUTPUT_DIRECTORY_RELWITHDEBINFO "${CMAKE_BINARY_DIR}/bin"
  XCODE_ATTRIBUTE_CONFIGURATION_BUILD_DIR "${CMAKE_BINARY_DIR}/bin"
)

#—— Copy resources to output directory for all platforms ——
add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD
  COMMAND ${CMAKE_COMMAND} -E echo "Copying resources to ${CMAKE_BINARY_DIR}/bin/resources"
  COMMAND ${CMAKE_COMMAND} -E make_directory "${CMAKE_BINARY_DIR}/bin/resources"
  COMMAND ${CMAKE_COMMAND} -E copy_directory
    "${CMAKE_SOURCE_DIR}/resources" 
    "${CMAKE_BINARY_DIR}/bin/resources"
)

#—— Copy DLLs to output directory on Windows ——
if(WIN32)
  add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_if_different
      "${CMAKE_SOURCE_DIR}/vendor/SDL2/lib/SDL2.dll"
      "${CMAKE_SOURCE_DIR}/vendor/SDL2_image/lib/SDL2_image.dll"
      "${CMAKE_SOURCE_DIR}/vendor/SDL2_mixer/lib/SDL2_mixer.dll"
      "${CMAKE_SOURCE_DIR}/vendor/SDL2_ttf/lib/SDL2_ttf.dll"
      $<TARGET_FILE_DIR:${PROJECT_NAME}>
  )
endif()

#—— INSTALL RULES ——
install(TARGETS ${PROJECT_NAME}
        RUNTIME DESTINATION bin)

# Install resources alongside the executable for Windows/macOS, 
# and in a standard share directory for Linux
if(UNIX AND NOT APPLE)
  # Linux convention: binaries in bin/, resources in share/
  install(DIRECTORY ${CMAKE_SOURCE_DIR}/resources
          DESTINATION share/${PROJECT_NAME})
else()
  # Windows/macOS: keep resources next to executable
  install(DIRECTORY ${CMAKE_SOURCE_DIR}/resources
          DESTINATION bin)
endif()

# Platform-specific install rules
if(WIN32)
  # Install DLLs alongside the executable on Windows
  install(FILES 
    "${CMAKE_SOURCE_DIR}/vendor/SDL2/lib/SDL2.dll"
    "${CMAKE_SOURCE_DIR}/vendor/SDL2_image/lib/SDL2_image.dll"
    "${CMAKE_SOURCE_DIR}/vendor/SDL2_mixer/lib/SDL2_mixer.dll"
    "${CMAKE_SOURCE_DIR}/vendor/SDL2_ttf/lib/SDL2_ttf.dll"
    DESTINATION bin)
elseif(APPLE)
  # Install frameworks alongside the executable on macOS
  install(DIRECTORY
    "${CMAKE_SOURCE_DIR}/vendor/SDL2/lib/SDL2.framework"
    "${CMAKE_SOURCE_DIR}/vendor/SDL2_image/lib/SDL2_image.framework"
    "${CMAKE_SOURCE_DIR}/vendor/SDL2_mixer/lib/SDL2_mixer.framework"
    "${CMAKE_SOURCE_DIR}/vendor/SDL2_ttf/lib/SDL2_ttf.framework"
    DESTINATION bin/Frameworks)
endif()

#—— CPack packaging ——
include(InstallRequiredSystemLibraries)

set(CPACK_PACKAGE_NAME        "${PROJECT_NAME}")
set(CPACK_PACKAGE_VENDOR      "Lots of places")
set(CPACK_PACKAGE_DESCRIPTION_SUMMARY "A simple 2D C++ game engine")
set(CPACK_PACKAGE_VERSION_MAJOR ${PROJECT_VERSION_MAJOR})
set(CPACK_PACKAGE_VERSION_MINOR ${PROJECT_VERSION_MINOR})
set(CPACK_PACKAGE_VERSION_PATCH ${PROJECT_VERSION_PATCH})
set(CPACK_GENERATOR           "TGZ;ZIP")
set(CPACK_PACKAGING_INSTALL_PREFIX "\${CMAKE_INSTALL_PREFIX}")

include(CPack)