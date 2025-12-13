#ifndef TEXT_RENDERER_H
#define TEXT_RENDERER_H

#include "glad/glad.h"
#include <glm/glm.hpp>
#include <map>
#include <string>

#include "Shader.h"
#include "Texture2D.h"

class TextRenderer {
public:
  TextRenderer(unsigned int width, unsigned int height);

  void Load(std::string font, unsigned int fontSize);

  void RenderText(std::string text, float x, float y, float scale,
                  glm::vec3 color = glm::vec3(1.0f));

private:
  unsigned int VAO, VBO;
  unsigned int TextureID;
  Shader TextShader;

  void *cdata;
  unsigned int atlasWidth, atlasHeight;
};

#endif
