#pragma once
#include <vector>

#include <QString>
#include <QOpenGLBuffer>
#include <QVector3D>
#include <QMatrix4x4>
#include <QOpenGLTexture>

struct aiScene;
struct aiNode;

struct Mesh {
	// Stores vertex attribute data
	QOpenGLBuffer m_vertexBuffer;
	size_t m_positionOffset;
	size_t m_normalOffset;
	size_t m_uvOffset;
	size_t m_colorOffset;

	// Allows for different sized positions, colors, normals, and texture coords
	int m_numPositionComponents;
	int m_numNormalComponents;
	int m_numUVComponents;
	int m_numColorComponents;

	// Stores the indices in order to generate faces
	QOpenGLBuffer m_indexBuffer;
	int m_indexCount;

	// Define material properties
	GLfloat m_ambient[4];
	GLfloat m_specular[4];
	GLfloat m_diffuse[4];
	GLfloat m_shininess;

	// We only allow 1 texture per mesh for now. Usually there are not more.
	QOpenGLTexture* m_texture = nullptr;

	// Keep track of what features this mesh has
	bool m_hasNormals;
	bool m_hasUVCoordinates;
	bool m_hasColors;
	bool m_hasTexture;

	// Store the transformation matrix for this mesh
	QMatrix4x4 m_transform;

	// Define 2 points, the min and max of the axis aligned bounding box
	QVector3D m_AABBMin;
	QVector3D m_AABBMax;
};

struct Model {
	bool m_isValid = false;
	std::vector<Mesh> m_meshes;

	// Define 2 points, the min and max of the axis aligned bounding box
	QVector3D m_AABBMin;
	QVector3D m_AABBMax;

	void Finalize() {
		// Scale the AABB by the transformation matrix
		for (auto& it : m_meshes) {
			it.m_AABBMin = it.m_transform * it.m_AABBMin;
			it.m_AABBMax = it.m_transform * it.m_AABBMax;
		}

		// Compute the AABB for the entire model
		m_AABBMin = { FLT_MAX, FLT_MAX, FLT_MAX };
		m_AABBMax = { FLT_MIN, FLT_MIN, FLT_MIN };

		for (auto& it : m_meshes) {
			m_AABBMin[0] = std::min(m_AABBMin[0], it.m_AABBMin[0]);
			m_AABBMin[1] = std::min(m_AABBMin[1], it.m_AABBMin[1]);
			m_AABBMin[2] = std::min(m_AABBMin[2], it.m_AABBMin[2]);
			m_AABBMax[0] = std::max(m_AABBMax[0], it.m_AABBMax[0]);
			m_AABBMax[1] = std::max(m_AABBMax[1], it.m_AABBMax[1]);
			m_AABBMax[2] = std::max(m_AABBMax[2], it.m_AABBMax[2]);
		}

		// Set the model as valid if there is at least one mesh
		if (m_meshes.size() > 0) {
			m_isValid = true;
		}
	}
};

class ModelLoader
{
public:
	Model LoadModel(const QString& file);

private:
	Mesh ProcessMesh(aiScene const* pObject, uint meshIdx);
	void TraverseNodeTree(aiNode const* pNode, aiScene const* pObject, Model& model, QMatrix4x4 transform);
	Model ProcessModel(aiScene const* pScene);
};