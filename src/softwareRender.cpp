// Used for Window Creation and Input reading
#include <SDL3/SDL.h>
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
#include <loaders/objLoader.hpp>
#include <softwareRender/textsystem.hpp>
#include <softwareRender/perfANDmonitoring.hpp>
#include <softwareRender/mouseANDkeyboard.hpp>
#include <logs/logger.hpp>
// end


constexpr float EPSILON = .0000001;

// Takes in mesh data and sends it to rasterizer
void RenderModel(LoadedModel const &model, framebuffer &frameBufferData, Transformation const &transform, Camera &cameraTransf);
void RenderMesh(Mesh2d const &mesh, framebuffer &frameBufferData, Texture const &texture, 
    Matrix4x4 const &toWorldSpaceMatrix, Matrix4x4 const &NormalMatrix, Matrix4x4 const &viewMatrix, Matrix4x4 const &projectionMatrix);
Matrix4x4 createWorldSpaceMatrix(Transformation const &transform);
Matrix4x4 createNormalMatrix( Transformation const &transform);
Matrix4x4 createCameraSpaceMatrix(Camera const &cameraTransf);
void toWorldSpace(Vector4D &vector4DA, Matrix4x4 const &toWorldMatrix);
void toWorldSpaceNormal(Vector &vectorA, Matrix4x4 const &NormalMatrix);
void toCameraSpace(Vector4D &vector4DA, Matrix4x4 const &toCameraMatrix);
float planeValue(Vector4D const &position, ClipPlanes plane);
void toClipSpace(Vector4D &vector4DA, Matrix4x4 const &ClipSpaceMatrix);
void processEdgeClip(Vertex4D const &Vertex1, Vertex4D const &Vertex2, std::vector<Vertex4D> &clippedVertices, ClipPlanes plane);
std::vector<Vertex4D> clipPolygonAgainstPlane(std::vector<Vertex4D> const &polygon, ClipPlanes plane);
std::vector<Vertex4D> clipPolygonAgainstFrustum(std::vector<Vertex4D> polygon);
Vertex toNDC(Vertex4D const &Vertex4d);
std::vector<triangle> triangulatePolygon(std::vector<Vertex> const &polygon);
void toScreenSpace(triangle &triangle);
BoundingBoxData boundingBox(auto &meshdata);
bool TopLeftFillFunc(Vector &start, Vector &end);
float getDeterminant(Vector &VertexA, Vector &VertexB, Vector &pointC);
void barycentricColor(Determinant &determinants, auto &meshdata, Vertex &pixel, 
    float totalDeterminant, float percentA, float percentB, float percentC, float interpolatedInverseW);
void barycentricNormal(Determinant &determinants, auto &meshdata, Vertex &pixel, 
    float totalDeterminant, float percentA, float percentB, float percentC, float interpolatedInverseW);
void barycentricUV(Determinant &determinants, auto &meshdata, Vertex &pixel, 
    float totalDeterminant, float percentA, float percentB, float percentC, float interpolatedInverseW);
void barycentricZ(Determinant &determinants, auto &meshdata, Vertex &pixel, 
    float totalDeterminant, float percentA, float percentB, float percentC);
bool depthTest(framebuffer &buffer, Vector &pixel);
void drawToBuffer(Vertex &pixel, framebuffer &frameBufferData, auto &meshdata);
void RASTERIZE(auto &meshdata, framebuffer &frameBufferData, Texture const &texture);
void calculatelighting(Vertex &pixel, Vector const &directionToLight);
void FramePackager(Vertex &pixel, framebuffer &buffer, auto &meshdata);
std::uint32_t pixelPackager(RGBA &pixel);
//
void performanceAndMonitoring(SETTINGS &settings);
void drawPerformanceAndMonitoring(SETTINGS &settings, framebuffer &buffer);
//
RGBA sampleTexture(Texture const &texture, VectorUV const &UV);


void RenderModel(LoadedModel const &model, framebuffer &frameBufferData, Transformation const &transform, Camera &cameraTransf)
{
    Matrix4x4 toWorldSpaceMatrix  = createWorldSpaceMatrix(transform);
    Matrix4x4 NormalMatrix  = createNormalMatrix(transform);
    if (cameraTransf.viewUpdater)
    {
        cameraTransf.viewMatrix = createCameraSpaceMatrix(cameraTransf);
        cameraTransf.viewUpdater = false;
    }
    if (cameraTransf.projectionUpdater)
    {
        cameraTransf.projectionMatrix = toClipSpaceMatrix(cameraTransf);
        cameraTransf.projectionUpdater = false;
    }
    for (SubMesh const &submesh : model.subMeshes)
    {
        if (submesh.mesh.Indices.empty())
        {
            continue;
        }
        int materialIndex = submesh.materialIndex;
        Material const &material = model.materials[materialIndex];

        RenderMesh(submesh.mesh, frameBufferData, material.diffuseTexture, toWorldSpaceMatrix, NormalMatrix, cameraTransf.viewMatrix, cameraTransf.projectionMatrix);
    }
}

void RenderMesh(Mesh2d const &mesh, framebuffer &frameBufferData, Texture const &texture, Matrix4x4 const &toWorldSpaceMatrix, Matrix4x4 const &NormalMatrix, Matrix4x4 const &viewMatrix, Matrix4x4 const &projectionMatrix)
{
    if (mesh.Indices.size() % 3 != 0)
    {
        return;
    }
    std::vector<Vertex4D> transformedVertices {};
    transformedVertices.reserve(mesh.Vertices.size());

    for (Vertex const &vertex : mesh.Vertices)
    {
        Vertex4D vertex4d {};
        vertex4d.position = toVector4D(vertex.position);
        vertex4d.normal = vertex.normal;
        vertex4d.UV = vertex.UV;
        vertex4d.colorData = vertex.colorData;
        toWorldSpace(vertex4d.position, toWorldSpaceMatrix);
        toWorldSpaceNormal(vertex4d.normal, NormalMatrix);
        toCameraSpace(vertex4d.position, viewMatrix);
        toClipSpace(vertex4d.position, projectionMatrix);
        transformedVertices.emplace_back(vertex4d);
    }

    for(int i = 0; i < (mesh.Indices.size()); i+=3)
    {
        triangle4D triangleBuffer {0};
        triangleBuffer.Vertex4DA = transformedVertices[mesh.Indices[i]];
        triangleBuffer.Vertex4DB = transformedVertices[mesh.Indices[i+1]];
        triangleBuffer.Vertex4DC = transformedVertices[mesh.Indices[i+2]];

        std::vector<Vertex4D> polygon {};
        polygon.emplace_back(triangleBuffer.Vertex4DA);
        polygon.emplace_back(triangleBuffer.Vertex4DB);
        polygon.emplace_back(triangleBuffer.Vertex4DC);

        // Under Construction
        std::vector<Vertex4D> clippedPolygon = clipPolygonAgainstFrustum(polygon);

        std::vector<Vertex> NDCclippedPolygon;
        for (int i = 0; i < clippedPolygon.size(); i++)
        {
            NDCclippedPolygon.emplace_back(toNDC(clippedPolygon[i]));
        }

        std::vector<triangle> triangles = triangulatePolygon(NDCclippedPolygon);
        for (int i = 0; i < triangles.size(); i++)
        {
        triangle triangle3D = triangles[i];
        toScreenSpace(triangle3D);
        RASTERIZE(triangle3D, frameBufferData, texture);
        }
    }
}

// New implementation of Converting from Object Space to World space using transform Matrices
Matrix4x4 createWorldSpaceMatrix(Transformation const &transform)
{
    // Declarations For Transformation Matrices
    Matrix4x4 Scale;
    Matrix4x4 RotationX;
    Matrix4x4 RotationY;
    Matrix4x4 RotationZ;
    Matrix4x4 Translation;
    Scale = toScaleMatrix(BaseMatrix, transform.scale.x, transform.scale.y, transform.scale.z);
    RotationX = toRotateXMatrix(BaseMatrix, transform.rotation.x);
    RotationY = toRotateYMatrix(BaseMatrix, transform.rotation.y);
    RotationZ = toRotateZMatrix(BaseMatrix, transform.rotation.z);
    Translation = toTranslationMatrix(BaseMatrix, transform.position.x, transform.position.y, transform.position.z);
    Matrix4x4 toWorldMatrix = (Translation *(RotationZ *(RotationY *(RotationX * Scale))));
    return toWorldMatrix;
}

Matrix4x4 createNormalMatrix( Transformation const &transform)
{
    Matrix4x4 InverseScale;
    Matrix4x4 RotationX;
    Matrix4x4 RotationY;
    Matrix4x4 RotationZ;
    InverseScale = toInverseScaleMatrix(BaseMatrix, transform.scale.x, transform.scale.y, transform.scale.z);
    RotationX = toRotateXMatrix(BaseMatrix, transform.rotation.x);
    RotationY = toRotateYMatrix(BaseMatrix, transform.rotation.y);
    RotationZ = toRotateZMatrix(BaseMatrix, transform.rotation.z);
    Matrix4x4 NormalMatrix = RotationZ *(RotationY *(RotationX * InverseScale));
    return NormalMatrix;
}

Matrix4x4 createCameraSpaceMatrix(Camera const &cameraTransf)
{
    // Declarations For Transformation Matrices
    Matrix4x4 RotationX;
    Matrix4x4 RotationY;
    Matrix4x4 RotationZ;
    Matrix4x4 Translation;
    // The following functions have negative values because Camera transformations are inverse transformations
    // IE if you look right, the world moves left and vice versa. Same with up and down
    RotationX = toRotateXMatrix(BaseMatrix, -cameraTransf.rotation.x);
    RotationY = toRotateYMatrix(BaseMatrix, -cameraTransf.rotation.y);
    RotationZ = toRotateZMatrix(BaseMatrix, -cameraTransf.rotation.z);
    Translation = toTranslationMatrix(BaseMatrix, -cameraTransf.position.x, -cameraTransf.position.y, -cameraTransf.position.z);
    Matrix4x4 toCamMatrix  = (RotationX *(RotationY *(RotationZ * Translation)));
    return toCamMatrix;
}

void toWorldSpace(Vector4D &vector4DA, Matrix4x4 const &toWorldMatrix)
{
    vector4DA = toWorldMatrix * vector4DA;
}

void toWorldSpaceNormal(Vector &vectorA, Matrix4x4 const &NormalMatrix)
{
    Vector4D vector4DA = NormaltoVector4D(vectorA);
    vector4DA = NormalMatrix * vector4DA;
    vectorA = normalize(toVector(vector4DA));
}

void toCameraSpace(Vector4D &vector4DA, Matrix4x4 const &toCameraMatrix)
{
    vector4DA = toCameraMatrix * vector4DA;
}

void toClipSpace(Vector4D &vector4DA, Matrix4x4 const &ClipSpaceMatrix)
{
    vector4DA = ClipSpaceMatrix * vector4DA;
}

float planeValue(Vector4D const &position, ClipPlanes plane)
{
    switch (plane)
    {
    case (ClipPlanes::left):
    // Logic: x >= -w
    // x + w >= 0
    return position.x + position.w;
    case (ClipPlanes::right):
    // Logic: x <= w
    // w - x >= 0
    return position.w - position.x;
    case (ClipPlanes::bottom):
    // Logic: y >= -w
    // y + w >= 0
    return position.y + position.w;
    case (ClipPlanes::top):
    // Logic: y <= w
    // w - y >= 0
    return position.w - position.y;
    case (ClipPlanes::near):
    // Logic: z >= 0
    return position.z;
    case (ClipPlanes::far):
    // Logic: z <= w
    // w - z >= 0
    return position.w - position.z;
    }
    return 0.0f;
}

void processEdgeClip(Vertex4D const &Vertex1, Vertex4D const &Vertex2, std::vector<Vertex4D> &clippedVertices, ClipPlanes plane)
{
    // Calculates a point's value relative to the selected plane left/right/bottom/top/etc...
    // Value >= 0 means inside/on the plane, Value < 0 means outside the plane
    float d1 = planeValue(Vertex1.position, plane);
    float d2 = planeValue(Vertex2.position, plane);
    // Because planeValue restructs the equations to be based on (W +/- Var >= 0),
    // we only need to compare d1/d2 to 0.0f
    // if d1 or d2 is less than 0 it is out of the chosen plane
    bool p1 = d1 >= 0.0f;
    bool p2 = d2 >= 0.0f;

    // Given whether (a) point(s) is inside or outside chooses which
    // logic is chosen in adding zero, one, or 2 vertices to the polygon

    // V1→V2 : IN→IN add V2
    if (p1 == true && p2 == true)
    {
        clippedVertices.emplace_back(Vertex2);
        return;
    }
    // V1→V2 : OUT→IN add newVertex, then V2
    if (p1 == false && p2 == true)
    {
        float t = d1 / (d1 - d2);
        // newAB.x = A.x + t * (B.x - A.x)
        // newAB.y = A.y + t * (B.y - A.y)
        // newAB.z = A.z + t * (B.z - A.z)
        // newAB.w = A.w + t * (B.w - A.w)
        Vertex4D newVertex{};
        newVertex.position.x = Vertex1.position.x + (t * (Vertex2.position.x - Vertex1.position.x));
        newVertex.position.y = Vertex1.position.y + (t * (Vertex2.position.y - Vertex1.position.y));
        newVertex.position.z = Vertex1.position.z + (t * (Vertex2.position.z - Vertex1.position.z));
        newVertex.position.w = Vertex1.position.w + (t * (Vertex2.position.w - Vertex1.position.w));
        newVertex.colorData  = Vertex1.colorData  + (t * (Vertex2.colorData  - Vertex1.colorData));
        newVertex.normal = Vertex1.normal + (t * (Vertex2.normal - Vertex1.normal));
        newVertex.UV = Vertex1.UV + (t * (Vertex2.UV - Vertex1.UV));
        clippedVertices.emplace_back(newVertex);
        clippedVertices.emplace_back(Vertex2);
        return;
    }
    // V1→V2 : IN→OUT add newVertex
    if (p1 == true && p2 == false)
    {
        float t = d1 / (d1 - d2);
        Vertex4D newVertex{};
        newVertex.position.x = Vertex1.position.x + t * (Vertex2.position.x - Vertex1.position.x);
        newVertex.position.y = Vertex1.position.y + t * (Vertex2.position.y - Vertex1.position.y);
        newVertex.position.z = Vertex1.position.z + t * (Vertex2.position.z - Vertex1.position.z);
        newVertex.position.w = Vertex1.position.w + t * (Vertex2.position.w - Vertex1.position.w);
        newVertex.colorData  = Vertex1.colorData  + t * (Vertex2.colorData  - Vertex1.colorData);
        newVertex.normal = Vertex1.normal + (t * (Vertex2.normal - Vertex1.normal));
        newVertex.UV = Vertex1.UV + (t * (Vertex2.UV - Vertex1.UV));
        clippedVertices.emplace_back(newVertex);
        return;
    }
    // OUT->OUT add nothing
    if (p1 == false && p2 == false)
    {
        return;
    }
}

std::vector<Vertex4D> clipPolygonAgainstPlane(std::vector<Vertex4D> const &polygon, ClipPlanes plane)
{
    std::vector<Vertex4D> clippedVertices;
    if (polygon.empty())
    {
        return clippedVertices;
    }
    for (int i = 0; i < polygon.size(); i++)
    {
        int next = (i + 1) % polygon.size();
        processEdgeClip(polygon[i], polygon[next], clippedVertices, plane);
    }
    return clippedVertices;
}

std::vector<Vertex4D> clipPolygonAgainstFrustum(std::vector<Vertex4D> polygon)
{
    polygon = clipPolygonAgainstPlane(polygon, ClipPlanes::left);
        if (polygon.empty()){return polygon;}
    polygon = clipPolygonAgainstPlane(polygon, ClipPlanes::right);
        if (polygon.empty()){return polygon;}
    polygon = clipPolygonAgainstPlane(polygon, ClipPlanes::bottom);
        if (polygon.empty()){return polygon;}
    polygon = clipPolygonAgainstPlane(polygon, ClipPlanes::top);
        if (polygon.empty()){return polygon;}
    polygon = clipPolygonAgainstPlane(polygon, ClipPlanes::near);
        if (polygon.empty()){return polygon;}
    polygon = clipPolygonAgainstPlane(polygon, ClipPlanes::far);
        return polygon;
}

Vertex toNDC(Vertex4D const &Vertex4d)
{
    Vertex Vertex3d {};
    Vertex3d.inverseW = 1.0f / Vertex4d.position.w;
    Vertex3d.position.x = Vertex4d.position.x * Vertex3d.inverseW;
    Vertex3d.position.y = Vertex4d.position.y * Vertex3d.inverseW;
    Vertex3d.position.z = Vertex4d.position.z * Vertex3d.inverseW;
    Vertex3d.colorData = Vertex4d.colorData;
    Vertex3d.normal = Vertex4d.normal;
    Vertex3d.UV = Vertex4d.UV;
    return Vertex3d;
}

void toScreenSpace(triangle &triangle)
{
    // Screenspace Grid works with (0,0) as the top left of the screen
    // NDC uses y+ as upward, whereas in ScreenSpace y+ is downward
    // therefore y is special in the fact it needs its sign (+) flipped (-)
    // in order to reverse the behavior of y
    triangle.VertexA.position.x =  (triangle.VertexA.position.x + 1) * 0.5f * settings.frameWidth;
    triangle.VertexB.position.x =  (triangle.VertexB.position.x + 1) * 0.5f * settings.frameWidth;
    triangle.VertexC.position.x =  (triangle.VertexC.position.x + 1) * 0.5f * settings.frameWidth;
    // Add note about y in screen space is turned negative
    triangle.VertexA.position.y =  (1 - triangle.VertexA.position.y) * 0.5f * settings.frameHeight;
    triangle.VertexB.position.y =  (1 - triangle.VertexB.position.y) * 0.5f * settings.frameHeight;
    triangle.VertexC.position.y =  (1 - triangle.VertexC.position.y) * 0.5f * settings.frameHeight;
    // Z is fine because NDC 0-1 stills works for depth
}

std::vector<triangle> triangulatePolygon(std::vector<Vertex> const &polygon)
{
    std::vector<triangle> triangles {};
        if (polygon.size() < 3){return triangles;}
    for (int i = 1; i + 1 < polygon.size(); i++)
    {
        triangle buffer {0};
        buffer.VertexA = polygon[0];
        buffer.VertexB = polygon[i];
        buffer.VertexC = polygon[i+1];
        triangles.emplace_back(buffer);
    }
    return triangles;
}

void RASTERIZE(auto &meshdata, framebuffer &frameBufferData, Texture const &texture)
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

            switch (settings.WindingMode) // Step 6
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
                        float totalDeterminant = determinants.AB + determinants.BC + determinants.CA;
                        float percentA = std::abs((determinants.BC) / totalDeterminant);
                        float percentB = std::abs((determinants.CA) / totalDeterminant);
                        float percentC = std::abs((determinants.AB) / totalDeterminant);
                    barycentricZ(determinants, meshdata, pixel, totalDeterminant, percentA, percentB, percentC);
                    bool shouldDraw = depthTest(frameBufferData, pixel.position);
                    if (!shouldDraw)
                    {
                        continue;
                    }
                    float interpolatedInverseW = meshdata.VertexA.inverseW * percentA + 
                        meshdata.VertexB.inverseW * percentB + meshdata.VertexC.inverseW * percentC;
                    barycentricColor(determinants, meshdata, pixel, totalDeterminant, percentA, percentB, percentC, interpolatedInverseW);
                    barycentricNormal(determinants, meshdata, pixel, totalDeterminant, percentA, percentB, percentC, interpolatedInverseW);
                    barycentricUV(determinants, meshdata, pixel, totalDeterminant, percentA, percentB, percentC, interpolatedInverseW);
                    RGBA textureColor = sampleTexture(texture, pixel.UV);
                    pixel.colorData = modulateColor(pixel.colorData, textureColor);
                    calculatelighting(pixel, directionToLight);
                    drawToBuffer(pixel, frameBufferData, meshdata);
                }
                break;
            
            case Both: // Points are POSITIVE or NEGATIVE inside of the triangle
                if (((determinants.AB >= (ABisTopOrLeft ? 0 : EPSILON)) && (determinants.BC  >= (BCisTopOrLeft ? 0 : EPSILON)) 
                && (determinants.CA >= (CAisTopOrLeft ? 0 : EPSILON))) || ((determinants.AB <= (ABisTopOrLeft ? 0 : -EPSILON)) 
                && (determinants.BC  <= (BCisTopOrLeft ? 0 : -EPSILON)) && (determinants.CA <= (CAisTopOrLeft ? 0 : -EPSILON))))
                {
                    float totalDeterminant = determinants.AB + determinants.BC + determinants.CA;
                    float percentA = std::abs((determinants.BC) / totalDeterminant);
                    float percentB = std::abs((determinants.CA) / totalDeterminant);
                    float percentC = std::abs((determinants.AB) / totalDeterminant);
                    float interpolatedInverseW = meshdata.VertexA.inverseW * percentA + 
                    meshdata.VertexB.inverseW * percentB + meshdata.VertexC.inverseW * percentC;
                    barycentricZ(determinants, meshdata, pixel, totalDeterminant, percentA, percentB, percentC);
                    bool shouldDraw = depthTest(frameBufferData, pixel.position);
                    if (!shouldDraw)
                    {
                        continue;
                    }
                    barycentricColor(determinants, meshdata, pixel, totalDeterminant, percentA, percentB, percentC, interpolatedInverseW);
                    barycentricNormal(determinants, meshdata, pixel, totalDeterminant, percentA, percentB, percentC, interpolatedInverseW);
                    barycentricUV(determinants, meshdata, pixel, totalDeterminant, percentA, percentB, percentC, interpolatedInverseW);
                    RGBA textureColor = sampleTexture(texture, pixel.UV);
                    pixel.colorData = modulateColor(pixel.colorData, textureColor);
                    calculatelighting(pixel, directionToLight);
                    drawToBuffer(pixel, frameBufferData, meshdata);
                }
                break;

            default: //default is CCW // Points are POSITIVE inside of the triangle
                if ((determinants.AB >= (ABisTopOrLeft ? 0 : EPSILON)) && (determinants.BC  >= (BCisTopOrLeft ? 0 : EPSILON)) 
                && (determinants.CA >= (CAisTopOrLeft ? 0 : EPSILON)))
                {
                    float totalDeterminant = determinants.AB + determinants.BC + determinants.CA;
                    float percentA = std::abs((determinants.BC) / totalDeterminant);
                    float percentB = std::abs((determinants.CA) / totalDeterminant);
                    float percentC = std::abs((determinants.AB) / totalDeterminant);
                    float interpolatedInverseW = meshdata.VertexA.inverseW * percentA + 
                    meshdata.VertexB.inverseW * percentB + meshdata.VertexC.inverseW * percentC;
                    barycentricZ(determinants, meshdata, pixel, totalDeterminant, percentA, percentB, percentC);
                    bool shouldDraw = depthTest(frameBufferData, pixel.position);
                    if (!shouldDraw)
                    {
                        continue;
                    }
                    barycentricColor(determinants, meshdata, pixel, totalDeterminant, percentA, percentB, percentC, interpolatedInverseW);
                    barycentricNormal(determinants, meshdata, pixel, totalDeterminant, percentA, percentB, percentC, interpolatedInverseW);
                    barycentricUV(determinants, meshdata, pixel, totalDeterminant, percentA, percentB, percentC, interpolatedInverseW);
                    RGBA textureColor = sampleTexture(texture, pixel.UV);
                    pixel.colorData = modulateColor(pixel.colorData, textureColor);
                    calculatelighting(pixel, directionToLight);
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
    xyMinMax.xmax = (xyMinMax.xmax >= settings.frameWidth) ? settings.frameWidth - 1: xyMinMax.xmax;

    xyMinMax.ymax = std::max(meshdata.VertexA.position.y, std::max(meshdata.VertexB.position.y, meshdata.VertexC.position.y));
    xyMinMax.ymax = (xyMinMax.ymax >= settings.frameHeight) ? settings.frameHeight - 1: xyMinMax.ymax;
    return xyMinMax;
}
// Finding determinant-Offeset for the topleft fill rule
bool TopLeftFillFunc(Vector &start, Vector &end)
{
    float edge[2];
    edge[0] = end.x - start.x;
    edge[1] = end.y - start.y;
    bool isLeftEdge = edge[1] < 0;
    bool isTopEdge  = edge[1] == 0 && edge[0] < 0;
    return isLeftEdge || isTopEdge;
}

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
void drawToBuffer(Vertex &pixel, framebuffer &frameBufferData, auto &meshdata)
{
    FramePackager(pixel, frameBufferData, meshdata);
}

// step 10
void FramePackager(Vertex &pixel, framebuffer &buffer, auto &meshdata)
{
    std::uint32_t packed_pixel;
    // Pixel packer needs to come back
    packed_pixel = pixelPackager(pixel.colorData);
    // converts x, y screen coords to pixel indice and used to be a helper function
    std::uint32_t indice = pixel.position.x + (pixel.position.y * settings.frameWidth);
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

void barycentricColor(Determinant &determinants, auto &meshdata, Vertex &pixel, float totalDeterminant, float percentA, float percentB, float percentC, float interpolatedInverseW)
{
    RGBA numerator = (meshdata.VertexA.colorData * meshdata.VertexA.inverseW) * percentA + 
    (meshdata.VertexB.colorData * meshdata.VertexB.inverseW) * percentB + (meshdata.VertexC.colorData * meshdata.VertexC.inverseW) * percentC;

    pixel.colorData = numerator / interpolatedInverseW;
}

void barycentricZ(Determinant &determinants, auto &meshdata, Vertex &pixel, float totalDeterminant, float percentA, float percentB, float percentC)
{
    pixel.position.z = meshdata.VertexA.position.z * percentA + 
    meshdata.VertexB.position.z * percentB + meshdata.VertexC.position.z * percentC;
}

void barycentricNormal(Determinant &determinants, auto &meshdata, Vertex &pixel, float totalDeterminant, float percentA, float percentB, float percentC, float interpolatedInverseW)
{
    Vector numerator = (meshdata.VertexA.normal * meshdata.VertexA.inverseW) * percentA + 
    (meshdata.VertexB.normal * meshdata.VertexB.inverseW) * percentB + (meshdata.VertexC.normal * meshdata.VertexC.inverseW) * percentC;

    pixel.normal = normalize(numerator / interpolatedInverseW);
}

void barycentricUV(Determinant &determinants, auto &meshdata, Vertex &pixel, float totalDeterminant, float percentA, float percentB, float percentC, float interpolatedInverseW)
{
    VectorUV numerator = (meshdata.VertexA.UV * meshdata.VertexA.inverseW) * percentA + 
    (meshdata.VertexB.UV * meshdata.VertexB.inverseW) * percentB + (meshdata.VertexC.UV * meshdata.VertexC.inverseW) * percentC;

    pixel.UV = numerator / interpolatedInverseW;
}

void calculatelighting(Vertex &pixel, Vector const &directionToLight)
{
    // This can/will be changed in the future to allow more dynamic lighting changes
    float diffuse = dotProduct(pixel.normal, directionToLight);
    if (diffuse < 0.0f){diffuse = 0.0f;}
    float ambient = .35f;
    float brightness = diffuse + ambient;
    if (brightness > 1.0f){brightness = 1.0f;}
    pixel.colorData = brightnessRGBACalc(brightness, pixel.colorData);
}

RGBA sampleTexture(Texture const &texture, VectorUV const &UV)
{
    if (!std::isfinite(UV.u) || !std::isfinite(UV.v) || UV.u < 0.0f ||
    UV.u > 1.0f || UV.v < 0.0f || UV.v > 1.0f)
    {
    std::cout
        << "BAD UV: "
        << UV.u << ", "
        << UV.v
        << '\n';
    return {255.0f, 0.0f, 255.0f, 255.0f};
    }

    int x = static_cast<int>(UV.u * (texture.width-1));
    int y = static_cast<int>(UV.v * (texture.height-1));
    int index = x + y * texture.width;
    return texture.pixels[index];
}

// Consider moving repeated calculations
bool depthTest(framebuffer &buffer, Vector &pixel)
{
    int pixelIndex = static_cast<int>(pixel.x) + (static_cast<int>(pixel.y) * settings.frameWidth);
    if (buffer.pixelDepth.depthVals[pixelIndex] > pixel.z)
    {
        buffer.pixelDepth.depthVals[pixelIndex] = pixel.z;
        return true;
    }
    return false;
}

