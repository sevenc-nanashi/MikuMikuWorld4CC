#pragma once

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

namespace MikuMikuWorld
{
	class Camera
	{
	  private:
		float yaw, pitch;
		glm::vec4 position{ 0.0f, 0.0f, -1.0f, 1.0f };
		glm::vec4 target{ 0.0f, 0.0f, 0.0f, 1.0f };
		glm::vec4 front{ 0.0f, 0.0f, 0.0f, 1.0f };
		const glm::vec4 up{ 0.0f, 1.0f, 0.0, 1.0f };

	  public:
		Camera();

		void setPositionY(float posY);

		glm::mat4 getViewMatrix() const;
		glm::mat4 getOrthographicProjection(float width, float height) const;
		glm::mat4 getOffCenterOrthographicProjection(float left, float right, float up,
		                                                     float down) const;
		glm::mat4 getPerspectiveProjection() const;
	};
}
