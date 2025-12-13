#include <fstream>
#include <iostream>
#include <vector>

#include "ResourceManager.h"
#include "TextRenderer.h"

#include <glm/gtc/matrix_transform.hpp>

#define STB_TRUETYPE_IMPLEMENTATION
#include "stb_truetype.h"

TextRenderer::TextRenderer(unsigned int width, unsigned int height) {

  this->TextShader =
      ResourceManager::LoadShader("shaders/text.vs", "shaders/text.fs", "text");

  this->TextShader.Use();

  glm::mat4 projection = glm::ortho(0.0f, static_cast<float>(width),
                                    static_cast<float>(height), 0.0f);
  this->TextShader.SetMatrix4("projection", projection);

  this->TextShader.SetInteger("text", 0);

  glGenVertexArrays(1, &this->VAO);
  glGenBuffers(1, &this->VBO);

  glBindVertexArray(this->VAO);
  glBindBuffer(GL_ARRAY_BUFFER, this->VBO);
  glBufferData(GL_ARRAY_BUFFER, sizeof(float) * 6 * 4, NULL, GL_DYNAMIC_DRAW);

  glEnableVertexAttribArray(0);
  glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, 4 * sizeof(float), 0);

  glBindBuffer(GL_ARRAY_BUFFER, 0);
  glBindVertexArray(0);

  this->atlasWidth = 1024;
  this->atlasHeight = 1024;
  this->cdata = malloc(sizeof(stbtt_bakedchar) * 96);
}

void TextRenderer::Load(std::string font, unsigned int fontSize) {

  std::ifstream file(font, std::ios::binary | std::ios::ate);
  if (!file.is_open()) {
    std::cout << "ERROR::TEXT: Failed to open font file: " << font << std::endl;
    return;
  }
  std::streamsize size = file.tellg();
  file.seekg(0, std::ios::beg);
  std::vector<unsigned char> buffer(size);
  if (!file.read((char *)buffer.data(), size))
    return;

  unsigned char *temp_bitmap = new unsigned char[atlasWidth * atlasHeight];

  stbtt_BakeFontBitmap(buffer.data(), 0, static_cast<float>(fontSize),
                       temp_bitmap, atlasWidth, atlasHeight, 32, 96,
                       (stbtt_bakedchar *)cdata);

  glGenTextures(1, &TextureID);
  glBindTexture(GL_TEXTURE_2D, TextureID);

  glTexImage2D(GL_TEXTURE_2D, 0, GL_RED, atlasWidth, atlasHeight, 0, GL_RED,
               GL_UNSIGNED_BYTE, temp_bitmap);

  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

  delete[] temp_bitmap;
}

void TextRenderer::RenderText(std::string text, float x, float y, float scale,
                              glm::vec3 color) {
  this->TextShader.Use();
  this->TextShader.SetVector3f("textColor", color);

  glActiveTexture(GL_TEXTURE0);
  glBindTexture(GL_TEXTURE_2D, this->TextureID);
  glBindVertexArray(this->VAO);

  stbtt_bakedchar *charData = (stbtt_bakedchar *)cdata;

  std::string::const_iterator c;
  for (c = text.begin(); c != text.end(); c++) {
    if (*c < 32 || *c > 126)
      continue;

    stbtt_aligned_quad q;

    stbtt_GetBakedQuad(charData, atlasWidth, atlasHeight, *c - 32, &x, &y, &q,
                       1);

    float vertices[6][4] = {{q.x0, q.y0, q.s0, q.t0}, {q.x0, q.y1, q.s0, q.t1},
                            {q.x1, q.y1, q.s1, q.t1},

                            {q.x0, q.y0, q.s0, q.t0}, {q.x1, q.y1, q.s1, q.t1},
                            {q.x1, q.y0, q.s1, q.t0}};

    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(vertices), vertices);
    glBindBuffer(GL_ARRAY_BUFFER, 0);

    glDrawArrays(GL_TRIANGLES, 0, 6);
  }

  glBindVertexArray(0);
  glBindTexture(GL_TEXTURE_2D, 0);
}
