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

constexpr float PIE = 3.141592;
constexpr float EPSILON = .0000001;

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
    Transformation OriginalTransform = transform;

    while (!done) {
        SDL_Event event;

        while (SDL_PollEvent(&event)) {
            if (event.type == SDL_EVENT_QUIT) {
                done = true;
            }
        }
        performanceAndMonitoring(frameBufferData);

        std::fill(frameBufferData.colorPixels.pixels.begin(), frameBufferData.colorPixels.pixels.end(), 0x000000FF);
        std::fill(frameBufferData.pixelDepth.depthVals.begin(), frameBufferData.pixelDepth.depthVals.end(), INFINITY_Render_Distance);
        // RendersMesh TorusMesh, SquareMesh, triangleMesh
        RenderMesh(TorusMesh, frameBufferData, transform, cameraTransf);
        drawPerformanceAndMonitoring(frameBufferData);
        // Cube animation
        float movementSpeed = 30.0f;
        float rotationSpeed = 15.0f;
        
        if (transform.position.z >= OriginalTransform.position.z +30)
        {
            moveaway = false;
        }
        else if (transform.position.z <= OriginalTransform.position.z -10)
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
    Vertex.position.y = y * std::cos((transform.rotation.x * PIE) / 180) - 
        z * std::sin((transform.rotation.x * PIE) / 180);
    Vertex.position.z = y * std::sin((transform.rotation.x * PIE) / 180) + 
        z * std::cos((transform.rotation.x * PIE) / 180);
    // Rotation around y (yaw):
    x = Vertex.position.x;
    y = Vertex.position.y;
    z = Vertex.position.z;
    Vertex.position.x = x * std::cos((transform.rotation.y * PIE) / 180) + 
        z * std::sin((transform.rotation.y * PIE) / 180);
    Vertex.position.y = y;
    Vertex.position.z = -x * std::sin((transform.rotation.y * PIE) / 180) + 
        z * std::cos((transform.rotation.y * PIE) / 180);
    // Rotation around Z (roll):
    x = Vertex.position.x;
    y = Vertex.position.y;
    z = Vertex.position.z;
    Vertex.position.x =  x * std::cos((transform.rotation.z * PIE) / 180) -
        y * std::sin((transform.rotation.z * PIE) / 180);
    Vertex.position.y =  x * std::sin((transform.rotation.z * PIE) / 180) +
        y * std::cos((transform.rotation.z * PIE) / 180);
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
    Vertex.position.x =  x * std::cos((-cameraTransf.CamRota.z * PIE) / 180) -
        y * std::sin((-cameraTransf.CamRota.z * PIE) / 180);
    Vertex.position.y =  x * std::sin((-cameraTransf.CamRota.z * PIE) / 180) +
        y * std::cos((-cameraTransf.CamRota.z * PIE) / 180);
    // Rotation around y (yaw):
    x = Vertex.position.x;
    y = Vertex.position.y;
    z = Vertex.position.z;
    Vertex.position.x = x * std::cos((-cameraTransf.CamRota.y * PIE) / 180) + 
        z * std::sin((-cameraTransf.CamRota.y * PIE) / 180);
    Vertex.position.y = y;
    Vertex.position.z = -x * std::sin((-cameraTransf.CamRota.y * PIE) / 180) + 
        z * std::cos((-cameraTransf.CamRota.y * PIE) / 180);
    // Rotation around X (pitch):
    x = Vertex.position.x;
    y = Vertex.position.y;
    z = Vertex.position.z;
    Vertex.position.x = x;
    Vertex.position.y = y * std::cos((-cameraTransf.CamRota.x * PIE) / 180) - 
        z * std::sin((-cameraTransf.CamRota.x * PIE) / 180);
    Vertex.position.z = y * std::sin((-cameraTransf.CamRota.x * PIE) / 180) + 
        z * std::cos((-cameraTransf.CamRota.x * PIE) / 180);
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
    // This Y flip is very important to note, it flips Winding logic
    // A lot of rework had to be done to reflect this single flip
    Vertex.position.y = -Vertex.position.y + (frameHeight / 2);
}


void RASTERIZE(auto &meshdata, framebuffer &frameBufferData)
{
    // For this data xyMinMax comes in the format of:
    // [0] = xmin [1] = ymin [2] = xmax [3] = ymax
    // Step 3
    BoundingBoxData xyMinMax = boundingBox(meshdata);
    // Top Left Fill Rule https://kristoffer-dyrkorn.github.io/triangle-rasterizer/4
    // determinantOffset can also be thought of as a true/false for IsTopleft true/false
    // IF is TOP or LEFT, function is TRUE and 
    bool ABisTopOrLeft = TopLeftFillFunc(meshdata.VertexA.position, meshdata.VertexB.position);
    bool BCisTopOrLeft = TopLeftFillFunc(meshdata.VertexB.position, meshdata.VertexC.position);
    bool CAisTopOrLeft = TopLeftFillFunc(meshdata.VertexC.position, meshdata.VertexA.position);
    // This p vector serves as the vector data to be filled in during the loop
    Vertex pixel;

    // Discard Straight-Line Triangles
    float testForDegenerateTriangle;
    testForDegenerateTriangle = getDeterminant(meshdata.VertexA.position, meshdata.VertexB.position, meshdata.VertexC.position);
    if (std::abs(testForDegenerateTriangle) < EPSILON)
    {
        return;
    }

    for (int y = xyMinMax.ymin; y <= xyMinMax.ymax; y++)
    {
        for (int x = xyMinMax.xmin; x <= xyMinMax.xmax; x++)
        {
            // update pixel values to current x,y coord
            pixel.position.x = x;
            pixel.position.y = y;

            Determinant determinants;
                determinants.AB = getDeterminant(meshdata.VertexA.position, meshdata.VertexB.position, pixel.position);
                determinants.BC = getDeterminant(meshdata.VertexB.position, meshdata.VertexC.position, pixel.position);
                determinants.CA = getDeterminant(meshdata.VertexC.position, meshdata.VertexA.position, pixel.position);
            
            /* For CCW in OBJECT/WORLD SPACE, a point inside of the triangle has all negative determinant values.
             For CW in OBJECT/WORLD SPACE, a point inside of the triangle has all positive determinant values.
             With my current implementation, the current windingModes reflect Object Space CCW/CW.
             This is not the same as my Screen Space CCW/CW winding, because Object/World Space works on a +x = right and +y = up,
             This is NOT the same in Screen Space, Why? Because screen space coordinates have 0,0 in the TOP LEFT of the screen therefore
             everything on screen is +x = right and +y = DOWN where a bigger Y value means its lower on the screen. So when we do toScreenSpace
             we actually have to flip the sign of the y value. This incidentally swaps the sign of determinant values. This creates a conundrum as
             Object/World space CCW/CW and ScreenSpace CCW/CW are opposites. There are a few ways to go about this, such as swapping the signs of
             the results from getDeterminants, but other functions such as TopLeftFillFunc relies on the determinant values. Ive decided to instead
             edit the Rasterizer's Switch and Case setup to calculats what is labeled CCW as CW and vice versa.
             So HERE CCW, a point inside of the triangle has all POSITIVE determinant values.
             HERE CW, a point inside of the triangle has all NEGATIVE determinant values.
            */
            switch (frameBufferData.WindingMode) // Step 6
            {
            case CW: // Points are NEGATIVE inside of the triangle
                // How the 0 : Epsilon Offset works In order to avoid potential conflict of two triangles claiming the SAME
                // pixel, we called the TopLeftFillFunc to determin if the line being check IE A->B/B->C/C->A is a TOP or Left edge.
                // If it IS, then it is INCLUSIONARY of 0. Therefore if (for example) ABisTopOrLeft is TRUE, determinants.AB <= 0 is
                // inclusionary so points with exactly 0 belong to that triangle. The Offset kicks in in order to make the cutoff slightly
                // NOT 0, so if the point lands on the line exactly it gets EXCLUDED. This needs to be NEGATIVE for SCREENVIEW CW (negative-inside raster case)
                // because SCREENVIEW CW in this case likes all NEGATIVE determinant points. The required sign is POSITIVE for SCREENVIEW CCW (positive-inside raster case).
                if ((determinants.AB <= (ABisTopOrLeft ? 0 : -EPSILON)) && (determinants.BC  <= (BCisTopOrLeft ? 0 : -EPSILON)) 
                && (determinants.CA <= (CAisTopOrLeft ? 0 : -EPSILON)))
                {
                    barycentrics(determinants, meshdata, pixel);
                    drawToBuffer(pixel, frameBufferData, meshdata);
                }
                break;
            
            case Both: // Points are POSITIVE or NEGATIVE inside of the triangle
                if (((determinants.AB >= (ABisTopOrLeft ? 0 : EPSILON)) && (determinants.BC  >= (BCisTopOrLeft ? 0 : EPSILON)) 
                && (determinants.CA >= (CAisTopOrLeft ? 0 : EPSILON))) || ((determinants.AB <= (ABisTopOrLeft ? 0 : -EPSILON)) 
                && (determinants.BC  <= (BCisTopOrLeft ? 0 : -EPSILON)) && (determinants.CA <= (CAisTopOrLeft ? 0 : -EPSILON))))
                {
                    barycentrics(determinants, meshdata, pixel);
                    drawToBuffer(pixel, frameBufferData, meshdata);
                }
                break;

            default: //default is CCW // Points are POSITIVE inside of the triangle
                if ((determinants.AB >= (ABisTopOrLeft ? 0 : EPSILON)) && (determinants.BC  >= (BCisTopOrLeft ? 0 : EPSILON)) 
                && (determinants.CA >= (CAisTopOrLeft ? 0 : EPSILON)))
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
BoundingBoxData boundingBox(auto &meshdata)
{
    // https://kristoffer-dyrkorn.github.io/triangle-rasterizer/1
    BoundingBoxData xyMinMax;
    xyMinMax.xmin = std::min(meshdata.VertexA.position.x, std::min(meshdata.VertexB.position.x, meshdata.VertexC.position.x));
    xyMinMax.xmin = (xyMinMax.xmin < 0) ? 0: xyMinMax.xmin;

    xyMinMax.ymin = std::min(meshdata.VertexA.position.y, std::min(meshdata.VertexB.position.y, meshdata.VertexC.position.y));
    xyMinMax.ymin = (xyMinMax.ymin < 0) ? 0: xyMinMax.ymin;

    xyMinMax.xmax = std::max(meshdata.VertexA.position.x, std::max(meshdata.VertexB.position.x, meshdata.VertexC.position.x));
    xyMinMax.xmax = (xyMinMax.xmax >= frameWidth) ? frameWidth - 1: xyMinMax.xmax;

    xyMinMax.ymax = std::max(meshdata.VertexA.position.y, std::max(meshdata.VertexB.position.y, meshdata.VertexC.position.y));
    xyMinMax.ymax = (xyMinMax.ymax >= frameHeight) ? frameHeight - 1: xyMinMax.ymax;
    return xyMinMax;
}
// Finding determinant-Offeset for the topleft fill rule
// Step 4
bool TopLeftFillFunc(Vector &start, Vector &end)
{
    float edge[2];
    edge[0] = end.x - start.x;
    edge[1] = end.y - start.y;
    bool isLeftEdge = edge[1] < 0;
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

// Takes pixel color values RGBA and turns them from floats to bits
// packed into a 32 bit packet of color info
std::uint32_t pixelPackager(RGBA &pixel)
{
    std::uint32_t packed_pixel {0};
    // Restrict color values to 0-255, this is done because BarycentricColoring
    // can potentially produce values greater than 255 or less than 0
    if (pixel.red   > 255){pixel.red   = 255;}
    if (pixel.green > 255){pixel.green = 255;}
    if (pixel.blue  > 255){pixel.blue  = 255;}
    if (pixel.alpha > 255){pixel.alpha = 255;}
    if (pixel.red   < 0){pixel.red   = 0;}
    if (pixel.green < 0){pixel.green = 0;}
    if (pixel.blue  < 0){pixel.blue  = 0;}
    if (pixel.alpha < 0){pixel.alpha = 0;}

    packed_pixel |= static_cast<std::uint32_t>(pixel.red)   << 24;
    packed_pixel |= static_cast<std::uint32_t>(pixel.green) << 16;
    packed_pixel |= static_cast<std::uint32_t>(pixel.blue)  <<  8;
    packed_pixel |= static_cast<std::uint32_t>(pixel.alpha) <<  0;
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
    float percentageA = std::abs((determinants.BC) / totalDeterminant);
    float percentageB = std::abs((determinants.CA) / totalDeterminant);
    float percentageC = std::abs((determinants.AB) / totalDeterminant);
    RGBA colorOverZA = meshdata.VertexA.colorData / meshdata.VertexA.position.z;
    RGBA colorOverZB = meshdata.VertexB.colorData / meshdata.VertexB.position.z;
    RGBA colorOverZC = meshdata.VertexC.colorData / meshdata.VertexC.position.z;

    RGBA interpolatedColorOverZ = colorOverZA * percentageA + colorOverZB * percentageB + colorOverZC * percentageC;

    float inverseZ = (1.0 / meshdata.VertexA.position.z) * percentageA +
        (1.0 / meshdata.VertexB.position.z) * percentageB +
        (1.0 / meshdata.VertexC.position.z) * percentageC;

    pixel.colorData = interpolatedColorOverZ / inverseZ;
}

void barycentricZ(Determinant &determinants, auto &meshdata, Vertex &pixel)
{
    float totalDeterminant = determinants.AB + determinants.BC + determinants.CA;
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
