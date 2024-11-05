#include "EntityManager.h"
#include <algorithm>

/* 将 toAdd 列表中的实体加入到现有列表中 */
void EntityManager::update() {
	// 根据 tag 将 toAdd 列表中的实体加入到 entityMap[tag]、entities 列表中
	for (auto e : m_toAdd) {
		m_entities.push_back(e);
		m_entityMap[e->tag()].push_back(e);
	}
	m_toAdd.clear();
}

/* 移除 active 为 false 的实体 */
void EntityManager::removeDeadEntities(EntityVec& vec) {
	// 移除 active 为 false 的实体
	vec.erase(remove_if(vec.begin(), vec.end(), [](std::shared_ptr<Entity> e) {
		return !e->isActive();
	}), vec.end());
}

/* 添加标签为 tag 的实体 */
std::shared_ptr<Entity> EntityManager::addEntity(const std::string& tag) {
	auto e = std::shared_ptr<Entity>(new Entity(this->m_totalEntities++, tag));
	if (e == nullptr) {
		return nullptr;
	}
	// 将实体加入到 toAdd 列表中，在下一次 update 时加入到实体列表中
	this->m_toAdd.push_back(e);
	return e;
}

/* 获取总 entities 列表 */
const EntityVec& EntityManager::getEntities() {
	return this->m_entities;
}

/* 根据 tag 获取 entities 列表 */
const EntityVec& EntityManager::getEntities(const std::string& tag) {
	return this->m_entityMap[tag];
}