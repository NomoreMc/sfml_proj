#pragma once
#include <memory>
#include <string>

#include "Components.h"

//typedef std::vector<Entity> EntityVec;

class Entity {
private:
	friend class EntityManager;

	bool m_active = true;
	size_t m_id = 0;
	std::string m_tag = "default";

	// 构造函数为私有，仅能通过友元来调用
	Entity(const size_t id, const std::string& tag);

public:
	// components pointer
	std::shared_ptr<CTransform> cTransform;
	std::shared_ptr<CShape> cShape;
	std::shared_ptr<CCollision> cCollision;
	std::shared_ptr<CInput> cInput;
	std::shared_ptr<CScore> cScore;
	std::shared_ptr<CLifespan> cLifespan;

	// private member access functions
	bool isActive() const;
	const std::string& tag() const;
	const size_t id() const;
	void destroy();
};