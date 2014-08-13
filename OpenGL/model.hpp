#ifndef MODEL_HPP
#define MODEL_HPP
 
#include <assimp/Importer.hpp>      // C++ importer interface
#include <assimp/scene.h>           // Output data structure
#include <assimp/postprocess.h>     // Post processing flags

#include "shaders.hpp"
#include "vbo.hpp"
#include "texture.hpp"

#include <vector>

class AssimpModel
{
public:
	bool LoadModelFromFile(char* sFilePath); //Loads model using Assimp library.

	static void FinalizeVBO(); //Uploads all loaded model data in one global models' VBO.
	static void BindModelsVAO(); //Binds VAO of models with their VBO.

	void RenderModel();
	AssimpModel();
private:
	bool bLoaded;
	static VBO vboModelData;
	static GLuint uiVAO;
	static std::vector<Texture> tTextures;
	std::vector<int> iMeshStartIndices;
	std::vector<int> iMeshSizes;
	std::vector<int> iMaterialIndices;
	int iNumMaterials;
};

#endif
