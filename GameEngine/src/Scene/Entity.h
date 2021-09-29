#pragma once
#include "Component.h"

class Entity
{
public:
	Entity();
	Entity(const uint32_t& id, const std::string& tag);
	~Entity() = default;
	Entity operator=(const Entity& other) = delete;
	Entity(const Entity& other) = delete;
	Entity& operator=(Entity && other) noexcept
	{
		if (this != &other)
		{
			m_id = other.m_id;
			m_tag = std::move(other.m_tag);
			m_components = std::move(other.m_components);
			for (auto& comp : m_components)
				comp.second->entity = this;
			
			other.m_id = 0;
		}
		return *this;
	}
	Entity(Entity&& other) noexcept
	{
		*this = std::move(other);
	}

	operator uint32_t() const { return m_id; }
	bool operator==(const Entity& other) const { return m_id == other.m_id; }
	bool operator!=(const Entity& other) const { return m_id != other.m_id; }
	bool operator==(const uint32_t& id) const { return m_id == id; }
	bool operator!=(const uint32_t& id) const { return m_id != id; }
private:
	uint32_t m_id;

	std::unordered_map<const char*, std::unique_ptr<Component>> m_components;
	
public:
	std::string m_tag;

public:
	uint32_t GetID() const { return m_id; }
	
	template<typename component, typename... Args>
	component& AddComponent(Args&&... args)
	{
		assert(!HasComponent<component>());
		const char* name = typeid(component).raw_name();
		m_components[name] = std::make_unique<component>(std::forward<Args>(args)...);
		m_components[name]->entity = this;
		return *(component*)m_components[name].get();
	}

	template<typename component>
	void RemoveComponent()
	{
		assert(HasComponent<component>());
		m_components.erase(typeid(component).raw_name());
	}

	template<typename component>
	component& GetComponent()
	{
		assert(HasComponent<component>());
		return *(component*)m_components[typeid(component).raw_name()].get();
	}
	
	template<typename component>
	bool HasComponent()
	{
		static_assert(std::is_base_of<Component, component>::value);
		return m_components.find(typeid(component).raw_name()) != m_components.end();
	}

private:
	static uint32_t GenerateID();

	friend class SceneHierarchyPanel;
	friend class EntityInspectorPanel;
};