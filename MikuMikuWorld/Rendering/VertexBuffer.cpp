#include "VertexBuffer.h"
#include "glad/glad.h"
#include <cassert>
#include <cstddef>
#include <iostream>

namespace MikuMikuWorld
{
	VertexBuffer::VertexBuffer(int _capacity)
	    : vertexCapacity{ _capacity }, bufferPos{ 0 }, vao{ 0 }, vbo{ 0 }, ebo{ 0 }
	{
		buffer = nullptr;
		indices = nullptr;
		indexCapacity = (vertexCapacity * 6) / 4;
	}

	void VertexBuffer::setup()
	{
		buffer = new Vertex[vertexCapacity];
		indices = new int[indexCapacity];

		size_t offset = 0;
		for (size_t index = 0; index < indexCapacity; index += 6)
		{
			indices[index + 0] = offset + 0;
			indices[index + 1] = offset + 1;
			indices[index + 2] = offset + 2;

			indices[index + 3] = offset + 2;
			indices[index + 4] = offset + 3;
			indices[index + 5] = offset + 0;

			offset += 4;
		}

		glGenVertexArrays(1, &vao);
		glBindVertexArray(vao);

		glGenBuffers(1, &vbo);
		glGenBuffers(1, &ebo);

		glBindBuffer(GL_ARRAY_BUFFER, vbo);
		glBufferData(GL_ARRAY_BUFFER, vertexCapacity * sizeof(Vertex), NULL, GL_DYNAMIC_DRAW);

		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo);
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, indexCapacity * sizeof(unsigned int), indices,
		             GL_STATIC_DRAW);

		glEnableVertexAttribArray(0);
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex),
		                      (void*)offsetof(Vertex, position));

		glEnableVertexAttribArray(1);
		glVertexAttribPointer(1, 4, GL_FLOAT, GL_FALSE, sizeof(Vertex),
		                      (void*)offsetof(Vertex, color));

		glEnableVertexAttribArray(2);
		glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex),
		                      (void*)offsetof(Vertex, uv));

		glBindBuffer(GL_ARRAY_BUFFER, 0);
		glBindVertexArray(0);

		if (buffer == nullptr || indices == nullptr)
		{
			std::cerr << "ERROR: VertexBuffer::setup() Failed to allocate memory for buffer\n";
			assert(false);
		}
		else
		{
			std::cout << "VertexBuffer::setup() Allocated " << vertexCapacity * sizeof(Vertex)
			          << " bytes\n";
		}
	}

	void VertexBuffer::dispose()
	{
		delete[] buffer;
		delete[] indices;

		buffer = nullptr;
		indices = nullptr;

		glDeleteVertexArrays(1, &vao);
		glDeleteBuffers(1, &vbo);
		glDeleteBuffers(1, &ebo);
	}

	void VertexBuffer::bind() const
	{
		glBindVertexArray(vao);
		glBindBuffer(GL_ARRAY_BUFFER, vbo);
	}

	int VertexBuffer::getSize() const { return bufferPos * sizeof(Vertex); }

	int VertexBuffer::getCapacity() const { return vertexCapacity; }

	void VertexBuffer::pushBuffer(const Quad& q)
	{
		for (int offset = 0; offset < 4; ++offset)
		{
			buffer[bufferPos + offset].position =
			    DirectX::XMVector2Transform(q.vertices[offset].position, q.matrix);
			buffer[bufferPos + offset].color = q.vertices[offset].color;
			buffer[bufferPos + offset].uv = q.vertices[offset].uv;
		}

		bufferPos += 4;
	}

	void VertexBuffer::resetBufferPos() { bufferPos = 0; }

	void VertexBuffer::uploadBuffer()
	{
		size_t size = getSize();
		glBufferSubData(GL_ARRAY_BUFFER, 0, size, buffer);
	}

	void VertexBuffer::flushBuffer()
	{
		size_t numIndices = (bufferPos / 4) * 6;
		glDrawElements(GL_TRIANGLES, numIndices, GL_UNSIGNED_INT, 0);
	}
}
