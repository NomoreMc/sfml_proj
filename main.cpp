#include <iostream>
#include <memory>
#include <string>

#include <SFML/Graphics.hpp>
//#include "Game.h"
#include "Vec2.h"
#include "Entity.h"
#include "EntityManager.h"

void Vec2Test() {
    Vec2 v1(1, 1);
    Vec2 v2(2, 2);
}

void EntityManagerTest() {
    // entity tag
    std::string t = "enemy";
    // EntityManager instance
    std::shared_ptr<EntityManager> em = std::make_shared<EntityManager>();

    // 生成新的类型为 t 的 entity
    for (size_t i = 0; i < 10; ++i) {
        em->addEntity(t);
    }

    // 调用 update 将新生成的 entity 加入到队列中
    em->update();

    // 获取 entities 列表
    auto entities = em->getEntities();

    // 输出列表中的 entity 实例状态
    for (size_t i = 0; i < 10; ++i) {
        std::cout << entities[i]->tag() << " " << entities[i]->isActive() << std::endl;
    }
}

int main()
{
    EntityManagerTest();

    {
        sf::RenderWindow window(sf::VideoMode(200, 200), "SFML works!");
        sf::CircleShape shape(100.f);
        shape.setFillColor(sf::Color::Green);

        while (window.isOpen())
        {
            sf::Event event;
            while (window.pollEvent(event))
            {
                if (event.type == sf::Event::Closed)
                    window.close();
            }

            window.clear();
            window.draw(shape);
            window.display();
        }
    }

    return 0;

    // entrance
    //Game g("config.txt");
    //g.run();

    // Vec2 Test
    //Vec2Test();

    //return 0;
}