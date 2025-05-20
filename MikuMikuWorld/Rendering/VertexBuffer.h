#pragma once
#include "Quad.h"
#include <glad/glad.h>

namespace MikuMikuWorld
{
	class VertexBuffer
	{
	  private:
		Vertex* buffer;
		int* indices;
		int indexCapacity;
		int vertexCapacity;
		int bufferPos;

		GLuint vao;
		unsigned int vbo;
		unsigned int ebo;

	  public:
		VertexBuffer(int _capacity);

		VertexBuffer(const VertexBuffer&) = delete;

		void setup();
		void dispose();
		void bind() const;
		void pushBuffer(const Quad& q);
		void resetBufferPos();
		void uploadBuffer();
		void flushBuffer();
		int getCapacity() const;
		int getSize() const;
	};
}
