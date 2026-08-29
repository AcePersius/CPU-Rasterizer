#include <SDL3/SDL.h>
#include <SDL3/SDL_main.h>
// used for std::uint8-32_t
#include <cstdint>
// used for std::size
#include <iterator>
// used for std::min, std::max
#include <algorithm>

// my supporting files
#include <testmeshes.hpp>
// end


triangle SRTriangle1 = {
{{200, 150}, {0 , 0, 255, 255}},
{{200, 300}, {0 , 0, 255, 255}},
{{600, 300}, {0 , 0, 255, 255}},
};


triangle SRTriangle2 = {
{{600, 300}, {255, 0, 0, 255}},
{{600, 150}, {255, 0, 0, 255}},
{{200, 150}, {255, 0, 0, 255}},
};

// Rasterizer Modes relating to determinant values
// IE if a mesh's vertexes are in a Counter Clockwise Order us CCW
// Clock Wise use CW, if order is unknown use both
// Default will be CCW
enum windingModes{
CCW = 0,
CW = 1,
Both = 2
};

struct framebuffer{
    const static int width = 800;
    const static int height = 450;
    std::uint32_t pixels[width * height];
    const windingModes WindingMode = CCW;
};


struct BoundingBoxData{
    int xmin;
    int ymin;
    int xmax;
    int ymax;
};

// New Rasterizer + Functions
BoundingBoxData boundingBox(triangle &meshdata);
bool TopLeftFillFunc(Vector2d &start, Vector2d &end);
int getDeterminant(Vector2d &VertexA, Vector2d &VertexB, Vector2d &pointC);
void drawToBuffer(Vertex2d &pixel, framebuffer &frameBufferData, triangle &meshdata);
void RASTERIZE(triangle &meshdata, framebuffer &frameBufferData);
void FramePackager(Vertex2d &pixel, framebuffer &buffer, triangle &meshdata);
std::uint32_t pixelPackager(RGBA &pixel);
void fillpixelcolor(RGBA &pixel, triangle &meshdata);


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


    // Retrieves finalized buffer of data from other functions
    framebuffer frameBufferData {0};

    // Step1: Feed FrameBuffer and Mesh into Rasterizer
    RASTERIZE(SRTriangle1, frameBufferData);
    RASTERIZE(SRTriangle2, frameBufferData);


    // Currently acts as a preset buffer to write pixels to before displaying
    SDL_Surface * surface = SDL_CreateSurfaceFrom(width, height, SDL_PIXELFORMAT_RGBA8888, frameBufferData.pixels, frameBufferData.width *4);
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
        // step 9
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


// Rasterizer
// Step 2, follow rasterizing steps
void RASTERIZE(triangle &meshdata, framebuffer &frameBufferData)
{
    // For this data xyMinMax comes in the format of:
    // [0] = xmin [1] = ymin [2] = xmax [3] = ymax
    // Step 3
    BoundingBoxData xyMinMax = boundingBox(meshdata);
    // Top Left Fill Rule https://kristoffer-dyrkorn.github.io/triangle-rasterizer/4
    // determinantOffset can also be thought of as a true/false for IsTopleft true/false
    // Might change this later to that name instead
    // Step 4
    bool detABoffset = TopLeftFillFunc(meshdata.VertexA.position, meshdata.VertexB.position);
    bool detBCoffset = TopLeftFillFunc(meshdata.VertexB.position, meshdata.VertexC.position);
    bool detCAoffset = TopLeftFillFunc(meshdata.VertexC.position, meshdata.VertexA.position);
    // This p vector serves as the vector data to be filled in during the loop
    Vertex2d pixel;
    for (int y = xyMinMax.ymin; y <= xyMinMax.ymax; y++)
    {
        for (int x = xyMinMax.xmin; x <= xyMinMax.xmax; x++)
        
        {
            // update pixel values to current x,y coord
            pixel.position.x = x;
            pixel.position.y = y;
            // Step 5
            int determinants[3];
                determinants[0] = getDeterminant(meshdata.VertexA.position, meshdata.VertexB.position, pixel.position);
                determinants[1] = getDeterminant(meshdata.VertexB.position, meshdata.VertexC.position, pixel.position);
                determinants[2] = getDeterminant(meshdata.VertexC.position, meshdata.VertexA.position, pixel.position);
            switch (frameBufferData.WindingMode) // Step 6
            {
            case CW:
                /* Note explaining the detoffset, the current offset checker for is a CCW, luckily we can still 
                   use the results from that function, BUT the values associated with determinants that are CW 
                   are opposite of CCW, IE negative not positive. that explains the flipped < operator, similar
                   logic applies to the offset. Due to the offset function be made for CCW, the CW options of 
                   0 and -1 need to be flipped to -1 and 0 because CCW true is CW false
                */
                if ((determinants[0] <= (detABoffset ? -1 : 0)) && (determinants[1]  <= (detBCoffset ? -1 : 0)) 
                && (determinants[2] <= (detCAoffset ? -1 : 0)))
                {
                    drawToBuffer(pixel, frameBufferData, meshdata);
                }
                break;
            
            case Both:
                if (((determinants[0] >= (detABoffset ? 0 : 1)) && (determinants[1]  >= (detBCoffset ? 0 : 1)) 
                && (determinants[2] >= (detCAoffset ? 0 : 1))) || ((determinants[0] <= (detABoffset ? -1 : 0)) 
                && (determinants[1]  <= (detBCoffset ? -1 : 0)) && (determinants[2] <= (detCAoffset ? -1 : 0))))
                {
                    drawToBuffer(pixel, frameBufferData, meshdata);
                }
                break;

            default: //default is CCW
                if ((determinants[0] >= (detABoffset ? 0 : 1)) && (determinants[1]  >= (detBCoffset ? 0 : 1)) 
                && (determinants[2] >= (detCAoffset ? 0 : 1)))
                {
                    drawToBuffer(pixel, frameBufferData, meshdata);
                }
                break;
            }

        }
    }

}
// Finding candidate pixels aka Creating Bounding Box
// Step 3
BoundingBoxData boundingBox(triangle &meshdata)
{
    // https://kristoffer-dyrkorn.github.io/triangle-rasterizer/1
    BoundingBoxData xyMinMax;
    xyMinMax.xmin = std::min(meshdata.VertexA.position.x, std::min(meshdata.VertexB.position.x, meshdata.VertexC.position.x));
    xyMinMax.ymin = std::min(meshdata.VertexA.position.y, std::min(meshdata.VertexB.position.y, meshdata.VertexC.position.y));
    xyMinMax.xmax = std::max(meshdata.VertexA.position.x, std::max(meshdata.VertexB.position.x, meshdata.VertexC.position.x));
    xyMinMax.ymax = std::max(meshdata.VertexA.position.y, std::max(meshdata.VertexB.position.y, meshdata.VertexC.position.y));
    return xyMinMax;

}
// Finding determinant-Offeset for the topleft fill rule
// Step 4
bool TopLeftFillFunc(Vector2d &start, Vector2d &end)
{
    int edge[2];
    edge[0] = end.x - start.x;
    edge[1] = end.y - start.y;
    bool isLeftEdge = edge[1] > 0;
    bool isTopEdge  = edge[1] == 0 && edge[0] < 0;
    return isLeftEdge || isTopEdge;
}

// Step 5
int getDeterminant(Vector2d &VertexA, Vector2d &VertexB, Vector2d &pointC)
{
    // https://kristoffer-dyrkorn.github.io/triangle-rasterizer/1
    // ab and ac both could be converted into Vector2d types but Im choosing not to
    // as it keeps things more straight forward imo without hiding data behind user created
    // types, which because I dont need to pass arrays of data, its perfectly fine to do it this way
    int ab[2];
    int ac[2];
        ab[0] = VertexB.x - VertexA.x;
        ab[1] = VertexB.y - VertexA.y;
        ac[0] = pointC.x  - VertexA.x;
        ac[1] = pointC.y  - VertexA.y;
    int determinant = ab[1] * ac[0] - ab[0] * ac[1];
    return determinant;
}

// Takes valid pixels (ie fits in frame) and attaches the pixel's data to its correct
// place in the buffer using the FramePackager
// barebones rn but functions will be added
// Step 6
void drawToBuffer(Vertex2d &pixel, framebuffer &frameBufferData, triangle &meshdata)
{
        // checking for out of bounds indice
        if (pixel.position.x >= framebuffer::width || pixel.position.y >= framebuffer::height)
        {
            return;
        }
        FramePackager(pixel, frameBufferData, meshdata);
}

// step 7
void FramePackager(Vertex2d &pixel, framebuffer &buffer, triangle &meshdata)
{
    std::uint32_t packed_pixel;
    // temp gives pixels the avg color of all three vertices colors
    fillpixelcolor(pixel.colorData, meshdata);
    // Pixel packer needs to come back
    packed_pixel = pixelPackager(pixel.colorData);
    // converts x, y screen coords to pixel indice and used to be a helper function
    std::uint32_t indice = pixel.position.x + (pixel.position.y * framebuffer::width);
    buffer.pixels[indice] = packed_pixel;
}

// step 8
std::uint32_t pixelPackager(RGBA &pixel)
{
    std::uint32_t packed_pixel {0};
    packed_pixel |= static_cast<std::uint32_t>(pixel.red) << 24;
    packed_pixel |= static_cast<std::uint32_t>(pixel.green) << 16;
    packed_pixel |= static_cast<std::uint32_t>(pixel.blue) << 8;
    packed_pixel |= static_cast<std::uint32_t>(pixel.alpha) << 0;
    return packed_pixel;
}

// TEMP
void fillpixelcolor( RGBA &pixel, triangle &meshdata)
{
    pixel.red   = ((meshdata.VertexA.colorData.red   + meshdata.VertexB.colorData.red   + meshdata.VertexC.colorData.red  ) / 3);
    pixel.green = ((meshdata.VertexA.colorData.green + meshdata.VertexB.colorData.green + meshdata.VertexC.colorData.green) / 3);
    pixel.blue  = ((meshdata.VertexA.colorData.blue  + meshdata.VertexB.colorData.blue  + meshdata.VertexC.colorData.blue ) / 3);
    pixel.alpha = ((meshdata.VertexA.colorData.alpha + meshdata.VertexB.colorData.alpha + meshdata.VertexC.colorData.alpha) / 3);
}