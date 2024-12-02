#include "Game.h"
#include "Vec2.h"
#include <stdlib.h>
#include <time.h>
#include <iostream>
#include <fstream>
#include <string>
#include <sstream>
#include <cmath>

#define FIXED_BORDER_LEN 2

Game::Game(const std::string& configFilePath) {
    init(configFilePath);
}

// done
void Game::init(const std::string& configFilePath) {
    std::ifstream fin(configFilePath);
    if (!fin.is_open()) {
        std::cerr << "无法打开配置文件！" << std::endl;
        return;
    }

    /* load config from file */
    std::string line;
    while (std::getline(fin, line)) {
        std::istringstream lineStream(line);
        std::string label;
        // 行首是配置的类型
        lineStream >> label;

        if (label == "Window") {
            lineStream >> m_windowConfig.W >> m_windowConfig.H >>              // Window Width, window Height
                m_windowConfig.F >> m_windowConfig.M;                          // Window Frame Rate, Window Mode
        }
        else if (label == "Font") {
            lineStream >> m_fontConfig.FS >> m_fontConfig.S >>                 // Font File Path, Font Size
                m_fontConfig.FR >> m_fontConfig.FG >> m_fontConfig.FB;         // Font RGB
        }
        else if (label == "Player") {
            lineStream >> m_playerConfig.SR >> m_playerConfig.CR >>            // Shape Radius, Collision Radius
                m_playerConfig.S >>                                            // Speed
                m_playerConfig.FR >> m_playerConfig.FG >> m_playerConfig.FB >> // Fill RGB
                m_playerConfig.OR >> m_playerConfig.OG >> m_playerConfig.OB >> // Outline RGB
                m_playerConfig.OT >> m_playerConfig.V;                         // Outline Thickness
        }
        else if (label == "Enemy") {
            lineStream >> m_enemyConfig.SR >> m_enemyConfig.CR >>              // Shape Radius, Collisions Radius
                m_enemyConfig.SMIN >> m_enemyConfig.SMAX >>                    // Speed [Min, Max]
                m_enemyConfig.OR >> m_enemyConfig.OG >> m_enemyConfig.OB >>    // Outline RGB
                m_enemyConfig.OT >>                                            // Outline Thickness
                m_enemyConfig.VMIN >> m_enemyConfig.VMAX >>                    // Vertices [Min, Max]
                m_enemyConfig.L >> m_enemyConfig.SI;                           // Small Enemies Lifespan, Spawn Interval
        }
        else {
            lineStream >> m_bulletConfig.SR >> m_bulletConfig.CR >>            // Shape Radius, Collision Radius
                m_bulletConfig.S >>                                            // Speed
                m_bulletConfig.FR >> m_bulletConfig.FG >> m_bulletConfig.FB >> // Fill RGB
                m_bulletConfig.OR >> m_bulletConfig.OG >> m_bulletConfig.OB >> // Outline RGB
                m_bulletConfig.OT >> m_bulletConfig.V >> m_bulletConfig.L;     // Outline Thickness, Vertices, Lifespan
        }
    }
    fin.close();

    /* init window */
    if (m_windowConfig.M) {
        m_window.create(sf::VideoMode(m_windowConfig.W, m_windowConfig.H, sf::Style::Fullscreen), "Assignment 2");
    }
    else {
        m_window.create(sf::VideoMode(m_windowConfig.W, m_windowConfig.H), "Assignment 2");
    }
    m_window.setFramerateLimit(m_windowConfig.F);

    /* init font and game name */
    if (!m_font.loadFromFile(m_fontConfig.FS)) {
        std::cout << "font style file load failed!" << std::endl;
        return;
    }
    m_text.setFont(m_font);
    /*m_text.setString("Shape Combat Score ");*/
    m_text.setCharacterSize(m_fontConfig.S);
    m_text.setFillColor(sf::Color(m_fontConfig.FR, m_fontConfig.FG, m_fontConfig.FB));
    m_text.setPosition(0 + FIXED_BORDER_LEN, m_windowConfig.H - m_fontConfig.S - FIXED_BORDER_LEN);

    /* spawn player here */
    spawnPlayer();
}

void Game::run() {
    while (m_running) {
        m_entities.update();    // 更新实体

        // 如果游戏被暂停，只渲染
        if (!m_pause) {
            sEnemySpawner();        // 生成敌人 return
            sMovement();            // 计算下一帧移动到的位置 player, bullet
            sCollision();           // 碰撞检测 return
            sLifeSpan();            // 所有含有 lifespan 的 entities lifespan - 1

            ++m_currentFrame;
        }
        sUserInput();           // 用户输入
        sRender();              // 渲染

        ++m_currentFrame;
    }
}

void Game::setPaused(bool pause) {
    m_pause = pause;
}

// systems
void Game::sMovement() {
    //return;
    // TODO: implement all entity movement in this function
    //       you should reaad the m_player->cInput component to determine if the player is moving

    m_player->cTransform->velocity.x = 0.0f;
    m_player->cTransform->velocity.y = 0.0f;

    // 根据输入更新速度
    if (m_player->cInput->up) {
        m_player->cTransform->velocity.y = -5.0f;
    }
    if (m_player->cInput->down) {
        m_player->cTransform->velocity.y = 5.0f;
    }
    if (m_player->cInput->left) {
        m_player->cTransform->velocity.x = -5.0f;
    }
    if (m_player->cInput->right) {
        m_player->cTransform->velocity.x = 5.0f;
    }

    //
    for (auto& e : m_entities.getEntities()) {
        if (e->tag() == "player") {
            continue;
        }
        e->cTransform->pos.x += e->cTransform->velocity.x;
        e->cTransform->pos.y += e->cTransform->velocity.y;
    }

    // player 更新位置，需要判断是否超出边界
    if (m_player->cTransform->pos.x - m_player->cShape->circle.getRadius() <= 0 && m_player->cTransform->velocity.x < 0) {
        m_player->cTransform->velocity.x = 0;
    }
    else if (m_player->cTransform->pos.x + m_player->cShape->circle.getRadius() >= m_window.getSize().x && m_player->cTransform->velocity.x > 0) {
        m_player->cTransform->velocity.x = 0;
    }

    if (m_player->cTransform->pos.y - m_player->cShape->circle.getRadius() <= 0 && m_player->cTransform->velocity.y < 0) {
        m_player->cTransform->velocity.y = 0;
    }
    else if (m_player->cTransform->pos.y + m_player->cShape->circle.getRadius() >= m_window.getSize().y && m_player->cTransform->velocity.y > 0) {
        m_player->cTransform->velocity.y = 0;
    }

    // 根据 x, y 方向上的速度更新 pos
    m_player->cTransform->pos.x += m_player->cTransform->velocity.x;
    m_player->cTransform->pos.y += m_player->cTransform->velocity.y;
}

void Game::sUserInput() {
    sf::Event event;
    while (m_window.pollEvent(event)) {
        // this event is triggered when the window is closed
        if (event.type == sf::Event::Closed) {
            m_running = false;
        }

        // this event is triggered when a key is pressed
        if (!m_pause && event.type == sf::Event::KeyPressed) {
            switch (event.key.code) {
            case sf::Keyboard::W:
                // std::cout << "W pressed" << std::endl;
                m_player->cInput->up = true;
                break;
            case sf::Keyboard::S:
                // std::cout << "S pressed" << std::endl;
                m_player->cInput->down = true;
                break;
            case sf::Keyboard::A:
                // std::cout << "A pressed" << std::endl;
                m_player->cInput->left = true;
                break;
            case sf::Keyboard::D:
                // std::cout << "D pressed" << std::endl;
                m_player->cInput->right = true;
                break;
            case sf::Keyboard::Escape:
                // std::cout << "ESC pressed" << std::endl;
                m_running = false;
            default:
                break;
            }
        }

        // this event is triggered when a key is released
        if (event.type == sf::Event::KeyReleased) {
            switch (event.key.code) {
            case sf::Keyboard::W:
                // std::cout << "W released" << std::endl;
                m_player->cInput->up = false;
                break;
            case sf::Keyboard::S:
                // std::cout << "S released" << std::endl;
                m_player->cInput->down = false;
                break;
            case sf::Keyboard::A:
                // std::cout << "A released" << std::endl;
                m_player->cInput->left = false;
                break;
            case sf::Keyboard::D:
                // std::cout << "D released" << std::endl;
                m_player->cInput->right = false;
                break;
            case sf::Keyboard::P:
                // std::cout << "P released" << std::endl;
                setPaused(!m_pause);

                break;
            default:
                break;
            }
        }

        // this event is triggered when a mouse button is pressed
        if (!m_pause && event.type == sf::Event::MouseButtonPressed) {
            if (event.mouseButton.button == sf::Mouse::Left) {
                // std::cout << "Left mouse button pressed at (" << event.mouseButton.x << ", " << event.mouseButton.y << ")" << std::endl;
                // call spawnBullet here
                spawnBullet(m_player, Vec2(event.mouseButton.x, event.mouseButton.y));
            }
            if (event.mouseButton.button == sf::Mouse::Right) {
                std::cout << "Right mouse button pressed at (" << event.mouseButton.x << ", " << event.mouseButton.y << ")" << std::endl;
                // call spawnSpecialWeapon here
                spawnSpecialWeapon(m_player);
            }
        }
    }
}

void Game::sLifeSpan() {
    for (auto& e : m_entities.getEntities()) {
        if (e->cLifespan != nullptr) {
            if (e->cLifespan->remaining > 0) {
                --e->cLifespan->remaining;
            }
            else {
                e->destroy();
            }
        }
    }
}

void Game::sRender() {
    m_window.clear();

    // 敌人渲染
    for (auto& e : m_entities.getEntities()) {
        // position
        e->cShape->circle.setPosition(e->cTransform->pos.x, e->cTransform->pos.y);

        // rotation
        if (e->tag() != "bullet" && e->tag() != "smallenemy") {
            e->cTransform->angle += 1.0f;
            e->cShape->circle.setRotation(e->cTransform->angle);
        }

        if (e->tag() == "smallenemy") {
            e->cTransform->angle += 5.0f;
            e->cShape->circle.setRotation(e->cTransform->angle);
        }

        // lifespan transparency
        if (e->cLifespan != nullptr) {
            e->cShape->circle.setFillColor(sf::Color(e->cShape->circle.getFillColor().r, e->cShape->circle.getFillColor().g, e->cShape->circle.getFillColor().b, 255 * e->cLifespan->remaining / e->cLifespan->total));
        }

        m_window.draw(e->cShape->circle);
    }

    // 子弹渲染
    for (auto& e : m_entities.getEntities("bullet")) {
        e->cShape->circle.setPosition(e->cTransform->pos.x, e->cTransform->pos.y);
        m_window.draw(e->cShape->circle);
    }

    // text
    std::string str = "Shape Combat --- Score: " + std::to_string(m_score);
    m_text.setString(str);
    m_window.draw(m_text);

    // window display
    m_window.display();
}

// 生成敌人
void Game::sEnemySpawner() {
    // if it has been long enough, spawn an enemy
    if (m_currentFrame - m_lastEnemySpawnTime >= m_enemyConfig.SI) {
        spawnEnemy();
        m_lastEnemySpawnTime = m_currentFrame;
    }
}

// 碰撞检测
void Game::sCollision() {
    // 射击检测
    for (auto b : m_entities.getEntities("bullet")) {
        for (auto e : m_entities.getEntities("enemy")) {
            if (b->cTransform->pos.distance(e->cTransform->pos) < (b->cShape->circle.getRadius() + e->cShape->circle.getRadius())) {
                if (e->tag() == "enemy") {
                    spawnSmallEnemies(e);
                }
                b->destroy();
                e->destroy();
                m_score += 100;
            }
        }
        for (auto e : m_entities.getEntities("smallenemy")) {
            if (b->cTransform->pos.distance(e->cTransform->pos) < (b->cShape->circle.getRadius() + e->cShape->circle.getRadius())) {
                b->destroy();
                e->destroy();
                m_score += 300;
            }
        }
    }

    // 敌人：边界检测
    for (auto e : m_entities.getEntities("enemy")) {
        if ((e->cTransform->pos.x - e->cShape->circle.getRadius() <= 0) || (e->cTransform->pos.x + +e->cShape->circle.getRadius() >= m_window.getSize().x)) {
            e->cTransform->velocity.x = -e->cTransform->velocity.x;
        }
        if (e->cTransform->pos.y - e->cShape->circle.getRadius() <= 0 || e->cTransform->pos.y + e->cShape->circle.getRadius() >= m_window.getSize().y) {
            e->cTransform->velocity.y = -e->cTransform->velocity.y;
        }
    }

    // 小敌人：边界检测
    for (auto e : m_entities.getEntities("smallenemy")) {
        if ((e->cTransform->pos.x - e->cShape->circle.getRadius() <= 0) || (e->cTransform->pos.x + +e->cShape->circle.getRadius() >= m_window.getSize().x)) {
            e->cTransform->velocity.x = -e->cTransform->velocity.x;
        }
        if (e->cTransform->pos.y - e->cShape->circle.getRadius() <= 0 || e->cTransform->pos.y + e->cShape->circle.getRadius() >= m_window.getSize().y) {
            e->cTransform->velocity.y = -e->cTransform->velocity.y;
        }
    }

    // 玩家与敌人碰撞检测
    for (auto e : m_entities.getEntities("enemy")) {
        if (m_player->cTransform->pos.distance(e->cTransform->pos) < (m_player->cShape->circle.getRadius() + e->cShape->circle.getRadius())) {
            m_player->destroy();
            e->destroy();
            spawnPlayer();
            m_score -= 100;
        }
    }

    // 玩家与小敌人碰撞检测
    for (auto e : m_entities.getEntities("smallenemy")) {
        if (m_player->cTransform->pos.distance(e->cTransform->pos) < (m_player->cShape->circle.getRadius() + e->cShape->circle.getRadius())) {
            m_player->destroy();
            e->destroy();
            spawnPlayer();
            m_score -= 100;
        }
    }
}

// helper function
void Game::spawnPlayer() {
    auto entity = m_entities.addEntity("player");

    entity->cTransform = std::make_shared<CTransform>(
        Vec2(m_windowConfig.W / 2, m_windowConfig.H / 2),   // initial pos
        Vec2(0.0f, 0.0f),                                   // initial movement velocity
        0.0f);                                              // initial movement angle

    // the entity's shape will have radius 32, 8 sides, dark grey fill, and red outline of thickness 4
    // init player shape base on playerConfig
    entity->cShape = std::make_shared<CShape>(
        m_playerConfig.SR,                                                     // shape raduis
        m_playerConfig.V,                                                      // shape vertices
        sf::Color(m_playerConfig.FR, m_playerConfig.FG, m_playerConfig.FB),    // fill color
        sf::Color(m_playerConfig.OR, m_playerConfig.OG, m_playerConfig.OB),    // outline color
        m_playerConfig.OT);                                                    // thickness

    // set the center point as the shape center instead of using the top left corner
    entity->cShape->circle.setOrigin(m_playerConfig.SR, m_playerConfig.SR);

    // add an input component to the player so that we can use inputs
    entity->cInput = std::make_shared<CInput>();

    m_player = entity;
}

void Game::spawnEnemy() {
    // std::cout<<"enemy spawned"<<std::endl;
    auto entity = m_entities.addEntity("enemy");

    // 坐标
    float ex = rand() % m_window.getSize().x;
    float ey = rand() % m_window.getSize().y;

    // 速度
    float sx = ((rand() % (int)(m_enemyConfig.SMAX - m_enemyConfig.SMIN)) + m_enemyConfig.SMIN) * (rand() % 2 ? 1 : -1);
    float sy = ((rand() % (int)(m_enemyConfig.SMAX - m_enemyConfig.SMIN)) + m_enemyConfig.SMIN) * (rand() % 2 ? 1 : -1);

    entity->cTransform = std::make_shared<CTransform>(Vec2(ex, ey), Vec2(sx, sy), 0.0f);

    // 边的数量
    int points = rand() % (m_enemyConfig.VMAX - m_enemyConfig.VMIN) + m_enemyConfig.VMIN;

    // 填充颜色
    uint8_t f_r = rand() % 256;
    uint8_t f_g = rand() % 256;
    uint8_t f_b = rand() % 256;

    // 轮廓颜色
    uint8_t o_r = rand() % 256;
    uint8_t o_g = rand() % 256;
    uint8_t o_b = rand() % 256;

    entity->cShape = std::make_shared<CShape>(m_enemyConfig.SR, points, sf::Color(f_r, f_g, f_b), sf::Color(o_r, o_g, o_b), m_enemyConfig.OT);

    entity->cShape->circle.setOrigin(m_enemyConfig.SR, m_playerConfig.SR);
}

void Game::spawnSmallEnemies(std::shared_ptr<Entity> entity) {
    std::cout << "small enemy spawned" << std::endl;

    // 当前敌人的坐标
    float x = entity->cTransform->pos.x;
    float y = entity->cTransform->pos.y;

    // 当前敌人的边数
    int points = entity->cShape->circle.getPointCount();

    // 填充颜色
    uint8_t f_r = entity->cShape->circle.getFillColor().r;
    uint8_t f_g = entity->cShape->circle.getFillColor().g;
    uint8_t f_b = entity->cShape->circle.getFillColor().b;

    // 轮廓颜色
    uint8_t o_r = entity->cShape->circle.getOutlineColor().r;
    uint8_t o_g = entity->cShape->circle.getOutlineColor().g;
    uint8_t o_b = entity->cShape->circle.getOutlineColor().b;

    // 速度
    float sx = entity->cTransform->velocity.x;
    float sy = entity->cTransform->velocity.y;
    float s = sqrt(sx * sx + sy * sy);

    // 生成死亡敌人的边数的小敌人
    for (int i = 0; i < entity->cShape->circle.getPointCount(); ++i) {
        auto e = m_entities.addEntity("smallenemy");
        // 速度继承 entity 的，角度为 360/v * i，math sin cos 使用的是弧度制，需要 * pi / 180
        float angle = (360.0f / points * i) * 3.1415926 / 180;
        // std::cout<<"angle: "<<angle<<std::endl;

        sx = s * cos(angle);
        // std::cout<<"cos: "<<cos(angle)<<" sx: "<<sx<<std::endl;
        sy = s * sin(angle);
        // std::cout<<"sin: "<<sin(angle)<<" sy: "<<sy<<std::endl;

        e->cTransform = std::make_shared<CTransform>(Vec2(x, y), Vec2(sx, sy), 0.0f);
        e->cShape = std::make_shared<CShape>(m_enemyConfig.SR / 2, points, sf::Color(f_r, f_g, f_b), sf::Color(o_r, o_g, o_b), m_enemyConfig.OT);
        e->cShape->circle.setOrigin(m_enemyConfig.SR / 2, m_playerConfig.SR / 2);
        e->cLifespan = std::make_shared<CLifespan>(m_enemyConfig.L);
    }
}

void Game::spawnBullet(std::shared_ptr<Entity> entity, const Vec2& mousePos) {
    // get player position
    sf::Vector2f playerPos = m_player->cShape->circle.getPosition();
    Vec2 startPos(playerPos.x, playerPos.y);

    // calculate sqrt((Bx - Px)^2 + (By - Py)^2)
    float distance = std::sqrt((mousePos.x - playerPos.x) * (mousePos.x - playerPos.x) + (mousePos.y - playerPos.y) * (mousePos.y - playerPos.y));

    // std::cout << "distance: " << distance << std::endl;

    float sx = (m_bulletConfig.S * (mousePos.x - playerPos.x)) / distance;
    float sy = (m_bulletConfig.S * (mousePos.y - playerPos.y)) / distance;

    std::cout << "sx: " << sx << std::endl;
    std::cout << "sy: " << sy << std::endl;

    auto bullet = m_entities.addEntity("bullet");

    bullet->cTransform = std::make_shared<CTransform>(startPos, Vec2(sx, sy), 0);
    // bullet->cShape = std::make_shared<CShape>(float , int points, sf::Color & fill, const sf::Color & outline, float thickness);
    bullet->cShape = std::make_shared<CShape>(8, 8, sf::Color(255, 255, 255), sf::Color(255, 0, 0), 2);

    bullet->cLifespan = std::make_shared<CLifespan>(m_bulletConfig.L);
}

void Game::spawnSpecialWeapon(std::shared_ptr<Entity> eneity) {
    // get player position
    sf::Vector2f playerPos = m_player->cShape->circle.getPosition();
    Vec2 startPos(playerPos.x, playerPos.y);

    for (size_t i = 0; i < 36; ++i) {
        auto bullet = m_entities.addEntity("bullet");

        float sx = m_bulletConfig.S * cos(i * 10 * 3.1415926 / 180);
        float sy = m_bulletConfig.S * sin(i * 10 * 3.1415926 / 180);

        bullet->cTransform = std::make_shared<CTransform>(startPos, Vec2(sx, sy), 0);

        bullet->cShape = std::make_shared<CShape>(8, 8, sf::Color(255, 255, 255), sf::Color(255, 0, 0), 2);

        bullet->cLifespan = std::make_shared<CLifespan>(m_bulletConfig.L);
    }
}