#include "asset_manager.h"

//TEXT FILES
#include <fstream>
#include <sstream>

std::string ce::AssetManager::load_text_file(std::string path)
{
	std::fstream file;
	std::string text = "";
	file.exceptions(std::fstream::failbit || std::fstream::badbit);
	try
	{
		file.open(path);
		if (!file.good()) {
			logger->Warn("FILE_DOES_NOT_EXIST: " + path);
			return "";
		}
		std::stringstream filestream;
		filestream << file.rdbuf();
		file.close();
		text = filestream.str();
		logger->Log("LOADED_FILE: " + path);
	}
	catch (std::fstream::failure e)
	{
		logger->Error("FILE_NOT_SUCCESSFULLY_READ: " + std::string(e.what()));
	}
	return text;
}

ce::ShaderFile ce::AssetManager::getShaderFile(std::string filename)
{
	std::string path = SHADER_FOLDER + "/" + filename;
	ShaderFile shaderFile;
	shaderFile.name = filename;
	shaderFile.vertex = load_text_file(path + ".vert");
	if (shaderFile.vertex == "") shaderFile.vertex = load_text_file(path + ".vs");
	shaderFile.fragment = load_text_file(path + ".frag");
	if (shaderFile.fragment == "") shaderFile.fragment = load_text_file(path + ".fs");
	shaderFile.geometry = load_text_file(path + ".geom");
	if (shaderFile.geometry == "") shaderFile.geometry = load_text_file(path + ".gs");
	return shaderFile;
}
