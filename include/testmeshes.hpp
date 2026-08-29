#ifndef TESTMESHES_HPP
#define TESTMESHES_HPP


struct Vector2d{
    int x;
    int y;
};

struct PixelData{
    std::uint8_t red = 255;
    std::uint8_t green = 255;
    std::uint8_t blue = 255;
    std::uint8_t alpha = 255;
    std::uint32_t pixelColor = pixelColorAttributer(red, green, blue, alpha);
    std::uint32_t x;
    std::uint32_t y;
    std::uint32_t indice; // =  xyToIndex(x, y);
};

struct Vertex{

    Vector2d position;



};


// (x, y) or (width, height) format
    int triangle [] {
        400, 150,
        200, 300,
        600, 300
    };

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

    int triangleflippedBC [] {
        400, 150,
        600, 300,
        200, 300
    };
#endif