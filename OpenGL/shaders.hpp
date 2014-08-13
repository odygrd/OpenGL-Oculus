//  LoadShaders() returns the shader program value (as returned by
//    glCreateProgram()) on success, or zero on failure. 
//
//  The array of structures is terminated by a final Shader with the
//    "type" field set to GL_NONE.
//
#ifndef SHADER_HPP
#define SHADER_HPP

#include <GL\glew.h>

#include <string.h>
#include <iostream>
#include <vector>
#include <algorithm>

//Shader - OpenGL shader loading and compiling.

class Shader
{
public:
	bool LoadShader(std::string sFile, int shader_type); //Loads and compiles shader.
	void DeleteShader(); //Deletes shader and frees memory in GPU.

	bool IsLoaded(); // True if shader was loaded and compiled.
	GLuint GetShaderID(); // Returns ID of a generated shader.

	Shader();

private:
	GLuint _shaderId; // ID of shader
	int _type; // GL_VERTEX_SHADER, GL_FRAGMENT_SHADER...
	bool _loaded; // Whether shader was loaded and compiled
};


//ShaderProgram - OpenGL shader program.

class ShaderProgram
{
public:
	void CreateProgram(); //Creates a new program.
	void DeleteProgram(); //Deletes program and frees memory on GPU.

	bool AddShaderToProgram(Shader* shShader); //Adds a compiled shader to program.
	bool LinkProgram(); //Performs final linkage of OpenGL program.

	void UseProgram(); //Tells OpenGL to use this program.
	GLuint GetProgramID(); //Returns OpenGL generated shader program ID.

	ShaderProgram();

private:
	GLuint _programId; // ID of program
	bool _linked; // Whether program was linked and is ready to use
};

bool LinkShaderPrograms(); // Load shaders and create shader program
void DeleteShaders();

 

#endif