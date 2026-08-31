#ifndef TESTMESHES_HPP
#define TESTMESHES_HPP


struct Vector2d{
    float x;
    float y;
};

struct RGBA{
    float red;
    float green;
    float blue;
    float alpha;
};

struct Vertex2d{
    Vector2d position;
    RGBA colorData;
};

struct triangle{
    Vertex2d VertexA;
    Vertex2d VertexB;
    Vertex2d VertexC;    
};

struct square{
    Vertex2d Vertices[4];
    int Indices[6];
};

struct Transformation{
    Vector2d position;
    float rotation;
    Vector2d scale;
};

struct Mesh2d{
    std::vector<Vertex2d> Vertices;
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

/*
triangle SRTriangle1 = {
{{200, 150}, {0 , 0, 255, 255}},
{{200, 300}, {0 , 0, 255, 255}},
{{600, 300}, {0 , 0, 255, 255}},
{{600, 300}, {255, 0, 0, 255}},
{{600, 150}, {255, 0, 0, 255}},
{{200, 150}, {255, 0, 0, 255}},
};
triangle SRTriangle2 = {
{{600, 300}, {255, 0, 0, 255}},
{{600, 150}, {255, 0, 0, 255}},
{{200, 150}, {255, 0, 0, 255}},
};
*/

// (x, y) or (width, height) format
    int rightTriangle1 [] {
        200, 150,
        200, 300,
        600, 300
    };
    int rightTriangle2 [] {
        600, 300,
        600, 150,
        200, 150
    };

    RGBA operator+(const RGBA &lhs, const RGBA &rhs)
    {
        RGBA result;
        result.red   = lhs.red   + rhs.red;
        result.green = lhs.green + rhs.red;
        result.blue  = lhs.blue  + rhs.blue;
        result.alpha = lhs.alpha + rhs.alpha;
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

    Vector2d operator+(const Vector2d &lhs, const Vector2d &rhs)
    {
        Vector2d result;
        result.x = lhs.x + rhs.x;
        result.y = lhs.y + rhs.y;
        return result;
    }
        Vector2d operator*(const Vector2d &lhs, const Vector2d &rhs)
    {
        Vector2d result;
        result.x = lhs.x * rhs.x;
        result.y = lhs.y * rhs.y;
        return result;
    }

#endif