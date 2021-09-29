#pragma once
#include "Core/Layer.h"

#include "Scene/Scene.h"
#include "Scene/SceneViewport.h"
#include "Panels/SceneHierarchyPanel.h"
#include "Panels/EntityPropertiesPanel.h"

class EditorLayer : public Layer
{
public:
	EditorLayer() = default;
	virtual ~EditorLayer() override = default;
	EditorLayer operator=(const EditorLayer & other) = delete;
	EditorLayer(const EditorLayer & other) = delete;
	EditorLayer operator=(EditorLayer && other) = delete;
	EditorLayer(EditorLayer && other) = delete;

private:
	std::shared_ptr<Scene> m_Active_Scene;
	
	SceneViewport m_scene_viewport;
	SceneHierarchyPanel m_Scene_Hierarchy_Panel;
	EntityPropertiesPanel m_Entity_Properties_Panel;

	enum class SceneState
	{
		Play, Edit, Pause, NextFrame
	};

	SceneState m_SceneState = SceneState::Edit;
	
public:
	void OnAttach() override;
	void OnUpdate(float delta_time) override;
	void OnFixedUpdate(float timestep) override;
	void OnImGuiRender() override;

private:
	void NewScene();

};