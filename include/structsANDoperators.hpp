#ifndef STRUCTSANDOPERATORS_HPP
#define STRUCTSANDOPERATORS_HPP

#include <array>
#include <cmath>
#include <string>

constexpr float INFINITY_Render_Distance = 9999999.0f;
constexpr float PIE = 3.141592;

// Rasterizer Modes relating to determinant values
// IE if a mesh's vertexes are in a Counter Clockwise Order us CCW
// Clock Wise use CW, if order is unknown use both
// Default will be CCW
enum windingModes{
CCW = 0,
CW = 1,
Both = 2
};

enum pANDmOptions{
None = 0,
Fps  = 1,
Ft   = 2,
FpsandFt = 3
};

enum struct ClipPlanes{
left = 0,
right = 1,
bottom = 2,
top = 3,
near = 4,
far = 5,
};

enum MovementModes{
    FPSstyle,
    NOCLIP
};

enum Resolutions{
    Res2560x1440,
    Res1920x1080,
    Res1600x900,
    Res1280x720,
};

struct VectorUV{
    float u;
    float v;
};

struct Vector{
    float x;
    float y;
    float z = 0;
};

struct XYcoord{
    int x;
    int y;
};

struct RGBA{
    float red;
    float green;
    float blue;
    float alpha;
};

struct Vertex{
    Vector position;
    RGBA colorData;
    float inverseW;
    Vector normal {0, 0, 0};
    VectorUV UV {0, 0};
};


// Obj Loader Structs

struct OBJVertexIndex
{
    int positionIndex = -1;
    int UVIndex = -1;
    int normalIndex = -1;
};

struct OBJFace
{
    std::vector<OBJVertexIndex> vertices;

    std::string materialName;
};

struct OBJData
{
    std::vector<Vector> positions;
    std::vector<VectorUV> UVs;
    std::vector<Vector> normals;
    std::vector<OBJFace> faces;

    std::string materialLibrary;
};

// Mtl Loader Structs

struct MTLMaterialData
{
    // Name of Material
    std::string name;
    RGBA diffuseColor {255, 255, 255, 255};
    // File Path to the Texture
    std::string diffuseTexturePath;
};

struct MTLData{
    std::vector<MTLMaterialData> materials;
};

struct triangle{
    Vertex VertexA;
    Vertex VertexB;
    Vertex VertexC;    
};

struct Texture
{
    int width{0};
    int height{0};
    std::vector<RGBA> pixels;
};

struct Transformation{
    Vector position;
    Vector rotation;
    Vector scale;
};

struct Camera{
    Vector position;
    Vector rotation;
    float FOV;
    float AspectRatio;
    float nearPlane;
    float farPlane;
};

void initResolution(int &frameWidth, int &frameHeight, Resolutions &Resolution, Camera &cameraTransf);

struct SETTINGS{
    const windingModes WindingMode = CW;
    const pANDmOptions PerfAndMonitoring = None;
    MovementModes MovementMode = NOCLIP;
    Resolutions Resolution = Res1280x720;
    int frameWidth;
    int frameHeight;
};
SETTINGS settings;

struct colorBuffer{
    std::vector<std::uint32_t> pixels;
    colorBuffer()
    : pixels(settings.frameHeight * settings.frameWidth)
    {
    }
};

struct depthBuffer{
    std::vector<float> depthVals;
    depthBuffer()
    : depthVals(settings.frameHeight * settings.frameWidth, INFINITY_Render_Distance)
    {
    }
};

struct framebuffer{
    colorBuffer colorPixels;
    depthBuffer pixelDepth;
};
framebuffer frameBufferData;

struct Mesh2d{
    std::vector<Vertex> Vertices;
    std::vector<int> Indices; 
};

// New Model Structs
struct Material
{
    std::string name;
    RGBA diffuseColor{255.0f, 255.0f, 255.0f, 255.0f};
    Texture diffuseTexture;
};

struct SubMesh
{
    Mesh2d mesh;
    int materialIndex{-1};
};

struct LoadedModel
{
    std::vector<SubMesh> subMeshes;
    std::vector<Material> materials;
};

// Default Texture
Texture createWhiteTexture()
{
    Texture texture{};

    texture.width = 1;
    texture.height = 1;

    texture.pixels.emplace_back(
        RGBA{255.0f, 255.0f, 255.0f, 255.0f}
    );

    return texture;
}


struct Determinant{
    float AB;
    float BC;
    float CA;
};

struct BoundingBoxData{
    float xmin;
    float ymin;
    float xmax;
    float ymax;
};

struct Vector4D{
    float x;
    float y;
    float z;
    float w;
};

struct Vertex4D{
    Vector4D position;
    RGBA colorData;
    Vector normal {0, 0, 0};
    VectorUV UV {0, 0};
};

struct triangle4D{
    Vertex4D Vertex4DA;
    Vertex4D Vertex4DB;
    Vertex4D Vertex4DC;    
};

struct Matrix4x4{
    float matrix[4][4];
};

void initResolution(int &frameWidth, int &frameHeight, Resolutions &Resolution, Camera &cameraTransf)
{
    if (Resolution == Res2560x1440)
    {
        frameWidth = 2560;
        frameHeight = 1440;
    }
    else if (Resolution == Res1920x1080)
    {
        frameWidth = 1920;
        frameHeight = 1080;
    }
    else if (Resolution == Res1600x900)
    {
        frameWidth = 1600;
        frameHeight = 900;
    }
    else if (Resolution == Res1280x720)
    {
        frameWidth = 1280;
        frameHeight = 720;
    }
    frameBufferData.colorPixels.pixels.resize(frameWidth * frameHeight);
    frameBufferData.pixelDepth.depthVals.assign(frameWidth * frameHeight, INFINITY_Render_Distance);
    cameraTransf.AspectRatio = static_cast<float>(frameWidth)/ static_cast<float>(frameHeight);
}


//fix
float tx;
float ty;
float tz;

RGBA GenericColor{
    188,
    188,
    188,
    255
};

Matrix4x4 BaseMatrix{
    {{1, 0, 0, 0},
     {0, 1, 0, 0},
     {0, 0, 1, 0},
     {0, 0, 0, 1}}
};

Matrix4x4 Translation{
    {{1, 0, 0, 1},
     {0, 1, 0, 1},
     {0, 0, 1, 1},
     {0, 0, 0, 1}}
};

Matrix4x4 Scale{
    {{1, 0, 0, 0},
     {0, 1, 0, 0},
     {0, 0, 1, 0},
     {0, 0, 0, 1}}
};

// Because I chose to use Column Vectors, I must do Matrix rows * Vector columns
// Order also Matters when it comes to Applying Matrices to our 4D vector:
// Matrices A * B * C * Vectord4D D read right to left in order that must be done so:
//  A * (B * (C * D)
Vector4D operator*(Matrix4x4 lhs,Vector4D rhs)
{
    Vector4D buffer;
    buffer.x = lhs.matrix[0][0] * rhs.x + lhs.matrix[0][1] * rhs.y + lhs.matrix[0][2] * rhs.z + lhs.matrix[0][3] * rhs.w;
    buffer.y = lhs.matrix[1][0] * rhs.x + lhs.matrix[1][1] * rhs.y + lhs.matrix[1][2] * rhs.z + lhs.matrix[1][3] * rhs.w;
    buffer.z = lhs.matrix[2][0] * rhs.x + lhs.matrix[2][1] * rhs.y + lhs.matrix[2][2] * rhs.z + lhs.matrix[2][3] * rhs.w;
    buffer.w = lhs.matrix[3][0] * rhs.x + lhs.matrix[3][1] * rhs.y + lhs.matrix[3][2] * rhs.z + lhs.matrix[3][3] * rhs.w;
    return buffer;
};

Vector4D toVector4D(Vector threeD)
{
    Vector4D buffer;
    buffer.x = threeD.x;
    buffer.y = threeD.y;
    buffer.z = threeD.z;
    buffer.w = 1;
    return buffer;
};

Vector4D NormaltoVector4D(Vector threeD)
{
    Vector4D buffer;
    buffer.x = threeD.x;
    buffer.y = threeD.y;
    buffer.z = threeD.z;
    buffer.w = 0.0f;
    return buffer;
};

Vector toVector(Vector4D fourD)
{
    Vector buffer;
    buffer.x = fourD.x;
    buffer.y = fourD.y;
    buffer.z = fourD.z;
    return buffer;
};

Vertex toVertex3D(Vertex4D const &Vertex4D)
{
    Vertex A {};
    A.position.x = Vertex4D.position.x;
    A.position.y = Vertex4D.position.y;
    A.position.z = Vertex4D.position.z;
    A.colorData  = Vertex4D.colorData;
    return A;
};

// I dont think this has a use anymore but I'll keep it anyways
// For now
triangle toTriangle3D(triangle4D const &triangle4D)
{
    Vertex A = toVertex3D(triangle4D.Vertex4DA);
    Vertex B = toVertex3D(triangle4D.Vertex4DB);
    Vertex C = toVertex3D(triangle4D.Vertex4DC);
    triangle triangle3D {};
    triangle3D.VertexA = A;
    triangle3D.VertexB = B;
    triangle3D.VertexC = C;
    return triangle3D;
};

// Standard Matrix Multiplication is Matrix A's rows * Matrix B's Columns
// Something I learned! Matrix Math is non-communicative, but it is associative!
// What does that mean? There are particular rules such as:
// ORDER matters: A × B ≠ B × A, but GROUPING does not: (A × B) × C = A × (B × C)
Matrix4x4 operator*(Matrix4x4 lhs_rows,Matrix4x4 rhs_columns)
{
    Matrix4x4 buffer {0};
    for (int i = 0; i < 4; i++)
    {
        for (int j = 0; j < 4; j++)
        {
            for (int k = 0; k < 4; k++)
            buffer.matrix[i][j] += lhs_rows.matrix[i][k]* rhs_columns.matrix[k][j];
        }
    }
    return buffer;
};

Matrix4x4 toTranslationMatrix(Matrix4x4 BaseMatrix, const float &tx, const float &ty, const float &tz)
{
    BaseMatrix.matrix[0][3] = tx;
    BaseMatrix.matrix[1][3] = ty;
    BaseMatrix.matrix[2][3] = tz;
    return BaseMatrix;
}

Matrix4x4 toScaleMatrix(Matrix4x4 BaseMatrix, const float &tx, const float &ty, const float &tz)
{
    BaseMatrix.matrix[0][0] = tx;
    BaseMatrix.matrix[1][1] = ty;
    BaseMatrix.matrix[2][2] = tz;
    return BaseMatrix;
}

Matrix4x4 toInverseScaleMatrix(Matrix4x4 BaseMatrix, const float &sx, const float &sy, const float &sz)
{
    BaseMatrix.matrix[0][0] = 1.0f / sx;
    BaseMatrix.matrix[1][1] = 1.0f / sy;
    BaseMatrix.matrix[2][2] = 1.0f / sz;
    return BaseMatrix;
}

// Consider Cleaning up the trig calls and calculation degrees to
// Radians once in the function instead of everytime
Matrix4x4 toRotateXMatrix(Matrix4x4 BaseMatrix, const float angle)
{
    BaseMatrix.matrix[1][1] =  std::cos(((angle * PIE) / 180));
    BaseMatrix.matrix[1][2] = -std::sin(((angle * PIE) / 180));
    BaseMatrix.matrix[2][1] =  std::sin(((angle * PIE) / 180));
    BaseMatrix.matrix[2][2] =  std::cos(((angle * PIE) / 180));
    return BaseMatrix;
}

Matrix4x4 toRotateYMatrix(Matrix4x4 BaseMatrix, const float angle)
{
    BaseMatrix.matrix[0][0] =  std::cos(((angle * PIE) / 180));
    BaseMatrix.matrix[0][2] =  std::sin(((angle * PIE) / 180));
    BaseMatrix.matrix[2][0] = -std::sin(((angle * PIE) / 180));
    BaseMatrix.matrix[2][2] =  std::cos(((angle * PIE) / 180));
    return BaseMatrix;
}

Matrix4x4 toRotateZMatrix(Matrix4x4 BaseMatrix, const float angle)
{
    BaseMatrix.matrix[0][0] =  std::cos(((angle * PIE) / 180));
    BaseMatrix.matrix[0][1] = -std::sin(((angle * PIE) / 180));
    BaseMatrix.matrix[1][0] =  std::sin(((angle * PIE) / 180));
    BaseMatrix.matrix[1][1] =  std::cos(((angle * PIE) / 180));
    return BaseMatrix;
}

Matrix4x4 toClipSpaceMatrix(Camera const &cameraTransf)
{
    /*| Xscale   0       0       0 |
      |   0    Yscale    0       0 |
      |   0      0       A       B |
      |   0      0       1       0 |*/
    // X row → controls horizontal FOV
    // Y row → controls vertical FOV
    // Z row → maps near/far depth into [0,1]
    // W row → stores camera-space z so perspective divide can happen
    Matrix4x4 ClipSpaceMatrix {0};
    float Yscale = 1.0f / std::tan((cameraTransf.FOV * PIE / 180.0f) / 2.0f);
    float Xscale = Yscale / cameraTransf.AspectRatio;
    float A = cameraTransf.farPlane / (cameraTransf.farPlane - cameraTransf.nearPlane);
    float B = -(cameraTransf.farPlane * cameraTransf.nearPlane) / (cameraTransf.farPlane - cameraTransf.nearPlane);
    ClipSpaceMatrix.matrix[0][0] = Xscale;
    ClipSpaceMatrix.matrix[1][1] = Yscale;
    ClipSpaceMatrix.matrix[2][2] = A;
    ClipSpaceMatrix.matrix[2][3] = B;
    ClipSpaceMatrix.matrix[3][2] = 1;
    return ClipSpaceMatrix;
}

RGBA brightnessRGBACalc(const auto &lhs, const RGBA &rhs)
{
    RGBA result;
    result.red   = rhs.red   * lhs;
    result.green = rhs.green * lhs;
    result.blue  = rhs.blue  * lhs;
    result.alpha = rhs.alpha;
    return result;
}

RGBA operator+(const RGBA &lhs, const RGBA &rhs)
{
    RGBA result;
    result.red   = lhs.red   + rhs.red;
    result.green = lhs.green + rhs.green;
    result.blue  = lhs.blue  + rhs.blue;
    result.alpha = lhs.alpha + rhs.alpha;
    return result;
}
RGBA operator-(const RGBA &lhs, const RGBA &rhs)
{
    RGBA result;
    result.red   = lhs.red   - rhs.red;
    result.green = lhs.green - rhs.green;
    result.blue  = lhs.blue  - rhs.blue;
    result.alpha = lhs.alpha - rhs.alpha;
    return result;
}
RGBA operator*(const RGBA &lhs, const auto &rhs)
{
    RGBA result;
    result.red   = lhs.red   * rhs;
    result.green = lhs.green * rhs;
    result.blue  = lhs.blue  * rhs;
    result.alpha = lhs.alpha * rhs;
    return result;
}
RGBA operator*(const auto &rhs, const RGBA &lhs)
{
    RGBA result;
    result.red   = lhs.red   * rhs;
    result.green = lhs.green * rhs;
    result.blue  = lhs.blue  * rhs;
    result.alpha = lhs.alpha * rhs;
    return result;
}
RGBA operator/(const RGBA &lhs, const auto &rhs)
{
    RGBA result;
    result.red   = lhs.red   / rhs;
    result.green = lhs.green / rhs;
    result.blue  = lhs.blue  / rhs;
    result.alpha = lhs.alpha / rhs;
    return result;
}
RGBA operator/(const auto &rhs, const RGBA &lhs)
{
    RGBA result;
    result.red   = lhs.red   / rhs;
    result.green = lhs.green / rhs;
    result.blue  = lhs.blue  / rhs;
    result.alpha = lhs.alpha / rhs;
    return result;
}

RGBA modulateColor(const RGBA &basecolor, const RGBA &texturecolor)
{
    RGBA result;
    result.red = basecolor.red * texturecolor.red / 255.0f;
    result.green = basecolor.green * texturecolor.green / 255.0f;
    result.blue = basecolor.blue * texturecolor.blue / 255.0f;
    result.alpha = basecolor.alpha;
    return result;
}

Vector operator+(const Vector &lhs, const Vector &rhs)
{
    Vector result;
    result.x = lhs.x + rhs.x;
    result.y = lhs.y + rhs.y;
    result.z = lhs.z + rhs.z;
    return result;
}
Vector operator-(const Vector &lhs, const Vector &rhs)
{
    Vector result;
    result.x = lhs.x - rhs.x;
    result.y = lhs.y - rhs.y;
    result.z = lhs.z - rhs.z;
    return result;
}
Vector operator*(const Vector &lhs, const Vector &rhs)
{
    Vector result;
    result.x = lhs.x * rhs.x;
    result.y = lhs.y * rhs.y;
    result.z = lhs.z * rhs.z;

    return result;
}

Vector operator*(const float &lhs, const Vector &rhs)
{
    Vector result;
    result.x = lhs * rhs.x;
    result.y = lhs * rhs.y;
    result.z = lhs * rhs.z;

    return result;
}

Vector operator*(const Vector &lhs, const float &rhs)
{
    Vector result;
    result.x = lhs.x * rhs;
    result.y = lhs.y * rhs;
    result.z = lhs.z * rhs;

    return result;
}

Vector operator/(const Vector &lhs, const float &rhs)
{
    Vector result;
    result.x = lhs.x / rhs;
    result.y = lhs.y / rhs;
    result.z = lhs.z / rhs;

    return result;
}

XYcoord operator+(const XYcoord &lhs, const XYcoord &rhs)
{
    XYcoord result;
    result.x = lhs.x + rhs.x;
    result.y = lhs.y + rhs.y;
    return result;
}

VectorUV operator+(const VectorUV &lhs, const VectorUV &rhs)
{
    VectorUV result;
    result.u = lhs.u + rhs.u;
    result.v = lhs.v + rhs.v;
    return result;
}

VectorUV operator-(const VectorUV &lhs, const VectorUV &rhs)
{
    VectorUV result;
    result.u = lhs.u - rhs.u;
    result.v = lhs.v - rhs.v;
    return result;
}

VectorUV operator*(const float &lhs, const VectorUV &rhs)
{
    VectorUV result;
    result.u = lhs * rhs.u;
    result.v = lhs * rhs.v;
    return result;
}

VectorUV operator*(const VectorUV &rhs, const float &lhs)
{
    VectorUV result;
    result.u = lhs * rhs.u;
    result.v = lhs * rhs.v;
    return result;
}

VectorUV operator/(const VectorUV &lhs, const float &rhs)
{
    VectorUV result;
    result.u = lhs.u / rhs;
    result.v = lhs.v / rhs;
    return result;
}

Vector crossProduct(Vector const &A, Vector const &B)
{
    /*A × B =
      x = AyBz - AzBy
      y = AzBx - AxBz
      z = AxBy - AyBx*/
    Vector result;
    result.x = A.y * B.z - A.z * B.y;
    result.y = A.z * B.x - A.x * B.z;
    result.z = A.x * B.y - A.y * B.x;
    return result;
}

float dotProduct(Vector const &A, Vector const &B)
{
    // A · B = AxBx + AyBy + AzBz
    return (A.x * B.x + A.y * B.y + A.z * B.z);
}

float vectorLength(Vector const &A)
{
    // sqrt(x^2 + y^2 + z^2)
    return std::sqrt(A.x * A.x + A.y * A.y + A.z * A.z);
}

Vector normalize(Vector const &V)
{
    Vector result;
    float length = vectorLength(V);
    if (length == 0){return V;}
    result.x = V.x / length;
    result.y = V.y / length;
    result.z = V.z / length;
    return result;
}

Vector calculateFaceNormal(Vector const &A, Vector const &B, Vector const &C)
{
    Vector AB = B - A;
    Vector AC = C - A;
    Vector crossProductVector = crossProduct(AB, AC);
    Vector normalizedVector = normalize(crossProductVector);
    return normalizedVector;
}

// Nums and Letters
const std::array<uint8_t, 7> A = {
    0b0001000,
    0b0010100,
    0b0100010,
    0b0111110,
    0b0100010,
    0b0100010,
    0b0100010,
};
const std::array<uint8_t, 7> B = {
    0b0111100,
    0b0100010,
    0b0100010,
    0b0111100,
    0b0100010,
    0b0100010,
    0b0111100,
};
const std::array<uint8_t, 7> C = {
    0b0111110,
    0b0100000,
    0b0100000,
    0b0100000,
    0b0100000,
    0b0100000,
    0b0111110,
};
const std::array<uint8_t, 7> D = {
    0b0111000,
    0b0100100,
    0b0100010,
    0b0100010,
    0b0100010,
    0b0100100,
    0b0111000,
};
const std::array<uint8_t, 7> E = {
    0b0111110,
    0b0100000,
    0b0100000,
    0b0111000,
    0b0100000,
    0b0100000,
    0b0111110,
};
const std::array<uint8_t, 7> F = {
    0b0111110,
    0b0100000,
    0b0100000,
    0b0111000,
    0b0100000,
    0b0100000,
    0b0100000,
};
const std::array<uint8_t, 7> G = {
    0b0111110,
    0b0100000,
    0b0100000,
    0b0101110,
    0b0100010,
    0b0100010,
    0b0111110,
};
const std::array<uint8_t, 7> H = {
    0b0100010,
    0b0100010,
    0b0100010,
    0b0111110,
    0b0100010,
    0b0100010,
    0b0100010,
};
const std::array<uint8_t, 7> I = {
    0b0111110,
    0b0001000,
    0b0001000,
    0b0001000,
    0b0001000,
    0b0001000,
    0b0111110,
};
const std::array<uint8_t, 7> J = {
    0b0111110,
    0b0001000,
    0b0001000,
    0b0001000,
    0b0001000,
    0b0101000,
    0b0010000,
};
const std::array<uint8_t, 7> K = {
    0b0100010,
    0b0100100,
    0b0101000,
    0b0110000,
    0b0101000,
    0b0100100,
    0b0100010,
};
const std::array<uint8_t, 7> L = {
    0b0100000,
    0b0100000,
    0b0100000,
    0b0100000,
    0b0100000,
    0b0100000,
    0b0111110,
};
const std::array<uint8_t, 7> M = {
    0b0100010,
    0b0110110,
    0b0101010,
    0b0101010,
    0b0100010,
    0b0100010,
    0b0100010,
};
const std::array<uint8_t, 7> N = {
    0b0100010,
    0b0110010,
    0b0101010,
    0b0101010,
    0b0100110,
    0b0100010,
    0b0100010,
};
const std::array<uint8_t, 7> O = {
    0b0111110,
    0b0100010,
    0b0100010,
    0b0100010,
    0b0100010,
    0b0100010,
    0b0111110,
};
const std::array<uint8_t, 7> P = {
    0b0111110,
    0b0100010,
    0b0100010,
    0b0111110,
    0b0100000,
    0b0100000,
    0b0100000,
};
const std::array<uint8_t, 7> Q = {
    0b0111110,
    0b0100010,
    0b0100010,
    0b0100010,
    0b0100110,
    0b0100010,
    0b0111111,
};
const std::array<uint8_t, 7> R = {
    0b0111110,
    0b0100010,
    0b0100010,
    0b0111110,
    0b0101000,
    0b0100100,
    0b0100010,
};
const std::array<uint8_t, 7> S = {
    0b0111110,
    0b0100000,
    0b0100000,
    0b0111110,
    0b0000010,
    0b0000010,
    0b0111110,
};
const std::array<uint8_t, 7> T = {
    0b0111110,
    0b0001000,
    0b0001000,
    0b0001000,
    0b0001000,
    0b0001000,
    0b0001000,
};
const std::array<uint8_t, 7> U = {
    0b0100010,
    0b0100010,
    0b0100010,
    0b0100010,
    0b0100010,
    0b0100010,
    0b0111110,
};
const std::array<uint8_t, 7> V = {
    0b0100010,
    0b0100010,
    0b0100010,
    0b0100010,
    0b0100010,
    0b0010100,
    0b0001000,
};
const std::array<uint8_t, 7> W = {
    0b0100010,
    0b0100010,
    0b0100010,
    0b0100010,
    0b0101010,
    0b0101010,
    0b0010100,
};
const std::array<uint8_t, 7> X = {
    0b0100010,
    0b0100010,
    0b0010100,
    0b0001000,
    0b0010100,
    0b0100010,
    0b0100010,
};
const std::array<uint8_t, 7> Y = {
    0b0100010,
    0b0100010,
    0b0010100,
    0b0001000,
    0b0001000,
    0b0001000,
    0b0001000,
};
const std::array<uint8_t, 7> Z = {
    0b0111110,
    0b0000100,
    0b0001000,
    0b0010000,
    0b0100000,
    0b0100000,
    0b0111110,
};
const std::array<uint8_t, 7> SPACE = {
    0b0000000,
    0b0000000,
    0b0000000,
    0b0000000,
    0b0000000,
    0b0000000,
    0b0000000,
};
const std::array<uint8_t, 7> ZERO = {
    0b0111110,
    0b0100010,
    0b0100010,
    0b0100010,
    0b0100010,
    0b0100010,
    0b0111110,
};
const std::array<uint8_t, 7> ONE = {
    0b0001000,
    0b0011000,
    0b0101000,
    0b0001000,
    0b0001000,
    0b0001000,
    0b0111110,
};
const std::array<uint8_t, 7> TWO = {
    0b0011100,
    0b0100010,
    0b0000100,
    0b0001000,
    0b0010000,
    0b0100000,
    0b0111110,
};
const std::array<uint8_t, 7> THREE = {
    0b0111100,
    0b0000010,
    0b0000100,
    0b0111100,
    0b0000100,
    0b0000010,
    0b0111100,
};
const std::array<uint8_t, 7> FOUR = {
    0b0100010,
    0b0100010,
    0b0100010,
    0b0011110,
    0b0000010,
    0b0000010,
    0b0000010,
};
const std::array<uint8_t, 7> FIVE = {
    0b0111110,
    0b0100000,
    0b0100000,
    0b0111110,
    0b0000010,
    0b0000010,
    0b0111110,
};
const std::array<uint8_t, 7> SIX = {
    0b0111110,
    0b0100000,
    0b0100000,
    0b0111110,
    0b0100010,
    0b0100010,
    0b0111110,
};
const std::array<uint8_t, 7> SEVEN = {
    0b0111110,
    0b0000010,
    0b0000100,
    0b0001000,
    0b0010000,
    0b0010000,
    0b0010000,
};
const std::array<uint8_t, 7> EIGHT = {
    0b0111110,
    0b0100010,
    0b0100010,
    0b0011100,
    0b0100010,
    0b0100010,
    0b0111110,
};
const std::array<uint8_t, 7> NINE = {
    0b0111110,
    0b0100010,
    0b0100010,
    0b0111110,
    0b0000010,
    0b0000010,
    0b0000010,
};
const std::array<uint8_t, 7> COLON = {
    0b0000000,
    0b0001100,
    0b0001100,
    0b0000000,
    0b0001100,
    0b0001100,
    0b0000000,
};
const std::array<uint8_t, 7> PERIOD = {
    0b0000000,
    0b0000000,
    0b0000000,
    0b0000000,
    0b0011100,
    0b0011100,
    0b0011100,
};
const std::array<uint8_t, 7> BANG = {
    0b0001100,
    0b0001100,
    0b0001100,
    0b0001100,
    0b0001100,
    0b0000000,
    0b0001100,
};
const std::array<uint8_t, 7> QUESTION = {
    0b0011100,
    0b0100010,
    0b0000100,
    0b0001000,
    0b0001000,
    0b0000000,
    0b0001000,
};
const std::array<uint8_t, 7> COMMA = {
    0b0000000,
    0b0000000,
    0b0000000,
    0b0000000,
    0b0000000,
    0b0001000,
    0b0010000,
};
const std::array<uint8_t, 7> FSLASH = {
    0b0000000,
    0b0000010,
    0b0000100,
    0b0001000,
    0b0010000,
    0b0100000,
    0b0000000,
};
const std::array<uint8_t, 7> BSLASH = {
    0b0000000,
    0b0100000,
    0b0010000,
    0b0001000,
    0b0000100,
    0b0000010,
    0b0000000,
};
const std::array<uint8_t, 7> LPARENTH = {
    0b0001000,
    0b0010000,
    0b0100000,
    0b0100000,
    0b0100000,
    0b0010000,
    0b0001000,
};
const std::array<uint8_t, 7> RPARENTH = {
    0b0001000,
    0b0000100,
    0b0000010,
    0b0000010,
    0b0000010,
    0b0000100,
    0b0001000,
};

#endif