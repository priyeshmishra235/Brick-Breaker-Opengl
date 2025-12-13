#include "Game.h"
#include "ResourceManager.h"
#include "SpriteRenderer.h"
#include "TextRenderer.h"
#include <algorithm>
#include <iostream>
#include <random>

float randFloat(float a, float b) {
  return a +
         (b - a) * (static_cast<float>(rand()) / static_cast<float>(RAND_MAX));
}
SpriteRenderer *Renderer;
TextRenderer *Text;

const extern unsigned int SCREEN_WIDTH;
const extern unsigned int SCREEN_HEIGHT;

Game::Game(unsigned int width, unsigned int height)
    : State(GAME_AIMING), Width(width), Height(height), Level(1), TotalBalls(1),
      BallSpeed(600.0f), LauncherPos(width / 2.0f, height - 50.0f) {}

void Game::Init() {

  ResourceManager::LoadShader("shaders/sprite.vs", "shaders/sprite.fs",
                              "sprite");

  glm::mat4 projection = glm::ortho(0.0f, (float)SCREEN_WIDTH,
                                    (float)SCREEN_HEIGHT, 0.0f, -1.0f, 1.0f);

  Shader spriteShader = ResourceManager::GetShader("sprite");
  spriteShader.Use().SetInteger("image", 0);
  spriteShader.SetMatrix4("projection", projection);

  ResourceManager::LoadTexture("assets/tile.png", true, "tileBlock");
  ResourceManager::LoadTexture("assets/ball.png", true, "ball");

  Renderer = new SpriteRenderer(spriteShader);

  Text = new TextRenderer(this->Width, this->Height);
  Text->Load("assets/font.ttf", 24);

  Balls.push_back(BallObject(LauncherPos, 10.0f, glm::vec2(0, 0),
                             ResourceManager::GetTexture("ball")));

  SpawnRow();
}

void Game::SpawnRow() {
  int hitPoints = this->Level;
  for (auto &box : Bricks) {
    box.Position.y += 60.0f;
    if (box.Position.y > Height - 100) {
      std::cout << "GAME OVER" << std::endl;
    }
  }

  std::random_device rd;
  std::mt19937 gen(rd());
  std::uniform_int_distribution<> distr(0, 5);

  for (int i = 0; i < 6; i++) {
    if (distr(gen) > 2) {
      glm::vec2 pos(i * 75.0f + 10.0f, 60.0f);
      glm::vec2 size(70.0f, 50.0f);

      GameObject obj(pos, size, ResourceManager::GetTexture("tileBlock"));

      if (this->Level % 5 == 0) {
        hitPoints = randFloat(this->Level, this->Level + 30);
      }
      obj.HitPoints = randFloat(this->Level, hitPoints + 10);

      float r = randFloat(0.1f, 1.0f);
      float g = randFloat(0.1f, 1.0f);
      float b = randFloat(0.1f, 1.0f);
      obj.Color = glm::vec3(r, g, b);

      Bricks.push_back(obj);
    }
  }
  Level++;
}

float fireTimer = 0.0f;
int ballsFired = 0;

void Game::Update(float dt) {

  if (State == GAME_SHOOTING) {
    fireTimer += dt;

    if (fireTimer >= 0.1f && ballsFired < static_cast<int>(Balls.size())) {
      Balls[ballsFired].Stuck = false;
      Balls[ballsFired].Velocity = AimDir * BallSpeed;
      ballsFired++;
      fireTimer = 0.0f;
    }

    if (ballsFired == static_cast<int>(Balls.size())) {
      State = GAME_RESOLVING;
    }
  }

  int ballsResting = 0;

  for (auto &ball : Balls) {
    if (ball.Stuck) {
      ballsResting++;
      continue;
    }

    ball.Move(dt, Width);

    if (ball.Position.y >= Height) {
      ball.Stuck = true;
      ball.Velocity = glm::vec2(0.0f);

      if (!FirstBallReturned) {
        NextLauncherPos = glm::vec2(ball.Position.x, Height - 50.0f);
        FirstBallReturned = true;
      }
      ball.Position = NextLauncherPos;
    }

    for (auto &brick : Bricks) {
      if (!brick.Destroyed) {

        glm::vec2 center(ball.Position + ball.Radius);
        glm::vec2 half(brick.Size.x / 2.0f, brick.Size.y / 2.0f);
        glm::vec2 aabb_center(brick.Position.x + half.x,
                              brick.Position.y + half.y);
        glm::vec2 diff = center - aabb_center;
        glm::vec2 clamped = glm::clamp(diff, -half, half);
        glm::vec2 closest = aabb_center + clamped;
        diff = closest - center;

        if (glm::length(diff) < ball.Radius) {

          brick.HitPoints--;
          if (brick.HitPoints <= 0)
            brick.Destroyed = true;

          glm::vec2 compass[] = {glm::vec2(0.0f, 1.0f), glm::vec2(1.0f, 0.0f),
                                 glm::vec2(0.0f, -1.0f),
                                 glm::vec2(-1.0f, 0.0f)};
          float max = 0.0f;

          int best_match = -1;
          for (int i = 0; i < 4; i++) {
            float dot = glm::dot(glm::normalize(diff), compass[i]);
            if (dot > max) {
              max = dot;
              best_match = i;
            }
          }

          if (best_match != -1) {
            if (best_match == 0 || best_match == 2)
              ball.Velocity.y = -ball.Velocity.y;
            else
              ball.Velocity.x = -ball.Velocity.x;
          }
        }
      }
    }
  }

  Bricks.erase(std::remove_if(Bricks.begin(), Bricks.end(),
                              [](const GameObject &o) { return o.Destroyed; }),
               Bricks.end());

  if (State == GAME_RESOLVING &&
      ballsResting == static_cast<int>(Balls.size())) {
    NextTurn();
  }
}

void Game::NextTurn() {
  LauncherPos = NextLauncherPos;

  for (auto &ball : Balls) {
    ball.Position = LauncherPos;
    ball.Stuck = true;
  }

  Balls.push_back(BallObject(LauncherPos, 10.0f, glm::vec2(0, 0),
                             ResourceManager::GetTexture("ball")));
  TotalBalls = Balls.size();

  ballsFired = 0;
  FirstBallReturned = false;
  SpawnRow();
  State = GAME_AIMING;
}

void Game::ProcessInput(float dt) {
  if (State == GAME_AIMING && IsMouseDown) {
  }
}

void Game::OnMouseDown(double x, double y) {
  if (State == GAME_AIMING)
    IsMouseDown = true;
}

void Game::OnMouseUp(double x, double y) {
  if (State == GAME_AIMING && IsMouseDown) {
    IsMouseDown = false;

    glm::vec2 mousePos(x, y);
    glm::vec2 dir = glm::normalize(mousePos - LauncherPos);

    if (dir.y > -0.1f)
      dir.y = -0.1f;

    AimDir = dir;
    State = GAME_SHOOTING;
  }
}

void Game::Render() {

  for (GameObject &brick : Bricks) {
    brick.Draw(*Renderer);
  }

  for (BallObject &ball : Balls) {
    ball.Draw(*Renderer);
  }

  for (GameObject &brick : Bricks) {
    if (!brick.Destroyed) {
      std::string hp = std::to_string(brick.HitPoints);

      float xOffset = (hp.length() == 1) ? 30.0f : 22.0f;

      Text->RenderText(hp, brick.Position.x + xOffset - 3,
                       brick.Position.y + 35.0f, 1.0f, glm::vec3(1.0f));
    }
  }

  Text->RenderText("STAGE 75", 195.0f, 20.0f, 1.0f,
                   glm::vec3(1.0f, 1.0f, 1.0f));

  Text->RenderText("Level: " + std::to_string(Level), 10.0f, 20.0f, 0.7f,
                   glm::vec3(1.0f));
}
Game::~Game() {
  delete Renderer;
  delete Text;
}
