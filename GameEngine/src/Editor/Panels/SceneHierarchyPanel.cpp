#include "stdafx.h"
#include "SceneHierarchyPanel.h"

#include "imgui/imgui.h"

#include "Mesh/Mesh.h"
#include "Mesh/Primitives/StairsMesh.h"

#include "Math/Random.h"

SceneHierarchyPanel::SceneHierarchyPanel():
	m_Selected_Entity(std::make_shared<std::weak_ptr<Entity>>())
{
}

void SceneHierarchyPanel::OnImGuiRender()
{
	ImGui::Begin("Scene Hierarchy");

	for (auto it = m_Active_Scene->m_Registry.begin(); it != m_Active_Scene->m_Registry.end();)
	{
		const auto& entity = *it;
		//TEMP
		if (DrawEntityNode(entity)) {
			it = m_Active_Scene->m_Registry.erase(it);
			if (!m_Selected_Entity->expired())
				if (*(m_Selected_Entity->lock()) == *entity)
					m_Selected_Entity->reset();
		}
		else ++it;
	}

	if (ImGui::IsMouseDown(0) && ImGui::IsWindowHovered())
		m_Selected_Entity->reset();

	// Right-click on blank space
	if (ImGui::BeginPopupContextWindow(0, 1, false))
	{
		if (ImGui::MenuItem("Create Empty"))
		{
			m_Active_Scene->CreateEntity();
		}
		if (ImGui::MenuItem("Create Cube")) 
		{
			auto entity = m_Active_Scene->CreateEntity();
			entity->m_tag = "New Cube";
			entity->AddComponent<TransformComponent>();
			auto& rendererComponent = entity->AddComponent<RendererComponent>();
			rendererComponent.mesh = Mesh::Create(MeshPrimitives::CUBE);
			rendererComponent.shader = std::make_shared<Shader>(L"res/Shaders/BasicShader.hlsl");
		}
		if (ImGui::MenuItem("Create Plane"))
		{
			Entity entity;
			entity.m_tag = "New Plane";
			entity.AddComponent<TransformComponent>();
			auto& rendererComponent = entity.AddComponent<RendererComponent>();
			rendererComponent.mesh = Mesh::Create(MeshPrimitives::PLANE);
			rendererComponent.shader = std::make_shared<Shader>(L"res/Shaders/BasicShader.hlsl");
			m_Active_Scene->AddEntity(std::move(entity));
		}
		if (ImGui::MenuItem("Create Cylinder"))
		{
			Entity entity;
			entity.m_tag = "New Cylinder";
			entity.AddComponent<TransformComponent>();
			auto& rendererComponent = entity.AddComponent<RendererComponent>();
			rendererComponent.mesh = Mesh::Create(MeshPrimitives::CYLINDER);
			rendererComponent.shader = std::make_shared<Shader>(L"res/Shaders/BasicShader.hlsl");
			m_Active_Scene->AddEntity(std::move(entity));
		}
		if (ImGui::MenuItem("Create Capsule"))
		{
			Entity entity;
			entity.m_tag = "New Capsule";
			entity.AddComponent<TransformComponent>();
			auto& rendererComponent = entity.AddComponent<RendererComponent>();
			rendererComponent.mesh = Mesh::Create(MeshPrimitives::CAPSULE);
			rendererComponent.shader = std::make_shared<Shader>(L"res/Shaders/BasicShader.hlsl");
			m_Active_Scene->AddEntity(std::move(entity));
		}
		if (ImGui::MenuItem("Create Sphere"))
		{
			Entity entity;
			entity.m_tag = "New Sphere";
			entity.AddComponent<TransformComponent>();
			auto& rendererComponent = entity.AddComponent<RendererComponent>();
			rendererComponent.mesh = Mesh::Create(MeshPrimitives::SPHERE);
			rendererComponent.shader = std::make_shared<Shader>(L"res/Shaders/BasicShader.hlsl");
			m_Active_Scene->AddEntity(std::move(entity));
		}
		if (ImGui::MenuItem("Create Stairs"))
		{
			Entity entity;
			entity.m_tag = "New Stair";
			entity.AddComponent<TransformComponent>();
			auto& rendererComponent = entity.AddComponent<RendererComponent>();
			rendererComponent.mesh = StairsMesh::StartStairMesh();
			rendererComponent.shader = std::make_shared<Shader>(L"res/Shaders/BasicShader.hlsl");
			m_Active_Scene->AddEntity(std::move(entity));
		}
		if (ImGui::MenuItem("Ho Physic Cubes"))
		{
			for (int i = 0; i < 50; ++i)
			{	
				Entity entity;
				entity.m_tag = "Cube " + std::to_string(i);
				auto& transform = entity.AddComponent<TransformComponent>();
				transform.Translation = {Random::Float(-0.25f, 0.25f), Random::Float(-0.25f, 0.25f), Random::Float(-0.25f, 0.25f) };
				transform.Rotation = DirectX::XMQuaternionRotationRollPitchYaw(Random::Float(0.f, 0.785f), Random::Float(0.f, 0.785f), Random::Float(0.f, 0.785f));
				auto& rendererComponent = entity.AddComponent<RendererComponent>();
				rendererComponent.mesh = Mesh::Create(MeshPrimitives::CUBE);
				rendererComponent.shader = std::make_shared<Shader>(L"res/Shaders/BasicShader.hlsl");
				entity.AddComponent<BoxColliderComponent>(&entity);
				auto& rb = entity.AddComponent<RigidbodyComponent>(&entity, m_Active_Scene->GetPhysicsWorld());
				rb.body->setType(rp3d::BodyType::DYNAMIC);
				rb.body->enableGravity(true);
				m_Active_Scene->AddEntity(std::move(entity));
			}
		}

		ImGui::EndPopup();
	}

	ImGui::End();
}

bool SceneHierarchyPanel::DrawEntityNode(const std::shared_ptr<Entity>& entity)
{
	auto& tag = entity->m_tag;

	ImGuiTreeNodeFlags flags = ImGuiTreeNodeFlags_OpenOnArrow;
	if (!m_Selected_Entity->expired()) flags |= ((*(m_Selected_Entity->lock()) == *entity) ? ImGuiTreeNodeFlags_Selected : 0);
	flags |= ImGuiTreeNodeFlags_SpanAvailWidth;
	bool opened = ImGui::TreeNodeEx((void*)(uint64_t)(*entity.get()), flags, tag.c_str());
	if (ImGui::IsItemClicked())
	{
		*m_Selected_Entity = entity;
	}

	bool entityDeleted = false;
	if (ImGui::BeginPopupContextItem())
	{
		if (ImGui::MenuItem("Delete Entity"))
			entityDeleted = true;

		ImGui::EndPopup();
	}

	if (opened)
	{
		ImGuiTreeNodeFlags flags = ImGuiTreeNodeFlags_OpenOnArrow | ImGuiTreeNodeFlags_SpanAvailWidth;
		bool opened = ImGui::TreeNodeEx((void*)9817239, flags, tag.c_str());
		if (opened)
			ImGui::TreePop();
		ImGui::TreePop();
	}

	return entityDeleted;
}
