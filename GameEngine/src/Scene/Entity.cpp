#include "stdafx.h"
#include "Entity.h"

Entity::Entity() :
	m_id(GenerateID()),
	m_tag("Entity")
{
}

//id temp, dont use from file
Entity::Entity(const uint32_t& id, const std::string& tag):
	m_id(GenerateID()),
	m_tag(tag)
{
}

uint32_t Entity::GenerateID()
{
	static uint32_t id_counter = 0;
	id_counter++;
	return id_counter;
}
