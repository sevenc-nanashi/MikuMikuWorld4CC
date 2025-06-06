#include "Camera.h"

namespace MikuMikuWorld
{
	Camera::Camera() {}

	void Camera::setPositionY(float posY) { position.y = posY; }

	glm::mat4 Camera::getOrthographicProjection(float width, float height) const
	{
		// Using Right-Handed coordinate system, Depth: [0, 1]
		// Parameters for glm::orthoRH: left, right, bottom, top, near, far
		// Assuming +Y up based on typical 3D conventions and original usage
		return glm::orthoRH(0.0f, width, 0.0f, height, 0.001f, 100.0f);
	}

	glm::mat4 Camera::getOffCenterOrthographicProjection(float left, float right, float up,
	                                                             float down) const
	{
		// Using Right-Handed coordinate system, Depth: [0, 1]
		// Parameters for glm::orthoRH: left, right, bottom, top, near, far
		return glm::orthoRH(left, right, down, up, 0.001f, 100.0f);
	}

	glm::mat4 Camera::getViewMatrix() const
	{
		// Using Right-Handed coordinate system, Y-up
		return glm::lookAtRH(glm::vec3(position), glm::vec3(target), glm::vec3(up));
	}
}
