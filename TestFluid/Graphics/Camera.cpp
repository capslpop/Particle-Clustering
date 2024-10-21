#include "Camera.h"

#include "../main.h"

// position of camera, direction of camera, width in pixels, height in pixels
Camera::Camera(glm::vec3 pos, float aspectRatio)
{
	position = pos;
	aspectRatio = aspectRatio;

	// now init project matrix
	perspective = glm::perspective(glm::radians(zoom), aspectRatio, 0.01f, 10000.0f);
}

glm::mat4 Camera::getCameraMat()
{
	// multiply using MVP
	return perspective * glm::lookAt(position, position + direction, glm::vec3(0.0, 1.0, 0.0));
}

void Camera::rotate(glm::vec2 rot)
{
	rotation += rot;

	// clamp y rotation so you dont break your neck
	rotation.y = glm::clamp(rotation.y, -3.14159265359f / 2.0f + 0.1f, 3.14159265359f / 2.0f - 0.1f);

	// loop x so it does not get too big
	if (rotation.x < 0.0)
	{
		rotation.x += 3.14159265359f * 2.0f;
	}
	if (rotation.x > 3.14159265359f * 2.0f)
	{
		rotation.x -= 3.14159265359f * 2.0;
	}

	// convert pitch and yaw to a unit vector
	direction.x = glm::cos(rotation.y) * glm::sin(rotation.x);
	direction.y = glm::sin(rotation.y);
	direction.z = glm::cos(rotation.y) * glm::cos(rotation.x);
}

void Camera::updateAspectRatio(float aspectRatio)
{
	aspectRatio = aspectRatio;
	perspective = glm::perspective(glm::radians(zoom), aspectRatio, 0.01f, 10000.0f);
}

void Camera::updateFOV(float fov)
{
	zoom = fov;

	perspective = glm::perspective(glm::radians(zoom), aspectRatio, 0.01f, 10000.0f);
}

glm::vec3 Camera::getDirection()
{
	return direction;
}

Camera::~Camera()
{
}