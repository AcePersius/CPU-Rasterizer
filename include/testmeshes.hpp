#ifndef TESTMESHES_HPP
#define TESTMESHES_HPP


struct Vector2d{
    int x;
    int y;
};

struct RGBA{
    std::uint8_t red;
    std::uint8_t green;
    std::uint8_t blue;
    std::uint8_t alpha;
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

/*
triangle SRTriangle1 = {
{{200, 150}, {0 , 0, 255, 255}},
{{200, 300}, {0 , 0, 255, 255}},
{{600, 300}, {0 , 0, 255, 255}},
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

#endif