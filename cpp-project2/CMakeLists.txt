# CMakeList.txt : CMake project for cpp-project2, include source and define
# project specific logic here.
#

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)


include(FetchContent)

# Fetch GLFW
FetchContent_Declare(
    glfw
    GIT_REPOSITORY https://github.com/glfw/glfw.git
    GIT_TAG        3.4
)
set(GLFW_BUILD_DOCS OFF CACHE BOOL "" FORCE)
set(GLFW_BUILD_TESTS OFF CACHE BOOL "" FORCE)
set(GLFW_BUILD_EXAMPLES OFF CACHE BOOL "" FORCE)
FetchContent_MakeAvailable(glfw)

# Fetch GLM (Header-only)
FetchContent_Declare(
    glm
    GIT_REPOSITORY https://github.com/g-truc/glm.git
    GIT_TAG        1.0.1
)
FetchContent_MakeAvailable(glm)

FetchContent_Declare(
    tinyobjloader
    GIT_REPOSITORY https://github.com/tinyobjloader/tinyobjloader.git
    GIT_TAG release
)

FetchContent_MakeAvailable(tinyobjloader)

FetchContent_Declare(
    glad
    GIT_REPOSITORY https://github.com/Dav1dde/glad.git
    GIT_TAG        v2.0.8
    SOURCE_SUBDIR  cmake
)
# glad 2.x has no top-level CMakeLists.txt; its CMake support lives in the
# cmake/ subdirectory and exposes a glad_add_library() function.
FetchContent_MakeAvailable(glad)

FetchContent_Declare(
    spdlog
    GIT_REPOSITORY https://github.com/gabime/spdlog.git
    GIT_TAG v1.15.3
)

FetchContent_MakeAvailable(spdlog)

FetchContent_Declare(json URL https://github.com/nlohmann/json/releases/download/v3.12.0/json.tar.xz)
FetchContent_MakeAvailable(json)

FetchContent_Declare(
    stb
    GIT_REPOSITORY https://github.com/nothings/stb.git
    GIT_TAG        master
)
FetchContent_MakeAvailable(stb)

FetchContent_Declare(
    JoltPhysics
    GIT_REPOSITORY https://github.com/jrouwe/JoltPhysics.git
    GIT_TAG        "v5.3.0"
    SOURCE_SUBDIR  "Build"
)
FetchContent_MakeAvailable(JoltPhysics)

# Generate an OpenGL 4.6 Core C loader as the `glad` target.
glad_add_library(glad STATIC API gl:core=4.6)

# System OpenGL (always provided by OS drivers)
find_package(OpenGL REQUIRED)

# Add source to this project's executable.
add_executable (cpp-project2 "cpp-project2.cpp" "cpp-project2.h" "window.cpp" "engine.cpp" "mesh.cpp" "shader_program.cpp" "camera.cpp" "material.cpp" "io.cpp" "world_object.cpp" "asset_loader.cpp" "scene.cpp" "material_registry.cpp" "mesh_registry.cpp"
  player_controller.hpp
  player_controller.cpp
  framebuffer.hpp
  deferred_light_pass.hpp
  deferred_light_pass.cpp
  framebuffer.cpp
  light_manager.hpp
  light_manager.cpp
  texture.hpp
  texture.cpp
  gl_buffer.hpp
  gl_buffer.cpp
  level_loader.hpp
  level_data_structs.hpp
  level_loader.cpp
  physics_engine.hpp
  physics_engine.cpp
  collider.hpp
  collider.cpp
  colliders_registry.hpp
  colliders_registry.cpp
  virtual_character.hpp
  virtual_character.cpp
  crash_handler.hpp
  crash_handler.cpp
  text_renderer.hpp
  text_mesh.hpp
  text_renderer.cpp
  text_mesh.cpp
  shards_manager.hpp
  shards_manager.cpp
  player_stats.hpp
  player_stats.cpp)

if (CMAKE_VERSION VERSION_GREATER 3.12)
  set_property(TARGET cpp-project2 PROPERTY CXX_STANDARD 20)
endif()

# TODO: Add tests and install targets if needed.

target_include_directories(cpp-project2 PRIVATE ${stb_SOURCE_DIR})

target_link_libraries(cpp-project2 PRIVATE
    OpenGL::GL
    glfw
    glm::glm
)
target_link_libraries(cpp-project2 PRIVATE glad)

target_link_libraries(cpp-project2 PRIVATE tinyobjloader)

target_link_libraries(cpp-project2 PRIVATE spdlog::spdlog)

target_link_libraries(cpp-project2 PRIVATE nlohmann_json::nlohmann_json)

target_link_libraries(cpp-project2 PRIVATE Jolt)
target_include_directories(cpp-project2 PRIVATE ${JoltPhysics_SOURCE_DIR}/..)

target_link_libraries(cpp-project2 PRIVATE
    Dbghelp
)