#include <SDL3/SDL.h>
#include <SDL3/SDL_main.h>
// used for std::uint8-32_t
#include <cstdint>
// used for std::size
#include <iterator>
// used for std::min, std::max
#include <algorithm>
// used for std::abs(), std::sin() std::cos()
#include <cmath>
// used for std::vector<>
#include <vector>

// my supporting files
#include <testmeshes.hpp>
// end

constexpr float pie = 3.141592;

Transformation transform{
{400, 200},
90,
{.5, .5}
};

Mesh2d SquareMesh = {
{{{100, 100}, {255,255,0,255}},
{{100, 200}, {255,255,255,255}},
{{300, 200}, {255,0,0,255}},
{{300, 100}, {0,255,0,255}}},
{0, 1, 2, 2, 3, 0}
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
    const static int height = 600;
    std::uint32_t pixels[width * height];
    const windingModes WindingMode = CCW;
};

// Takes in mesh data and sends it to rasterizer
void RenderMesh(Mesh2d &mesh, framebuffer &frameBufferData, Transformation &transform);
void TransformationStage(triangle &triangleBuffer, Transformation &transform);
// New Rasterizer + Functions
BoundingBoxData boundingBox(auto &meshdata);
bool TopLeftFillFunc(Vector2d &start, Vector2d &end);
float getDeterminant(Vector2d &VertexA, Vector2d &VertexB, Vector2d &pointC);
void barycentricColor(Determinant &determinants, auto &meshdata, Vertex2d &pixel);
void drawToBuffer(Vertex2d &pixel, framebuffer &frameBufferData, auto &meshdata);
void RASTERIZE(auto &meshdata, framebuffer &frameBufferData);
void FramePackager(Vertex2d &pixel, framebuffer &buffer, auto &meshdata);
std::uint32_t pixelPackager(RGBA &pixel);
void fillpixelcolor(RGBA &pixel, auto &meshdata);


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
    int height = 600;
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

    // Applies Transformations
    void TransformationStage();

    // RendersMesh
    RenderMesh(SquareMesh, frameBufferData, transform);

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
        // step 11
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


void RenderMesh(Mesh2d &mesh, framebuffer &frameBufferData, Transformation &transform)
{
    // Buffer for drawing triangles
    if (mesh.Indices.size() % 3 != 0)
    {
        return;
    }
        for(std::size_t i = 0; i < (mesh.Indices.size()); i+=3)
    {
        triangle triangleBuffer {0};
        triangleBuffer.VertexA = mesh.Vertices[mesh.Indices[i]];
        triangleBuffer.VertexB = mesh.Vertices[mesh.Indices[i+1]];
        triangleBuffer.VertexC = mesh.Vertices[mesh.Indices[i+2]];
        TransformationStage(triangleBuffer, transform);
        RASTERIZE(triangleBuffer, frameBufferData);
    }
}
// Apply Transformation Properties
// double check rotation for rotating around the middle pixel rather than rotating around where the triange actually should be

void TransformationStage(triangle &triangleBuffer, Transformation &transform)
{
    // For Scale
    triangleBuffer.VertexA.position = triangleBuffer.VertexA.position * transform.scale;
    triangleBuffer.VertexB.position = triangleBuffer.VertexB.position * transform.scale;
    triangleBuffer.VertexC.position = triangleBuffer.VertexC.position * transform.scale;
    // For Rotation
    float x;
    float y;
    x = triangleBuffer.VertexA.position.x;
    y = triangleBuffer.VertexA.position.y;
    triangleBuffer.VertexA.position.x =  x * std::cos((transform.rotation * pie) / 180) -
        y * std::sin((transform.rotation * pie) / 180);
    triangleBuffer.VertexA.position.y =  x * std::sin((transform.rotation * pie) / 180) +
        y * std::cos((transform.rotation * pie) / 180);
    x = triangleBuffer.VertexB.position.x;
    y = triangleBuffer.VertexB.position.y;
    triangleBuffer.VertexB.position.x =  x * std::cos((transform.rotation * pie) / 180) -
        y * std::sin((transform.rotation * pie) / 180);
    triangleBuffer.VertexB.position.y =  x * std::sin((transform.rotation * pie) / 180) +
        y * std::cos((transform.rotation * pie) / 180);
    x = triangleBuffer.VertexC.position.x;
    y = triangleBuffer.VertexC.position.y;
    triangleBuffer.VertexC.position.x =  x * std::cos((transform.rotation * pie) / 180) -
        y * std::sin((transform.rotation * pie) / 180);
    triangleBuffer.VertexC.position.y =  x * std::sin((transform.rotation * pie) / 180) +
        y * std::cos((transform.rotation * pie) / 180);
    // For Position
    triangleBuffer.VertexA.position =  triangleBuffer.VertexA.position + transform.position;
    triangleBuffer.VertexB.position =  triangleBuffer.VertexB.position + transform.position;
    triangleBuffer.VertexC.position =  triangleBuffer.VertexC.position + transform.position;
}


// Rasterizer
// Step 2, follow rasterizing steps
void RASTERIZE(auto &meshdata, framebuffer &frameBufferData)
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
            Determinant determinants;
                determinants.AB = getDeterminant(meshdata.VertexA.position, meshdata.VertexB.position, pixel.position);
                determinants.BC = getDeterminant(meshdata.VertexB.position, meshdata.VertexC.position, pixel.position);
                determinants.CA = getDeterminant(meshdata.VertexC.position, meshdata.VertexA.position, pixel.position);
            switch (frameBufferData.WindingMode) // Step 6
            {
            case CW:
                /* Note explaining the detoffset, the current offset checker for is a CCW, luckily we can still 
                   use the results from that function, BUT the values associated with determinants that are CW 
                   are opposite of CCW, IE negative not positive. that explains the flipped < operator, similar
                   logic applies to the offset. Due to the offset function be made for CCW, the CW options of 
                   0 and -1 need to be flipped to -1 and 0 because CCW true is CW false
                */
                if ((determinants.AB <= (detABoffset ? -.00001 : 0)) && (determinants.BC  <= (detBCoffset ? -.00001 : 0)) 
                && (determinants.CA <= (detCAoffset ? -.00001 : 0)))
                {
                    // Step 7
                    barycentricColor(determinants, meshdata, pixel);
                    // Step 8
                    drawToBuffer(pixel, frameBufferData, meshdata);
                }
                break;
            
            case Both:
                if (((determinants.AB >= (detABoffset ? 0 : .00001)) && (determinants.BC  >= (detBCoffset ? 0 : .00001)) 
                && (determinants.CA >= (detCAoffset ? 0 : .00001))) || ((determinants.AB <= (detABoffset ? -.00001 : 0)) 
                && (determinants.BC  <= (detBCoffset ? -.00001 : 0)) && (determinants.CA <= (detCAoffset ? -.00001 : 0))))
                {
                    barycentricColor(determinants, meshdata, pixel);
                    drawToBuffer(pixel, frameBufferData, meshdata);
                }
                break;

            default: //default is CCW
                if ((determinants.AB >= (detABoffset ? 0 : .00001)) && (determinants.BC  >= (detBCoffset ? 0 : .00001)) 
                && (determinants.CA >= (detCAoffset ? 0 : .00001)))
                {
                    barycentricColor(determinants, meshdata, pixel);
                    drawToBuffer(pixel, frameBufferData, meshdata);
                }
                break;
            }

        }
    }

}
// Finding candidate pixels aka Creating Bounding Box
// Step 3
BoundingBoxData boundingBox(auto &meshdata)
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
    float edge[2];
    edge[0] = end.x - start.x;
    edge[1] = end.y - start.y;
    bool isLeftEdge = edge[1] > 0;
    bool isTopEdge  = edge[1] == 0 && edge[0] < 0;
    return isLeftEdge || isTopEdge;
}

// Step 5
float getDeterminant(Vector2d &VertexA, Vector2d &VertexB, Vector2d &pointC)
{
    // https://kristoffer-dyrkorn.github.io/triangle-rasterizer/1
    // ab and ac both could be converted into Vector2d types but Im choosing not to
    // as it keeps things more straight forward imo without hiding data behind user created
    // types, which because I dont need to pass arrays of data, its perfectly fine to do it this way
    float ab[2];
    float ac[2];
        ab[0] = VertexB.x - VertexA.x;
        ab[1] = VertexB.y - VertexA.y;
        ac[0] = pointC.x  - VertexA.x;
        ac[1] = pointC.y  - VertexA.y;
    float determinant = ab[1] * ac[0] - ab[0] * ac[1];
    return determinant;
}

// Takes valid pixels (ie fits in frame) and attaches the pixel's data to its correct
// place in the buffer using the FramePackager
// barebones rn but functions will be added
// Step 8
void drawToBuffer(Vertex2d &pixel, framebuffer &frameBufferData, auto &meshdata)
{
        // checking for out of bounds indice
        if (pixel.position.x >= framebuffer::width || pixel.position.y >= framebuffer::height || pixel.position.x < 0 || pixel.position.y < 0)
        {
            return;
        }
        FramePackager(pixel, frameBufferData, meshdata);
}

// step 10
void FramePackager(Vertex2d &pixel, framebuffer &buffer, auto &meshdata)
{
    std::uint32_t packed_pixel;
    // Pixel packer needs to come back
    packed_pixel = pixelPackager(pixel.colorData);
    // converts x, y screen coords to pixel indice and used to be a helper function
    std::uint32_t indice = pixel.position.x + (pixel.position.y * framebuffer::width);
    buffer.pixels[indice] = packed_pixel;
}

// step 9
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
void fillpixelcolor( RGBA &pixel, auto &meshdata)
{
    pixel.red   = ((meshdata.VertexA.colorData.red   + meshdata.VertexB.colorData.red   + meshdata.VertexC.colorData.red  ) / 3);
    pixel.green = ((meshdata.VertexA.colorData.green + meshdata.VertexB.colorData.green + meshdata.VertexC.colorData.green) / 3);
    pixel.blue  = ((meshdata.VertexA.colorData.blue  + meshdata.VertexB.colorData.blue  + meshdata.VertexC.colorData.blue ) / 3);
    pixel.alpha = ((meshdata.VertexA.colorData.alpha + meshdata.VertexB.colorData.alpha + meshdata.VertexC.colorData.alpha) / 3);
}
// Step 7
// barycentric coordinates
void barycentricColor(Determinant &determinants, auto &meshdata, Vertex2d &pixel)
{
    float totalDeterminant = determinants.AB + determinants.BC + determinants.CA;
    if (totalDeterminant == 0)
    {
        totalDeterminant = 1;
    }
    float percentageA = std::abs((determinants.BC) / totalDeterminant);
    RGBA A = meshdata.VertexA.colorData * percentageA;
    float percentageB = std::abs((determinants.CA) / totalDeterminant);
    RGBA B = meshdata.VertexB.colorData * percentageB;
    float percentageC = std::abs((determinants.AB) / totalDeterminant);
    RGBA C = meshdata.VertexC.colorData * percentageC;

    pixel.colorData = A + B + C;
}