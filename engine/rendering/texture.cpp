#include "texture.h"

#include <managers/asset_manager.h>

ce::Texture::Texture(std::string filename, GLenum type):
	width(0),
	height(0),
	channelCount(0),
	type(type)
{
	TextureFile textureFile = ce::AssetManager::getTextureFile(filename);
	
	width = textureFile.width;
	height = textureFile.height;
	channelCount = textureFile.channelCount;
	
	glGenTextures(1, &texture);
	bind();
	glTexParameteri(type, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(type, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameteri(type, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(type, GL_TEXTURE_MIN_FILTER, GL_REPEAT);


	if (textureFile.data) {
		glTexImage2D(type, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, textureFile.data);
		glGenerateMipmap(type);
		GetLogger()->Log(textureFile.name + " has been loaded.");
	}
	else {
		GetLogger()->Error("TEXTURE_LOADING_FAILED: " + textureFile.name);
	}
	unbind();
	ce::AssetManager::freeTextureFile(textureFile);
}

ce::Texture::~Texture()
{;
	glDeleteTextures(1, &texture);
}

ce::Logger* ce::Texture::GetLogger()
{
	return new Logger("texture");
}

void ce::Texture::bind()
{
	glBindTexture(type, texture);
	glEnable(type);
}

void ce::Texture::activate(int slot=0)
{
	glActiveTexture(GL_TEXTURE0+slot);
	bind();
}

void ce::Texture::unbind() {
	glDisable(type);
	glActiveTexture(0);
	glBindTexture(this->type, 0);
}