#include "mesh.h"

#include "shader.h"
#include <ce_math.h>
#include <managers/asset_manager.h>
#include <string>

ce::Mesh::Mesh(const char* filename) {
	MeshFile file = ce::AssetManager::getMeshFile(filename);
	Mesh(
		&file.vertPos[0], file.vertPos.size(),
		&file.vertUV[0], file.vertUV.size(),
		&file.vertCol[0], file.vertCol.size(),
		&file.indices[0], file.indices.size());
}

ce::Mesh::Mesh(glm::vec3* posArray, const unsigned posCount,
	glm::vec2* uvArray, const unsigned uvCount,
	glm::vec4* colArray, const unsigned colCount,
	GLuint* indexArray, const unsigned indexCount)
	: m_posCount(posCount),
	  m_uvCount(uvCount),
	  m_colCount(colCount),
	  m_indexCount(indexCount),
	  m_VAO(0), m_VBO(0), m_EBO(0) {
	glGenVertexArrays(1, &m_VAO);
	glBindVertexArray(m_VAO);
	initVAO(posArray, uvArray, colArray, indexArray);
	glBindVertexArray(0);
}

ce::Mesh::~Mesh() {
	glDeleteVertexArrays(1, &m_VAO);
	glDeleteBuffers(1, &m_VBO);
	glDeleteBuffers(1, &m_EBO);
}

void ce::Mesh::bind() {
	glBindVertexArray(m_VAO);
	glBindBuffer(GL_ARRAY_BUFFER, m_VBO);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_EBO);
}
void ce::Mesh::unbind() {
	glBindVertexArray(0);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
}

void ce::Mesh::sendToShader(ce::Shader* shader, bool bind) {
	if (bind) {
		glBindVertexArray(m_VAO);
		glBindBuffer(GL_ARRAY_BUFFER, m_VBO);
	}
	shader->vertexAttribPointer("aPos", 3, GL_FLOAT, GL_FALSE, sizeof(glm::vec3), 0);
	shader->vertexAttribPointer("aTexCoord", 2, GL_FLOAT, GL_FALSE, sizeof(glm::vec2), (void*)(long)m_posCount);
	shader->vertexAttribPointer("aColor", 4, GL_FLOAT, GL_FALSE, sizeof(glm::vec4), (void*)(long)(m_posCount + m_uvCount));
	if (bind) {
		glBindVertexArray(0);
		glBindBuffer(GL_ARRAY_BUFFER, 0);
	}
}

void ce::Mesh::initVAO(glm::vec3* posArray, glm::vec2* uvArray, glm::vec4* colArray, GLuint* indexArray) {
	glGenBuffers(1, &m_VBO);
	glBindBuffer(GL_ARRAY_BUFFER, m_VBO);
	glBufferData(GL_ARRAY_BUFFER, m_posCount * sizeof(glm::vec3), posArray, GL_STATIC_DRAW);
	glBufferSubData(GL_ARRAY_BUFFER, m_posCount * sizeof(glm::vec3), m_uvCount * sizeof(glm::vec2), posArray);
	glBufferSubData(GL_ARRAY_BUFFER, m_posCount * sizeof(glm::vec3) + m_uvCount * sizeof(glm::vec2), m_colCount * sizeof(glm::vec4), posArray);
	glBindBuffer(GL_ARRAY_BUFFER, 0);

	if (m_indexCount > 0) {
		glGenBuffers(1, &m_EBO);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_EBO);
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, m_indexCount * sizeof(GLuint), indexArray, GL_STATIC_DRAW);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
	}
}
