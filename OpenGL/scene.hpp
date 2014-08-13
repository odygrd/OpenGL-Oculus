#ifndef SCENE_HPP
#define SCENE_HPP

#include "glm\glm.hpp"
#include <glm\gtc\matrix_transform.hpp> 

#include "vbo.hpp"
#include "shaders.hpp"
#include "texture.hpp"
#include "skybox.hpp"


class Scene {
public:
	Scene();
	virtual ~Scene();

	void Init(void);
	void Render(glm::mat4 &projection_matrix, glm::mat4 &modelview_matrix);
private:
	GLuint _vertexArray;
	VBO* _vboSceneObjects;

	Texture _tGold;
	Texture _tSnow;

	Skybox* _skybox;
};

#endif