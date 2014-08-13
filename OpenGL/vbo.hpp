//	Wraps OpenGL vertex buffer

#ifndef VBO_HPP
#define VBO_HPP

#include "GL\glew.h"
#include <vector>

typedef unsigned char BYTE;

class VBO
{
public:
	VBO();
	virtual ~VBO();

	void CreateVBO(int buffer_size = 0); // Creates vertex buffer object. buffer_size - Initial size of the buffer

	void* MapBufferToMemory(int usage_hint); // Maps whole buffer data to memory and returns pointer to data. usage_hint - GL_READ_ONLY, GL_WRITE_ONLY... 
	void* MapSubBufferToMemory(int usage_hint, GLuint data_offset, GLuint data_length); // Maps specified part of buffer to memory.	
	void UnmapBuffer(); // Unmaps previously mapped buffer.

	void BindVBO(int buffer_type = GL_ARRAY_BUFFER); //Binds this VBO. a_iBufferType - buffer type (GL_ARRAY_BUFFER, ...)
	void UploadDataToGPU(int usage_hint); //Sends data to GPU. usage_hint - GL_STATIC_DRAW, GL_DYNAMIC_DRAW...

	void AddData(void* ptrData, GLuint data_size); //Adds arbitrary data to VBO. Params:	ptrData - pointer to arbitrary data_size in bites
	int GetCurrentSize();

	void* GetDataPointer(); //Returns data pointer (only before uplading).
	GLuint GetBuffer(); //Returns VBO ID.

private:
	GLuint _uiBuffer;
	int _iSize;
	int _iBufferType;
	int _currentSize;

	std::vector<BYTE> _data;

	bool _dataUploaded;
};


#endif