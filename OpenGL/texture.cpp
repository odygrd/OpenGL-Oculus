#include "texture.hpp"

using namespace std;

Texture::Texture()
{
	_mipMapsGenerated = false;
}

void Texture::ReleaseTexture()
{
	glDeleteSamplers(1, &_sampler);
	glDeleteTextures(1, &_texture);
}

bool Texture::LoadTexture2D(string sPath, bool generateMipMaps)
{
	FREE_IMAGE_FORMAT fif = FIF_UNKNOWN;
	FIBITMAP* dib(0);

	fif = FreeImage_GetFileType(sPath.c_str(), 0); // Check the file signature and deduce its format

	if (fif == FIF_UNKNOWN) // If still unknown, try to guess the file format from the file extension
		fif = FreeImage_GetFIFFromFilename(sPath.c_str());

	if (fif == FIF_UNKNOWN) // If still unkown, return failure
		return false;

	if (FreeImage_FIFSupportsReading(fif)) // Check if the plugin has reading capabilities and load the file
		dib = FreeImage_Load(fif, sPath.c_str());
	if (!dib)
		return false;

	BYTE* bDataPointer = FreeImage_GetBits(dib); // Retrieve the image data

	_width = FreeImage_GetWidth(dib); // Get the image width and height
	_height = FreeImage_GetHeight(dib);
	_bpp = FreeImage_GetBPP(dib);

	// If somehow one of these failed (they shouldn't), return failure
	if (bDataPointer == NULL || _width == 0 || _height == 0)
		return false;

	// Generate an OpenGL texture ID for this texture
	glGenTextures(1, &_texture);
	glBindTexture(GL_TEXTURE_2D, _texture);

	int iFormat = _bpp == 24 ? GL_BGR : _bpp == 8 ? GL_LUMINANCE : 0;
	int iInternalFormat = _bpp == 24 ? GL_RGB : GL_DEPTH_COMPONENT;

	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, _width, _height, 0, iFormat, GL_UNSIGNED_BYTE, bDataPointer);

	if (generateMipMaps) glGenerateMipmap(GL_TEXTURE_2D);

	FreeImage_Unload(dib);

	glGenSamplers(1, &_sampler);

	sPath = sPath;
	_mipMapsGenerated = generateMipMaps;

	return true; // Success
}

void Texture::SetFiltering(int tfMagnification, int tfMinification)
{
	// Set magnification filter
	if (tfMagnification == TEXTURE_FILTER_MAG_NEAREST)
		glSamplerParameteri(_sampler, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	else if (tfMagnification == TEXTURE_FILTER_MAG_BILINEAR)
		glSamplerParameteri(_sampler, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

	// Set minification filter
	if (tfMinification == TEXTURE_FILTER_MIN_NEAREST)
		glSamplerParameteri(_sampler, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	else if (tfMinification == TEXTURE_FILTER_MIN_BILINEAR)
		glSamplerParameteri(_sampler, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	else if (tfMinification == TEXTURE_FILTER_MIN_NEAREST_MIPMAP)
		glSamplerParameteri(_sampler, GL_TEXTURE_MIN_FILTER, GL_NEAREST_MIPMAP_NEAREST);
	else if (tfMinification == TEXTURE_FILTER_MIN_BILINEAR_MIPMAP)
		glSamplerParameteri(_sampler, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_NEAREST);
	else if (tfMinification == TEXTURE_FILTER_MIN_TRILINEAR)
		glSamplerParameteri(_sampler, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);

	_tfMinification = tfMagnification;
	_tfMagnification = tfMinification;
}

void Texture::SetSamplerParameter(GLenum parameter, GLenum value)
{
	glSamplerParameteri(_sampler, parameter, value);
}

void Texture::BindTexture(int iTextureUnit)
{
	glActiveTexture(GL_TEXTURE0 + iTextureUnit);
	glBindTexture(GL_TEXTURE_2D, _texture);
	glBindSampler(iTextureUnit, _sampler);
}

int Texture::GetMinificationFilter()
{
	return _tfMinification;
}

int Texture::GetMagnificationFilter()
{
	return _tfMagnification;
}

string Texture::GetPath()
{
	return _sPath;
}