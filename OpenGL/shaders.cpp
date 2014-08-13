#include "shaders.hpp"

using namespace std;

Shader shVertex, shFragment;
ShaderProgram shpMain;

Shader::Shader()
{
	_loaded = false;
}

bool Shader::LoadShader(string sFile, int shader_type)
{
	FILE* fp = fopen(sFile.c_str(), "rt");
	if (!fp){
		cerr << "Unable to open file '" << sFile.c_str() << "'" << endl;
		return false;
	}

	// Get all lines from a file

	vector<string> sLines;
	char sLine[255];
	while (fgets(sLine, 255, fp))sLines.push_back(sLine);
	fclose(fp);

	const char** sProgram = new const char*[(int)sLines.size()];
	for (int i = 0; i < (int)sLines.size(); i++)
		sProgram[i] = sLines[i].c_str();

	_shaderId = glCreateShader(shader_type);

	glShaderSource(_shaderId, (int)sLines.size(), sProgram, NULL);
	glCompileShader(_shaderId);

	delete[] sProgram;

	//Check Shader for Errors
	GLint status;
	glGetShaderiv(_shaderId, GL_COMPILE_STATUS, &status);

	if (status == GL_FALSE)
	{
		GLint infoLogLength;
		glGetShaderiv(_shaderId, GL_INFO_LOG_LENGTH, &infoLogLength);

		GLchar *strInfoLog = new GLchar[infoLogLength + 1];
		glGetShaderInfoLog(_shaderId, infoLogLength, NULL, strInfoLog);
		cerr << "Compile failure in " << sFile.c_str() << endl;
		cerr << strInfoLog << endl;
		delete[] strInfoLog;

		return false;
	}

	_type = shader_type;
	_loaded = true;

	return 1;
}

bool Shader::IsLoaded()
{
	return _loaded;
}

GLuint Shader::GetShaderID()
{
	return _shaderId;
}


void Shader::DeleteShader()
{
	if (!IsLoaded())return;
	_loaded = false;
	glDeleteShader(_shaderId);
}


ShaderProgram::ShaderProgram()
{
	_linked = false;
}


void ShaderProgram::CreateProgram()
{
	_programId = glCreateProgram();
}


bool ShaderProgram::AddShaderToProgram(Shader* shShader)
{
	if (!shShader->IsLoaded())return false;

	glAttachShader(_programId, shShader->GetShaderID());

	return true;
}

bool ShaderProgram::LinkProgram()
{
	glLinkProgram(_programId);
	//Check the program
	GLint status;
	glGetProgramiv(_programId, GL_LINK_STATUS, &status);
	if (status == GL_FALSE)
	{
		GLint infoLogLength;
		glGetProgramiv(_programId, GL_INFO_LOG_LENGTH, &infoLogLength);

		GLchar *strInfoLog = new GLchar[infoLogLength + 1];
		glGetProgramInfoLog(_programId, infoLogLength, NULL, strInfoLog);
		cerr << "Shader linking failed: " << strInfoLog << std::endl;
		delete[] strInfoLog;

		return false;
	}
	_linked = status == GL_TRUE;
	return _linked;
}

void ShaderProgram::DeleteProgram()
{
	if (!_linked)return;
	_linked = false;
	glDeleteProgram(_programId);
}

void ShaderProgram::UseProgram()
{
	if (_linked)glUseProgram(_programId);
}

GLuint ShaderProgram::GetProgramID()
{
	return _programId;

}

bool LinkShaderPrograms()
{
	shVertex.LoadShader("shaders/VertexShader.glsl", GL_VERTEX_SHADER);
	shFragment.LoadShader("shaders/FragmentShader.glsl", GL_FRAGMENT_SHADER);

	shpMain.CreateProgram();
	shpMain.AddShaderToProgram(&shVertex);
	shpMain.AddShaderToProgram(&shFragment);

	shpMain.LinkProgram();
	
	if (!shpMain.LinkProgram())
	{
		return false;
	}
	return true;
}

void DeleteShaders()
{
	shpMain.DeleteProgram();

    shVertex.DeleteShader();
	shFragment.DeleteShader();
}