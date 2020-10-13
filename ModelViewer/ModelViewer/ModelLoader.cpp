#include "ModelLoader.h"

#include <assimp/Importer.hpp>      // C++ importer interface
#include <assimp/scene.h>           // Output data structure
#include <assimp/postprocess.h>     // Post processing flags

#include <vector>

Model ModelLoader::LoadModel(const QString& file)
{
	// Create an instance of the Importer class
	Assimp::Importer importer;
	
	// And have it read the given file with some example postprocessing
	// Usually - if speed is not the most important aspect for you - you'll
	// probably to request more postprocessing than we do in this example.
	const aiScene* pScene = importer.ReadFile(file.toStdString(), aiProcess_Triangulate | aiProcess_JoinIdenticalVertices | aiProcess_SortByPType);
	
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

		// Generate and bind  vertex/index buffers
		newMesh.m_vertexBuffer = QOpenGLBuffer(QOpenGLBuffer::VertexBuffer);
		if (!newMesh.m_vertexBuffer.create()) {
			return ret;
		}
		if (!newMesh.m_vertexBuffer.bind()) {
			return ret;
		}
		newMesh.m_indexBuffer = QOpenGLBuffer(QOpenGLBuffer::IndexBuffer);
		if (!newMesh.m_indexBuffer.create()) {
			return ret;
		}
		if (!newMesh.m_indexBuffer.bind()) {
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

		// Determine if we have colors, normals, or texture coordinates
		newMesh.m_hasNormals = (pMesh->mNormals != NULL);
		newMesh.m_hasTextureCoords = (pMesh->mTextureCoords != NULL);
		newMesh.m_hasColors = (pMesh->mColors != NULL);

		// Sizes
		const int vertexBufferSize = sizeof(float) * pMesh->mNumVertices * 3;
		const int colorBufferSize = sizeof(float) * pMesh->mNumVertices * 3;
		const int vertexBufferTotalSize = vertexBufferSize + colorBufferSize;

		// Offsets
		newMesh.m_vertexOffset = 0;
		newMesh.m_colorOffset = vertexBufferSize;

		// Positions
		newMesh.m_vertexBuffer.allocate(vertexBufferTotalSize);
		newMesh.m_vertexBuffer.write(0, pMesh->mVertices, vertexBufferSize);

		// Colors
		std::vector<float> colorBuffer;

		for (int j = 0; j < pMesh->mNumVertices; ++j) {
			// Color = grey
			colorBuffer.push_back(0.6f);
			colorBuffer.push_back(0.6f);
			colorBuffer.push_back(0.6f);
		}

		newMesh.m_vertexBuffer.write(newMesh.m_colorOffset, colorBuffer.data(), colorBufferSize);

		// Load all colors
		/*if (newMesh.m_hasColors)
		{
			const int colorBufferSize = sizeof(float) * pMesh->mNumVertices * 4;
			newMesh.m_colorBuffer.allocate(colorBufferSize);
			newMesh.m_colorBuffer.write(0, pMesh->mColors, colorBufferSize);
		}*/

		// Load indices
		newMesh.m_indexCount = pMesh->mNumFaces * 3;
		const int indexBufferSize = sizeof(int) * newMesh.m_indexCount;
		newMesh.m_indexBuffer.allocate(indexBufferSize);

		std::vector<int> indices;
		indices.reserve(newMesh.m_indexCount);

		// Loop through all faces
		// Potentially use OpenMP to multithread this if slow
		for (int j = 0; j < pMesh->mNumFaces; ++j) {
			aiFace const& face = pMesh->mFaces[j];
			indices.push_back(face.mIndices[0]);
			indices.push_back(face.mIndices[1]);
			indices.push_back(face.mIndices[2]);
		}

		newMesh.m_indexBuffer.write(0, indices.data(), indexBufferSize);


		// Append this mesh to the list
		ret.m_meshes.push_back(newMesh);
	}

	ret.m_isValid = true;
	return ret;
}
