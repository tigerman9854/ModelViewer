#include "ModelLoader.h"

#include <assimp/Importer.hpp>      // C++ importer interface
#include <assimp/scene.h>           // Output data structure
#include <assimp/postprocess.h>     // Post processing flags

#include <vector>
#include <QMatrix4x4>

Model ModelLoader::LoadModel(const QString& file)
{
	// Create an instance of the Importer class
	Assimp::Importer importer;
	
	// And have it read the given file with some example postprocessing
	// Usually - if speed is not the most important aspect for you - you'll
	// probably to request more postprocessing than we do in this example.
	const aiScene* pScene = importer.ReadFile(file.toStdString(), aiProcessPreset_TargetRealtime_Quality ^ aiProcess_GenSmoothNormals | aiProcess_GenNormals | aiProcess_GenBoundingBoxes);
	
	// If the import failed, report it
	if (!pScene) {
		return Model();
	}

	// We're done. Everything will be cleaned up by the importer destructor
	return ProcessModel(pScene);
}

Mesh ModelLoader::ProcessMesh(aiScene const* pScene, uint meshIdx)
{
	Mesh newMesh;

	// Generate and bind vertex/index buffers
	newMesh.m_vertexBuffer = QOpenGLBuffer(QOpenGLBuffer::VertexBuffer);
	if (!newMesh.m_vertexBuffer.create()) {
		return newMesh;
	}
	if (!newMesh.m_vertexBuffer.bind()) {
		return newMesh;
	}
	newMesh.m_indexBuffer = QOpenGLBuffer(QOpenGLBuffer::IndexBuffer);
	if (!newMesh.m_indexBuffer.create()) {
		return newMesh;
	}
	if (!newMesh.m_indexBuffer.bind()) {
		return newMesh;
	}

	// Get pointers to the important information from ASSIMP
	aiMesh const* pMesh = pScene->mMeshes[meshIdx];
	aiMaterial const* pMaterial = pScene->mMaterials[pMesh->mMaterialIndex];


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
	newMesh.m_numNormalComponents = 3;
	newMesh.m_numUVComponents = pMesh->mNumUVComponents[0];  // Various
	newMesh.m_numColorComponents = 4;

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


	// Load axis aligned bounding box (AABB)
	newMesh.m_AABBMin.setX(pMesh->mAABB.mMin.x);
	newMesh.m_AABBMin.setY(pMesh->mAABB.mMin.y);
	newMesh.m_AABBMin.setZ(pMesh->mAABB.mMin.z);
	newMesh.m_AABBMax.setX(pMesh->mAABB.mMax.x);
	newMesh.m_AABBMax.setY(pMesh->mAABB.mMax.y);
	newMesh.m_AABBMax.setZ(pMesh->mAABB.mMax.z);

	return newMesh;
}

void ModelLoader::TraverseNodeTree(aiNode const* pNode, aiScene const* pScene, Model& model, QMatrix4x4 transform)
{
	// Get the transformation matrix for this node
	const aiMatrix4x4 t = pNode->mTransformation;
	const QMatrix4x4 qTrans = QMatrix4x4(t.a1, t.a2, t.a3, t.a4, t.b1, t.b2, t.b3, t.b4, t.c1, t.c2, t.c3, t.c4, t.d1, t.d2, t.d3, t.d4);

	// Load all meshes in this node
	for (int i = 0; i < pNode->mNumMeshes; ++i) {
		uint meshIdx = pNode->mMeshes[i];
		Mesh newMesh = ProcessMesh(pScene, meshIdx);
		newMesh.m_transform = transform * qTrans;
		model.m_meshes.push_back(newMesh);
	}

	// Traverse all child nodes
	for (int i = 0; i < pNode->mNumChildren; ++i) {
		aiNode* child = pNode->mChildren[i];
		TraverseNodeTree(child, pScene, model, qTrans);
	}
}

Model ModelLoader::ProcessModel(aiScene const* pScene)
{
	Model ret;

	aiNode* root = pScene->mRootNode;
	TraverseNodeTree(root, pScene, ret, QMatrix4x4());

	ret.Finalize();
	return ret;
}
