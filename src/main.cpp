#include <SDL3/SDL.h>
#include <SDL3/SDL_main.h>
#include <cstdint>


struct framebuffer{
    const static int width = 800;
    const static int height = 450;
    std::uint32_t pixels[width * height];
};

/*
    Needs to find a home
    std::uint32_t x;
    std::uint32_t y;
    std::uint32_t index = xyToIndex(x, y, height);
*/

// converts x, y screen coords to pixel indice
std::uint32_t xyToIndex(std::uint32_t x, std::uint32_t y)
{
    std::uint32_t index = x + (y * framebuffer::width);
    return index;
}

struct color{
    std::uint8_t red;
    std::uint8_t green;
    std::uint8_t blue;
    std::uint8_t alpha;
};

// Helper function for pixelColorAttributer function
std::uint32_t pixelPackager(color pixel)
{
    std::uint32_t packed_pixel {0};
    packed_pixel |= static_cast<std::uint32_t>(pixel.red) << 24;
    packed_pixel |= static_cast<std::uint32_t>(pixel.green) << 16;
    packed_pixel |= static_cast<std::uint32_t>(pixel.blue) << 8;
    packed_pixel |= static_cast<std::uint32_t>(pixel.alpha) << 0;
    return packed_pixel;
}
// Receives Pixel RGBA data and turns it into a 32bit "packed" pixel 
// Takes input from:
// Uses Function(s): pixelpackager
// Sends output to: Pixel Data
std::uint32_t pixelColorAttributer(std::uint8_t red, std::uint8_t green, std::uint8_t blue, std::uint8_t alpha)
{

    color pixel;
    pixel.red = red;
    pixel.green = green;
    pixel.blue = blue;
    pixel.alpha = alpha;
    std::uint32_t packed_pixel = pixelPackager(pixel);
    return packed_pixel;
}

// temp data
struct PixelData1{
    std::uint8_t red = 200;
    std::uint8_t green = 200;
    std::uint8_t blue = 150;
    std::uint8_t alpha = 255;
    std::uint32_t pixelColor = pixelColorAttributer(red, green, blue, alpha);
};

// temp data
struct PixelData2{
    std::uint8_t red = 50;
    std::uint8_t green = 50;
    std::uint8_t blue = 100;
    std::uint8_t alpha = 255;
    std::uint32_t pixelColor = pixelColorAttributer(red, green, blue, alpha);
};

// Takes temp pixel data, and assemblies it into the data buffer for display
framebuffer FramePackager(PixelData1 pixel1, PixelData2 pixel2)
{

    framebuffer buffer;
    for (std::uint32_t i = 0; i < (framebuffer::width * framebuffer::height); i++)
    {
        std::uint32_t packed_pixel;
        if (i >= xyToIndex(350, 100) && i <= xyToIndex(350, 350))
        {
            packed_pixel = pixel1.pixelColor;
            buffer.pixels[i] = packed_pixel;
            continue;
        }
        packed_pixel = pixel2.pixelColor;
        buffer.pixels[i] = packed_pixel;
    }
    return buffer;
}





int main(int argc, char* argv[]) {

    SDL_Window *window;                                 // Declare a pointer
    bool done = false;

    bool sdl_init = SDL_Init(SDL_INIT_VIDEO);           // Initialize SDL3
    // Check that the window was successfully created
    if (!sdl_init) {
        // In the case that the window could not be made...
        SDL_LogError(SDL_LOG_CATEGORY_ERROR, "Could not init sdl: %s\n", SDL_GetError());
        return 1;
    }
    int width =  800;
    int height = 450;
    // Create an application window with the following settings:
    window = SDL_CreateWindow(
        "An SDL3 window",                  // window title
        width,                               // width, in pixels
        height,                               // height, in pixels
        0                                  // flags - see below
    );
    // Check that the window was successfully created
    if (window == NULL) {
        // In the case that the window could not be made...
        SDL_LogError(SDL_LOG_CATEGORY_ERROR, "Could not create window: %s\n", SDL_GetError());
        return 1;
    }

    // temp pixel data
    PixelData1 pixel1;
    PixelData2 pixel2;

    // Retrieves finalized buffer of data from other functions
    framebuffer frameBufferData = FramePackager(pixel1, pixel2);

    // Currently acts as a preset buffer to write pixels to before displaying
    SDL_Surface * surface = SDL_CreateSurfaceFrom(width, height, SDL_PIXELFORMAT_RGBA32, frameBufferData.pixels, frameBufferData.width *4);
    if (surface == nullptr)
    {
        SDL_LogError(SDL_LOG_CATEGORY_ERROR, "Could not create surface: %s\n", SDL_GetError());
        return 1;
    }

    while (!done) {
        SDL_Event event;

        while (SDL_PollEvent(&event)) {
            if (event.type == SDL_EVENT_QUIT) {
                done = true;
            }
        }
        SDL_BlitSurface(surface, NULL, SDL_GetWindowSurface(window), NULL);
        SDL_UpdateWindowSurface(window);
        // Do game logic, present a frame, etc.
    }

    // Close and destroy the window
    SDL_DestroyWindow(window);

    // Clean up
    SDL_Quit();
    return 0;
}
