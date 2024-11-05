#include "EntityManager.h"
#include <algorithm>

/* ����ʵ����뵽���ж����� */
void EntityManager::update() {
	// �� toAdd �б��������ɵ� entity ���뵽 entities �Լ� entityMap ��
	for (auto e : m_toAdd) {
		m_entities.push_back(e);
		m_entityMap[e->tag()].push_back(e);
	}
}

/* �� EntityVec �Լ� EntityMap ���Ƴ��� m_active = false ��ʵ�� */
void EntityManager::removeDeadEntities(EntityVec& vec) {


	vec.erase(remove_if(vec.begin(), vec.end(), [](std::shared_ptr<Entity> e) {
		return !e->isActive();
	}), vec.end());
}

/* ��ӱ�ǩΪ tag ��ʵ�� */
std::shared_ptr<Entity> EntityManager::addEntity(const std::string& tag) {
	auto e = std::shared_ptr<Entity>(new Entity(this->m_totalEntities++, tag));
	// �ȼ��� toAdd �б���
	this->m_toAdd.push_back(e);
	return e;
}

/* ��ȡ������ʵ���б� */
const EntityVec& EntityManager::getEntities() {
	return this->m_entities;
}

/* ���� tag ��ȡ��Ӧ��ʵ���б� */
const EntityVec& EntityManager::getEntities(const std::string& tag) {
	return this->m_entityMap[tag];
}