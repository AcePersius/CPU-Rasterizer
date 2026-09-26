#include <loaders/gltfLoader.hpp>
#include <fastgltf/core.hpp>
#include <fastgltf/types.hpp>
#include <fastgltf/tools.hpp>
#include <SDL3/SDL.h>


// C++ Files
#include <filesystem>
#include <string>

// My Files
#include <logs/logger.hpp>
#include <softwareRender/structsANDoperators.hpp>
#include <loaders/objLoader.hpp>

/*
FILE ON DISK
    ↓
GltfDataBuffer
    ↓
Parser
    ↓
Asset
    ↓
Mesh
    ↓
Primitive
    ↓
Attributes / Indices
    ↓
Accessor
    ↓
BufferView
    ↓
Buffer
    ↓
actual typed values
    ↓
YOUR engine structs

POSITION
NORMAL
TANGENT
TEXCOORD_n
COLOR_n
JOINTS_n
WEIGHTS_n
*/


LoadedModel GLTFLoader(char const *filepath)
{
    LoadedModel loadedModel{};

    std::filesystem::path path{filepath};

    auto data = fastgltf::GltfDataBuffer::FromPath(path);

    if (data.error() != fastgltf::Error::None)
    {
        Logger(
            logLevel::Error,
            "Failed to read glTF file: " + path.string()
        );

        return loadedModel;
    }

    fastgltf::Parser parser{};

    auto asset = parser.loadGltf(
    data.get(),
    path.parent_path(),
    fastgltf::Options::LoadExternalBuffers
    );

    if (asset.error() != fastgltf::Error::None)
    {
        Logger(
            logLevel::Error,
            "Failed to parse glTF file: " +
            path.string() +
            " Error code: " +
            std::to_string(static_cast<int>(asset.error()))
        );

        return loadedModel;
    }

    fastgltf::Asset gltfAsset = std::move(asset.get());

    Logger(
        logLevel::Info,
        "glTF meshes: " +
        std::to_string(gltfAsset.meshes.size())
    );

    Logger(
        logLevel::Info,
        "glTF accessors: " +
        std::to_string(gltfAsset.accessors.size())
    );

    Logger(
        logLevel::Info,
        "glTF materials: " +
        std::to_string(gltfAsset.materials.size())
    );

    Logger(
        logLevel::Info,
        "glTF nodes: " +
        std::to_string(gltfAsset.nodes.size())
    );

    if (gltfAsset.meshes.empty())
    {
        Logger(logLevel::Error, "gltfAsset.meshes is empty. \n");
        return loadedModel;
    }

    fastgltf::Mesh const &mesh = gltfAsset.meshes[0];

    if (mesh.primitives.empty())
    {
        Logger(logLevel::Error, "mesh.primitives is empty. \n");
        return loadedModel;
    }

    Logger(logLevel::Info, "Primitive Count: " + std::to_string(mesh.primitives.size()));

    for (int primitiveIndex = 0; primitiveIndex < static_cast<int>(mesh.primitives.size()); primitiveIndex++)
    {
        Mesh2d Mesh {};
        SubMesh subMesh{};

        fastgltf::Primitive const &primitive = mesh.primitives[primitiveIndex];

        // Here is the Position Vectors
        auto positionIt = primitive.findAttribute("POSITION");
        if (positionIt == primitive.attributes.end())
        {
            Logger(logLevel::Error, "Primitive has no POSITION attribute. \n");
            continue;
        }
        std::size_t positionAccessorIndex = positionIt->accessorIndex;

        fastgltf::Accessor const &positionAccessor = gltfAsset.accessors[positionAccessorIndex];
        Logger(logLevel::Info, ("Position Vertex Count = " + std::to_string(positionAccessor.count)));
        std::vector<Vector> positions;
        // First time using the reserve member function
        positions.reserve(positionAccessor.count);

        // This is my first time ever using a Lambda function, cool as heck
        fastgltf::iterateAccessor<fastgltf::math::fvec3>(gltfAsset, positionAccessor, [&positions](fastgltf::math::fvec3 position)
        {
            positions.push_back({position[0], position[1], position[2]});
        }
        );
        Logger(logLevel::Info, ("Stored Position Size: " + std::to_string(positions.size()) + "\n"));

        // Here is the Indices
        if (!primitive.indicesAccessor.has_value())
        {
            Logger(logLevel::Error, "Primitive.indicesAccessor is empty.\n");
            continue;
        }
        std::size_t primitiveIndicesIndex = primitive.indicesAccessor.value();
        fastgltf::Accessor const &indicesAccessor = gltfAsset.accessors[primitiveIndicesIndex];
        Logger(logLevel::Info, ("Indices Accessor Count = " + std::to_string(indicesAccessor.count)));

        std::vector<std::uint32_t> indices;
        indices.reserve(indicesAccessor.count);

        fastgltf::iterateAccessor<std::uint32_t>(gltfAsset, indicesAccessor, [&indices](std::uint32_t index)
        {
            indices.push_back(index);
        }
        );
        Logger(logLevel::Info, ("Stored Indices: " + std::to_string(indices.size()) + "\n"));

        // Here is the Normals
        auto normalIt = primitive.findAttribute("NORMAL");
        if (normalIt == primitive.attributes.end())
        {
            Logger(logLevel::Error, "Primitive has no NORMAL attribute. \n");
            continue;
        }

        std::size_t normalAccessorIndex = normalIt->accessorIndex;
        fastgltf::Accessor const & normalAccessor = gltfAsset.accessors[normalAccessorIndex];
        Logger(logLevel::Info, ("Normal count: " + std::to_string(normalAccessor.count) + "\n"));

        std::vector<Vector> normals;
        normals.reserve(normalAccessor.count);

        fastgltf::iterateAccessor<fastgltf::math::fvec3>(gltfAsset, normalAccessor, [&normals](fastgltf::math::fvec3 normal)
        {
            normals.push_back({normal[0], normal[1], normal[2]});
        }
        );
        Logger(logLevel::Info, ("Stored Normals: " + std::to_string(normals.size()) + "\n"));

        // Here is the UV's (Texture_Coords)
        auto UVIt = primitive.findAttribute("TEXCOORD_0");
        if (UVIt == primitive.attributes.end())
        {
            Logger(logLevel::Error, "Primitive has no TEXCOORD_0 attribute. \n");
            continue;
        }
        std::size_t UVAccessorIndex = UVIt->accessorIndex;
        fastgltf::Accessor const &UVAccessor = gltfAsset.accessors[UVAccessorIndex];
        Logger(logLevel::Info, ("UV count: " + std::to_string(UVAccessor.count) + "\n"));

        std::vector<VectorUV> UVs;
        UVs.reserve(UVAccessor.count);

        fastgltf::iterateAccessor<fastgltf::math::fvec2>(gltfAsset, UVAccessor, [&UVs](fastgltf::math::fvec2 UV)
        {
            UVs.push_back({UV[0], UV[1]});
        }
        );
        Logger(logLevel::Info, ("UV count: " + std::to_string(UVs.size()) + "\n"));

        if (UVs.empty())
        {
            Logger(logLevel::Error, "TEXCOORD_0 contained no UV data.");
            continue;
        }

        float minU = UVs[0].u;
        float maxU = UVs[0].u;
        float minV = UVs[0].v;
        float maxV = UVs[0].v;

        for (VectorUV const &uv : UVs)
        {
            if (uv.u < minU) minU = uv.u;
            if (uv.u > maxU) maxU = uv.u;

            if (uv.v < minV) minV = uv.v;
            if (uv.v > maxV) maxV = uv.v;
        }

        Logger(
        logLevel::Info,
        "UV Range: U[" +
        std::to_string(minU) + ", " +
        std::to_string(maxU) +
        "] V[" +
        std::to_string(minV) + ", " +
        std::to_string(maxV) + "]"
        );

        // Here is Vertices Assembler
        std::vector<Vertex> Vertices;
        Vertices.reserve(positionAccessor.count);

        if (positions.size() != normals.size() || positions.size() != UVs.size())
        {
            Logger(
                logLevel::Error,
                "Primitive vertex attribute counts do not match."
            );
            continue;
        }

        for (int i = 0; i < static_cast<int>(positions.size()); i++)
        {
            Vertex vertex {};
            vertex.position = positions[i];
            vertex.normal = normals[i];
            vertex.UV = UVs[i];
            vertex.colorData = {255.0f, 255.0f, 255.0f, 255.0f};
            Vertices.push_back(vertex);
        }

        // Here is the Mesh Assembler
        Mesh.Vertices = std::move(Vertices);
        Mesh.Indices.reserve(indices.size());
        for (std::uint32_t index : indices)
        {
            Mesh.Indices.push_back(static_cast<int>(index));
        }
        // temp
        Logger(logLevel::Info, ("Mesh Size: Vertices: " + std::to_string(Mesh.Vertices.size()) + " Indices: " + std::to_string(Mesh.Indices.size()) + "\n"));

        // This is the Materials Loader

        /*Material
        └── PBR data
            ├── base color
            ├── base color texture
            ├── metallic
            └── roughness*/
        if (!primitive.materialIndex.has_value())
        {
            Logger(logLevel::Warning, "Primitive does not contain materials. \n");
            continue;
        }

        std::size_t materialIndex = primitive.materialIndex.value();
        fastgltf::Material const &gltfMaterial = gltfAsset.materials[materialIndex];
        Logger(logLevel::Info, "Material Info: " + std::string(gltfMaterial.name));

        if (gltfMaterial.pbrData.baseColorTexture.has_value())
        {
            Logger(logLevel::Info, "Material has a base color texture. \n");
        }
        else
        {
            Logger(logLevel::Info, "Material has no base color texture. \n");
            continue;
        }

        auto const &baseColorTextureInfo = gltfMaterial.pbrData.baseColorTexture.value();
        Logger(
        logLevel::Info,
        "Base Color TEXCOORD index: " +
        std::to_string(baseColorTextureInfo.texCoordIndex)
        );
        std::size_t textureIndex = baseColorTextureInfo.textureIndex;
        fastgltf::Texture const &gltfTexture = gltfAsset.textures[textureIndex];

        if (!gltfTexture.imageIndex.has_value())
        {
            Logger(logLevel::Error, "Base color texture has no image reference. \n");
            continue;
        }

        std::size_t imageIndex = gltfTexture.imageIndex.value();
        fastgltf::Image const &gltfImage = gltfAsset.images[imageIndex];
        Logger(logLevel::Info, "Successfully found base color image. \n");

        Material material {};
        material.name = std::string(gltfMaterial.name);
        material.diffuseColor = {255.0f, 255.0f, 255.0f, 255.0f};

        if (auto bufferViewSource = std::get_if<fastgltf::sources::BufferView>(&gltfImage.data))
        {
            Logger(logLevel::Info, "Image Source: BufferView");
            fastgltf::BufferView const &imageBufferView = gltfAsset.bufferViews[bufferViewSource->bufferViewIndex];
            std::size_t bufferIndex = imageBufferView.bufferIndex;
            fastgltf::Buffer const &imageBuffer = gltfAsset.buffers[bufferIndex];
            Logger(logLevel::Info, "Buffer Index: " + std::to_string(bufferIndex));
            Logger(logLevel::Info, "Buffer Offset: " + std::to_string(imageBufferView.byteOffset));
            Logger(logLevel::Info, "Byte Length: " + std::to_string(imageBufferView.byteLength));

            auto bufferData =
            std::get_if<fastgltf::sources::Array>(&imageBuffer.data);

            if (bufferData == nullptr)
            {
                Logger(
                    logLevel::Error,
                    "GLB buffer is not stored as an in-memory Array."
                );
                continue;
            }

            auto const *imageBytes = bufferData->bytes.data() + imageBufferView.byteOffset;
            SDL_IOStream *imageStream = SDL_IOFromConstMem(imageBytes, imageBufferView.byteLength);
            if (imageStream == nullptr)
            {
                Logger(logLevel::Error, "Failed to create SDL image stream: " + std::string(SDL_GetError()));
                continue;
            }
            SDL_Surface *loadedSurface = SDL_LoadSurface_IO(imageStream, true);

            if (loadedSurface == nullptr)
            {
                Logger(logLevel::Error, "Failed to decode embedded glTF image: " + std::string(SDL_GetError()));
                continue;
            }

            material.diffuseTexture = textureLoader(loadedSurface);

            Logger(logLevel::Info, "Decoded embedded image: " + std::to_string(loadedSurface->w) + "x" + std::to_string(loadedSurface->h));

            SDL_DestroySurface(loadedSurface);

        }


        else if (auto urisource = std::get_if<fastgltf::sources::URI>(&gltfImage.data))
        {
            Logger(logLevel::Info, "Image Source: URI \n");
        }

        else
        {
            Logger(logLevel::Error, "Image Source Unknown. \n");
        }

        loadedModel.materials.push_back(std::move(material));
        subMesh.mesh = std::move(Mesh);
        subMesh.materialIndex = static_cast<int>(loadedModel.materials.size() - 1);
        loadedModel.subMeshes.push_back(std::move(subMesh));
    }
    Logger(logLevel::Info, "LoadedModel SubMeshes: " + std::to_string(loadedModel.subMeshes.size()) + " Materials: " + std::to_string(loadedModel.materials.size()));

    return loadedModel;
}