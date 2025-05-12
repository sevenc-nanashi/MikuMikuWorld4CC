#include "BinaryReader.h"
#include "IO.h"

namespace IO
{
	BinaryReader::BinaryReader(const std::string& filename)
	{
#ifdef CHOC_WINDOWS
		stream = NULL;
		std::wstring wFilename = mbToWideStr(filename);
		stream = _wfopen(wFilename.c_str(), L"rb");
#else
		stream = fopen(filename.c_str(), "rb");
#endif
	}

	BinaryReader::~BinaryReader() { close(); }

	bool BinaryReader::isStreamValid() { return stream; }

	void BinaryReader::close()
	{
		if (stream)
			fclose(stream);

		stream = NULL;
	}

	size_t BinaryReader::getFileSize()
	{
		size_t pos = ftell(stream);
		fseek(stream, 0, SEEK_END);

		size_t size = ftell(stream);
		fseek(stream, pos, SEEK_SET);

		return size;
	}

	size_t BinaryReader::getStreamPosition() { return ftell(stream); }

	uint16_t BinaryReader::readUInt16()
	{
		uint16_t data = 0;
		if (stream)
			fread(&data, sizeof(uint16_t), 1, stream);

		return data;
	}

	uint32_t BinaryReader::readUInt32()
	{
		uint32_t data = 0;
		if (stream)
			fread(&data, sizeof(uint32_t), 1, stream);

		return data;
	}

	int16_t BinaryReader::readInt16()
	{
		int16_t data = 0;
		if (stream)
			fread(&data, sizeof(int16_t), 1, stream);

		return data;
	}

	int32_t BinaryReader::readInt32()
	{
		int32_t data = 0;
		if (stream)
			fread(&data, sizeof(int32_t), 1, stream);

		return data;
	}

	float BinaryReader::readSingle()
	{
		float data = 0;
		if (stream)
			fread(&data, sizeof(float), 1, stream);

		return data;
	}

	std::string BinaryReader::readString()
	{
		char c = 'a';
		std::string data = "";
		if (stream)
		{
			while (c && !feof(stream))
			{
				fread(&c, sizeof(uint8_t), 1, stream);
				if (c)
					data += c;
			}
		}
		return data;
	}

	void BinaryReader::seek(size_t pos)
	{
		if (stream)
			fseek(stream, pos, SEEK_SET);
	}
}
