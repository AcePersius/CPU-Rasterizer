#ifndef TEMP_HPP
#define TEMP_HPP

Texture gooberTexture = createCheckerboard(
    64,
    64,
    8,
    {255.0f, 255.0f, 255.0f, 255.0f},
    {30.0f, 30.0f, 30.0f, 255.0f}
);

void addTexturedQuad(
    Mesh2d &mesh,
    Vector const &A,
    Vector const &B,
    Vector const &C,
    Vector const &D,
    RGBA const &color)
{
    int baseIndex = static_cast<int>(mesh.Vertices.size());

    Vector positions[4]
    {
        A,
        B,
        C,
        D
    };

    VectorUV UVs[4]
    {
        {0.0f, 0.0f},
        {1.0f, 0.0f},
        {1.0f, 1.0f},
        {0.0f, 1.0f}
    };

    for (int i = 0; i < 4; i++)
    {
        Vertex vertex{};

        vertex.position = positions[i];
        vertex.colorData = color;
        vertex.UV = UVs[i];

        mesh.Vertices.emplace_back(vertex);
    }

    mesh.Indices.emplace_back(baseIndex + 0);
    mesh.Indices.emplace_back(baseIndex + 1);
    mesh.Indices.emplace_back(baseIndex + 2);

    mesh.Indices.emplace_back(baseIndex + 0);
    mesh.Indices.emplace_back(baseIndex + 2);
    mesh.Indices.emplace_back(baseIndex + 3);
}

void addTexturedBox(
    Mesh2d &mesh,
    Vector const &center,
    Vector const &size,
    RGBA const &color)
{
    float hx = size.x * 0.5f;
    float hy = size.y * 0.5f;
    float hz = size.z * 0.5f;

    float xMin = center.x - hx;
    float xMax = center.x + hx;

    float yMin = center.y - hy;
    float yMax = center.y + hy;

    float zMin = center.z - hz;
    float zMax = center.z + hz;

    Vector v0{xMin, yMin, zMin};
    Vector v1{xMax, yMin, zMin};
    Vector v2{xMax, yMax, zMin};
    Vector v3{xMin, yMax, zMin};

    Vector v4{xMin, yMin, zMax};
    Vector v5{xMax, yMin, zMax};
    Vector v6{xMax, yMax, zMax};
    Vector v7{xMin, yMax, zMax};

    // +Z
    addTexturedQuad(mesh, v4, v5, v6, v7, color);

    // -Z
    addTexturedQuad(mesh, v0, v3, v2, v1, color);

    // -X
    addTexturedQuad(mesh, v0, v4, v7, v3, color);

    // +X
    addTexturedQuad(mesh, v1, v2, v6, v5, color);

    // +Y
    addTexturedQuad(mesh, v3, v7, v6, v2, color);

    // -Y
    addTexturedQuad(mesh, v0, v1, v5, v4, color);
}

void addBox(
    Mesh2d &mesh,
    Vector center,
    Vector size,
    RGBA color)
{
    float hx = size.x / 2.0f;
    float hy = size.y / 2.0f;
    float hz = size.z / 2.0f;

    int baseIndex = static_cast<int>(mesh.Vertices.size());

    mesh.Vertices.emplace_back(
        Vertex{{center.x - hx, center.y - hy, center.z - hz}, color, 0.0f});

    mesh.Vertices.emplace_back(
        Vertex{{center.x + hx, center.y - hy, center.z - hz}, color, 0.0f});

    mesh.Vertices.emplace_back(
        Vertex{{center.x + hx, center.y + hy, center.z - hz}, color, 0.0f});

    mesh.Vertices.emplace_back(
        Vertex{{center.x - hx, center.y + hy, center.z - hz}, color, 0.0f});

    mesh.Vertices.emplace_back(
        Vertex{{center.x - hx, center.y - hy, center.z + hz}, color, 0.0f});

    mesh.Vertices.emplace_back(
        Vertex{{center.x + hx, center.y - hy, center.z + hz}, color, 0.0f});

    mesh.Vertices.emplace_back(
        Vertex{{center.x + hx, center.y + hy, center.z + hz}, color, 0.0f});

    mesh.Vertices.emplace_back(
        Vertex{{center.x - hx, center.y + hy, center.z + hz}, color, 0.0f});

    int boxIndices[] =
    {
        // +Z
        4, 5, 6,
        4, 6, 7,

        // -Z
        0, 3, 2,
        0, 2, 1,

        // -X
        0, 4, 7,
        0, 7, 3,

        // +X
        1, 2, 6,
        1, 6, 5,

        // +Y
        3, 7, 6,
        3, 6, 2,

        // -Y
        0, 1, 5,
        0, 5, 4
    };

    for (int index : boxIndices)
    {
        mesh.Indices.emplace_back(baseIndex + index);
    }
}

Mesh2d createTexturedGoober()
{
    Mesh2d texturedGoober{};

    RGBA blue
    {50.0f, 110.0f, 220.0f, 255.0f};

    RGBA green
    {40.0f, 210.0f, 110.0f, 255.0f};

    RGBA white
    {255.0f, 255.0f, 255.0f, 255.0f};

    RGBA yellow
    {255.0f, 220.0f, 40.0f, 255.0f};

    RGBA black
    {15.0f, 15.0f, 15.0f, 255.0f};

    RGBA red
    {220.0f, 65.0f, 55.0f, 255.0f};

    RGBA magenta
    {220.0f, 60.0f, 210.0f, 255.0f};

    // Body
    addTexturedBox(
        texturedGoober,
        {0.0f, 0.0f, 0.0f},
        {2.5f, 2.5f, 1.5f},
        blue
    );

    // Head
    addTexturedBox(
        texturedGoober,
        {0.0f, 2.4f, 0.0f},
        {4.5f, 2.5f, 2.0f},
        green
    );

    // Left eye
    addTexturedBox(
        texturedGoober,
        {-1.2f, 2.7f, -1.15f},
        {1.1f, 1.1f, 0.4f},
        white
    );

    // Left pupil
    addTexturedBox(
        texturedGoober,
        {-1.2f, 2.7f, -1.40f},
        {0.4f, 0.4f, 0.2f},
        black
    );

    // Right eye
    addTexturedBox(
        texturedGoober,
        {1.25f, 2.5f, -1.15f},
        {0.75f, 0.75f, 0.4f},
        yellow
    );

    // Right pupil
    addTexturedBox(
        texturedGoober,
        {1.25f, 2.5f, -1.40f},
        {0.25f, 0.25f, 0.2f},
        black
    );

    // Left foot
    addTexturedBox(
        texturedGoober,
        {-0.8f, -1.65f, 0.15f},
        {0.8f, 0.8f, 1.7f},
        red
    );

    // Right foot
    addTexturedBox(
        texturedGoober,
        {0.8f, -1.65f, 0.15f},
        {0.8f, 0.8f, 1.7f},
        red
    );

    // Antenna stalk
    addTexturedBox(
        texturedGoober,
        {0.0f, 4.3f, 0.0f},
        {0.25f, 1.2f, 0.25f},
        magenta
    );

    // Antenna cap
    addTexturedBox(
        texturedGoober,
        {0.0f, 4.9f, 0.0f},
        {0.65f, 0.65f, 0.65f},
        magenta
    );

    return texturedGoober;
}

Mesh2d createGooberMesh()
{
    Mesh2d goober{};

    RGBA headColor   {70, 200, 120, 255};
    RGBA bodyColor   {70, 120, 220, 255};
    RGBA eyeColor1   {255, 255, 255, 255};
    RGBA eyeColor2   {255, 220, 80, 255};
    RGBA pupilColor  {20, 20, 20, 255};
    RGBA footColor   {220, 80, 80, 255};
    RGBA antennaColor{220, 80, 220, 255};

    // Body
    addBox(
        goober,
        {0.0f, 0.0f, 0.0f},
        {2.5f, 2.5f, 1.5f},
        bodyColor);

    // Oversized head
    addBox(
        goober,
        {0.0f, 2.4f, 0.0f},
        {4.5f, 2.5f, 2.0f},
        headColor);

    // Left giant eye
    addBox(
        goober,
        {-1.2f, 2.7f, -1.15f},
        {1.1f, 1.1f, 0.4f},
        eyeColor1);

    addBox(
        goober,
        {-1.2f, 2.7f, -1.40f},
        {0.40f, 0.40f, 0.20f},
        pupilColor);

    // Right eye — deliberately smaller
    addBox(
        goober,
        {1.25f, 2.5f, -1.15f},
        {0.75f, 0.75f, 0.4f},
        eyeColor2);

    addBox(
        goober,
        {1.25f, 2.5f, -1.40f},
        {0.25f, 0.25f, 0.20f},
        pupilColor);

    // Tiny feet
    addBox(
        goober,
        {-0.8f, -1.65f, 0.15f},
        {0.8f, 0.8f, 1.7f},
        footColor);

    addBox(
        goober,
        {0.8f, -1.65f, 0.15f},
        {0.8f, 0.8f, 1.7f},
        footColor);

    // Antenna stalk
    addBox(
        goober,
        {0.0f, 4.3f, 0.0f},
        {0.25f, 1.2f, 0.25f},
        antennaColor);

    // Antenna cap
    addBox(
        goober,
        {0.0f, 4.9f, 0.0f},
        {0.65f, 0.65f, 0.65f},
        antennaColor);

    return goober;
}

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