#pragma once

#include <algorithm>
#include <map>
#include <string>
#include <vector>

#include <ce_assets.h>
#include <ce_math.h>
#include <ce_rendering.h>

using namespace glm;

namespace ce {
	class Shader {
	 private:
		inline static int MIN_LOC = 0;

		GLuint m_program;
		std::vector<std::string> m_attributes, m_uniforms;

		void linkProgram(
			int vertexShader, int fragmentShader, int geometryShader);

		int registerAttribute(std::string name),
			registerUniform(std::string name);

		void
		setUniform(GLuint location, bool value),
			setUniform(GLuint location, int value),
			setUniform(GLuint location, float value),

			setUniform(GLuint location, vec2 value),
			setUniform(GLuint location, float x, float y),

			setUniform(GLuint location, vec3 value),
			setUniform(GLuint location, float x, float y, float z),

			setUniform(GLuint location, vec4 value),
			setUniform(GLuint location, float x, float y, float z, float w),

			setUniform(GLuint location, mat2 mat),
			setUniform(GLuint location, mat3 mat),
			setUniform(GLuint location, mat4 mat);

	 public:
		Shader(const char* name, std::map<std::string, std::string> options = {})
			: Shader(name, name, name, options){};
		Shader(const char* vertName, const char* fragName, std::map<std::string, std::string> options = {})
			: Shader(vertName, NULL, fragName, options){};
		Shader(const char* vertName, const char* geoName, const char* fragName, std::map<std::string, std::string> options = {});
		~Shader();

		void bind(), unbind();

		GLuint getShader(), getAttribLocation(const std::string name),
			getUniformLocation(const std::string name);

		void vertexAttribPointer(std::string attrib, GLint size, GLenum type,
			GLboolean normalized, GLsizei stride, const void* pointer);

		template <class U>
		void setUniform(const std::string name, U value) {
			bind();
			GLuint location = getUniformLocation(name);
			if (location < (GLuint)Shader::MIN_LOC)
				return;
			setUniform(location, value);
			unbind();
		};
		void
		setUniform(const std::string name, float x, float y),
			setUniform(const std::string name, float x, float y, float z),
			setUniform(const std::string name, float x, float y, float z, float w);
	};
}