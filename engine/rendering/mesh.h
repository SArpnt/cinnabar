#pragma once

#include <ce_math.h>
#include <ce_rendering.h>
#include <string>

namespace ce {
	class Mesh {
	 private:
		unsigned m_posCount, m_uvCount, m_colCount, m_indexCount;
		GLuint m_VAO, m_VBO, m_EBO;

		void initVAO(glm::vec3* posArray, glm::vec2* uvArray, glm::vec4* colArray, GLuint* indexArray);

	 public:
		Mesh(const char* filename);
		Mesh(glm::vec3* posArray, const unsigned posCount,
			glm::vec2* uvArray = NULL, const unsigned uvCount = 0,
			glm::vec4* colArray = NULL, const unsigned colCount = 0,
			GLuint* indexArray = NULL, const unsigned indexCount = 0);
		~Mesh();

		void sendToShader(class Shader* shader, bool bind = true);

		unsigned GetIndexCount() { return m_indexCount; };
		void bind(), unbind();
	};
}
