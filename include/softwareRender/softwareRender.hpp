#ifndef SOFTWARE_RENDER_HPP
#define SOFTWARE_RENDER_HPP

#include <softwareRender/structsANDoperators.hpp>

// Takes in mesh data and sends it to rasterizer
void RenderModel(LoadedModel const &model, framebuffer &frameBufferData, Transformation const &transform, Camera &cameraTransf);
void RenderMesh(Mesh2d const &mesh, framebuffer &frameBufferData, Texture const &texture, 
    Matrix4x4 const &toWorldSpaceMatrix, Matrix4x4 const &NormalMatrix, Matrix4x4 const &viewMatrix, Matrix4x4 const &projectionMatrix);

#endif