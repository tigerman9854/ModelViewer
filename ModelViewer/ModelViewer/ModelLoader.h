#pragma once
#include <vector>

#include <QString>
#include <QOpenGLBuffer>

struct aiScene;

struct Mesh {
	// Seperate vertex and index buffers for drawing
	QOpenGLBuffer m_vertexBuffer;
	QOpenGLBuffer m_indexBuffer;

	// Define material properties
	GLfloat m_ambient[4];
	GLfloat m_specular[4];
	GLfloat m_diffuse[4];
	GLfloat m_shininess;

	bool m_hasNormals;
	bool m_hasTextureCoords;
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