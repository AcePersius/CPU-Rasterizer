#ifndef OBJLOADER_HPP
#define OBJLOADER_HPP

#include <SDL3/SDL.h>
#include <SDL3/SDL_main.h>
#include <cstdint>

#include <structsANDoperators.hpp>

Texture loadTexture(char const *filepath)
{
    SDL_Surface *loadedSurface = SDL_LoadSurface(filepath);

    if (loadedSurface == nullptr)
{
    std::cout
        << "Failed to load texture: "
        << SDL_GetError()
        << '\n';
    return {};
}

    SDL_Surface *rgbaSurface = SDL_ConvertSurface(loadedSurface, SDL_PIXELFORMAT_RGBA32);
    SDL_DestroySurface(loadedSurface);

    if (rgbaSurface == nullptr)
    {
        std::cout
            << "Failed to convert texture data type: "
            << SDL_GetError()
            << '\n';
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

#endif