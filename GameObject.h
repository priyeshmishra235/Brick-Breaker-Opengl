#ifndef GAMEOBJECT_H
#define GAMEOBJECT_H

#include "SpriteRenderer.h"
#include "Texture2D.h"
#include <glm/glm.hpp>

enum ObjectType {
  BLOCK_SOLID,
  BLOCK_TRIANGLE_NW,
  BLOCK_TRIANGLE_NE,
  ITEM_ADD_BALL
};

class GameObject {
public:
  glm::vec2 Position, Size, Velocity;
  glm::vec3 Color;
  float Rotation;
  bool IsSolid;
  bool Destroyed;
  int HitPoints;
  ObjectType Type;
  Texture2D Sprite;

  GameObject()
      : Position(0.0f), Size(1.0f), Velocity(0.0f), Color(1.0f), Rotation(0.0f),
        IsSolid(false), Destroyed(false), HitPoints(1), Type(BLOCK_SOLID) {}

  GameObject(glm::vec2 pos, glm::vec2 size, Texture2D sprite,
             glm::vec3 color = glm::vec3(1.0f),
             glm::vec2 velocity = glm::vec2(0.0f, 0.0f))
      : Position(pos), Size(size), Velocity(velocity), Color(color),
        Rotation(0.0f), Sprite(sprite), IsSolid(true), Destroyed(false),
        HitPoints(50), Type(BLOCK_SOLID) {}

  virtual void Draw(SpriteRenderer &renderer) {
    renderer.DrawSprite(this->Sprite, this->Position, this->Size,
                        this->Rotation, this->Color);
  }
};

#endif
