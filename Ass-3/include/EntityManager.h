#pragma once
#include "Entity.h"

typedef std::vector<std::shared_ptr<Entity>> EntityVec;
typedef std::map<std::string, EntityVec> EntityMap;

class EntityManager {
private:
	EntityVec m_entities;	    // 总的 entities 列表
	EntityVec m_toAdd;			// 待添加的 entities 列表
	EntityMap m_entityMap;		// 按 tag 分类的 entities 列表
	size_t m_totalEntities = 0;
public:
	EntityManager() {};

	void update();
	void removeDeadEntities(EntityVec &vec);
	std::shared_ptr<Entity> addEntity(const std::string& tag);
	const EntityVec& getEntities();
	const EntityVec& getEntities(const std::string& tag);
};