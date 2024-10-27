#pragma once
#include "Entity.h"

typedef std::vector<std::shared_ptr<Entity>> EntityVec;
typedef std::map<std::string, EntityVec> EntityMap;

class EntityManager {
private:
	EntityVec m_entities;	    // 保存当前游戏中所有的 entities
	EntityVec m_toAdd;			// 待加入列表中的 entities，将在下一帧才加入
	EntityMap m_entityMap;		// 根据 tag 保存每类 entities
	size_t m_totalEntities = 0;
public:
	EntityManager() {};

	void update();
	void removeDeadEntities(EntityVec &vec);
	std::shared_ptr<Entity> addEntity(const std::string& tag);
	const EntityVec& getEntities();
	const EntityVec& getEntities(const std::string& tag);
};