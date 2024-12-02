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
    } else {
        m_window.create(sf::VideoMode(m_windowConfig.W, m_windowConfig.H), "Assignment 2");
    }
    m_window.setFramerateLimit(m_windowConfig.F);

    /* init font and game name */
    if (!m_font.loadFromFile(m_fontConfig.FS)) {
        std::cout << "font style file load failed!" << std::endl;
        return;
    }
    m_text.setFont(m_font);
    m_text.setString("Shape Combat");
    m_text.setCharacterSize(m_fontConfig.S);
    m_text.setFillColor(sf::Color(m_fontConfig.FR, m_fontConfig.FG, m_fontConfig.FB));
    m_text.setPosition(m_windowConfig.H - FIXED_BORDER_LEN, m_windowConfig.W - FIXED_BORDER_LEN);

    /* spawn player here */
    spawnPlayer();
}

void Game::run() {
    // TODO: add pause functionallity
    //       some systems should function while paused (rendering)
    //       some systems shouldn't (movement, input)
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
        // sEnemySpawner();        // 生成敌人 return
        // sMovement();            // 计算下一帧移动到的位置 player, bullet
        // sCollision();           // 碰撞检测 return
        sUserInput();           // 用户输入
        // sLifeSpan();            // 所有含有 lifespan 的 entities lifespan - 1
        sRender();              // 渲染

        ++m_currentFrame;
    }
}

void Game::setPaused(bool pause) {
    // m_pause = pause;
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
    for (auto &e : m_entities.getEntities()) {
        if (e->tag() == "player") {
            continue;
        }
        e->cTransform->pos.x += e->cTransform->velocity.x;
        e->cTransform->pos.y += e->cTransform->velocity.y;
    }

    // for (auto &e : m_entities.getEntities("enemy")) {
    //     e->cTransform->pos.x += e->cTransform->velocity.x;
    //     e->cTransform->pos.y += e->cTransform->velocity.y;
    // }

    // player 更新位置，需要判断是否超出边界
    if (m_player->cTransform->pos.x - m_player->cShape->circle.getRadius() <= 0 && m_player->cTransform->velocity.x < 0) {
        m_player->cTransform->velocity.x = 0;
    } else if (m_player->cTransform->pos.x + m_player->cShape->circle.getRadius() >= m_window.getSize().x && m_player->cTransform->velocity.x > 0) {
        m_player->cTransform->velocity.x = 0;
    }

    if (m_player->cTransform->pos.y - m_player->cShape->circle.getRadius() <= 0 && m_player->cTransform->velocity.y < 0) {
        m_player->cTransform->velocity.y = 0;
    } else if (m_player->cTransform->pos.y + m_player->cShape->circle.getRadius() >= m_window.getSize().y && m_player->cTransform->velocity.y > 0) {
        m_player->cTransform->velocity.y = 0;
    }

    // 根据 x, y 方向上的速度更新 pos
    m_player->cTransform->pos.x += m_player->cTransform->velocity.x;
    m_player->cTransform->pos.y += m_player->cTransform->velocity.y;
}

void Game::sUserInput() {
    // TODO: handle user input here
    // note that you should only be setting the player's input component variables here
    // you should not implement the player's movement logic here
    // the movement system will read the variables you set in this function

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
                    m_pause = !m_pause;
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
                // std::cout << "Right mouse button pressed at (" << event.mouseButton.x << ", " << event.mouseButton.y << ")" << std::endl;
                // call spawnSpecialWeapon here
            }
        }
    }
}

void Game::sLifeSpan() {
    //return;
    // TODO: implement the lifespan system

    // for all entities
    // if entity has no lifespan component, skip it
    // if entity has > 0 remaining lifespan, subtract 1
    // it if has lifespan and is alive
    // scale its alpha channel properly
    // if it has lifespan and its time is up
    // destroy the entity

    for (auto& e : m_entities.getEntities()) {
        if (e->cLifespan != nullptr) {
            if (e->cLifespan->remaining > 0) {
                --e->cLifespan->remaining;
            } else {
                e->destroy();
            }
            // std::cout << "bullet life remain: " << e->cLifespan->remaining << std::endl;c
        }
    }
}

void Game::sRender() {
    // TODO: change the code below to draw all of the entities
    //       sample drawing of the player entity that we have created
    m_window.clear();

    // 敌人渲染
    for (auto& e : m_entities.getEntities()) {
        // set the position of the shape based on the entity's transform->pos
        // set pos
        e->cShape->circle.setPosition(e->cTransform->pos.x, e->cTransform->pos.y);

        // 旋转
        if (e->tag() != "bullet") {
            e->cTransform->angle += 1.0f;
            e->cShape->circle.setRotation(e->cTransform->angle);
        }

        if (e->cLifespan != nullptr) {
            
        }

        m_window.draw(e->cShape->circle);
    }

    // 子弹渲染
    for (auto& e : m_entities.getEntities("bullet")) {
        e->cShape->circle.setPosition(e->cTransform->pos.x, e->cTransform->pos.y);
        m_window.draw(e->cShape->circle);
    }

    // draw the player entity
    // m_player->cShape->circle.setPosition(m_player->cTransform->pos.x, m_player->cTransform->pos.y);
    // m_player->cTransform->angle += 1.0f;
    // m_player->cShape->circle.setRotation(m_player->cTransform->angle);
    // m_window.draw(m_player->cShape->circle);

    // draw the text
    m_window.draw(m_text);

    // display the window
    m_window.display();
}

// 生成敌人
void Game::sEnemySpawner() {
    // return;
    // TODO: code which implements enemey spawning should go here
    //
    // (use m_currentFrame - m_lastEnemySpawnTime) to determine
    // how long it has veen since the last enemy was spawned

    // if it has been long enough, spawn an enemy
    if (m_currentFrame - m_lastEnemySpawnTime >= m_enemyConfig.SI) {
        spawnEnemy();
        m_lastEnemySpawnTime = m_currentFrame;
    }

    // spawnEnemy();
}

// 碰撞检测
void Game::sCollision() {
    // return;
    // TODO: implement all proper collisions between entities
    //       be sure to use the collision radius, Not the shape radius

    // 射击检测
    for (auto b : m_entities.getEntities("bullet")) {
        for (auto e : m_entities.getEntities("enemy")) {
            if (b->cTransform->pos.distance(e->cTransform->pos) < (b->cShape->circle.getRadius() + e->cShape->circle.getRadius())) {
                if (e->tag() == "enemy") {
                    spawnSmallEnemies(e);
                }
                b->destroy();
                e->destroy();
            }
        }
        for (auto e : m_entities.getEntities("smallenemy")) {
            if (b->cTransform->pos.distance(e->cTransform->pos) < (b->cShape->circle.getRadius() + e->cShape->circle.getRadius())) {
                b->destroy();
                e->destroy();
            }
        }
    }

    // 敌人：边界检测
    for (auto e : m_entities.getEntities("enemy")) {
        if ((e->cTransform->pos.x - e->cShape->circle.getRadius() <= 0) || (e->cTransform->pos.x + + e->cShape->circle.getRadius() >= m_window.getSize().x)) {
            e->cTransform->velocity.x = -e->cTransform->velocity.x;
        }
        if (e->cTransform->pos.y - e->cShape->circle.getRadius() <= 0  || e->cTransform->pos.y + e->cShape->circle.getRadius() >= m_window.getSize().y) {
            e->cTransform->velocity.y = -e->cTransform->velocity.y;
        }
    }

    // 小敌人：边界检测
    for (auto e : m_entities.getEntities("smallenemy")) {
        if ((e->cTransform->pos.x - e->cShape->circle.getRadius() <= 0) || (e->cTransform->pos.x + + e->cShape->circle.getRadius() >= m_window.getSize().x)) {
            e->cTransform->velocity.x = -e->cTransform->velocity.x;
        }
        if (e->cTransform->pos.y - e->cShape->circle.getRadius() <= 0  || e->cTransform->pos.y + e->cShape->circle.getRadius() >= m_window.getSize().y) {
            e->cTransform->velocity.y = -e->cTransform->velocity.y;
        }
    }

    // 玩家与敌人碰撞检测
    for (auto e : m_entities.getEntities("enemy")) {
        if (m_player->cTransform->pos.distance(e->cTransform->pos) < (m_player->cShape->circle.getRadius() + e->cShape->circle.getRadius())) {
            m_player->destroy();
            e->destroy();
            spawnPlayer();
        }
    }
}

// helper function
void Game::spawnPlayer() {
    // TODO: Finish adding all properties of the player with the correct values from the config

    // we create every entity by calling EntityManager.addEntity(tag)
    // this return a std::shared_ptr<Entity>, so we use 'auto' to save typing
    auto entity = m_entities.addEntity("player");

    // give this entity a Transform so it spawns at (200, 200) with veloctiy (1, 1) and angle 0
    // m_window.getSize().x / 2, m_window.getSize().y / 2
    //entity->cTransform = std::make_shared<CTransform>(Vec2(200.0f, 200.0f), Vec2(0.0f, 0.0f), 0.0f);
    entity->cTransform = std::make_shared<CTransform>(
        Vec2(m_windowConfig.W / 2, m_windowConfig.H / 2),   // initial pos
        Vec2(0.0f, 0.0f),                                   // initial movement velocity
        0.0f);                                              // initial movement angle

    // std::cout << "W /2: " << m_windowConfig.W / 2 << std::endl;
    // std::cout << "H /2: " << m_windowConfig.H / 2 << std::endl;

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

    // std::cout << "Shape Radius: " << m_playerConfig.SR << std::endl;
    // std::cout << "Vertices: " << m_playerConfig.V << std::endl;
    // std::cout << "Thickness: " << m_playerConfig.OT << std::endl;

    // add an input component to the player so that we can use inputs
    entity->cInput = std::make_shared<CInput>();

    // since we ant this entity to be our player, set our Game's player variable to be this entity
    // this goes slightly against the EntityManager paradigm, but we use the player so much it's worth it
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
    // return;
    // TODO: spawn small enemies at the location of the input enemy e

    // when we create the smaller enemy, we have to read the values of the original enemy
    // - spawn a number of small enemies equal to the ;vertices of the original enemy
    // - set each small enemy to the same color as the original, half the size
    // - small enemies are worh double points of the original enemy

    std::cout<<"small enemy spawned"<<std::endl;
    // auto e = m_entities.addEntity("enemy");

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
        // 速度继承 entity 的，角度为 360/v * i，需要计算
        float angle = (360.0f / points * i) * 3.1415926 / 180;
        std::cout<<"angle: "<<angle<<std::endl;
        
        sx = s * cos(angle);
        std::cout<<"cos: "<<cos(angle)<<" sx: "<<sx<<std::endl;
        sy = s * sin(angle);
        std::cout<<"sin: "<<sin(angle)<<" sy: "<<sy<<std::endl;

        // std::cout<<"sx: "<<sx<<" sy: "<<sy<<std::endl;

        // 

        e->cTransform = std::make_shared<CTransform>(Vec2(x, y), Vec2(sx, sy), 0.0f);
        e->cShape = std::make_shared<CShape>(m_enemyConfig.SR / 2, points, sf::Color(f_r, f_g, f_b), sf::Color(o_r, o_g, o_b), m_enemyConfig.OT);
        e->cShape->circle.setOrigin(m_enemyConfig.SR / 2, m_playerConfig.SR / 2);
        e->cLifespan = std::make_shared<CLifespan>(m_enemyConfig.L);
    }
}

void Game::spawnBullet(std::shared_ptr<Entity> entity, const Vec2& mousePos) {
    // TODO: implement the spawning of a bullet which travels towards the mouse position
    //       - bullet speeed is given as a scalar speed
    //       - you must set he velocity by using formula in notes

    // get player position
    sf::Vector2f playerPos = m_player->cShape->circle.getPosition();
    Vec2 startPos(playerPos.x, playerPos.y);

    // calculate sqrt((Bx - Px)^2 + (By - Py)^2)
    float distance = std::sqrt((mousePos.x - playerPos.x)*(mousePos.x - playerPos.x) + (mousePos.y - playerPos.y) * (mousePos.y - playerPos.y));

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
    // TODO: implement your own special weapon
}