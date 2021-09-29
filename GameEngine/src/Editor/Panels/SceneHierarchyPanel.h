#pragma once

#include "Scene/Scene.h"

class SceneHierarchyPanel
{
public:
	SceneHierarchyPanel();
	~SceneHierarchyPanel() = default;
	SceneHierarchyPanel operator=(const SceneHierarchyPanel & other) = delete;
	SceneHierarchyPanel(const SceneHierarchyPanel & other) = delete;
	SceneHierarchyPanel operator=(SceneHierarchyPanel && other) = delete;
	SceneHierarchyPanel(SceneHierarchyPanel && other) = delete;

public:
	std::shared_ptr<Scene> m_Active_Scene = nullptr;
	std::shared_ptr<std::weak_ptr<Entity>> m_Selected_Entity;
	
public:
	void OnImGuiRender();

private:
	bool DrawEntityNode(const std::shared_ptr<Entity>& entity);
};