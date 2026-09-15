#ifndef PERFORMANCEANDMONITORING_HPP
#define PERFORMANCEANDMONITORING_HPP

// Used for std::array
#include <array>
// Used for uint_t vals
#include <cstdint>

// Used for FrameTime/ DeltaTime/ FPS
uint64_t previousTime = SDL_GetPerformanceCounter();
double statsTimer = 0;
int frameCount = 0;
double AvgFPS = 0;
double AvgFrameTimeMS = 0;
double deltaTime;

// Coord for the Counters
XYcoord FirstElement  = {0, 0};
XYcoord SecondElement = {0, 8};

// deltaTime/ frameTime/ FPS
void performanceAndMonitoring(framebuffer &buffer)
{
    uint64_t currentTime = SDL_GetPerformanceCounter();
    deltaTime = static_cast<double>(currentTime - previousTime) / static_cast<double>(SDL_GetPerformanceFrequency());
    previousTime = currentTime;
    std::string bufferString = "";
    if (buffer.PerfAndMonitoring == None)
    {
        return;
    } 
    if (buffer.PerfAndMonitoring == FpsandFt)
    {
        statsTimer += deltaTime;
        frameCount ++;
        if (statsTimer >= 0.5)
        {
            AvgFPS = frameCount / statsTimer;
            AvgFrameTimeMS = (statsTimer / frameCount) * 1000.0;
            statsTimer = 0.0;
            frameCount = 0;
            std::cout
            << "FPS: " << AvgFPS
            << "Frame Time: " << AvgFrameTimeMS
            << " ms\n";
        }
        return;
    }
    if (buffer.PerfAndMonitoring == Fps)
    {
        statsTimer += deltaTime;
        frameCount ++;
        if (statsTimer >= 0.5)
        {
            AvgFPS = frameCount / statsTimer;
            statsTimer = 0.0;
            frameCount = 0;
            std::cout
            << "FPS: " << AvgFPS
            << " \n";
        }
        return;
    }
    if (buffer.PerfAndMonitoring == Ft)
    {
        statsTimer += deltaTime;
        frameCount ++;
        if (statsTimer >= 0.5)
        {
            AvgFrameTimeMS = (statsTimer / frameCount) * 1000.0;
            statsTimer = 0.0;
            frameCount = 0;
            std::cout
            << "Frame Time: " << AvgFrameTimeMS
            << " ms\n";
        }
        return;
    }
}


void drawPerformanceAndMonitoring(framebuffer &buffer)
{
    std::string bufferString = "";
    if (buffer.PerfAndMonitoring == None)
    {
        return;
    } 
    if (buffer.PerfAndMonitoring == FpsandFt)
    {
        bufferString = "FPS: " + std::to_string(AvgFPS);
        drawString(FirstElement, buffer, bufferString);
        bufferString = "FRAME TIME: " + std::to_string(AvgFrameTimeMS) + "MS";
        drawString(SecondElement, buffer, bufferString);
        return;
    }
    if (buffer.PerfAndMonitoring == Fps)
    {
        bufferString = "FPS: " + std::to_string(AvgFPS);
        drawString(FirstElement, buffer, bufferString);
        return;
    }
    if (buffer.PerfAndMonitoring == Ft)
    {
        bufferString = "FRAME TIME: " + std::to_string(AvgFrameTimeMS) + "MS";
        drawString(FirstElement, buffer, bufferString);
        return;
    }
}

#endif