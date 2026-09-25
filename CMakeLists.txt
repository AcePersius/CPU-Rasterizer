cmake_minimum_required(VERSION 3.20)

project(TinyRasterizer)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

include(FetchContent)
FetchContent_Declare(
    fastgltf
    GIT_REPOSITORY https://github.com/spnda/fastgltf.git
    GIT_TAG v0.9.0
    )
FetchContent_MakeAvailable(fastgltf)


find_package(SDL3 CONFIG REQUIRED)

add_executable(TinyRasterizer
    src/main.cpp
    src/structsANDoperators.cpp
    src/logger.cpp
    src/gltfLoader.cpp
    src/objLoader.cpp
)

target_link_libraries(TinyRasterizer 
PRIVATE 
    SDL3::SDL3
    fastgltf::fastgltf
)

target_include_directories(TinyRasterizer PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/include
)