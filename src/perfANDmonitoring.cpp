#include <softwareRender/perfANDmonitoring.hpp>

double deltaTime;
uint64_t previousTime = SDL_GetPerformanceCounter();
double statsTimer = 0;
int frameCount = 0;
double AvgFPS = 0;
double AvgFrameTimeMS = 0;

// deltaTime/ frameTime/ FPS
void performanceAndMonitoring(SETTINGS &settings)
{
    uint64_t currentTime = SDL_GetPerformanceCounter();
    deltaTime = static_cast<double>(currentTime - previousTime) / static_cast<double>(SDL_GetPerformanceFrequency());
    previousTime = currentTime;
    std::string bufferString = "";
    if (settings.PerfAndMonitoring == None)
    {
        return;
    } 
    if (settings.PerfAndMonitoring == FpsandFt)
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
    if (settings.PerfAndMonitoring == Fps)
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
    if (settings.PerfAndMonitoring == Ft)
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

// Coord for the Counters
XYcoord ElementCoords  = {0, 0};
XYcoord NextTextRow = {0, 8};
void drawPerformanceAndMonitoring(SETTINGS &settings, framebuffer &buffer)
{
    ElementCoords  = {0, 0};
    std::string bufferString = "";
    if (settings.PerfAndMonitoring == None)
    {
        goto CameraMode;
    } 
    if (settings.PerfAndMonitoring == FpsandFt)
    {
        bufferString = "FPS: " + std::to_string(AvgFPS);
        drawString(ElementCoords, buffer, bufferString);
        ElementCoords = ElementCoords + NextTextRow;

        bufferString = "FRAME TIME: " + std::to_string(AvgFrameTimeMS) + "MS";
        drawString(ElementCoords, buffer, bufferString);
        ElementCoords = ElementCoords + NextTextRow;

        goto CameraMode;
    }
    if (settings.PerfAndMonitoring == Fps)
    {
        bufferString = "FPS: " + std::to_string(AvgFPS);
        drawString(ElementCoords, buffer, bufferString);
        ElementCoords = ElementCoords + NextTextRow;

        goto CameraMode;
    }
    if (settings.PerfAndMonitoring == Ft)
    {
        bufferString = "FRAME TIME: " + std::to_string(AvgFrameTimeMS) + "MS";
        drawString(ElementCoords, buffer, bufferString);
        ElementCoords = ElementCoords + NextTextRow;

        goto CameraMode;
    }

    CameraMode:

    if (settings.MovementMode == FPSstyle)
    {
        bufferString = "MODE: FPS";
        drawString(ElementCoords, buffer, bufferString);
        ElementCoords = ElementCoords + NextTextRow;
        goto END;
    }
    if (settings.MovementMode == NOCLIP)
    {
        bufferString = "MODE: NOCLIP";
        drawString(ElementCoords, buffer, bufferString);
        ElementCoords = ElementCoords + NextTextRow;
        goto END;
    }

    END:

    bufferString = "USABLE CHARS: ABCDEFGHIJKLMNOPQRSTUVWXYZ123456789:!.,()/\\";
    drawString(ElementCoords, buffer, bufferString);
    ElementCoords = ElementCoords + NextTextRow;
}
