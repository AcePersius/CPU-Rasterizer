#include <softwareRender/mouseANDkeyboard.hpp>

// pitch (x rotation)
// yaw (y rotation)
// roll (z rotation)
void updateCamera(Camera &camera, float const &deltaTime, SETTINGS &settings)
{
    const bool *key_states = SDL_GetKeyboardState(nullptr);
    float movementSpeed = 5.0f;
    float movement = movementSpeed * deltaTime;
    Vector forward{
        std::sin(camera.rotation.y * (PIE / 180)),
        0.0f,
        std::cos(camera.rotation.y * (PIE / 180))
    };
    Vector right{
        std::cos(camera.rotation.y * (PIE / 180)),
        0.0f,
        -std::sin(camera.rotation.y * (PIE / 180))
    };
    if (settings.MovementMode == NOCLIP)
    {
        forward.x = std::sin(camera.rotation.y * (PIE / 180)) * std::cos(camera.rotation.x * (PIE / 180));
        forward.y = -std::sin(camera.rotation.x * (PIE / 180));
        forward.z = std::cos(camera.rotation.y * (PIE / 180)) * std::cos(camera.rotation.x * (PIE / 180));
    }

    if(key_states[SDL_SCANCODE_W])
    {
        camera.position.x += forward.x * movement;
        camera.position.y += forward.y * movement;
        camera.position.z += forward.z * movement;
        camera.viewUpdater = true;
    }
    if(key_states[SDL_SCANCODE_A])
    {
        camera.position.x -= right.x * movement;
        camera.position.z -= right.z * movement;
        camera.viewUpdater = true;
    }
    if(key_states[SDL_SCANCODE_S])
    {
        camera.position.x -= forward.x * movement;
        camera.position.y -= forward.y * movement;
        camera.position.z -= forward.z * movement;
        camera.viewUpdater = true;
    }
    if(key_states[SDL_SCANCODE_D])
    {
        camera.position.x += right.x * movement;
        camera.position.z += right.z * movement;
        camera.viewUpdater = true;
    }
    if(key_states[SDL_SCANCODE_SPACE])
    {
        camera.position.y += movement;
        camera.viewUpdater = true;
    }
    if(key_states[SDL_SCANCODE_LCTRL])
    {
        camera.position.y -= movement;
        camera.viewUpdater = true;
    }    
}
/*
Y-Axis rotation
| cosθ   0   sinθ |
|   0    1     0  |
| -sinθ  0   cosθ |
(0, 0, 1) = forward direction
Therefore:
| cosθ   0   sinθ |   | 0 |
|   0    1     0  | * | 0 |
| -sinθ  0   cosθ |   | 1 |
= 
forward.x = sinθ
forward.y = 0
forward.z = cosθ

| cosθ   0   sinθ |   | 1 |
|   0    1     0  | * | 0 |
| -sinθ  0   cosθ |   | 0 |
(1, 0, 0) = right
=
right.x =  cosθ
right.y =  0
right.z = -sinθ
*/