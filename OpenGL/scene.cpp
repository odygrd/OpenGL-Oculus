#include "scene.hpp"
#include "static_geometry.hpp"
#include "model.hpp"

extern ShaderProgram shpMain;

AssimpModel amModels[3];

Scene::Scene()
{
	_skybox = new Skybox();
	_vboSceneObjects = new VBO();
	//	_tGold = new Texture();
	//	_tSnow = new Texture();
}

Scene::~Scene()
{
	DeleteShaders();
	delete _vboSceneObjects;
	glDeleteVertexArrays(1, &_vertexArray);
	_tGold.ReleaseTexture();
	_tSnow.ReleaseTexture();

	delete _skybox;
}

void Scene::Init(void)
{
	//create a VAO

	glGenVertexArrays(1, &_vertexArray);
	glBindVertexArray(_vertexArray);

	_vboSceneObjects->CreateVBO();
	_vboSceneObjects->BindVBO();

	// Add cube to VBO

	for (int i = 0; i < 36; i++)
	{
		_vboSceneObjects->AddData(&vCubeVertices[i], sizeof(glm::vec3));
		_vboSceneObjects->AddData(&vCubeTexCoords[i % 6], sizeof(glm::vec2));
	}

	// Add pyramid to VBO

	for (int i = 0; i < 12; i++)
	{
		_vboSceneObjects->AddData(&vPyramidVertices[i], sizeof(glm::vec3));
		_vboSceneObjects->AddData(&vPyramidTexCoords[i % 3], sizeof(glm::vec2));
	}

	// Add ground to VBO

	for (int i = 0; i < 6; i++)
	{
		_vboSceneObjects->AddData(&vGround[i], sizeof(glm::vec3));
		vCubeTexCoords[i] *= 5.0f;
		_vboSceneObjects->AddData(&vCubeTexCoords[i % 6], sizeof(glm::vec2));
	}

	_vboSceneObjects->UploadDataToGPU(GL_STATIC_DRAW);

	// Vertex positions start on zero index, and distance between two consecutive is sizeof whole vertex data (position and tex. coord)
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(glm::vec3) + sizeof(glm::vec2), 0);
	// Texture coordinates start right after positon, thus on (sizeof(glm::vec3)) index,
	// and distance between two consecutive is sizeof whole vertex data (position and tex. coord)
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, sizeof(glm::vec3) + sizeof(glm::vec2), (void*)sizeof(glm::vec3));

	// Load shaders and create shader program

	LinkShaderPrograms();
	shpMain.UseProgram();

	glClearColor(0.2f, 0.3f, 0.4f, 1.0f);
	glClearDepth(1.0f);

	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LEQUAL);

	// Load  textures
	_tGold.LoadTexture2D("data/textures/golddiag.jpg", true);
	_tGold.SetFiltering(TEXTURE_FILTER_MAG_BILINEAR, TEXTURE_FILTER_MIN_BILINEAR_MIPMAP);

	_tSnow.LoadTexture2D("data/textures/snow.jpg", true);
	_tSnow.SetFiltering(TEXTURE_FILTER_MAG_BILINEAR, TEXTURE_FILTER_MIN_BILINEAR_MIPMAP);
	glEnable(GL_TEXTURE_2D);

	_skybox->LoadSkybox("data/skyboxes/jajlands1/", "jajlands1_ft.jpg", "jajlands1_bk.jpg", "jajlands1_lf.jpg", "jajlands1_rt.jpg", "jajlands1_up.jpg", "jajlands1_dn.jpg");

	amModels[0].LoadModelFromFile("data/models/Wolf/Wolf.obj");
	amModels[1].LoadModelFromFile("data/models/house/house.3ds");

	AssimpModel::FinalizeVBO();
}

void Scene::Render(glm::mat4 &projection_matrix, glm::mat4 &modelview_matrix)
{
	GLuint MVPMatrixLocation = glGetUniformLocation(shpMain.GetProgramID(), "MVPMatrix");

	_skybox->RenderSkybox(projection_matrix, modelview_matrix, shpMain.GetProgramID());

	glBindVertexArray(_vertexArray);
	int iSamplerLoc = glGetUniformLocation(shpMain.GetProgramID(), "gSampler");
	glUniform1i(iSamplerLoc, 0);

	_tGold.BindTexture(0);

	////// Rendering of cube
	glm::mat4 mCurrent;
	mCurrent = glm::translate(modelview_matrix, glm::vec3(0.0f, 0.0f, 0.0f));
	////mCurrent = glm::scale(mCurrent, glm::vec3(5.0f, 5.0f, 5.0f));
	//mCurrent = glm::rotate(mCurrent, 60.0f, glm::vec3(1.0f, 0.0f, 0.0f));
	glm::mat4 MVPMatrix = projection_matrix * mCurrent;
	//glUniformMatrix4fv(MVPMatrixLocation, 1, GL_FALSE, glm::value_ptr(MVPMatrix));

	//glDrawArrays(GL_TRIANGLES, 0, 36);

	//// Rendering of pyramid

	//mCurrent = glm::translate(modelview_matrix, glm::vec3(2.0f, 0.0f, 0.0f));
	//MVPMatrix = projection_matrix * mCurrent;
	//glUniformMatrix4fv(MVPMatrixLocation, 1, GL_FALSE, glm::value_ptr(MVPMatrix));

	//glDrawArrays(GL_TRIANGLES, 36, 12);

	// Render ground
	_tSnow.BindTexture(0);

	MVPMatrix = projection_matrix * mCurrent;
	glUniformMatrix4fv(MVPMatrixLocation, 1, GL_FALSE, glm::value_ptr(MVPMatrix));
	glDrawArrays(GL_TRIANGLES, 48, 6);

	// Render some houses

	AssimpModel::BindModelsVAO();

	mCurrent = glm::translate(modelview_matrix, glm::vec3(0.0f, -10.0f, -25.0f));
	//mCurrent = glm::scale(mCurrent, glm::vec3(5.0f, 5.0f, 5.0f));
	//mCurrent = glm::rotate(mCurrent, 60.0f, glm::vec3(1.0f, 0.0f, 0.0f));
	MVPMatrix = projection_matrix * mCurrent;
	glUniformMatrix4fv(MVPMatrixLocation, 1, GL_FALSE, glm::value_ptr(MVPMatrix));
	amModels[1].RenderModel();

	mCurrent = glm::translate(modelview_matrix, glm::vec3(-12.0f, -10.0f, -15.0f));
	//mCurrent = glm::scale(mCurrent, glm::vec3(5.0f, 5.0f, 5.0f));
	mCurrent = glm::rotate(mCurrent, 60.0f, glm::vec3(0.0f, 1.0f, 0.0f));
	MVPMatrix = projection_matrix * mCurrent;
	glUniformMatrix4fv(MVPMatrixLocation, 1, GL_FALSE, glm::value_ptr(MVPMatrix));
	amModels[1].RenderModel();

	mCurrent = glm::translate(modelview_matrix, glm::vec3(10.0f, -10.0f, -10.0f));
	//mCurrent = glm::scale(mCurrent, glm::vec3(5.0f, 5.0f, 5.0f));
	mCurrent = glm::rotate(mCurrent, -80.0f, glm::vec3(0.0f, 1.0f, 0.0f));
	MVPMatrix = projection_matrix * mCurrent;
	glUniformMatrix4fv(MVPMatrixLocation, 1, GL_FALSE, glm::value_ptr(MVPMatrix));
	amModels[1].RenderModel();

	// ... and also some wolfs
	AssimpModel::BindModelsVAO();

	mCurrent = glm::translate(modelview_matrix, glm::vec3(10.0f, -10.0f, -20.0f));
	//mCurrent = glm::scale(mCurrent, glm::vec3(5.0f, 5.0f, 5.0f));
	mCurrent = glm::rotate(mCurrent, -60.0f, glm::vec3(0.0f, 1.0f, 0.0f));
	MVPMatrix = projection_matrix * mCurrent;
	glUniformMatrix4fv(MVPMatrixLocation, 1, GL_FALSE, glm::value_ptr(MVPMatrix));
	amModels[0].RenderModel();

}
