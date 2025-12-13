#ifndef GAME_H
#define GAME_H

#include "BallObject.h"
#include "GameObject.h"
#include "glad/glad.h"
#include <GLFW/glfw3.h>
#include <vector>

enum GameState { GAME_ACTIVE, GAME_AIMING, GAME_SHOOTING, GAME_RESOLVING };

class Game {
public:
  GameState State;
  bool Keys[1024];
  unsigned int Width, Height;

  std::vector<GameObject> Bricks;
  std::vector<BallObject> Balls;
  unsigned int Level;
  unsigned int TotalBalls;
  float BallSpeed;
  glm::vec2 LauncherPos;
  glm::vec2 NextLauncherPos;
  bool FirstBallReturned;

  glm::vec2 AimDir;
  bool IsMouseDown;

  Game(unsigned int width, unsigned int height);
  ~Game();
  void Init();
  void ProcessInput(float dt);
  void Update(float dt);
  void Render();

  void OnMouseDown(double x, double y);
  void OnMouseUp(double x, double y);

  void SpawnRow();
  void FireBall();
  void NextTurn();
};

#endif
