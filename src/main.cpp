#include <SDL3/SDL.h>
#include <SDL3/SDL_main.h>
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
#include <structsANDoperators.hpp>
#include <textsystem.hpp>
#include <perfANDmonitoring.hpp>
#include <testmeshes.hpp>
#include <temp.hpp>
// end

constexpr float pie = 3.141592;

//TEMP TEST
Mesh2d TorusMesh = makeTorus();

// Takes in mesh data and sends it to rasterizer
void RenderMesh(Mesh2d const &mesh, framebuffer &frameBufferData, Transformation const &transform, Camera const &cameraTransf);
void toWorldSpace(Vertex &Vertex, Transformation const &transform);
void toCameraSpace(Vertex &Vertex, Camera const &cameraTransf);
std::vector<triangle> nearPlaneClipping(triangle &triangleBuffer);
void processEdge(Vertex &Vertex1, Vertex &Vertex2, std::vector<Vertex> &clippedTriangle);
void perspectiveProjection(Vertex &Vertex);
void toScreenSpace(Vertex &Vertex);
BoundingBoxData boundingBox(auto &meshdata);
bool TopLeftFillFunc(Vector &start, Vector &end);
float getDeterminant(Vector &VertexA, Vector &VertexB, Vector &pointC);
void barycentrics(Determinant &determinants, auto &meshdata, Vertex &pixel);
void barycentricColor(Determinant &determinants, auto &meshdata, Vertex &pixel);
void barycentricZ(Determinant &determinants, auto &meshdata, Vertex &pixel);
bool depthTest(framebuffer &buffer, Vector &pixel);
void drawToBuffer(Vertex &pixel, framebuffer &frameBufferData, auto &meshdata);
void RASTERIZE(auto &meshdata, framebuffer &frameBufferData);
void FramePackager(Vertex &pixel, framebuffer &buffer, auto &meshdata);
std::uint32_t pixelPackager(RGBA &pixel);
void fillpixelcolor(RGBA &pixel, auto &meshdata);
void performanceAndMonitoring(framebuffer &buffer);


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
        "Rasterizer",                  // window title
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

    framebuffer frameBufferData;
    // Currently acts as a preset buffer to write pixels to before displaying
    // SDL wants is a pointer to the first actual pixel element stored inside the vector. For std::vector, the standard way to get that contiguous memory pointer is .data()
    SDL_Surface * surface = SDL_CreateSurfaceFrom(width, height, SDL_PIXELFORMAT_RGBA8888, frameBufferData.colorPixels.pixels.data(), frameWidth *4);
    if (surface == nullptr)
    {
        SDL_LogError(SDL_LOG_CATEGORY_ERROR, "Could not create surface: %s\n", SDL_GetError());
        return 1;
    }

    // TEMP
    bool moveaway = true;

    while (!done) {
        SDL_Event event;

        while (SDL_PollEvent(&event)) {
            if (event.type == SDL_EVENT_QUIT) {
                done = true;
            }
        }
        performanceAndMonitoring(frameBufferData);

        std::fill(frameBufferData.colorPixels.pixels.begin(), frameBufferData.colorPixels.pixels.end(), 0x000000FF);
        std::fill(frameBufferData.pixelDepth.depthVals.begin(), frameBufferData.pixelDepth.depthVals.end(), 1000.0f);
        // RendersMesh
        RenderMesh(TorusMesh, frameBufferData, transform, cameraTransf);
        drawPerformanceAndMonitoring(frameBufferData);
        // Cube animation
        float movementSpeed = 30.0f;
        float rotationSpeed = 15.0f;
        if (transform.position.z >= 30)
        {
            moveaway = false;
        }
        else if (transform.position.z <= -10)
        {
            moveaway = true;
        }
        if (moveaway == true)
        {
            transform.position.z += movementSpeed * deltaTime;
        }
        if (moveaway == false)
        {
            transform.position.z -= movementSpeed * deltaTime;
        }
        transform.rotation.x += rotationSpeed * deltaTime;
        transform.rotation.y += rotationSpeed * deltaTime;
        transform.rotation.z += rotationSpeed * deltaTime;

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


void RenderMesh(Mesh2d const &mesh, framebuffer &frameBufferData, Transformation const &transform, Camera const &cameraTransf)
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

        toWorldSpace(triangleBuffer.VertexA, transform);
        toWorldSpace(triangleBuffer.VertexB, transform);
        toWorldSpace(triangleBuffer.VertexC, transform);

        toCameraSpace(triangleBuffer.VertexA, cameraTransf);
        toCameraSpace(triangleBuffer.VertexB, cameraTransf);
        toCameraSpace(triangleBuffer.VertexC, cameraTransf);        

        std::vector<triangle> clippedTriangles = nearPlaneClipping(triangleBuffer);
        if (clippedTriangles.size() == 0)
        {
            continue;
        }

        for (int j = 0; j < clippedTriangles.size(); j++)
        {
        perspectiveProjection(clippedTriangles[j].VertexA);
        perspectiveProjection(clippedTriangles[j].VertexB);
        perspectiveProjection(clippedTriangles[j].VertexC);

        toScreenSpace(clippedTriangles[j].VertexA);        
        toScreenSpace(clippedTriangles[j].VertexB);
        toScreenSpace(clippedTriangles[j].VertexC);

        RASTERIZE(clippedTriangles[j], frameBufferData);
        }
    }
}
// Apply Transformation Properties
// double check rotation for rotating around the middle pixel rather than rotating around where the triange actually should be
// Consider calculating the sin and cos before the function rather than having to redo the calculation every time
void toWorldSpace(Vertex &Vertex, Transformation const &transform)
{
    // For Scale
    Vertex.position = Vertex.position * transform.scale;
    // For Rotation
    float x;
    float y;
    float z;
    // Rotation around X (pitch):
    x = Vertex.position.x;
    y = Vertex.position.y;
    z = Vertex.position.z;
    Vertex.position.x = x;
    Vertex.position.y = y * std::cos((transform.rotation.x * pie) / 180) - 
        z * std::sin((transform.rotation.x * pie) / 180);
    Vertex.position.z = y * std::sin((transform.rotation.x * pie) / 180) + 
        z * std::cos((transform.rotation.x * pie) / 180);
    // Rotation around y (yaw):
    x = Vertex.position.x;
    y = Vertex.position.y;
    z = Vertex.position.z;
    Vertex.position.x = x * std::cos((transform.rotation.y * pie) / 180) + 
        z * std::sin((transform.rotation.y * pie) / 180);
    Vertex.position.y = y;
    Vertex.position.z = -x * std::sin((transform.rotation.y * pie) / 180) + 
        z * std::cos((transform.rotation.y * pie) / 180);
    // Rotation around Z (roll):
    x = Vertex.position.x;
    y = Vertex.position.y;
    z = Vertex.position.z;
    Vertex.position.x =  x * std::cos((transform.rotation.z * pie) / 180) -
        y * std::sin((transform.rotation.z * pie) / 180);
    Vertex.position.y =  x * std::sin((transform.rotation.z * pie) / 180) +
        y * std::cos((transform.rotation.z * pie) / 180);
    Vertex.position.z = z;
    // For Position
    Vertex.position =  Vertex.position + transform.position;
}

// This functions is basically the inverse of transformVerPos
void toCameraSpace(Vertex &Vertex, Camera const &cameraTransf)
{
    // For Position
    Vertex.position = Vertex.position - cameraTransf.CamPos;    
    // For Rotation
    float x;
    float y;
    float z;

    // Rotation around Z (roll):
    x = Vertex.position.x;
    y = Vertex.position.y;
    z = Vertex.position.z;
    Vertex.position.x =  x * std::cos((-cameraTransf.CamRota.z * pie) / 180) -
        y * std::sin((-cameraTransf.CamRota.z * pie) / 180);
    Vertex.position.y =  x * std::sin((-cameraTransf.CamRota.z * pie) / 180) +
        y * std::cos((-cameraTransf.CamRota.z * pie) / 180);
    // Rotation around y (yaw):
    x = Vertex.position.x;
    y = Vertex.position.y;
    z = Vertex.position.z;
    Vertex.position.x = x * std::cos((-cameraTransf.CamRota.y * pie) / 180) + 
        z * std::sin((-cameraTransf.CamRota.y * pie) / 180);
    Vertex.position.y = y;
    Vertex.position.z = -x * std::sin((-cameraTransf.CamRota.y * pie) / 180) + 
        z * std::cos((-cameraTransf.CamRota.y * pie) / 180);
    // Rotation around X (pitch):
    x = Vertex.position.x;
    y = Vertex.position.y;
    z = Vertex.position.z;
    Vertex.position.x = x;
    Vertex.position.y = y * std::cos((-cameraTransf.CamRota.x * pie) / 180) - 
        z * std::sin((-cameraTransf.CamRota.x * pie) / 180);
    Vertex.position.z = y * std::sin((-cameraTransf.CamRota.x * pie) / 180) + 
        z * std::cos((-cameraTransf.CamRota.x * pie) / 180);
}

std::vector<triangle> nearPlaneClipping(triangle &triangleBuffer)
{
    std::vector<Vertex> clippedVertices;

    processEdge(triangleBuffer.VertexA, triangleBuffer.VertexB, clippedVertices);
    processEdge(triangleBuffer.VertexB, triangleBuffer.VertexC, clippedVertices);
    processEdge(triangleBuffer.VertexC, triangleBuffer.VertexA, clippedVertices);

    if (clippedVertices.size() == 3)
    {
        triangleBuffer.VertexA = clippedVertices[0];
        triangleBuffer.VertexB = clippedVertices[1];
        triangleBuffer.VertexC = clippedVertices[2];
        std::vector<triangle> clippedTriangle;
        clippedTriangle.emplace_back(triangleBuffer);
        return clippedTriangle;
    }
    
    if (clippedVertices.size() == 4)
    {
        triangleBuffer.VertexA = clippedVertices[0];
        triangleBuffer.VertexB = clippedVertices[1];
        triangleBuffer.VertexC = clippedVertices[2];
        triangle clippedTriangle2;
        clippedTriangle2.VertexA = clippedVertices[2];
        clippedTriangle2.VertexB = clippedVertices[3];
        clippedTriangle2.VertexC = clippedVertices[0];
        std::vector<triangle> clippedTriangles;        
        clippedTriangles.emplace_back(triangleBuffer);
        clippedTriangles.emplace_back(clippedTriangle2);
        return clippedTriangles;
    }

    // if clippedVertices.size() == 0;
    std::vector<triangle> clippedTriangle;
    return clippedTriangle;

}

void processEdge(Vertex &Vertex1, Vertex &Vertex2, std::vector<Vertex> &clippedTriangle)
{
    float nearPlane = 1.0f;
    bool p1 = (Vertex1.position.z >= nearPlane);
    bool p2 = (Vertex2.position.z >= nearPlane);
    /*V1→V2 : IN→IN  V1→V2 : IN→OUT  V1→V2 : OUT→IN       OUT->OUT
          add V2       add newp1p2   add newp1p2, then V2   add nothing*/
    if (p1 == true && p2 == true)
    {
        clippedTriangle.emplace_back(Vertex2);
        return;
    }
    if (p1 == false && p2 == true)
    {
        // t = (nearPlane - Az) / (Bz - Az)    
        float tp1p2 = (nearPlane - Vertex1.position.z) / (Vertex2.position.z - Vertex1.position.z);
        /* newAB.x = A.x + tAB * (B.x - A.x)
        newAB.y = A.y + tAB * (B.y - A.y)
        newAB.z = A.z + tAB * (B.z - A.z) */
        Vertex newp1p2;
        newp1p2.position.x = Vertex1.position.x + tp1p2 * (Vertex2.position.x - Vertex1.position.x);
        newp1p2.position.y = Vertex1.position.y + tp1p2 * (Vertex2.position.y - Vertex1.position.y);
        newp1p2.position.z = nearPlane;
        newp1p2.colorData = Vertex1.colorData + tp1p2 * (Vertex2.colorData - Vertex1.colorData);
        clippedTriangle.emplace_back(newp1p2);
        clippedTriangle.emplace_back(Vertex2);
        return;
    }
    if (p1 == true && p2 == false)
    {
        float tp1p2 = (nearPlane - Vertex1.position.z) / (Vertex2.position.z - Vertex1.position.z);
        Vertex newp1p2;
        newp1p2.position.x = Vertex1.position.x + tp1p2 * (Vertex2.position.x - Vertex1.position.x);
        newp1p2.position.y = Vertex1.position.y + tp1p2 * (Vertex2.position.y - Vertex1.position.y);
        newp1p2.position.z = nearPlane;
        newp1p2.colorData = Vertex1.colorData + tp1p2 * (Vertex2.colorData - Vertex1.colorData);
        clippedTriangle.emplace_back(newp1p2);
        return;
    }
    if (p1 == false && p2 == false)
    {
        return;
    }
}

void perspectiveProjection(Vertex &Vertex)
{
    // arbitrary temp value
    float FOCALPOINT = 300;
    // Adds Depth so objects get smaller depending on the Z value
    // Focalpoint currently acts as a zoom value 
    Vertex.position.x = (Vertex.position.x / Vertex.position.z) * FOCALPOINT;
    Vertex.position.y = (Vertex.position.y / Vertex.position.z) * FOCALPOINT;
}

void toScreenSpace(Vertex &Vertex)
{
    // Converting to Screen Coordinates aka centers object
    Vertex.position.x = Vertex.position.x + (frameWidth / 2);
    Vertex.position.y = Vertex.position.y + (frameHeight / 2);
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
    Vertex pixel;
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
                    barycentrics(determinants, meshdata, pixel);
                    drawToBuffer(pixel, frameBufferData, meshdata);
                }
                break;
            
            case Both:
                if (((determinants.AB >= (detABoffset ? 0 : .00001)) && (determinants.BC  >= (detBCoffset ? 0 : .00001)) 
                && (determinants.CA >= (detCAoffset ? 0 : .00001))) || ((determinants.AB <= (detABoffset ? -.00001 : 0)) 
                && (determinants.BC  <= (detBCoffset ? -.00001 : 0)) && (determinants.CA <= (detCAoffset ? -.00001 : 0))))
                {
                    barycentrics(determinants, meshdata, pixel);
                    drawToBuffer(pixel, frameBufferData, meshdata);
                }
                break;

            default: //default is CCW
                if ((determinants.AB >= (detABoffset ? 0 : .00001)) && (determinants.BC  >= (detBCoffset ? 0 : .00001)) 
                && (determinants.CA >= (detCAoffset ? 0 : .00001)))
                {
                    barycentrics(determinants, meshdata, pixel);
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
    xyMinMax.xmin = (xyMinMax.xmin < 0) ? 0: xyMinMax.xmin;

    xyMinMax.ymin = std::min(meshdata.VertexA.position.y, std::min(meshdata.VertexB.position.y, meshdata.VertexC.position.y));
    xyMinMax.ymin = (xyMinMax.ymin < 0) ? 0: xyMinMax.ymin;

    xyMinMax.xmax = std::max(meshdata.VertexA.position.x, std::max(meshdata.VertexB.position.x, meshdata.VertexC.position.x));
    xyMinMax.xmax = (xyMinMax.xmax > frameWidth) ? frameWidth - 1: xyMinMax.xmax;

    xyMinMax.ymax = std::max(meshdata.VertexA.position.y, std::max(meshdata.VertexB.position.y, meshdata.VertexC.position.y));
    xyMinMax.ymax = (xyMinMax.ymax > frameHeight) ? frameHeight - 1: xyMinMax.ymax;
    return xyMinMax;
}
// Finding determinant-Offeset for the topleft fill rule
// Step 4
bool TopLeftFillFunc(Vector &start, Vector &end)
{
    float edge[2];
    edge[0] = end.x - start.x;
    edge[1] = end.y - start.y;
    bool isLeftEdge = edge[1] > 0;
    bool isTopEdge  = edge[1] == 0 && edge[0] < 0;
    return isLeftEdge || isTopEdge;
}

// Step 5
float getDeterminant(Vector &VertexA, Vector &VertexB, Vector &pointC)
{
    // https://kristoffer-dyrkorn.github.io/triangle-rasterizer/1
    // ab and ac both could be converted into Vector types but Im choosing not to
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
void drawToBuffer(Vertex &pixel, framebuffer &frameBufferData, auto &meshdata)
{
        // checking for out of bounds indice
        if (pixel.position.x >= frameWidth || pixel.position.y >= frameHeight || pixel.position.x < 0 || pixel.position.y < 0)
        {
            return;
        }
        bool shouldDraw = depthTest(frameBufferData, pixel.position);
        if (!shouldDraw)
        {
            return;
        }
        FramePackager(pixel, frameBufferData, meshdata);
}

// step 10
void FramePackager(Vertex &pixel, framebuffer &buffer, auto &meshdata)
{
    std::uint32_t packed_pixel;
    // Pixel packer needs to come back
    packed_pixel = pixelPackager(pixel.colorData);
    // converts x, y screen coords to pixel indice and used to be a helper function
    std::uint32_t indice = pixel.position.x + (pixel.position.y * frameWidth);
    buffer.colorPixels.pixels[indice] = packed_pixel;
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

void barycentrics(Determinant &determinants, auto &meshdata, Vertex &pixel)
{
    barycentricColor(determinants, meshdata, pixel);
    barycentricZ(determinants, meshdata, pixel);
}

// barycentric coordinates
void barycentricColor(Determinant &determinants, auto &meshdata, Vertex &pixel)
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

void barycentricZ(Determinant &determinants, auto &meshdata, Vertex &pixel)
{
    float totalDeterminant = determinants.AB + determinants.BC + determinants.CA;
    if (totalDeterminant == 0)
    {
        totalDeterminant = 1;
    }
    float percentageA = std::abs((determinants.BC) / totalDeterminant);
    float percentageB = std::abs((determinants.CA) / totalDeterminant);
    float percentageC = std::abs((determinants.AB) / totalDeterminant);
    float ZA = 1.0f / meshdata.VertexA.position.z;
    float ZB = 1.0f / meshdata.VertexB.position.z;
    float ZC = 1.0f / meshdata.VertexC.position.z;

    float inverseZ = ZA * percentageA + ZB * percentageB + ZC * percentageC;

    pixel.position.z = 1.0f / inverseZ;
}

bool depthTest(framebuffer &buffer, Vector &pixel)
{
    if (buffer.pixelDepth.depthVals[static_cast<int>(pixel.x) + (static_cast<int>(pixel.y) * frameWidth)] > pixel.z)
    {
        buffer.pixelDepth.depthVals[static_cast<int>(pixel.x) + (static_cast<int>(pixel.y) * frameWidth)] = pixel.z;
        return true;
    }
    return false;
}
