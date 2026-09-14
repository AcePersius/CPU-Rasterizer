#ifndef STRUCTSANDOPERATORS_HPP
#define STRUCTSANDOPERATORS_HPP

const static int frameWidth = 800;
const static int frameHeight = 600;

// Rasterizer Modes relating to determinant values
// IE if a mesh's vertexes are in a Counter Clockwise Order us CCW
// Clock Wise use CW, if order is unknown use both
// Default will be CCW
enum windingModes{
CCW = 0,
CW = 1,
Both = 2
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
    : depthVals(frameHeight * frameWidth, 1000.0f)
    {
    }
};

struct framebuffer{
    colorBuffer colorPixels;
    depthBuffer pixelDepth;
    const windingModes WindingMode = CCW;
};

struct Vector{
    float x;
    float y;
    float z = 0;
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
    RGBA operator*(const RGBA &lhs, const float &rhs)
    {
        RGBA result;
        result.red   = lhs.red   * rhs;
        result.green = lhs.green * rhs;
        result.blue  = lhs.blue  * rhs;
        result.alpha = lhs.alpha * rhs;
        return result;
    }
    RGBA operator*(const float &rhs, const RGBA &lhs)
    {
        RGBA result;
        result.red   = lhs.red   * rhs;
        result.green = lhs.green * rhs;
        result.blue  = lhs.blue  * rhs;
        result.alpha = lhs.alpha * rhs;
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

#endif