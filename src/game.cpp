#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <algorithm>
#include "game.h"
#include "SpriteRenderer.h"


Game::Game(GLuint w, GLuint h)
        : State(GAME_ACTIVE), Keys(), width(w), height(h) {}

Game::~Game() {
    delete Renderer;
}

void Game::init() {
    // Load shaders
    ResourceManager::loadShader("res/shaders/shader.vert", "res/shaders/shader.frag", nullptr, "sprite");
    // Configure shaders
    glm::mat4 projection = glm::ortho(0.0f, static_cast<GLfloat>(this->width), static_cast<GLfloat>(this->height), 0.0f,
                                      -1.0f, 1.0f);

    ResourceManager::getShader("sprite").use();
    ResourceManager::getShader("sprite").setInt("image", 0);
    ResourceManager::getShader("sprite").setMatrix4("projection", projection);

    // Set render-specific controls
    Renderer = new SpriteRenderer(ResourceManager::getShader("sprite"));

    // load textures
    ResourceManager::loadTexture("res/textures/background.jpg", GL_FALSE, "background");
    ResourceManager::loadTexture("res/textures/block.png", GL_FALSE, "block");
    ResourceManager::loadTexture("res/textures/block_solid.png", GL_FALSE, "block_solid");
    ResourceManager::loadTexture("res/textures/paddle.png", GL_TRUE, "paddle");
    ResourceManager::loadTexture("res/textures/face.png", GL_TRUE, "face");
    ResourceManager::loadTexture("res/textures/powerup.png", GL_TRUE, "speed_powerup");

    // load levels
    GameLevel one;
    one.load("res/levels/one.level", this->width, static_cast<GLuint>(this->height * 0.5));

    this->levels.push_back(one);
    this->level = 0;

    // load player
    glm::vec2 playerPos(this->width / 2.0f - PLAYER_SIZE.x / 2, this->height * 0.9);
    this->Player = new GameObject(playerPos, PLAYER_SIZE, ResourceManager::getTexture("paddle"), glm::vec3(1.0, 1.0, 1.0));

    // load ball
    glm::vec2 ballPos = playerPos + glm::vec2(PLAYER_SIZE.x / 2 - BALL_RADIUS, -1 * BALL_RADIUS * 2);
    this->balls.emplace_back(BallObject(ballPos, BALL_RADIUS, INITIAL_BALL_VELOCITY, ResourceManager::getTexture("face")));
}

void Game::update(float dt) {
    this->doCollisions();

    auto b = std::begin(this->balls);
    while (b != std::end(this->balls)) {
        b->Move(dt, this->width);

        if (b->position.y >= this->height) {
            if (this->balls.size() <= 1) {
                this->resetLevel();
                this->resetPlayer();
            }
            else {
                b = this->balls.erase(b);
                continue;
            }
        }
        ++b;
    }

    for (PowerUp & powerUp : this->powerUps) {
        powerUp.position += powerUp.velocity * dt;
        if (powerUp.active) {
            powerUp.duration -= dt;
            if (powerUp.duration <= 0.0f)
                powerUp.active = false;
        }
    }
    this->powerUps.erase(std::remove_if(this->powerUps.begin(), this->powerUps.end(),
                                        [](const PowerUp & powerUp) { return powerUp.isDestroyed && !powerUp.active; }
    ), this->powerUps.end());
}


void Game::processInput(GLfloat dt) {
    if (this->State == GAME_ACTIVE) {
        GLfloat velocity = PLAYER_SPEED * dt;
        // Move playerboard
        if (this->Keys[GLFW_KEY_A]) {
            if (Player->position.x >= 0) {
                Player->position.x -= velocity;
                if (this->balls[0].stuck)
                    this->balls[0].position.x -= velocity;
            }
        }

        if (this->Keys[GLFW_KEY_D]) {
            if (Player->position.x <= this->width - Player->size.x) {
                Player->position.x += velocity;
                if (this->balls[0].stuck)
                    this->balls[0].position.x += velocity;
            }
        }

        if (this->Keys[GLFW_KEY_SPACE])
            this->balls[0].stuck = GL_FALSE;
        if (this->Keys[GLFW_KEY_R]) {
            // place one ball at the paddle
            glm::vec2 newPos =
                    this->Player->position + glm::vec2(PLAYER_SIZE.x / 2 - BALL_RADIUS, -1 * BALL_RADIUS * 2);
            BallObject ball = this->balls[0];
            ball.reset(newPos);

            // remove all other balls and add the one
            this->balls.clear();
            this-> balls.push_back(ball);
        }
    }
}

void Game::render() {
    if (this->State == GAME_ACTIVE) {
        Renderer->DrawSprite(ResourceManager::getTexture("background"), glm::vec2(0, 0),
                             glm::vec2(this->width, this->height), 0.0f);
        this->levels[this->level].Draw(*Renderer);
        this->Player->Draw(*Renderer);

        for (BallObject & ball : this->balls)
            ball.Draw(*Renderer);

        for (PowerUp & powerUp : this->powerUps) {
            powerUp.Draw(*Renderer);
        }
    }
}

void Game::doCollisions() {
    for (BallObject & ball : this->balls) {
        for (GameObject & box : this->levels[this->level].bricks) {
            if (!box.isDestroyed) {
                Collision collision = CheckCollision(ball, box);
                // if there is a collision
                if (std::get<0>(collision)) {
                    // create a powerUp at the box's location
                    spawnPowerUp(box);
                    // destroy block if not solid
                    if (!box.isSolid)
                        box.isDestroyed = GL_TRUE;
                    // Collision resolution
                    Direction dir = std::get<1>(collision);
                    glm::vec2 diff_vector = std::get<2>(collision);
                    // Horizontal collision
                    if (dir == LEFT || dir == RIGHT) {
                        // Reverse horizontal velocity
                        ball.velocity.x = -ball.velocity.x;
                        // Relocate
                        GLfloat penetration = ball.radius - std::abs(diff_vector.x);
                        if (dir == LEFT)
                            ball.position.x += penetration; // Move ball to right
                        else
                            ball.position.x -= penetration; // Move ball to left;
                    }
                        // Vertical collision
                    else {
                        ball.velocity.y = -ball.velocity.y; // Reverse vertical velocity
                        // Relocate
                        GLfloat penetration = ball.radius - std::abs(diff_vector.y);
                        if (dir == UP)
                            ball.position.y -= penetration; // Move ball back up
                        else
                            ball.position.y += penetration; // Move ball back down
                    }
                }
            }
        }
    }

    // collisions with the ball and paddle
    for (BallObject & ball : this->balls) {
        Collision result = CheckCollision(ball, *Player);
        if (!ball.stuck && std::get<0>(result)) {
            // Check where it hit the board, and change velocity based on where it hit the board
            float centerBoard = Player->position.x + Player->size.x / 2;
            float distance = (ball.position.x + ball.radius) - centerBoard;
            float percentage = distance / (Player->size.x / 2);
            // Then move accordingly
            float strength = 2.0f;
            glm::vec2 oldvelocity = ball.velocity;
            ball.velocity.x = INITIAL_BALL_VELOCITY.x * percentage * strength;
            ball.velocity.y = -abs(ball.velocity.y);
            ball.velocity = glm::normalize(ball.velocity) * glm::length(oldvelocity);
        }
    }

    // powerup collisions with player
    for (PowerUp & powerUp : this->powerUps) {
        if (!powerUp.isDestroyed) {
            if (powerUp.position.y >= this->height)
                powerUp.isDestroyed = true;
            if (CheckCollision(*Player, powerUp)) {
                // activate and destroy the powerUp when it touces the player
                activatePowerUp(powerUp);
                powerUp.isDestroyed = GL_TRUE;
                powerUp.active = GL_TRUE;
            }
        }
    }
    // remove all destroyed power ups because you can't delete from a range based for loop
    this->powerUps.erase(std::remove_if(this->powerUps.begin(), this->powerUps.end(),
                                        [](const PowerUp & powerUp) { return powerUp.isDestroyed && !powerUp.active; }
    ), this->powerUps.end());
}

void Game::resetPlayer() {
    Player->position = glm::vec2(this->width / 2 - PLAYER_SIZE.x / 2, this->height * 0.9 - PLAYER_SIZE.y);
    this->balls.clear();
    this->balls.emplace_back(BallObject(Player->position + glm::vec2(PLAYER_SIZE.x / 2 - BALL_RADIUS, -(BALL_RADIUS * 2)),
                             BALL_RADIUS, INITIAL_BALL_VELOCITY, ResourceManager::getTexture("face")));
}

void Game::resetLevel() {

}

void Game::spawnPowerUp(GameObject & block) {
    int random = rand() % 30;

    switch ((powerUpType) random) {
        case powerUpType::speed:
            this->powerUps.emplace_back(powerUpType::speed, glm::vec3(1.0f, 0.5f, 1.0f), 0.0f, block.position,
                                        ResourceManager::getTexture("speed_powerup"));
            break;
        case powerUpType::paddleSize:
            this->powerUps.emplace_back(powerUpType::paddleSize, glm::vec3(1.0f, 0.5f, 1.0f), 0.0f, block.position,
                                        ResourceManager::getTexture("speed_powerup"));
            break;
        case extraBall:
            this->powerUps.emplace_back(powerUpType::extraBall, glm::vec3(1.0f, 0.5f, 1.0f), 0.0f, block.position,
                                        ResourceManager::getTexture("speed_powerup"));
            break;
        case passThrough:
            this->powerUps.emplace_back(powerUpType::speed, glm::vec3(1.0f, 0.5f, 1.0f), 0.0f, block.position,
                                        ResourceManager::getTexture("speed_powerup"));
            break;
        default:
            break;
    }
}

void Game::activatePowerUp(PowerUp & powerUp) {
    switch (powerUp.type) {
        case speed:
            for (BallObject & b : this->balls)
                b.velocity *= 1.1;
            break;
        case paddleSize:
            Player->size.x += 20;
            break;
        case extraBall: {
            glm::vec2 pos = Player->position + glm::vec2(PLAYER_SIZE.x / 2 - BALL_RADIUS, -1 * BALL_RADIUS * 2);
            BallObject ball(pos, BALL_RADIUS, INITIAL_BALL_VELOCITY, ResourceManager::getTexture("face"));
            ball.stuck = false;
            this->balls.emplace_back(ball);
            break;
        }
        case passThrough:
            break;
    }
}

bool CheckCollision(GameObject & one, GameObject & two) {
    // x axis
    bool collisionX = one.position.x + one.size.x >= two.position.x &&
                      two.position.x + two.size.x >= one.position.x;
    // y axis
    bool collisionY = one.position.y + one.size.y >= two.position.y &&
                      two.position.y + two.size.y >= one.position.y;

    // Collision only if on both axes
    return collisionX && collisionY;
}

Collision CheckCollision(BallObject & one, GameObject & two) {
    // circle center
    glm::vec2 center(one.position + one.radius);
    // calculate aabb info
    glm::vec2 aabb_half_extents(two.size.x / 2, two.size.y / 2);
    glm::vec2 aabb_center(two.position.x + aabb_half_extents.x, two.position.y + aabb_half_extents.y);
    // get difference vector between both centers
    glm::vec2 difference = center - aabb_center;
    glm::vec2 clamped = glm::clamp(difference, -aabb_half_extents, aabb_half_extents);
    // add clamped value to aabb_center to get the value of the box closest to the circle
    glm::vec2 closest = aabb_center + clamped;
    difference = closest - center;

    if (glm::length(difference) < one.radius)
        return std::make_tuple(GL_TRUE, VectorDirection(difference), difference);
    else
        return std::make_tuple(GL_FALSE, UP, glm::vec2(0, 0));
//    return glm::length(difference) < one.radius;
}

Direction VectorDirection(glm::vec2 target) {
    glm::vec2 compass[] = {
            glm::vec2(0.0f, 1.0f),    // up
            glm::vec2(1.0f, 0.0f),    // right
            glm::vec2(0.0f, -1.0f),    // down
            glm::vec2(-1.0f, 0.0f)    // left
    };
    GLfloat max = 0.0f;
    GLuint best_match = -1;
    for (GLuint i = 0; i < 4; i++) {
        GLfloat dot_product = glm::dot(glm::normalize(target), compass[i]);
        if (dot_product > max) {
            max = dot_product;
            best_match = i;
        }
    }
    return (Direction) best_match;
}

