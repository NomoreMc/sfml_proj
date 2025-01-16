#include "Entity.h"
#include <string>

Entity::Entity(const size_t id, const std::string & tag) {
	this->m_id = id;
	this->m_tag = std::string(tag);
}

bool Entity::isActive() const {
	return this->m_active;
}

const std::string& Entity::tag() const {
	return this->m_tag;
}

const size_t Entity::id() const {
	return this->m_id;
}

void Entity::destroy() {
	this->m_active = false;
}