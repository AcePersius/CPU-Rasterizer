#ifndef TEMP_HPP
#define TEMP_HPP


inline Mesh2d makeTorus(
    int majorSegments = 64,
    int minorSegments = 32,
    float majorRadius = 55.0f,
    float minorRadius = 20.0f)
{
    Mesh2d torus;

    constexpr float PI = 3.1415926535f;

    // Generate vertices
    for (int i = 0; i < majorSegments; ++i)
    {
        float u = (static_cast<float>(i) / majorSegments) * 2.0f * PI;

        for (int j = 0; j < minorSegments; ++j)
        {
            float v = (static_cast<float>(j) / minorSegments) * 2.0f * PI;

            float x =
                (majorRadius + minorRadius * std::cos(v))
                * std::cos(u);

            float y =
                minorRadius * std::sin(v);

            float z =
                (majorRadius + minorRadius * std::cos(v))
                * std::sin(u);

            // Just gives the torus some visual variation.
            float red =
                127.5f + 127.5f * std::cos(u);

            float green =
                127.5f + 127.5f * std::sin(v);

            float blue =
                127.5f + 127.5f * std::sin(u);

            torus.Vertices.push_back(
                {
                    {x, y, z},
                    {red, green, blue, 255}
                }
            );
        }
    }

    // Connect vertices into triangles
    for (int i = 0; i < majorSegments; ++i)
    {
        for (int j = 0; j < minorSegments; ++j)
        {
            int nextI = (i + 1) % majorSegments;
            int nextJ = (j + 1) % minorSegments;

            int A = i     * minorSegments + j;
            int B = nextI * minorSegments + j;
            int C = nextI * minorSegments + nextJ;
            int D = i     * minorSegments + nextJ;

            torus.Indices.push_back(A);
            torus.Indices.push_back(D);
            torus.Indices.push_back(B);

            torus.Indices.push_back(B);
            torus.Indices.push_back(D);
            torus.Indices.push_back(C);
        }
    }

    return torus;
}

#endif