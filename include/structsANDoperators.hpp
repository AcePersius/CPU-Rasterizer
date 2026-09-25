#ifndef STRUCTSANDOPERATORS_HPP
#define STRUCTSANDOPERATORS_HPP

#include <array>
#include <cmath>
#include <cstdint>
#include <string>
#include <vector>

inline constexpr float INFINITY_Render_Distance = 9999999.0f;
inline constexpr float PIE = 3.141592;

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
    const pANDmOptions PerfAndMonitoring = FpsandFt;
    MovementModes MovementMode = NOCLIP;
    Resolutions Resolution = Res1280x720;
    int frameWidth {0};
    int frameHeight {0};
};

extern SETTINGS settings;

struct colorBuffer{
    std::vector<std::uint32_t> pixels;
};

struct depthBuffer{
    std::vector<float> depthVals;
};

struct framebuffer{
    colorBuffer colorPixels;
    depthBuffer pixelDepth;
};

extern framebuffer frameBufferData;

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
Texture createWhiteTexture();

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

//fix
/*
float tx;
float ty;
float tz;
*/
// cant remember if these are still used or not/ will remove later

extern RGBA GenericColor;

extern Matrix4x4 BaseMatrix;

extern Matrix4x4 Translation;

extern Matrix4x4 Scale;

Vector4D operator*(Matrix4x4 lhs,Vector4D rhs);

Vector4D toVector4D(Vector threeD);

Vector4D NormaltoVector4D(Vector threeD);

Vector toVector(Vector4D fourD);

Vertex toVertex3D(Vertex4D const &Vertex4D);

triangle toTriangle3D(triangle4D const &triangle4D);

Matrix4x4 operator*(Matrix4x4 lhs_rows,Matrix4x4 rhs_columns);

Matrix4x4 toTranslationMatrix(Matrix4x4 BaseMatrix, const float &tx, const float &ty, const float &tz);

Matrix4x4 toScaleMatrix(Matrix4x4 BaseMatrix, const float &tx, const float &ty, const float &tz);

Matrix4x4 toInverseScaleMatrix(Matrix4x4 BaseMatrix, const float &sx, const float &sy, const float &sz);

Matrix4x4 toRotateXMatrix(Matrix4x4 BaseMatrix, const float angle);

Matrix4x4 toRotateYMatrix(Matrix4x4 BaseMatrix, const float angle);

Matrix4x4 toRotateZMatrix(Matrix4x4 BaseMatrix, const float angle);

Matrix4x4 toClipSpaceMatrix(Camera const &cameraTransf);

RGBA brightnessRGBACalc(const float &lhs, const RGBA &rhs);

RGBA operator+(const RGBA &lhs, const RGBA &rhs);

RGBA operator-(const RGBA &lhs, const RGBA &rhs);

RGBA operator*(const RGBA &lhs, const float &rhs);

RGBA operator*(const float &rhs, const RGBA &lhs);

RGBA operator/(const RGBA &lhs, const float &rhs);

RGBA operator/(const float &rhs, const RGBA &lhs);

RGBA modulateColor(const RGBA &basecolor, const RGBA &texturecolor);

Vector operator+(const Vector &lhs, const Vector &rhs);

Vector operator-(const Vector &lhs, const Vector &rhs);

Vector operator*(const Vector &lhs, const Vector &rhs);


Vector operator*(const float &lhs, const Vector &rhs);

Vector operator*(const Vector &lhs, const float &rhs);


Vector operator/(const Vector &lhs, const float &rhs);

XYcoord operator+(const XYcoord &lhs, const XYcoord &rhs);

VectorUV operator+(const VectorUV &lhs, const VectorUV &rhs);

VectorUV operator-(const VectorUV &lhs, const VectorUV &rhs);

VectorUV operator*(const float &lhs, const VectorUV &rhs);

VectorUV operator*(const VectorUV &rhs, const float &lhs);

VectorUV operator/(const VectorUV &lhs, const float &rhs);

Vector crossProduct(Vector const &A, Vector const &B);

float dotProduct(Vector const &A, Vector const &B);

float vectorLength(Vector const &A);

Vector normalize(Vector const &V);

Vector calculateFaceNormal(Vector const &A, Vector const &B, Vector const &C);

// Nums and Letters
extern const std::array<uint8_t, 7> A;
extern const std::array<uint8_t, 7> B;
extern const std::array<uint8_t, 7> C;
extern const std::array<uint8_t, 7> D;
extern const std::array<uint8_t, 7> E;
extern const std::array<uint8_t, 7> F;
extern const std::array<uint8_t, 7> G;
extern const std::array<uint8_t, 7> H;
extern const std::array<uint8_t, 7> I;
extern const std::array<uint8_t, 7> J;
extern const std::array<uint8_t, 7> K;
extern const std::array<uint8_t, 7> L;
extern const std::array<uint8_t, 7> M;
extern const std::array<uint8_t, 7> N;
extern const std::array<uint8_t, 7> O;
extern const std::array<uint8_t, 7> P;
extern const std::array<uint8_t, 7> Q;
extern const std::array<uint8_t, 7> R;
extern const std::array<uint8_t, 7> S;
extern const std::array<uint8_t, 7> T;
extern const std::array<uint8_t, 7> U;
extern const std::array<uint8_t, 7> V;
extern const std::array<uint8_t, 7> W;
extern const std::array<uint8_t, 7> X;
extern const std::array<uint8_t, 7> Y;
extern const std::array<uint8_t, 7> Z;
extern const std::array<uint8_t, 7> SPACE;
extern const std::array<uint8_t, 7> ZERO;
extern const std::array<uint8_t, 7> ONE;
extern const std::array<uint8_t, 7> TWO;
extern const std::array<uint8_t, 7> THREE;
extern const std::array<uint8_t, 7> FOUR;
extern const std::array<uint8_t, 7> FIVE;
extern const std::array<uint8_t, 7> SIX;
extern const std::array<uint8_t, 7> SEVEN;
extern const std::array<uint8_t, 7> EIGHT;
extern const std::array<uint8_t, 7> NINE;
extern const std::array<uint8_t, 7> COLON;
extern const std::array<uint8_t, 7> PERIOD;
extern const std::array<uint8_t, 7> BANG;
extern const std::array<uint8_t, 7> QUESTION;
extern const std::array<uint8_t, 7> COMMA;
extern const std::array<uint8_t, 7> FSLASH;
extern const std::array<uint8_t, 7> BSLASH;
extern const std::array<uint8_t, 7> LPARENTH;
extern const std::array<uint8_t, 7> RPARENTH;

#endif