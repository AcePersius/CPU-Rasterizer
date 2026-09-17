#ifndef STRUCTSANDOPERATORS_HPP
#define STRUCTSANDOPERATORS_HPP

#include <array>


const static int frameWidth = 800;
const static int frameHeight = 600;
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

struct colorBuffer{
    std::vector<std::uint32_t> pixels;
    colorBuffer()
    : pixels(frameHeight * frameWidth)
    {
    }
};

struct depthBuffer{
    std::vector<float> depthVals;
    depthBuffer()
    : depthVals(frameHeight * frameWidth, INFINITY_Render_Distance)
    {
    }
};

struct framebuffer{
    colorBuffer colorPixels;
    depthBuffer pixelDepth;
    const windingModes WindingMode = CCW;
    const pANDmOptions PerfAndMonitoring = FpsandFt;
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
};

struct triangle{
    Vertex VertexA;
    Vertex VertexB;
    Vertex VertexC;    
};

struct square{
    Vertex Vertices[4];
    int Indices[6];
};

struct Transformation{
    Vector position;
    Vector rotation;
    Vector scale;
};

struct Mesh2d{
    std::vector<Vertex> Vertices;
    std::vector<int> Indices; 
};

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

struct Matrix4x4{
    float matrix[4][4];
};

float tx;
float ty;
float tz;

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

Vector toVector(Vector4D fourD)
{
    Vector buffer;
    buffer.x = fourD.x;
    buffer.y = fourD.y;
    buffer.z = fourD.z;
    return buffer;
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
const std::array<uint8_t, 7> I = {
    0b0111110,
    0b0001000,
    0b0001000,
    0b0001000,
    0b0001000,
    0b0001000,
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
const std::array<uint8_t, 7> P = {
    0b0111110,
    0b0100010,
    0b0100010,
    0b0111110,
    0b0100000,
    0b0100000,
    0b0100000,
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
#endif