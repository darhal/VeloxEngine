#pragma once

#include <Core/Misc/Defines/Common.hpp>
#include "Color.hpp"
#include "Common.hpp"

TRE_NS_START

//Supported File Format
namespace ImageFileFormat
{
	enum image_file_format_t
	{
		BMP,
		TGA,
		JPEG,
		PNG
	};
}

class Color;
class ByteReader;
class ByteWriter;

class Image
{
public:
	Image();

	Image(uint16 width, uint16 height, const Color& background);
	Image(uint16 width, uint16 height, uint8* pixels);
	Image(uint16 width, uint16 height, Color* colors);
	Image(uint8* pixels, uint size);
	Image(const String& filename);

	~Image();

	void Load(uint8* pixels, uint size);
	void Load(const String& filename);
	void Save(const String& filename, ImageFileFormat::image_file_format_t format);

	FORCEINLINE uint16 GetWidth() const;
	FORCEINLINE uint16 GetHeight() const;
	FORCEINLINE const Color* GetPixels() const;

	FORCEINLINE Color GetPixel(uint x, uint y) const;
	FORCEINLINE void SetPixel(uint x, uint y, const Color& color);

	FORCEINLINE uint8* GetBytes() const { return (uint8*)image; }

	Image(const Image& i)
	{
		image = i.image;
		width = i.width;
		height = i.height;
	}
	const Image& operator=(const Image& i)
	{
		image = i.image;
		width = i.width;
		height = i.height;
	}

private:
	Color* image;
	uint16 width, height;

	void LoadBMP(ByteReader& data);
	void SaveBMP(const String& filename);

	void LoadTGA(ByteReader& data);
	void DecodeRLE(ByteReader& data, uint decodedLength, uint8 bytesPerPixel);
	void SaveTGA(const String& filename);
	void EncodeRLE(ByteWriter& data, Vector<uint8>& pixels, uint16 width);

	void LoadJPEG(ByteReader& data);
	void SaveJPEG(const String& filename);

	void LoadPNG(ByteReader& data);
	void SavePNG(const String& filename);
};

FORCEINLINE uint16 Image::GetWidth() const
{
	return width;
}

FORCEINLINE uint16 Image::GetHeight() const
{
	return height;
}

FORCEINLINE const Color* Image::GetPixels() const
{
	return image;
}

FORCEINLINE Color Image::GetPixel(uint x, uint y) const
{
	if (x >= width || y >= height) return Color();
	return image[x + y * width];
}

FORCEINLINE void Image::SetPixel(uint x, uint y, const Color& color)
{
	if (x >= width || y >= height) return;
	image[x + y * width] = color;
}

TRE_NS_END