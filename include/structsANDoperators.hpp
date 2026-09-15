#ifndef STRUCTSANDOPERATORS_HPP
#define STRUCTSANDOPERATORS_HPP

#include <array>


const static int frameWidth = 800;
const static int frameHeight = 600;
constexpr float INFINITY_Render_Distance = 9999999.0f;

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

struct Camera{
    Vector CamPos;
    Vector CamRota;
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