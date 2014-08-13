#ifndef TEXTURE_HPP
#define TEXTURE_HPP

#include "GL\glew.h"

#include <Freeimage\FreeImage.h>
#include <string>

//Class for texture loading
class Texture
{
public:
	Texture();

	bool LoadTexture2D(std::string sPath, bool generateMipMaps = false); //Loads texture from a file, supports mostgraphics formats.
	void BindTexture(int iTextureUnit = 0); //Binds the texture

	void SetFiltering(int tfMagnification, int tfMinification); // Sets magnification and minification texture filter.

	void SetSamplerParameter(GLenum parameter, GLenum value);

	int GetMinificationFilter();
	int GetMagnificationFilter();
	std::string GetPath();

	void ReleaseTexture(); // Frees all memory used by texture.
private:
	int _width, _height, _bpp; // Texture width, height, and bytes per pixel
	GLuint _texture; // Texture name
	GLuint _sampler; // Sampler name
	bool _mipMapsGenerated;

	int _tfMinification;
	int _tfMagnification;

	std::string _sPath;
};

enum ETextureFiltering
{
	TEXTURE_FILTER_MAG_NEAREST = 0, // Nearest criterion for magnification
	TEXTURE_FILTER_MAG_BILINEAR, // Bilinear criterion for magnification
	TEXTURE_FILTER_MIN_NEAREST, // Nearest criterion for minification
	TEXTURE_FILTER_MIN_BILINEAR, // Bilinear criterion for minification
	TEXTURE_FILTER_MIN_NEAREST_MIPMAP, // Nearest criterion for minification, but on closest mipmap
	TEXTURE_FILTER_MIN_BILINEAR_MIPMAP, // Bilinear criterion for minification, but on closest mipmap
	TEXTURE_FILTER_MIN_TRILINEAR, // Bilinear criterion for minification on two closest mipmaps, then averaged
};

#endif