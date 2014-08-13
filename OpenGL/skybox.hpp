#ifndef SKYBOX_HPP
#define SKYBOX_HPP

#include "glm\glm.hpp"
#include <glm\gtc\type_ptr.hpp> 

#include <string>

#include "texture.hpp"
#include "vbo.hpp"
#include "shaders.hpp"

//Class for using skybox.
class Skybox
{
public:
	Skybox();
	virtual ~Skybox();

	void LoadSkybox(std::string sDirectory, std::string sFront, std::string sBack, std::string sLeft, std::string sRight, std::string sTop, std::string sBottom); //Loads skybox and creates VAO and VBO for it.
	void RenderSkybox(glm::mat4 &projection_matrix, glm::mat4 &modelview_matrix, GLuint ShaderID);

private:
	GLuint _uiVAO;
	VBO* _vboRenderData;
	Texture _sTextures[6];
	std::string _directory;
	std::string _front, _back, _left, _right, _top, _bottom;
};

#endif