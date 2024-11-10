#include "Game.h"
#include "Vec2.h"
#include <stdlib.h>
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

        if (m_pause) {

        }
        sEnemySpawner();        // 生成敌人 return
        sMovement();            // 计算下一帧移动到的位置 player, bullet
        sCollision();           // 碰撞检测 return
        sUserInput();           // 用户输入
        sLifeSpan();            // 所有含有 lifespan 的 entities lifespan - 1
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

    for (auto &e : m_entities.getEntities()) {
        e->cTransform->pos.x += e->cTransform->velocity.x;
        e->cTransform->pos.y += e->cTransform->velocity.y;
    }


    // sample movement speed update
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
        if (event.type == sf::Event::KeyPressed) {
            switch (event.key.code) {
                case sf::Keyboard::W:
                    std::cout << "W pressed" << std::endl;
                    m_player->cInput->up = true;
                    break;
                case sf::Keyboard::S:
                    std::cout << "S pressed" << std::endl;
                    m_player->cInput->down = true;
                    break;
                case sf::Keyboard::A:
                    std::cout << "A pressed" << std::endl;
                    m_player->cInput->left = true;
                    break;
                case sf::Keyboard::D:
                    std::cout << "D pressed" << std::endl;
                    m_player->cInput->right = true;
                    break;
                case sf::Keyboard::Escape:
                    std::cout << "ESC pressed" << std::endl;
                    m_running = false;
                default:
                    break;
            }
        }

        // this event is triggered when a key is released
        if (event.type == sf::Event::KeyReleased) {
            switch (event.key.code) {
                case sf::Keyboard::W:
                    std::cout << "W released" << std::endl;
                    m_player->cInput->up = false;
                    break;
                case sf::Keyboard::S:
                    std::cout << "S released" << std::endl;
                    m_player->cInput->down = false;
                    break;
                case sf::Keyboard::A:
                    std::cout << "A released" << std::endl;
                    m_player->cInput->left = false;
                    break;
                case sf::Keyboard::D:
                    std::cout << "D released" << std::endl;
                    m_player->cInput->right = false;
                    break;
                default:
                    break;
            }
        }

        // this event is triggered when a mouse button is pressed
        if (event.type == sf::Event::MouseButtonPressed) {
            if (event.mouseButton.button == sf::Mouse::Left) {
                std::cout << "Left mouse button pressed at (" << event.mouseButton.x << ", " << event.mouseButton.y << ")" << std::endl;
                // call spawnBullet here
                spawnBullet(m_player, Vec2(event.mouseButton.x, event.mouseButton.y));
            }
            if (event.mouseButton.button == sf::Mouse::Right) {
                std::cout << "Right mouse button pressed at (" << event.mouseButton.x << ", " << event.mouseButton.y << ")" << std::endl;
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
            }
            else {
                e->destroy();
            }
            std::cout << "bullet life remain: " << e->cLifespan->remaining << std::endl;
        }
    }
}

void Game::sRender() {
    // TODO: change the code below to draw all of the entities
    //       sample drawing of the player entity that we have created
    m_window.clear();


    for (auto& e : m_entities.getEntities()) {
        // set the position of the shape based on the entity's transform->pos
        //e->cShape->circle.setPosition(e->cTransform->pos.x, e->cTransform->pos.y);
        e->cShape->circle.setPosition(e->cTransform->pos.x, e->cTransform->pos.y);

        // set the position of the shape based on the entity's transform->angle
        //e->cTransform->angle += 1.0f;
        //e->cShape->circle.setRotation(e->cTransform->angle);

        // draw the entity's sf::CircleShape
        m_window.draw(e->cShape->circle);
    }

    m_player->cShape->circle.setPosition(m_player->cTransform->pos.x, m_player->cTransform->pos.y);
    m_player->cTransform->angle += 1.0f;
    m_player->cShape->circle.setRotation(m_player->cTransform->angle);
    m_window.draw(m_player->cShape->circle);


    m_window.draw(m_text);

    m_window.display();
}

void Game::sEnemySpawner() {
    return;
    // TODO: code which implements enemey spawning should go here
    //
    // (user m_currentFrame - m_lastEnemySpawnTime) to determine
    // how long it has veen since the last enemy was spawned

    //spawnEnemy();
}

void Game::sCollision() {
    return;
    // TODO: implement all proper collisions between entities
    //       be sure to use the collision radius, Not the shape radius

    for (auto b : m_entities.getEntities("bullet")) {
        for (auto e : m_entities.getEntities("enemy")) {
            // if (b->cTransform->pos.distance(e->cTransform->pos) < b->cShape->radius + e->cShape->radius) {
            //     // collision detected
            //     // remove the bullet and the enemy
            //     m_entities.removeEntity(b);
            //     m_entities.removeEntity(e);
            // }
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

    std::cout << "W /2: " << m_windowConfig.W / 2 << std::endl;
    std::cout << "H /2: " << m_windowConfig.H / 2 << std::endl;

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

    std::cout << "Shape Radius: " << m_playerConfig.SR << std::endl;
    std::cout << "Vertices: " << m_playerConfig.V << std::endl;
    std::cout << "Thickness: " << m_playerConfig.OT << std::endl;

    // add an input component to the player so that we can use inputs
    entity->cInput = std::make_shared<CInput>();

    // since we ant this entity to be our player, set our Game's player variable to be this entity
    // this goes slightly against the EntityManager paradigm, but we use the player so much it's worth it
    m_player = entity;
}

void Game::spawnEnemy() {
    auto entity = m_entities.addEntity("enemy");

    float ex = rand() % m_window.getSize().x;
    float ey = rand() % m_window.getSize().y;

    entity->cTransform = std::make_shared<CTransform>(Vec2(ex, ey), Vec2(1.0f, 1.0f), 0.0f);

    entity->cShape = std::make_shared<CShape>(16.0f, 3, sf::Color(0, 0, 255), sf::Color(255, 255, 255), 4.0f);
}

void Game::spawnSmallEnemies(std::shared_ptr<Entity> entity) {
    // TODO: spawn small enemies at the location of the input enemy e

    // when we create the smaller enemy, we have to read the values of the original enemy
    // - spawn a number of small enemies equal to the ;vertices of the original enemy
    // - set each small enemy to the same color as the original, half the size
    // - small enemies are worh double points of the original enemy
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

    std::cout << "distance: " << distance << std::endl;

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