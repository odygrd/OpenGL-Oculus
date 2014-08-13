#include "vbo.hpp"

//Constructor
VBO::VBO()
{
	_dataUploaded = false;
}

VBO::~VBO()
{
	glDeleteBuffers(1, &_uiBuffer);
	_dataUploaded = false;
	_data.clear();
}

void VBO::CreateVBO(int buffer_size)
{
	glGenBuffers(1, &_uiBuffer);
	_data.reserve(buffer_size); //increase container capacity
	_iSize = buffer_size; 
	_currentSize = 0;
}

void* VBO::MapBufferToMemory(int usage_hint)
{
	if (!_dataUploaded)return NULL;

	void* ptrRes = glMapBuffer(_iBufferType, usage_hint); //generic pointer
	return ptrRes;
}

void* VBO::MapSubBufferToMemory(int usage_hint, GLuint data_offset, GLuint data_length)
{
	if (!_dataUploaded)return NULL;

	void* ptrRes = glMapBufferRange(_iBufferType, data_offset, data_length, usage_hint);
	return ptrRes;
}

void VBO::UnmapBuffer()
{
	glUnmapBuffer(_dataUploaded);
}

void VBO::BindVBO(int buffer_type)
{
	_iBufferType = buffer_type;
	glBindBuffer(_iBufferType, _uiBuffer);
}

void VBO::UploadDataToGPU(int usage_hint)
{
	glBufferData(_iBufferType, _data.size(), &_data[0], usage_hint);
	_dataUploaded = true;
	_data.clear();
}

void VBO::AddData(void* ptrData, GLuint data_size)
{
	_data.insert(_data.end(), (BYTE*)ptrData, (BYTE*)ptrData + data_size);
	_currentSize += data_size;
}

void* VBO::GetDataPointer()
{
	if (_dataUploaded)return NULL;
	return (void*)_data[0];
}

GLuint VBO::GetBuffer()
{
	return _uiBuffer;
}

int VBO::GetCurrentSize()
{
	return _currentSize;
}