#include "EntityManager.h"
#include <algorithm>

/* 将新实体加入到现有队列中 */
void EntityManager::update() {
	// 将 toAdd 列表中新生成的 entity 加入到 entities 以及 entityMap 中
	for (auto e : m_toAdd) {
		m_entities.push_back(e);
		m_entityMap[e->tag()].push_back(e);
	}
}

/* 从 EntityVec 以及 EntityMap 中移除被 m_active = false 的实体 */
void EntityManager::removeDeadEntities(EntityVec& vec) {


	vec.erase(remove_if(vec.begin(), vec.end(), [](std::shared_ptr<Entity> e) {
		return !e->isActive();
	}), vec.end());
}

/* 添加标签为 tag 的实体 */
std::shared_ptr<Entity> EntityManager::addEntity(const std::string& tag) {
	auto e = std::shared_ptr<Entity>(new Entity(this->m_totalEntities++, tag));
	// 先加入 toAdd 列表中
	this->m_toAdd.push_back(e);
	return e;
}

/* 获取完整的实体列表 */
const EntityVec& EntityManager::getEntities() {
	return this->m_entities;
}

/* 根据 tag 获取对应的实体列表 */
const EntityVec& EntityManager::getEntities(const std::string& tag) {
	return this->m_entityMap[tag];
}