#ifndef PERFORMANCEANDMONITORING_HPP
#define PERFORMANCEANDMONITORING_HPP

#include <SDL3/SDL.h>

#include <array>
#include <cstdint>
#include <iostream>

// my supporting files
#include <softwareRender/structsANDoperators.hpp>
#include <softwareRender/textsystem.hpp>

// Used for FrameTime/ DeltaTime/ FPS
extern double deltaTime;

void performanceAndMonitoring(SETTINGS &settings);
void drawPerformanceAndMonitoring(SETTINGS &settings, framebuffer &buffer);

#endif