#pragma once
#include "Scene/Entity.h"
#include "Scene/Scene.h"

class EntityPropertiesPanel
{
public:
	EntityPropertiesPanel() = default;
	~EntityPropertiesPanel() = default;
	EntityPropertiesPanel operator=(const EntityPropertiesPanel & other) = delete;
	EntityPropertiesPanel(const EntityPropertiesPanel & other) = delete;
	EntityPropertiesPanel operator=(EntityPropertiesPanel && other) = delete;
	EntityPropertiesPanel(EntityPropertiesPanel && other) = delete;

public:
	std::shared_ptr<std::weak_ptr<Entity>> m_Selected_Entity;
	std::shared_ptr<Scene> m_Active_Scene = nullptr;

public:
	void OnImGuiRender();
};
