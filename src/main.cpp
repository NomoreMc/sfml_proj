#include <iostream>
#include <memory>
#include <string>

#include <SFML/Graphics.hpp>
//#include "Game.h"
#include "Vec2.h"
#include "Entity.h"
#include "EntityManager.h"

#include "Game.h"

void Vec2Test() {
    Vec2 v1(1, 1);
    Vec2 v2(2, 2);
}

void EntityManagerTest() {
    // entity tag
    std::string t = "enemy";
    // EntityManager instance
    std::shared_ptr<EntityManager> em = std::make_shared<EntityManager>();

    // 添加 10 个 entity 到 EntityManager
    for (size_t i = 0; i < 10; ++i) {
        em->addEntity(t);
    }

    // 执行 update 将 toAdd 列表中的实体加入到现有列表中
    em->update();

    // 获取 EntityManager 中的所有 entity
    auto entities = em->getEntities();

    // 输出 EntityManager 中的所有 entity 的 tag 和 active 状态
    for (size_t i = 0; i < 10; ++i) {
        std::cout << entities[i]->tag() << " " << entities[i]->isActive() << std::endl;
    }
}

int main()
{
    //EntityManagerTest();

    // Vec2 Test
    //Vec2Test();

    // entrance
    Game g("../../config.txt");
    g.run();

    return 0;
}