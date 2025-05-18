#include "../File.h"
#include "../IO.h"
#include "Texture.h"
#include <cassert>
#include <glad/glad.h>
#include "GLFW/glfw3.h"
#include "stb_image.h"
#include <filesystem>
#include <fstream>
#include <iostream>

using namespace IO;

namespace MikuMikuWorld
{
	Texture::Texture(const std::string& filename)
	    : Texture(filename, TextureFilterMode::Linear, TextureFilterMode::Linear)
	{
	}

	Texture::Texture(const std::string& filename, TextureFilterMode min, TextureFilterMode mag)
	{
		this->filename = filename;
		name = File::getFilenameWithoutExtension(filename);
		read(filename, min, mag);

		std::string sprSheet = File::getFilepath(filename) + "spr/" + name + ".txt";
		if (File::exists(sprSheet))
		{
			readSprites(sprSheet);
		}
		else
		{
			sprites.push_back(Sprite(name, 0, 0, width, height));
		}
	}

	Texture::Texture(const std::string& filename, TextureFilterMode filter)
	    : Texture(filename, filter, filter)
	{
	}

	void Texture::bind() const { glBindTexture(GL_TEXTURE_2D, glID); }

	void Texture::dispose() const { glDeleteTextures(1, &glID); }

	void Texture::readSprites(const std::string& filename)
	{
		std::wstring wFilename = mbToWideStr(filename);
		File f(wFilename, L"r");
		std::vector<std::string> lines = f.readAllLines();
		f.close();

		for (auto& line : lines)
		{
			line = trim(line);
			if (!isComment(line, "#") && line.size())
				sprites.push_back(parseSprite(f, line));
		}
	}

	Sprite Texture::parseSprite(const File& f, const std::string& line)
	{
		std::vector<std::string> values = split(line, ",");
		int x = atoi(values[0].c_str());
		int y = atoi(values[1].c_str());
		int w = atoi(values[2].c_str());
		int h = atoi(values[3].c_str());

		return Sprite(name, x, y, w, h);
	}

	void Texture::read(const std::string& filename, TextureFilterMode minFilter,
	                   TextureFilterMode magFilter)
	{
		glGenTextures(1, &glID);
		glBindTexture(GL_TEXTURE_2D, glID);

		int nrChannels;
		stbi_set_flip_vertically_on_load(0);
		auto data = stbi_load(filename.c_str(), &width, &height, &nrChannels, 4);
		if (data == nullptr)
		{
			std::cerr << "Failed to load texture: " << filename << std::endl;
			std::cerr << "Error: " << stbi_failure_reason() << std::endl;
			glDeleteTextures(1, &glID);
			glBindTexture(GL_TEXTURE_2D, 0);
			return;
		}

		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);
		assert(glGetError() == GL_NO_ERROR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_BASE_LEVEL, 0);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
		glGenerateMipmap(GL_TEXTURE_2D);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, (GLint)minFilter);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, (GLint)magFilter);

		glBindTexture(GL_TEXTURE_2D, 0);
		stbi_image_free(data);
	}
}
