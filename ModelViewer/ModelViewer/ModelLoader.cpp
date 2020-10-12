#include "ModelLoader.h"

#include <assimp/Importer.hpp>      // C++ importer interface
#include <assimp/scene.h>           // Output data structure
#include <assimp/postprocess.h>     // Post processing flags

Model ModelLoader::LoadModel(const QString& file)
{
	// Create an instance of the Importer class
	Assimp::Importer importer;
	
	// And have it read the given file with some example postprocessing
	// Usually - if speed is not the most important aspect for you - you'll
	// probably to request more postprocessing than we do in this example.
	const aiScene* pScene = importer.ReadFile(file.toStdString(), aiProcess_CalcTangentSpace |aiProcess_Triangulate |aiProcess_JoinIdenticalVertices |aiProcess_SortByPType);
	
	// If the import failed, report it
	if (!pScene) {
		return Model();
	}

	// We're done. Everything will be cleaned up by the importer destructor
	return ProcessModel(pScene);
}

Model ModelLoader::ProcessModel(aiScene const* pObject)
{
	Model ret;

	for (int i = 0; i < pObject->mNumMeshes; ++i) {
		Mesh newMesh;

		// Generate vertex and index buffers
		newMesh.m_vertexBuffer = QOpenGLBuffer(QOpenGLBuffer::VertexBuffer);
		if (!newMesh.m_vertexBuffer.create()) {
			return ret;
		}
		newMesh.m_indexBuffer = QOpenGLBuffer(QOpenGLBuffer::IndexBuffer);
		if (!newMesh.m_indexBuffer.create()) {
			return ret;
		}

		// Grab the important items from the scene
		aiMesh const* pMesh = pObject->mMeshes[i];
		aiMaterial const* pMaterial = pObject->mMaterials[pMesh->mMaterialIndex];

		auto AssignColor = [](GLfloat* vec, aiColor3D col) {
			vec[0] = col.r;
			vec[1] = col.g;
			vec[2] = col.b;
			vec[3] = 1.f; // No alpha
		};

		// Set up the material for this mesh
		aiColor3D color;
		float val;
		pMaterial->Get(AI_MATKEY_COLOR_AMBIENT, color);
		AssignColor(newMesh.m_ambient, color);
		pMaterial->Get(AI_MATKEY_COLOR_DIFFUSE, color);
		AssignColor(newMesh.m_diffuse, color);
		pMaterial->Get(AI_MATKEY_COLOR_SPECULAR, color);
		AssignColor(newMesh.m_specular, color);
		pMaterial->Get(AI_MATKEY_SHININESS, val);
		newMesh.m_shininess = val;

		// TODO: Handle textures

		// Determine if we have normals and/or texture coordinates
		newMesh.m_hasNormals = (pMesh->mNormals != NULL);
		newMesh.m_hasTextureCoords = (pMesh->mTextureCoords != NULL);

		// Load all vertices
		const int size = sizeof(GLfloat) * pMesh->mNumVertices * 4 * (newMesh.m_hasNormals ? 3 : 1) * (newMesh.m_hasTextureCoords ? 2 : 1);
		newMesh.m_vertexBuffer.allocate(size);
		for (int j = 0; j < pMesh->mNumVertices; ++j) {
			
		}


		// Load indices
		


		// Append this mesh to the list
		ret.m_meshes.push_back(newMesh);
	}

	ret.m_isValid = true;
	return ret;
}
