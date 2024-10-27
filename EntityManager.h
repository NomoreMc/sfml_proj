#pragma once
#include "Entity.h"

typedef std::vector<std::shared_ptr<Entity>> EntityVec;
typedef std::map<std::string, EntityVec> EntityMap;

class EntityManager {
private:
	EntityVec m_entities;	    // ���浱ǰ��Ϸ�����е� entities
	EntityVec m_toAdd;			// �������б��е� entities��������һ֡�ż���
	EntityMap m_entityMap;		// ���� tag ����ÿ�� entities
	size_t m_totalEntities = 0;
public:
	EntityManager() {};

	void update();
	void removeDeadEntities(EntityVec &vec);
	std::shared_ptr<Entity> addEntity(const std::string& tag);
	const EntityVec& getEntities();
	const EntityVec& getEntities(const std::string& tag);
};