#ifndef OBJLOADER_HPP
#define OBJLOADER_HPP

#include <SDL3/SDL.h>

#include <structsANDoperators.hpp>

// This may or may not be staying
// Mesh2d OBJtoMesh(OBJData const &OBJdata, RGBA const &genericColor);
LoadedModel LoadModel(char const *filepath);
Texture textureLoader(SDL_Surface *Surface);

#endif