#include "skybox.hpp"

using namespace std;

Skybox::Skybox()
{
	_vboRenderData = new VBO();
}

Skybox::~Skybox()
{
	for (int i = 0; i < 6; i++)
	{
		_sTextures[i].ReleaseTexture();
	}
	glDeleteVertexArrays(1, &_uiVAO);
	delete _vboRenderData;
}

void Skybox::LoadSkybox(string sDirectory, string sFront, string sBack, string sLeft, string sRight, string sTop, string sBottom)
{
	_sTextures[0].LoadTexture2D(sDirectory + sFront);
	_sTextures[1].LoadTexture2D(sDirectory + sBack);
	_sTextures[2].LoadTexture2D(sDirectory + sLeft);
	_sTextures[3].LoadTexture2D(sDirectory + sRight);
	_sTextures[4].LoadTexture2D(sDirectory + sTop);
	_sTextures[5].LoadTexture2D(sDirectory + sBottom);

	sDirectory = _directory;

	_front = sFront;
	_back = sBack;
	_left = sLeft;
	_right = sRight;
	_top = sTop;
	_bottom = sBottom;

	for (int i = 0; i < 6; i++)
	{
		_sTextures[i].SetFiltering(TEXTURE_FILTER_MAG_BILINEAR, TEXTURE_FILTER_MIN_BILINEAR);
		_sTextures[i].SetSamplerParameter(GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
		_sTextures[i].SetSamplerParameter(GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	}

	glGenVertexArrays(1, &_uiVAO);
	glBindVertexArray(_uiVAO);

	_vboRenderData->CreateVBO();
	_vboRenderData->BindVBO();

	glm::vec3 vSkyBoxVertices[24] =
	{
		// Front face
		glm::vec3(50.0f, 50.0f, 50.0f), glm::vec3(50.0f, -50.0f, 50.0f), glm::vec3(-50.0f, 50.0f, 50.0f), glm::vec3(-50.0f, -50.0f, 50.0f),
		// Back face
		glm::vec3(-50.0f, 50.0f, -50.0f), glm::vec3(-50.0f, -50.0f, -50.0f), glm::vec3(50.0f, 50.0f, -50.0f), glm::vec3(50.0f, -50.0f, -50.0f),
		// Left face
		glm::vec3(-50.0f, 50.0f, 50.0f), glm::vec3(-50.0f, -50.0f, 50.0f), glm::vec3(-50.0f, 50.0f, -50.0f), glm::vec3(-50.0f, -50.0f, -50.0f),
		// Right face
		glm::vec3(50.0f, 50.0f, -50.0f), glm::vec3(50.0f, -50.0f, -50.0f), glm::vec3(50.0f, 50.0f, 50.0f), glm::vec3(50.0f, -50.0f, 50.0f),
		// Top face
		glm::vec3(-50.0f, 50.0f, -50.0f), glm::vec3(50.0f, 50.0f, -50.0f), glm::vec3(-50.0f, 50.0f, 50.0f), glm::vec3(50.0f, 50.0f, 50.0f),
		// Bottom face
		glm::vec3(50.0f, -50.0f, -50.0f), glm::vec3(-50.0f, -50.0f, -50.0f), glm::vec3(50.0f, -50.0f, 50.0f), glm::vec3(-50.0f, -50.0f, 50.0f),
	};
	glm::vec2 vSkyBoxTexCoords[4] =
	{
		glm::vec2(0.0f, 1.0f), glm::vec2(0.0f, 0.0f), glm::vec2(1.0f, 1.0f), glm::vec2(1.0f, 0.0f)
	};

	glm::vec3 vSkyBoxNormals[6] =
	{
		glm::vec3(0.0f, 0.0f, -1.0f),
		glm::vec3(0.0f, 0.0f, 1.0f),
		glm::vec3(1.0f, 0.0f, 0.0f),
		glm::vec3(-1.0f, 0.0f, 0.0f),
		glm::vec3(0.0f, -1.0f, 0.0f),
		glm::vec3(0.0f, 1.0f, 0.0f)
	};

	for (int i = 0; i < 24; i++)
	{
		_vboRenderData->AddData(&vSkyBoxVertices[i], sizeof(glm::vec3));
		_vboRenderData->AddData(&vSkyBoxTexCoords[i % 4], sizeof(glm::vec2));
		_vboRenderData->AddData(&vSkyBoxNormals[i / 4], sizeof(glm::vec3));
	}

	_vboRenderData->UploadDataToGPU(GL_STATIC_DRAW);

	// Vertex positions
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 2 * sizeof(glm::vec3) + sizeof(glm::vec2), 0);
	// Texture coordinates
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(glm::vec3) + sizeof(glm::vec2), (void*)sizeof(glm::vec3));
	// Normal vectors
	glEnableVertexAttribArray(2);
	glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, 2 * sizeof(glm::vec3) + sizeof(glm::vec2), (void*)(sizeof(glm::vec3) + sizeof(glm::vec2)));
}

void Skybox::RenderSkybox(glm::mat4 &projection_matrix, glm::mat4 &modelview_matrix, GLuint ShaderID)
{
	glDepthMask(0);
	glBindVertexArray(_uiVAO);
	GLuint MVPMatrixLocation = glGetUniformLocation(ShaderID, "MVPMatrix");
	glm::mat4 MVPMatrix = projection_matrix * modelview_matrix;
	glUniformMatrix4fv(MVPMatrixLocation, 1, GL_FALSE, glm::value_ptr(MVPMatrix));
	for (int i = 0; i < 6; i++)
	{
		_sTextures[i].BindTexture();
		glDrawArrays(GL_TRIANGLE_STRIP, i * 4, 4);
	}
	glDepthMask(1);
}
