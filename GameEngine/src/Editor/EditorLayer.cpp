#include "stdafx.h"
#include "EditorLayer.h"

#include "imgui/imgui.h"

#include "Mesh/Primitives/StairsMesh.h"

#include "Rendering/Shader.h"

#include "Scene/SceneSerializer.h"
#include "Files/FileDialogs.h"

void EditorLayer::OnAttach()
{
	NewScene();

	const std::shared_ptr<Mesh> mesh = Mesh::Create(MeshPrimitives::CUBE);
	auto entity = m_Active_Scene->CreateEntity();
	entity->m_tag = "Default Cube";
	entity->AddComponent<TransformComponent>();
	auto& rendererComponent = entity->AddComponent<RendererComponent>();
	rendererComponent.mesh = mesh;
	rendererComponent.shader = std::make_shared<Shader>(L"res/Shaders/BasicShader.hlsl");
}

void EditorLayer::OnUpdate(float delta_time)
{
	m_scene_viewport.BeginScene();
	m_Active_Scene->OnUpdate(delta_time);
	m_scene_viewport.EndScene();
}

void EditorLayer::OnFixedUpdate(float timestep)
{
	if (m_SceneState == SceneState::Play || m_SceneState == SceneState::NextFrame)
	{
		m_Active_Scene->OnFixedUpdate(timestep);
		if (m_SceneState == SceneState::NextFrame)
			m_SceneState = SceneState::Pause;
	}
	else
		m_Active_Scene->OnFixedUpdate(0.f);
}


void EditorLayer::OnImGuiRender()
{
	ImGui::DockSpaceOverViewport();
	
	//ImGui::ShowDemoWindow();

	m_scene_viewport.OnImGuiRender();
	
	m_Scene_Hierarchy_Panel.OnImGuiRender();

	m_Entity_Properties_Panel.OnImGuiRender();

	StairsMesh::StairMeshPanel();

	ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0, 2));
	
	ImGui::Begin("##Scene State", nullptr, ImGuiWindowFlags_NoDecoration | ImGuiWindowFlags_NoScrollWithMouse | ImGuiWindowFlags_NoScrollbar);

	ImGui::SetCursorPosX(ImGui::GetWindowContentRegionMax().x * 0.5f - 24.f);
	ImGui::SetCursorPosY(ImGui::GetWindowContentRegionMax().y * 0.5f - 10.f);
	
	if(m_SceneState == SceneState::Play || m_SceneState == SceneState::Pause || m_SceneState == SceneState::NextFrame)
	{
		if (ImGui::Button("Stop", ImVec2(48.f, 20.f)))
		{
			m_SceneState = SceneState::Edit;
			NewScene();
			SceneSerializer serializer(m_Active_Scene);
			serializer.Deserialize("res/Scenes/play.scene");
		}
		if (m_SceneState != SceneState::Pause && m_SceneState != SceneState::NextFrame)
		{
			ImGui::SameLine();
			if (ImGui::Button("Pause", ImVec2(48.f, 20.f))) 
				m_SceneState = SceneState::Pause;
		}
		else
		{
			ImGui::SameLine();
			if (ImGui::Button("Unpause"))
				m_SceneState = SceneState::Play;
			ImGui::SameLine();
			if (ImGui::Button("Next Frame"))
				m_SceneState = SceneState::NextFrame;
		}
	}
	else
	{
		if (ImGui::Button("Play", ImVec2(48.f, 20.f)))
		{
			m_SceneState = SceneState::Play;
			SceneSerializer serializer(m_Active_Scene);
			serializer.Serialize("res/Scenes/play.scene");
		}
	}
	
	ImGui::End();

	ImGui::PopStyleVar();
	
	if (ImGui::BeginMainMenuBar())
	{
		if (ImGui::BeginMenu("File"))
		{
			if (ImGui::MenuItem("New Scene"))
			{
				NewScene();

				const std::shared_ptr<Mesh> mesh = Mesh::Create(MeshPrimitives::CUBE);
				auto entity = m_Active_Scene->CreateEntity();
				entity->m_tag = "Default Cube";
				entity->AddComponent<TransformComponent>();
				auto& rendererComponent = entity->AddComponent<RendererComponent>();
				rendererComponent.mesh = mesh;
				rendererComponent.shader = std::make_shared<Shader>(L"res/Shaders/BasicShader.hlsl");
			}
			if (ImGui::MenuItem("Open Scene"))
			{
				auto filepath = FileDialogs::OpenFile("Scenes (*.scene)\0*.scene\0");
				if (!filepath.empty())
				{
					NewScene();
					SceneSerializer serializer(m_Active_Scene);
					serializer.Deserialize(filepath);
				}
			}
			if (ImGui::MenuItem("Save Scene"))
			{
				auto filepath = FileDialogs::SaveFile("Scenes (*.scene)\0*.scene\0");
				SceneSerializer serializer(m_Active_Scene);
				serializer.Serialize(filepath);
			}
			ImGui::EndMenu();
		}
		ImGui::EndMainMenuBar();
	}
	
	/*static bool isCreditScreenOpen = false;
	static bool isColorPickerOpen = false;
	if (ImGui::BeginMainMenuBar())
	{
		if (ImGui::BeginMenu("About"))
		{
			if (ImGui::MenuItem("Credit"))
			{
				isCreditScreenOpen = true;
			}
			ImGui::EndMenu();
		}
		if (ImGui::BeginMenu("ColorPicker"))
		{
			if (ImGui::MenuItem("ColorPicker"))
			{
				isColorPickerOpen = true;
			}
			ImGui::EndMenu();
		}
		ImGui::EndMainMenuBar();
	}

	if (isCreditScreenOpen) {
		ImGui::Begin("Credit Screen", &isCreditScreenOpen, ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_AlwaysAutoResize);

		static Texture logo("res/Textures/dlsu_logo.png");
		ImGui::Image(*logo, ImVec2((float)logo.GetWidth(), (float)logo.GetHeight()));
		
		ImGui::Text("About");
		ImGui::Text("Developed by : Richmond Ngo");
		ImGui::Text("Acknowledgement:");
		ImGui::Text("PardCode Game Engine Tutorial Series");
		ImGui::Text("https://www.youtube.com/playlist?list=PLv8DnRaQOs5-MR-zbP1QUdq5FL0FWqVzg");
		ImGui::Text("The Cherno Game Engine Tutorial Series");
		ImGui::Text("https://www.youtube.com/playlist?list=PLlrATfBNZ98dC-V-N3m0Go4deliWHPFwT");
		ImGui::Text("Sir Neil's Teachings");

		ImGui::NewLine();
		ImGui::SetCursorPosX((ImGui::GetWindowSize().x - 50) * 0.5f);
		if (ImGui::Button("Close", ImVec2(50, 25))) isCreditScreenOpen = false;

		ImGui::End();
	}

	if (isColorPickerOpen)
	{
		ImGui::Begin("Color Picker", &isColorPickerOpen, ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_AlwaysAutoResize);

		static float color[4] = { 0.f };
		ImGui::ColorPicker4("##ColorPicker", color);
		
		ImGui::End();
	}*/
}

void EditorLayer::NewScene()
{
	m_Active_Scene = std::make_shared<Scene>();

	m_scene_viewport.m_Active_Camera = m_Active_Scene->GetSceneCamera();
	m_scene_viewport.m_Selected_Entity = m_Scene_Hierarchy_Panel.m_Selected_Entity;
	m_Entity_Properties_Panel.m_Selected_Entity = m_Scene_Hierarchy_Panel.m_Selected_Entity;

	m_Scene_Hierarchy_Panel.m_Active_Scene = m_Active_Scene;
	m_scene_viewport.m_Active_Scene = m_Active_Scene;
	m_Entity_Properties_Panel.m_Active_Scene = m_Active_Scene;

	m_scene_viewport.m_camera_need_update = true;
}
