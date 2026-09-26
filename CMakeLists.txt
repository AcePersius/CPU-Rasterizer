cmake_minimum_required(VERSION 3.20)

project(TinyRasterizer)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

include(FetchContent)

find_package(SDL3 CONFIG REQUIRED)

# Tell vk-bootstrap to use the Vulkan-Headers revision
# that matches its own source.
set(
    VK_BOOTSTRAP_FORCE_FETCH_CONTENT
    ON
    CACHE BOOL ""
    FORCE
)

FetchContent_Declare(
    fastgltf
    GIT_REPOSITORY https://github.com/spnda/fastgltf.git
    GIT_TAG v0.9.0
    )
# VkBootstrap
FetchContent_Declare(
    vk_bootstrap
    GIT_REPOSITORY https://github.com/charles-lunarg/vk-bootstrap.git
    GIT_TAG 94e717e
)

# Vulkan Memory Allocator
FetchContent_Declare(
    vma
    GIT_REPOSITORY https://github.com/GPUOpen-LibrariesAndSDKs/VulkanMemoryAllocator.git
    GIT_TAG v3.4.0
)

# GLM
FetchContent_Declare(
    glm
    GIT_REPOSITORY https://github.com/g-truc/glm.git
    GIT_TAG 1.0.3
)

FetchContent_MakeAvailable(
    vk_bootstrap
    vma
    glm
    fastgltf
)

find_package(Vulkan REQUIRED)

add_executable(TinyRasterizer
    src/main.cpp
    src/vulkanRender.cpp
    src/softwareRender.cpp
    src/structsANDoperators.cpp
    src/logger.cpp
    src/gltfLoader.cpp
    src/objLoader.cpp
    src/textsystem.cpp
    src/perfANDmonitoring.cpp
    src/mouseANDkeyboard.cpp
)

target_link_libraries(TinyRasterizer 
PRIVATE 
    Vulkan::Vulkan
    vk-bootstrap::vk-bootstrap
    SDL3::SDL3
    GPUOpen::VulkanMemoryAllocator
    glm::glm
    fastgltf::fastgltf
)

target_include_directories(TinyRasterizer PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/include
)