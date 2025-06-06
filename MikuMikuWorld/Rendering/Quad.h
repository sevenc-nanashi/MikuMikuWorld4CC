#pragma once
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <array>

namespace MikuMikuWorld
{
	struct Vertex
	{
		glm::vec4 position;
		glm::vec4 color;
		glm::vec4 uv;
	};

	struct Quad
	{
		int zIndex;
		int sprite;
		int texture;
		Vertex vertices[4];
		glm::mat4 matrix;

		Quad() : zIndex{ 0 }, sprite{ 0 }, texture{ 0 } {}

		Quad(const std::array<Vertex, 4>& v, const glm::mat4& m, int tex, int z = 0)
		{
			sprite = 0;
			std::copy(v.begin(), v.end(), vertices);
				matrix = m;
			texture = tex;
			zIndex = z;
		}
	};
}
