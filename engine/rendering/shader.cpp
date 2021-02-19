#include "shader.h"

#include <core/tpnt_log.h>
#include <iostream>
#include <managers/asset_manager.h>
#include <map>

void checkCompileErrors(GLuint shader, GLint shaderType) {
	std::string type;
	switch (shaderType) {
		case GL_VERTEX_SHADER:
			type = "VERTEX";
			break;
		case GL_FRAGMENT_SHADER:
			type = "FRAGMENT";
			break;
		case GL_GEOMETRY_SHADER:
			type = "GEOMETRY";
			break;
		default:
			return;
	}
	int success;
	char infoLog[1024];
	glGetShaderiv(shader, GL_COMPILE_STATUS, &success);
	if (!success) {
		glGetShaderInfoLog(shader, 1024, NULL, infoLog);
		LOG_ERROR(
			"SHADER_COMPILATION_ERROR of type: " + type + "\n" + infoLog + "\n");
		exit(-1);
	}
}

void checkCompileErrors(GLuint program) {
	int success;
	char infoLog[1024];
	glGetProgramiv(program, GL_LINK_STATUS, &success);
	if (!success) {
		glGetProgramInfoLog(program, 1024, NULL, infoLog);
		LOG_ERROR("PROGRAM_LINKING_ERROR\n" + infoLog + "\n");
		exit(-1);
	}
}

int createShader(GLenum type, std::string source) {
	const char* shaderSourceStr = source.c_str();
	GLuint shader = glCreateShader(type);

	glShaderSource(shader, 1, &shaderSourceStr, NULL);
	glCompileShader(shader);

	checkCompileErrors(shader, type);
	return shader;
}

std::string setupShaderDefs(std::string source, std::map<std::string, std::string> options) {
	std::string shader = source;
	for (std::pair<std::string, std::string> option : options) {
		size_t defPos = shader.find("#define " + option.first);
		if (defPos == std::string::npos) {
			LOG_ERROR("Invalid Option: " + option.first);
			continue;
		}
		size_t defValuePos = defPos + option.first.length() + 9; // 8 is length of "#define " and space after name
		size_t lineEnd = shader.find('\n', defValuePos);
		if (lineEnd == std::string::npos)
			lineEnd = shader.length();

		shader = shader.replace(defValuePos, lineEnd - defValuePos, option.second);
	}
	return shader;
}

void ce::Shader::linkProgram(
	int vertexShader, int fragmentShader, int geometryShader) {
	if (vertexShader != 0)
		glAttachShader(m_program, vertexShader);
	if (fragmentShader != 0)
		glAttachShader(m_program, fragmentShader);
	if (geometryShader != 0)
		glAttachShader(m_program, geometryShader);
	glLinkProgram(m_program);
	checkCompileErrors(m_program);
}

int ce::Shader::registerAttribute(std::string name) {
	int location = glGetAttribLocation(m_program, name.c_str());
	if (location < Shader::MIN_LOC) {
		LOG_ERROR("Invalid Attribute: " + name);
		return MIN_LOC - 1;
	}
	m_attributes.insert(m_attributes.begin() + location, name);
	LOG_SUCCESS("Registered Attribute: " + name);
	return location;
}

int ce::Shader::registerUniform(std::string name) {
	int location = glGetUniformLocation(m_program, name.c_str());
	if (location < Shader::MIN_LOC) {
		LOG_ERROR("Invalid Uniform: " + name);
		return MIN_LOC - 1;
	}
	m_uniforms.insert(m_uniforms.begin() + location, name);
	LOG_SUCCESS("Registered Uniform: " + name);
	return location;
}

ce::Shader::Shader(const char* vertName, const char* geomName, const char* fragName, std::map<std::string, std::string> options)
	: m_program(glCreateProgram()) {
	ShaderFile shaderFile = ce::AssetManager::getShaderFiles(vertName, geomName, fragName);

	int vertexShader = 0;
	int fragmentShader = 0;
	int geometryShader = 0;

	if (shaderFile.vertex != "")
		vertexShader = createShader(GL_VERTEX_SHADER, setupShaderDefs(shaderFile.vertex, options));
	if (shaderFile.fragment != "")
		fragmentShader = createShader(GL_FRAGMENT_SHADER, setupShaderDefs(shaderFile.fragment, options));
	if (shaderFile.geometry != "")
		geometryShader = createShader(GL_GEOMETRY_SHADER, setupShaderDefs(shaderFile.geometry, options));
	linkProgram(vertexShader, fragmentShader, vertexShader);

	int attrCount = 0, uniformCount = 0;
	glGetProgramiv(m_program, GL_ACTIVE_ATTRIBUTES, &attrCount);
	glGetProgramiv(m_program, GL_ACTIVE_UNIFORMS, &uniformCount);

	m_attributes.resize(attrCount);
	m_uniforms.resize(uniformCount);
}

ce::Shader::~Shader() {
	glDeleteProgram(m_program);
}

void ce::Shader::bind() {
	glUseProgram(m_program);
}

void ce::Shader::unbind() {
	glUseProgram(0);
}

GLuint ce::Shader::getShader() {
	return m_program;
}

GLuint ce::Shader::getAttribLocation(const std::string name) {
	if (m_attributes.size() < (size_t)Shader::MIN_LOC)
		return registerAttribute(name.c_str());
	auto location = std::find(m_attributes.begin(), m_attributes.end(), name);
	if (location != m_attributes.end())
		return std::distance(m_attributes.begin(), location);
	else
		return registerAttribute(name.c_str());
}

GLuint ce::Shader::getUniformLocation(const std::string name) {
	if (m_attributes.size() < (size_t)Shader::MIN_LOC)
		return registerUniform(name.c_str());
	auto location = std::find(m_uniforms.begin(), m_uniforms.end(), name);
	if (location != m_uniforms.end())
		return std::distance(m_uniforms.begin(), location);
	else
		return registerUniform(name.c_str());
}

void ce::Shader::vertexAttribPointer(std::string attrib, GLint size,
	GLenum type, GLboolean normalized, GLsizei stride, const void* pointer) {
	GLuint location = getAttribLocation(attrib);
	if (location < (GLuint)Shader::MIN_LOC)
		return;
	glVertexAttribPointer(location, size, type, normalized, stride, pointer);
	glEnableVertexAttribArray(location);
}

void ce::Shader::setUniform(const std::string name, float x, float y) {
	bind();
	GLuint location = getUniformLocation(name);
	if (location < (GLuint)Shader::MIN_LOC)
		return;
	setUniform(location, x, y);
	unbind();
}
void ce::Shader::setUniform(const std::string name, float x, float y, float z) {
	bind();
	GLuint location = getUniformLocation(name);
	if (location < (GLuint)Shader::MIN_LOC)
		return;
	setUniform(location, x, y, z);
	unbind();
}
void ce::Shader::setUniform(const std::string name, float x, float y, float z, float w) {
	bind();
	GLuint location = getUniformLocation(name);
	if (location < (GLuint)Shader::MIN_LOC)
		return;
	setUniform(location, x, y, z, w);
	unbind();
}

void ce::Shader::setUniform(GLuint location, bool value) {
	glUniform1i(location, (int)value);
}
void ce::Shader::setUniform(GLuint location, int value) {
	glUniform1i(location, value);
}
void ce::Shader::setUniform(GLuint location, float value) {
	glUniform1f(location, value);
}

void ce::Shader::setUniform(GLuint location, vec2 value) {
	glUniform2fv(location, 1, &value[0]);
}
void ce::Shader::setUniform(GLuint location, float x, float y) {
	glUniform2f(location, x, y);
}

void ce::Shader::setUniform(GLuint location, vec3 value) {
	glUniform3fv(location, 1, &value[0]);
}
void ce::Shader::setUniform(GLuint location, float x, float y, float z) {
	glUniform3f(location, x, y, z);
}

void ce::Shader::setUniform(GLuint location, vec4 value) {
	glUniform4fv(location, 1, &value[0]);
}
void ce::Shader::setUniform(GLuint location, float x, float y, float z, float w) {
	glUniform4f(location, x, y, z, w);
}

void ce::Shader::setUniform(GLuint location, mat2 value) {
	glUniformMatrix2fv(location, 1, GL_FALSE, &value[0][0]);
}
void ce::Shader::setUniform(GLuint location, mat3 value) {
	glUniformMatrix3fv(location, 1, GL_FALSE, &value[0][0]);
}
void ce::Shader::setUniform(GLuint location, mat4 value) {
	glUniformMatrix4fv(location, 1, GL_FALSE, &value[0][0]);
}