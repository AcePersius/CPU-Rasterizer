// VULKAN
#include <vulkan/vulkan.h>
// Used for Window Creation and Input reading
#include <SDL3/SDL.h>
#include <SDL3/SDL_main.h>
// Loader for GLTF and GLB files
#include <loaders/gltfLoader.hpp>

// used for std::uint8-32_t
#include <cstdint>
// used for std::cout testing
#include <iostream>
// used for std::size
#include <iterator>
// used for std::min, std::max
#include <algorithm>
// used for std::abs(), std::sin() std::cos()
#include <cmath>
// used for std::vector<>
#include <vector>

// my supporting files
#include <VulkanRenderHPP/vulkanRender.hpp>
#include <softwareRender/structsANDoperators.hpp>
#include <softwareRender/softwareRender.hpp>
#include <loaders/objLoader.hpp>
#include <softwareRender/textsystem.hpp>
#include <softwareRender/perfANDmonitoring.hpp>
#include <softwareRender/mouseANDkeyboard.hpp>
#include <logs/logger.hpp>
// end

constexpr float EPSILON = .0000001;
Transformation transform{
{0, 0, 2},
{0, 0, 0},
{1, 1, 1}
};

Camera cameraTransf{
    // Position
    {0, 0, 0},
    // Rotation
    {0, 0, 0},
    // FOV
    {70},
    // Aspect Ratio
    {16.0f / 9.0f},
    // Nearplane
    {1.0f},
    // Farplane
    {1000.0f}
};


int main(int argc, char* argv[]) 
{

    // Initializes Logger
    initLogger("include/logs/latest.log");

    int version = SDL_GetVersion();

    std::string SDL_Version = "SDL version: " + std::to_string(SDL_VERSIONNUM_MAJOR(version)) + "." + 
    std::to_string(SDL_VERSIONNUM_MINOR(version)) + "." + std::to_string(SDL_VERSIONNUM_MICRO(version));
    Logger(logLevel::Info, SDL_Version);

    uint32_t vulkanVersion = VK_API_VERSION_1_0;
    VkResult result =
        vkEnumerateInstanceVersion(&vulkanVersion);

    if (result == VK_SUCCESS)
    {
        std::cout
            << "Vulkan API Version: "
            << VK_API_VERSION_MAJOR(vulkanVersion) << "."
            << VK_API_VERSION_MINOR(vulkanVersion) << "."
            << VK_API_VERSION_PATCH(vulkanVersion)
            << '\n';
    }
    else
    {
        std::cout << "Failed to query Vulkan version.\n";
    }

    // Initializes Settings
    initResolution(settings.frameWidth, settings.frameHeight, settings.Resolution, cameraTransf);

    Logger(logLevel::Info, 
    "Width: " + std::to_string(settings.frameWidth)
    + " x " + "Height: " + std::to_string(settings.frameHeight)
    + "\nColor Buffer size: " + std::to_string(frameBufferData.colorPixels.pixels.size())
    + "\nDepth Buffer size: " + std::to_string(frameBufferData.pixelDepth.depthVals.size())
    + '\n');

    SDL_Window *window;                                 // Declare a pointer
    bool done = false;

    bool sdl_init = SDL_Init(SDL_INIT_VIDEO);           // Initialize SDL3
    // Check that the window was successfully created
    if (!sdl_init) {
        // In the case that the window could not be made...
        SDL_LogError(SDL_LOG_CATEGORY_ERROR, "Could not init sdl: %s\n", SDL_GetError());
        return 1;
    }
    // Create an application window with the following settings:
    window = SDL_CreateWindow(
        "Rasterizer",                  // window title
        settings.frameWidth,           // width, in pixels
    settings.frameHeight,              // height, in pixels
        0                              // flags - see below
    );
    // Check that the window was successfully created
    if (window == NULL) {
        // In the case that the window could not be made...
        SDL_LogError(SDL_LOG_CATEGORY_ERROR, "Could not create window: %s\n", SDL_GetError());
        return 1;
    }
    // Mouse Input Mode
    if (!SDL_SetWindowRelativeMouseMode(window, true))
    {
        std::cout << "Failed to enable Relative Mouse mode: " <<
        SDL_GetError << "\n";

    }

    // Currently acts as a preset buffer to write pixels to before displaying
    // SDL wants is a pointer to the first actual pixel element stored inside the vector. For std::vector, the standard way to get that contiguous memory pointer is .data()
    SDL_Surface * surface = SDL_CreateSurfaceFrom(settings.frameWidth, settings.frameHeight, SDL_PIXELFORMAT_RGBA8888, frameBufferData.colorPixels.pixels.data(), settings.frameWidth *4);
    if (surface == nullptr)
    {
        SDL_LogError(SDL_LOG_CATEGORY_ERROR, "Could not create surface: %s\n", SDL_GetError());
        return 1;
    }

    // LoadedModel loadedModel = LoadModel("include/assets/Lowpoly_tree_sample.obj");
    LoadedModel loadedGLTFModel = GLTFLoader("include/assets/eezjyo2a.glb");
    Transformation OriginalTransform = transform;

    while (!done) {
        SDL_Event event;

        while (SDL_PollEvent(&event)) {
            if (event.type == SDL_EVENT_QUIT) {
                done = true;
            }
            if (event.type == SDL_EVENT_MOUSE_MOTION)
            {
                cameraTransf.viewUpdater = true;
                float mouseSensitivity = 0.1f;

                cameraTransf.rotation.y += event.motion.xrel * mouseSensitivity;
                cameraTransf.rotation.x += event.motion.yrel * mouseSensitivity;
                if (cameraTransf.rotation.x > 89.0f){cameraTransf.rotation.x = 89.0f;}
                if (cameraTransf.rotation.x < -89.0f){cameraTransf.rotation.x = -89.0f;}
            }
            if (event.type == SDL_EVENT_KEY_DOWN && !event.key.repeat)
            {
                if (event.key.scancode == SDL_SCANCODE_F)
                {
                    if (settings.MovementMode == FPSstyle)
                    {
                        settings.MovementMode = NOCLIP;
                    }
                    else
                    {
                        settings.MovementMode = FPSstyle;
                    }
                }
            }
        }
        performanceAndMonitoring(settings);
        updateCamera(cameraTransf, deltaTime, settings);

        std::fill(frameBufferData.colorPixels.pixels.begin(), frameBufferData.colorPixels.pixels.end(), 0x000000FF);
        std::fill(frameBufferData.pixelDepth.depthVals.begin(), frameBufferData.pixelDepth.depthVals.end(), INFINITY_Render_Distance);

        RenderModel(loadedGLTFModel, frameBufferData, transform, cameraTransf);
        // RenderMesh(MeshGLTF, frameBufferData, transform, cameraTransf, createWhiteTexture());

        drawPerformanceAndMonitoring(settings, frameBufferData);

        SDL_BlitSurface(surface, NULL, SDL_GetWindowSurface(window), NULL);
        SDL_UpdateWindowSurface(window);
        // Do game logic, present a frame, etc.
    }

    // Close and destroy the window
    SDL_DestroyWindow(window);

    // ShutdownLogging
    shutdownLogger();

    // Clean up
    SDL_Quit();
    return 0;
}
