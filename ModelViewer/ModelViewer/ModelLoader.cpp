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

		// Generate and bind vertex/index buffers
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

		// Get pointers to the important information from ASSIMP
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
		newMesh.m_hasUVCoordinates = pMesh->HasTextureCoords(0);
		newMesh.m_hasColors = pMesh->HasVertexColors(0);

		// Determine the size of a single attribute
		newMesh.m_numPositionComponents = 3;
		newMesh.m_numNormalComponents =  3;
		newMesh.m_numUVComponents = pMesh->mNumUVComponents[0];  // Various
		newMesh.m_numColorComponents = 4;    // 4 elements

		// Compute sizes for all attributes
		const int positionBufferSize = newMesh.m_numPositionComponents * pMesh->mNumVertices * sizeof(float);
		const int normalBufferSize = newMesh.m_numNormalComponents * pMesh->mNumVertices * sizeof(float);
		const int uvBufferSize = newMesh.m_numUVComponents * pMesh->mNumVertices * sizeof(float);
		const int colorBufferSize = newMesh.m_numColorComponents * pMesh->mNumVertices * sizeof(float);

		// Compute the offsets of features in the overall vertex buffer
		//   1. Positions (required)
		//   2. Normals (optional)
		//   3. UV Coordinates (optional)
		//   4. Colors (optional)
		int currentOffset = 0;
		if (true) {
			newMesh.m_positionOffset = currentOffset;
			currentOffset += positionBufferSize;
		}
		if (newMesh.m_hasNormals) {
			newMesh.m_normalOffset = currentOffset;
			currentOffset += normalBufferSize;
		}
		if (newMesh.m_hasUVCoordinates) {
			newMesh.m_uvOffset = currentOffset;
			currentOffset += uvBufferSize;
		}
		if (newMesh.m_hasColors) {
			newMesh.m_colorOffset = currentOffset;
			currentOffset += colorBufferSize;
		}
		const int vertexBufferTotalSize = currentOffset;

		// Allocate the vertex buffer all at once to save time
		newMesh.m_vertexBuffer.allocate(vertexBufferTotalSize);

		// Positions
		newMesh.m_vertexBuffer.write(newMesh.m_positionOffset, pMesh->mVertices, positionBufferSize);

		// Normals
		if (newMesh.m_hasNormals) {
			newMesh.m_vertexBuffer.write(newMesh.m_normalOffset, pMesh->mNormals, normalBufferSize);
		}

		// UV Coordinates
		if (newMesh.m_hasUVCoordinates) {
			newMesh.m_vertexBuffer.write(newMesh.m_uvOffset, pMesh->mTextureCoords[0], uvBufferSize);
		}

		// Colors
		if (newMesh.m_hasColors)
		{
			newMesh.m_vertexBuffer.write(newMesh.m_colorOffset, pMesh->mColors[0], colorBufferSize);
		}


		// Load indices
		newMesh.m_indexCount = pMesh->mNumFaces * 3;
		const int indexBufferSize = sizeof(int) * newMesh.m_indexCount;
		newMesh.m_indexBuffer.allocate(indexBufferSize);

		std::vector<int> indices;
		indices.reserve(newMesh.m_indexCount);

		// Loop through all faces
		// Potentially use OpenMP to multithread this if it is slow
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
