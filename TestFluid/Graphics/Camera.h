#pragma once

#include "../main.h"

class Camera
{
public:
    Camera(glm::vec3 position, float aspectRatio);

    glm::mat4 getCameraMat();
    void rotate(glm::vec2 rot); // using radians
    void updateAspectRatio(float aspectRatio);
    void updateFOV(float fov); // degrees
    glm::vec3 getDirection();

    ~Camera();

    glm::vec3 position;
    
private:
    glm::vec3 direction = glm::vec3(1.0, 0.0, 0.0);
    glm::vec2 rotation = glm::vec2(0.0);
    glm::mat4 perspective;
    float aspectRatio;
    float zoom = 90.0f;
};