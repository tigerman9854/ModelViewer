#pragma once
#include <vector>

#include <QString>
#include <QOpenGLBuffer>

struct aiScene;

struct Mesh {
	// Stores vertex attribute data
	QOpenGLBuffer m_vertexBuffer;
	int m_positionOffset;
	int m_normalOffset;
	int m_uvOffset;
	int m_colorOffset;

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

	// Keep track of what features this mesh has
	bool m_hasNormals;
	bool m_hasUVCoordinates;
	bool m_hasColors;
};

struct Model {
	bool m_isValid = false;
	std::vector<Mesh> m_meshes;
};

class ModelLoader
{
public:
	Model LoadModel(const QString& file);

private:
	Model ProcessModel(aiScene const* mObject);
};