#include "ResourceManager.h"

#include <fstream>
#include <iostream>
#include <sstream>

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

std::map<std::string, Texture2D> ResourceManager::Textures;
std::map<std::string, Shader> ResourceManager::Shaders;

Shader ResourceManager::LoadShader(const char *vShaderFile,
                                   const char *fShaderFile, std::string name) {
  std::string vertexCode;
  std::string fragmentCode;
  try {

    std::ifstream vertexShaderFile(vShaderFile);
    std::ifstream fragmentShaderFile(fShaderFile);
    std::stringstream vShaderStream, fShaderStream;

    vShaderStream << vertexShaderFile.rdbuf();
    fShaderStream << fragmentShaderFile.rdbuf();

    vertexShaderFile.close();
    fragmentShaderFile.close();

    vertexCode = vShaderStream.str();
    fragmentCode = fShaderStream.str();
  } catch (std::exception e) {
    std::cout << "ERROR::SHADER: Failed to read shader files" << std::endl;
  }
  const char *vShaderCode = vertexCode.c_str();
  const char *fShaderCode = fragmentCode.c_str();

  Shader shader;
  shader.Compile(vShaderCode, fShaderCode);
  Shaders[name] = shader;

  return shader;
}

Shader ResourceManager::GetShader(std::string name) { return Shaders[name]; }

Texture2D ResourceManager::LoadTexture(const char *file, bool alpha,
                                       std::string name) {

  Texture2D texture;

  if (alpha) {
    texture.Internal_Format = GL_RGBA;
    texture.Image_Format = GL_RGBA;
  }

  int width, height, nrChannels;
  unsigned char *data = stbi_load(file, &width, &height, &nrChannels, 0);

  if (data) {
    texture.Generate(width, height, data);
  } else {
    std::cout << "Failed to load texture: " << file << std::endl;
  }

  stbi_image_free(data);

  Textures[name] = texture;
  return texture;
}

Texture2D ResourceManager::GetTexture(std::string name) {
  return Textures[name];
}
