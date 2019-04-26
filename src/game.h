#pragma once

#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <vector>

#include "GameLevel.h"
#include "BallObject.h"
#include "PowerUp.h"

enum GameState {
    GAME_ACTIVE,
    GAME_MENU,
    GAME_WIN,
};

enum Direction {
    UP,
    RIGHT,
    DOWN,
    LEFT,
};

typedef std::tuple<GLboolean, Direction, glm::vec2> Collision;

// initial size of the player paddle
const glm::vec2 PLAYER_SIZE(100, 20);
// Initial velocity of the player paddle
const GLfloat PLAYER_SPEED(600.0f);

// initial velocity of the ball
const glm::vec2 INITIAL_BALL_VELOCITY(100.0f, -350.0f);
// Radius of the ball object
const GLfloat BALL_RADIUS = 12.5f;

bool CheckCollision(GameObject & one, GameObject & two);
Collision CheckCollision(BallObject & one, GameObject & two);
Direction VectorDirection(glm::vec2 target);

class Game
{
public:
    GameState State;
    GLboolean Keys[1024];
    GLuint width, height;

    std::vector<GameLevel> levels;

    GameObject * Player;
    SpriteRenderer * Renderer;
    //BallObject * Ball;
    std::vector<BallObject> balls;
    std::vector<PowerUp> powerUps;

    unsigned int level;

    Game(GLuint w, GLuint h);
    ~Game();

    void init();

    void processInput(GLfloat dt);
    void update(float dt);
    void render();
    void doCollisions();
    void spawnPowerUp(GameObject & block);
    void activatePowerUp(PowerUp & powerUp);

    void resetPlayer();
    void resetLevel();
};
