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

// converts x, y screen coords to pixel indice
// used in PixelData
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


// Takes temp pixel data, and assemblies it into the data buffer for display
// refirt to simply made a background
void FramePackagerBackground(PixelData &pixel, framebuffer &buffer)
{
    for (std::uint32_t i = 0; i < (framebuffer::width * framebuffer::height); i++)
    {
        std::uint32_t packed_pixel;
        packed_pixel = pixel.pixelColor;
        buffer.pixels[i] = packed_pixel;
    }
}


void FramePackager(PixelData &pixel, framebuffer &buffer)
{
    std::uint32_t packed_pixel;
    packed_pixel = pixel.pixelColor;
    buffer.pixels[pixel.indice] = packed_pixel;
}

// Turns Mesh Data into Plotted 2d points
// Im going to keep this as a dot plotter
void MeshToFrameXY(auto &meshdata, framebuffer &frameBufferData)
{
    // Rasterizer definitely should feed here as its own meshdata
    for (int i = 0; i < std::size(meshdata); i += 2)
    {
        PixelData pixel;
        pixel.x = meshdata[i];
        pixel.y = meshdata[i+1];
        // checking for out of bounds indice
        if (pixel.x >= framebuffer::width || pixel.y >= framebuffer::height)
        {
            continue;
        }
        pixel.indice = xyToIndex(pixel.x, pixel.y);
        FramePackager(pixel, frameBufferData);
    }

}

struct BoundingBoxData{
    int xmin;
    int ymin;
    int xmax;
    int ymax;
};

// New Rasterizer + Functions
BoundingBoxData boundingBox(Vector2d &vertexA, Vector2d &vertexB, Vector2d &vertexC);
bool determinantOffset(Vector2d &start, Vector2d &end);
int getDeterminant(Vector2d &vertexA, Vector2d &vertexB, Vector2d &pointC);
void drawToBuffer(PixelData &pixel, framebuffer &frameBufferData);
void RASTERIZE(auto &meshdata, framebuffer &frameBufferData, PixelData pixel);



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
    PixelData  WHITE;
    PixelData  GREEN;
    GREEN.red = 0;
    GREEN.green = 255;
    GREEN.blue = 0;
    GREEN.pixelColor =
    pixelColorAttributer(
        GREEN.red,
        GREEN.green,
        GREEN.blue,
        GREEN.alpha
    );
    PixelData  BLUE;
    BLUE.red = 0;
    BLUE.green = 0;
    BLUE.blue = 255;
    BLUE.pixelColor =
    pixelColorAttributer(
        BLUE.red,
        BLUE.green,
        BLUE.blue,
        BLUE.alpha
    );

    // Retrieves finalized buffer of data from other functions
    // This is the background for now
    framebuffer frameBufferData;
    FramePackagerBackground(WHITE, frameBufferData);


    // Plots vertex points of the triangles
    MeshToFrameXY(triangle, frameBufferData);
    // use New Rasterizer instead
    RASTERIZE(rightTriangle1, frameBufferData, GREEN);
    RASTERIZE(rightTriangle2, frameBufferData, BLUE);


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
void RASTERIZE(auto &meshdata, framebuffer &frameBufferData, PixelData pixel)
{
    // breaking down Meshdata into points
    // This step may be better to make a 2Dvector struct before
    // and use that as the intake arguement instead
    Vector2d a;
        a.x = meshdata[0];
        a.y = meshdata[1];      
    Vector2d b;
        b.x = meshdata[2];
        b.y = meshdata[3];
    Vector2d c;
        c.x = meshdata[4];
        c.y = meshdata[5];
    Vector2d p;
    // For this data xyMinMax comes in the format of:
    // [0] = xmin [1] = ymin [2] = xmax [3] = ymax
    BoundingBoxData xyMinMax = boundingBox(a, b, c);
    // Top Left Fill Rule https://kristoffer-dyrkorn.github.io/triangle-rasterizer/4
    // determinantOffset can also be thought of as a true/false for IsTopleft true/false
    // Might change this later to that name instead
    bool detABoffset = determinantOffset(a, b);
    bool detBCoffset = determinantOffset(b, c);
    bool detCAoffset = determinantOffset(c, a);
    for (int y = xyMinMax.ymin; y <= xyMinMax.ymax; y++)
    {
        for (int x = xyMinMax.xmin; x <= xyMinMax.xmax; x++)
        
        {
            p.x = x;
            p.y = y;
            int determinants[3];
                determinants[0] = getDeterminant(a, b, p);
                determinants[1] = getDeterminant(b, c, p);
                determinants[2] = getDeterminant(c, a, p);
            switch (frameBufferData.WindingMode)
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
                    pixel.x = p.x;
                    pixel.y = p.y;
                    drawToBuffer(pixel, frameBufferData);
                }
                break;
            
            case Both:
                if (((determinants[0] >= (detABoffset ? 0 : 1)) && (determinants[1]  >= (detBCoffset ? 0 : 1)) 
                && (determinants[2] >= (detCAoffset ? 0 : 1))) || ((determinants[0] <= (detABoffset ? -1 : 0)) 
                && (determinants[1]  <= (detBCoffset ? -1 : 0)) && (determinants[2] <= (detCAoffset ? -1 : 0))))
                {
                    pixel.x = p.x;
                    pixel.y = p.y;
                    drawToBuffer(pixel, frameBufferData);
                }
                break;

            default: //default is CCW
                if ((determinants[0] >= (detABoffset ? 0 : 1)) && (determinants[1]  >= (detBCoffset ? 0 : 1)) 
                && (determinants[2] >= (detCAoffset ? 0 : 1)))
                {
                    pixel.x = p.x;
                    pixel.y = p.y;
                    drawToBuffer(pixel, frameBufferData);
                }
                break;
            }

        }
    }

}
// Finding candidate pixels aka Creating Bounding Box
BoundingBoxData boundingBox(Vector2d &vertexA, Vector2d &vertexB, Vector2d &vertexC)
{
    // https://kristoffer-dyrkorn.github.io/triangle-rasterizer/1
    BoundingBoxData xyMinMax;
    xyMinMax.xmin = std::min(vertexA.x, std::min(vertexB.x, vertexC.x));
    xyMinMax.ymin = std::min(vertexA.y, std::min(vertexB.y, vertexC.y));
    xyMinMax.xmax = std::max(vertexA.x, std::max(vertexB.x, vertexC.x));
    xyMinMax.ymax = std::max(vertexA.y, std::max(vertexB.y, vertexC.y));
    return xyMinMax;

}
// Finding determinant-Offeset for the topleft fill rule
bool determinantOffset(Vector2d &start, Vector2d &end)
{
    int edge[2];
    edge[0] = end.x - start.x;
    edge[1] = end.y - start.y;
    bool isLeftEdge = edge[1] > 0;
    bool isTopEdge  = edge[1] == 0 && edge[0] < 0;
    return isLeftEdge || isTopEdge;
}

int getDeterminant(Vector2d &vertexA, Vector2d &vertexB, Vector2d &pointC)
{
    // https://kristoffer-dyrkorn.github.io/triangle-rasterizer/1
    // ab and ac both could be converted into Vector2d types but Im choosing not to
    // as it keeps things more straight forward imo without hiding data behind user created
    // types, which because I dont need to pass arrays of data, its perfectly fine to do it this way
    int ab[2];
    int ac[2];
        ab[0] = vertexB.x - vertexA.x;
        ab[1] = vertexB.y - vertexA.y;
        ac[0] = pointC.x  - vertexA.x;
        ac[1] = pointC.y  - vertexA.y;
    int determinant = ab[1] * ac[0] - ab[0] * ac[1];
    return determinant;
}

// Changes from MeshToFrameXY
// Takes valid pixels (ie fits in frame) and attaches the pixel's data to its correct
// place in the buffer using the FramePackager
// barebones rn but functions will be added
void drawToBuffer(PixelData &pixel, framebuffer &frameBufferData)
{
        // checking for out of bounds indice
        if (pixel.x >= framebuffer::width || pixel.y >= framebuffer::height)
        {
            return;
        }
        pixel.indice = xyToIndex(pixel.x, pixel.y);
        FramePackager(pixel, frameBufferData);
}