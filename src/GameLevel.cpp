#include <sstream>

#include "GameLevel.h"

void GameLevel::load(const char * filePath, GLuint levelWidth, GLuint levelHeight) {
    // remove any old data
    this->bricks.clear();

    // load from file
    GLuint tileCode;
    std::string line;
    std::ifstream file(filePath);
    std::vector<std::vector<GLuint>> tileData;

    if (file) {
        while (std::getline(file, line)) // read each line
        {
            std::istringstream sstream(line);
            std::vector<GLuint> row;
            while (sstream >> tileCode) // read each word seperated by spaces
            {
                row.push_back(tileCode);
            }
            tileData.push_back(row);
        }
        if (!tileData.empty())
            this->init(tileData, levelWidth, levelHeight);
    }
}

void GameLevel::Draw(SpriteRenderer & renderer) {
    for (auto & tile : this->bricks) {
        if (!tile.isDestroyed)
            tile.Draw(renderer);
    }
}

GLboolean GameLevel::isCompleted() {
    for (auto & tile : this->bricks) {
        if (!tile.isSolid && !tile.isDestroyed)
            return GL_FALSE;
    }
    return GL_TRUE;
}

void GameLevel::init(std::vector<std::vector<GLuint>> tileData, GLuint levelWidth, GLuint levelHeight) {
    auto height = (GLuint) tileData.size();
    auto width = (GLuint) tileData[0].size();
    GLfloat unit_width = levelWidth / static_cast<GLfloat>(width);
    GLfloat unit_height = levelHeight / static_cast<GLfloat>(height);

    // initialize level tiles based on tileData
    for (GLuint y = 0; y < height; ++y) {
        for (GLuint x = 0; x < width; ++x) {
            // check the block type
            if (tileData[y][x] == 1) // solid
            {
                glm::vec2 pos(unit_width * x, unit_height * y);
                glm::vec2 size(unit_width, unit_height);
                GameObject obj(pos, size, ResourceManager::getTexture("block_solid"), glm::vec3(0.8, 0.8, -.7));
                obj.isSolid = GL_TRUE;
                this->bricks.push_back(obj);
            } else if (tileData[y][x] > 1) // non-solid determine color based on level data
            {
                glm::vec3 color = glm::vec3(1.0);
                if (tileData[y][x] == 2)
                    color = glm::vec3(0.2f, 0.6f, 1.0f);
                else if (tileData[y][x] == 3)
                    color = glm::vec3(0.0f, 0.7f, 0.0f);
                else if (tileData[y][x] == 4)
                    color = glm::vec3(0.8f, 0.8f, 0.4f);
                else if (tileData[y][x] == 5)
                    color = glm::vec3(1.0f, 0.5f, 0.0f);

                glm::vec2 pos(unit_width * x, unit_height * y);
                glm::vec2 size(unit_width, unit_height);
                GameObject obj(pos, size, ResourceManager::getTexture("block"), color);
                this->bricks.push_back(obj);
            }
        }
    }
}

