#include <objLoader.hpp>
#include <logger.hpp>

#include <SDL3/SDL.h>

#include <cstdint>
#include <filesystem>
#include <fstream>
#include <iostream>
#include <string>
#include <sstream>

int findMaterialIndex(LoadedModel const &model, std::string const &materialName);
LoadedModel buildLoadedModel(OBJData const &OBJdata, MTLData const &MTLdata);
MTLData loadMTLdata(char const *filepath);
Texture textureLoader(SDL_Surface *Surface);
Texture loadTexture(char const *filepath);
int resolveOBJIndex(int OBJindex, int currentCount);
OBJVertexIndex parseToken(std::string const &vertexToken, int positioncount, int uvcount, int normalcount);
OBJData loadOBJdata(char const *filepath);


LoadedModel LoadModel(char const *filepath)
{
    OBJData OBJdata = loadOBJdata(filepath);

    std::filesystem::path OBJpath(filepath);
    std::filesystem::path OBJdirectory = OBJpath.parent_path();

    MTLData MTLdata{};
    if (!OBJdata.materialLibrary.empty())
    {
        std::filesystem::path mtlpath = (OBJdirectory / OBJdata.materialLibrary).lexically_normal();
        std::string mtlpathstr = mtlpath.string();
        MTLdata = loadMTLdata(mtlpathstr.c_str());
    }
    else
    {
        Logger(logLevel::Warning, "OBJdata.materialLibrary empty. Using default material.");
    }

    LoadedModel loadedModel = buildLoadedModel(OBJdata, MTLdata);
    return loadedModel;
}

OBJData loadOBJdata(char const *filepath)
{
    OBJData data{};
    std::ifstream file(filepath);
    if (!file.is_open())
    {
        Logger(logLevel::Error, "Failed to open OBJ file.");    
        return {};
    }

    std::string currentMaterial;

    std::string line;
    while (std::getline(file, line))
    {
        if (line.starts_with("v "))
        {
            std::istringstream stream(line);
            std::string prefix;
            float x, y, z;
            stream >> prefix >> x >> y >> z;
            data.positions.emplace_back(Vector{x, y, z});
        }
        if (line.starts_with("vt "))
        {
            std::istringstream stream(line);
            std::string prefix;
            float u, v;
            stream >> prefix >> u >> v;
            data.UVs.emplace_back(VectorUV{u, v});
        }
        if (line.starts_with("vn "))
        {
            std::istringstream stream(line);
            std::string prefix;
            float x, y, z;
            stream >> prefix >> x >> y >> z;
            data.normals.emplace_back(Vector{x, y, z});
        }
        if (line.starts_with("f "))
        {
            std::istringstream stream(line);
            std::string prefix;
            stream >> prefix;
            std::string vertexToken;
            OBJFace OBJface;
            while (stream >> vertexToken)
                {
                    OBJVertexIndex OJBvertexindex = parseToken(vertexToken, data.positions.size(), data.UVs.size(), data.normals.size());
                    OBJface.vertices.emplace_back(OJBvertexindex);
                }
            OBJface.materialName = currentMaterial;
            data.faces.emplace_back(OBJface);
        }
        if (line.starts_with("mtllib "))
        {
            std::istringstream stream(line);
            std::string prefix;
            stream >> prefix;
            std::string mtlFileName;
            stream >> mtlFileName;
            data.materialLibrary = mtlFileName;
        }
        if (line.starts_with("usemtl "))
        {
            std::istringstream stream(line);
            std::string prefix;
            stream >> prefix;
            std::string material;
            stream >> material;
            currentMaterial = material;
        }
    }
return data;
}

OBJVertexIndex parseToken(std::string const &vertexToken, int positioncount, int uvcount, int normalcount)
{
    std::istringstream tokenStream(vertexToken);
    std::string positionString;
    std::string uvString;
    std::string normalString;
    std::getline (tokenStream, positionString,'/');
    std::getline (tokenStream, uvString,'/');
    std::getline (tokenStream, normalString,'/');
    // -1 is because an Index of 1 (such as provided in the OBJ data)
    // is an index of 0 in c++ 
    int position = -1;
    int uv = -1;
    int normal = -1;
    position = resolveOBJIndex(std::stoi(positionString), positioncount);
    if (!uvString.empty())
    {
    uv = resolveOBJIndex(std::stoi(uvString), uvcount);
    }
    if (!normalString.empty())
    {
    normal = resolveOBJIndex(std::stoi(normalString), normalcount);
    }
    OBJVertexIndex result{position, uv, normal};
    return result;
}

int resolveOBJIndex(int OBJindex, int currentCount)
{
    if (OBJindex > 0)
    {
        OBJindex = OBJindex - 1;
        if (0 > OBJindex || OBJindex >= currentCount)
        {
            return -1;
        }
        return OBJindex;
    }
    if (OBJindex < 0)
    {
        OBJindex = OBJindex + currentCount;
        if (0 > OBJindex || OBJindex >= currentCount)
        {
            return -1;
        }
        return OBJindex;
    }

    return -1;
}

Texture loadTexture(char const *filepath)
{
    SDL_Surface *loadedSurface = SDL_LoadSurface(filepath);

    if (loadedSurface == nullptr)
    {
        Logger(logLevel::Warning, ("Failed to load texture: " + std::string(SDL_GetError())));
        return {};
    }

    Texture rgbaTexture = textureLoader(loadedSurface);

    return rgbaTexture;
}

Texture textureLoader(SDL_Surface *Surface)
{
    SDL_Surface *rgbaSurface = SDL_ConvertSurface(Surface, SDL_PIXELFORMAT_RGBA32);
    SDL_DestroySurface(Surface);

    if (rgbaSurface == nullptr)
    {
        Logger(logLevel::Warning, ("Failed to convert texture data type: " + std::string(SDL_GetError())));
        return {};
    }
    Texture texture{};
    texture.width = rgbaSurface->w;
    texture.height = rgbaSurface->h;
    texture.pixels.resize(texture.width * texture.height);

    Uint8 *sourcePixels = static_cast<Uint8 *>(rgbaSurface->pixels);

    for (int i = 0; i < texture.height; i++)
    {
        int rowOffset = i * rgbaSurface->pitch;
        for (int j = 0; j < texture.width; j++)
        {
            int sourceIndex = rowOffset + j *4;
            int textureIndex = j + i * texture.width;
            texture.pixels[textureIndex].red   = static_cast<float>(sourcePixels[sourceIndex]);
            texture.pixels[textureIndex].green = static_cast<float>(sourcePixels[sourceIndex + 1]);
            texture.pixels[textureIndex].blue  = static_cast<float>(sourcePixels[sourceIndex + 2]);
            texture.pixels[textureIndex].alpha = static_cast<float>(sourcePixels[sourceIndex + 3]);
        }
    }
    SDL_DestroySurface(rgbaSurface);
    return texture;
}


MTLData loadMTLdata(char const *filepath)
{
    MTLData data{};

    std::filesystem::path mtlpath{filepath};
    std::filesystem::path mtlDirectory = mtlpath.parent_path();


    std::ifstream file(filepath);
    if (!file.is_open())
    {
        Logger(logLevel::Warning, "Failed to open MTL file: " + std::string(filepath));
        return data;
    }
    std::string line;

    // Index for the Materials we have ecountered so far
    int currentMaterialIndex = -1;

    while (std::getline(file, line))
    {
        if (line.starts_with("newmtl "))
        {
            std::istringstream stream(line);
            std::string prefix;
            stream >> prefix;
            std::string newmtl;
            stream >> newmtl;
            MTLMaterialData MaterialData;
            MaterialData.name = newmtl;
            data.materials.emplace_back(MaterialData);
            currentMaterialIndex = static_cast<int>(data.materials.size() - 1);
        }

        if (line.starts_with("Kd "))
        {
            if (currentMaterialIndex < 0)
            {
                continue;
            }
            std::istringstream stream(line);
            std::string prefix;
            stream >> prefix;
            std::string diffuseColor_R;
            std::string diffuseColor_G;
            std::string diffuseColor_B;
            stream >> diffuseColor_R >> diffuseColor_G >> diffuseColor_B;
            float R = 255.0f * std::stof(diffuseColor_R);
            float G = 255.0f * std::stof(diffuseColor_G);
            float B = 255.0f * std::stof(diffuseColor_B);
            data.materials[currentMaterialIndex].diffuseColor = {R, G, B, 255.0f};
        }

        if(line.starts_with("map_Kd "))
        {
            if (currentMaterialIndex < 0)
            {
                continue;
            }
            std::istringstream stream(line);
            std::string prefix;
            stream >> prefix;
            std::string texturePath;
            stream >> texturePath;
            data.materials[currentMaterialIndex].diffuseTexturePath = texturePath;
            std::filesystem::path resolvedTexturePath = mtlDirectory / texturePath;
            data.materials[currentMaterialIndex].diffuseTexturePath = resolvedTexturePath.lexically_normal().string();
        }
    }
    return data;
}

LoadedModel buildLoadedModel(OBJData const &OBJdata, MTLData const &MTLdata)
{
    LoadedModel model{};
    Material defaultMaterial{};
    defaultMaterial.name = "Default";
    defaultMaterial.diffuseColor = {255, 255, 255, 255};
    defaultMaterial.diffuseTexture = createWhiteTexture();
    model.materials.emplace_back(defaultMaterial);

    for (MTLMaterialData const &MTLmaterial : MTLdata.materials)
    {
        Material material {};
        material.name = MTLmaterial.name;
        material.diffuseColor = MTLmaterial.diffuseColor;
        if (!MTLmaterial.diffuseTexturePath.empty())
        {
            material.diffuseTexture = loadTexture(MTLmaterial.diffuseTexturePath.c_str());
            if (material.diffuseTexture.pixels.empty())
            {
                material.diffuseTexture = createWhiteTexture();
            }
        }
        else
        {
            material.diffuseTexture = createWhiteTexture();
        }
        model.materials.emplace_back(material);
    }

    model.subMeshes.resize(model.materials.size());
    for (int i = 0; i < model.subMeshes.size(); i++)
    {
        model.subMeshes[i].materialIndex = i;
    }

    std::string bufferMaterialName;
    for (OBJFace const &face : OBJdata.faces)
    {
        int materialIndex = findMaterialIndex(model, face.materialName);
            if ((!face.materialName.empty()) && (materialIndex == 0) && (face.materialName != bufferMaterialName))
        {
            Logger(logLevel::Warning, ("Could Not Find Material: " + face.materialName + " Using Default Material"));
            bufferMaterialName = face.materialName;
        }
        Mesh2d &mesh = model.subMeshes[materialIndex].mesh;
        if (face.vertices.size() < 3)
        {
            continue;
        }

        bool invalidPosition = false;
        for (OBJVertexIndex const &vertexIndex : face.vertices)
        {
            if (vertexIndex.positionIndex < 0 || vertexIndex.positionIndex >= static_cast<int>(OBJdata.positions.size()))
            {
                invalidPosition = true;
                break;
            }
        }

        if (invalidPosition)
        {
            continue;
        }


        int baseIndex = static_cast<int>(mesh.Vertices.size());
        for (int i = 0; i < face.vertices.size(); i++)
        {
            Vertex vertexbuffer {};
            OBJVertexIndex vertexIndex = face.vertices[i];

            vertexbuffer.position = OBJdata.positions[vertexIndex.positionIndex];
            
            if (vertexIndex.normalIndex >= 0)
            {
                vertexbuffer.normal = OBJdata.normals[vertexIndex.normalIndex];
            }
            if (vertexIndex.UVIndex >= 0)
            {
                vertexbuffer.UV = OBJdata.UVs[vertexIndex.UVIndex];
            }
            vertexbuffer.colorData = model.materials[materialIndex].diffuseColor;
            mesh.Vertices.emplace_back(vertexbuffer);
        }
        for (int i = 1; i + 1 < face.vertices.size(); i++)
        {
        mesh.Indices.emplace_back(baseIndex);
        mesh.Indices.emplace_back (baseIndex + i);
        mesh.Indices.emplace_back(baseIndex + i + 1);
        }
    }

    return model;
}

int findMaterialIndex(LoadedModel const &model, std::string const &materialName)
{
    for (int i = 0; i < static_cast<int>(model.materials.size()); i++)
    {
        if (model.materials[i].name == materialName)
        {
            return i;
        }
    }
    // 0 is the default material
    return 0;
}


// No longer used, probably will be deleted
Mesh2d OBJtoMesh(OBJData const &OBJdata, RGBA const &genericColor)
{
    Mesh2d mesh;
    for (OBJFace const &face: OBJdata.faces)
    {
        if (face.vertices.size() < 3)
        {
            continue;
        }
        bool invalidPosition = false;
        for (OBJVertexIndex const &vertexIndex : face.vertices)
        {
            if (vertexIndex.positionIndex < 0)
            {
                invalidPosition = true;
                break;
            }
        }

        if (invalidPosition)
        {
            continue;
        }

        int baseIndex = static_cast<int>(mesh.Vertices.size());
        for (int i = 0; i < face.vertices.size(); i++)
        {
            Vertex vertexbuffer {};
            OBJVertexIndex vertexIndex = face.vertices[i];

            vertexbuffer.position = OBJdata.positions[vertexIndex.positionIndex];
            
            if (vertexIndex.normalIndex >= 0)
            {
                vertexbuffer.normal = OBJdata.normals[vertexIndex.normalIndex];
            }
            if (vertexIndex.UVIndex >= 0)
            {
                vertexbuffer.UV = OBJdata.UVs[vertexIndex.UVIndex];
            }
            vertexbuffer.colorData = genericColor;
            mesh.Vertices.emplace_back(vertexbuffer);
        }
        for (int i = 1; i + 1 < face.vertices.size(); i++)
        {
        mesh.Indices.emplace_back(baseIndex);
        mesh.Indices.emplace_back (baseIndex + i);
        mesh.Indices.emplace_back(baseIndex + i + 1);
        }
    }
    
    return mesh;
}
