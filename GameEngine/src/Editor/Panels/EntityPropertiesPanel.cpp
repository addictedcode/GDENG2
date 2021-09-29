#include "stdafx.h"
#include "EntityPropertiesPanel.h"

#include "imgui/imgui.h"
#include "imgui/imgui_internal.h"

#include "Mesh/MeshTools.h"

#include "Math/Math.h"
#include "Files/FileDialogs.h"

static void DrawVec3Control(const std::string& label, DirectX::XMVECTOR& vector, float resetValue = 0.0f, float columnWidth = 100.0f)
{
	ImGuiIO& io = ImGui::GetIO();
	auto boldFont = io.Fonts->Fonts[0];
	
	ImGui::PushID(label.c_str());

	ImGui::Columns(2);
	ImGui::SetColumnWidth(0, columnWidth);
	ImGui::Text(label.c_str());
	ImGui::NextColumn();

	ImGui::PushMultiItemsWidths(3, ImGui::CalcItemWidth());
	ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2{ 0, 0 });

	float lineHeight = GImGui->Font->FontSize + GImGui->Style.FramePadding.y * 2.0f;
	ImVec2 buttonSize = { lineHeight + 3.0f, lineHeight };

	ImGui::PushStyleColor(ImGuiCol_Button, ImVec4{ 0.8f, 0.1f, 0.15f, 1.0f });
	ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4{ 0.9f, 0.2f, 0.2f, 1.0f });
	ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImVec4{ 0.8f, 0.1f, 0.15f, 1.0f });
	ImGui::PushFont(boldFont);
	if (ImGui::Button("X", buttonSize))
		vector.m128_f32[0] = resetValue;
	ImGui::PopFont();
	ImGui::PopStyleColor(3);

	ImGui::SameLine();
	ImGui::DragFloat("##X", &(vector.m128_f32[0]), 0.1f, 0.0f, 0.0f, "%.2f");
	ImGui::PopItemWidth();
	ImGui::SameLine();

	ImGui::PushStyleColor(ImGuiCol_Button, ImVec4{ 0.2f, 0.7f, 0.2f, 1.0f });
	ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4{ 0.3f, 0.8f, 0.3f, 1.0f });
	ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImVec4{ 0.2f, 0.7f, 0.2f, 1.0f });
	ImGui::PushFont(boldFont);
	if (ImGui::Button("Y", buttonSize))
		vector.m128_f32[1] = resetValue;
	ImGui::PopFont();
	ImGui::PopStyleColor(3);

	ImGui::SameLine();
	ImGui::DragFloat("##Y", &(vector.m128_f32[1]), 0.1f, 0.0f, 0.0f, "%.2f");
	ImGui::PopItemWidth();
	ImGui::SameLine();

	ImGui::PushStyleColor(ImGuiCol_Button, ImVec4{ 0.1f, 0.25f, 0.8f, 1.0f });
	ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4{ 0.2f, 0.35f, 0.9f, 1.0f });
	ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImVec4{ 0.1f, 0.25f, 0.8f, 1.0f });
	ImGui::PushFont(boldFont);
	if (ImGui::Button("Z", buttonSize))
		vector.m128_f32[2] = resetValue;
	ImGui::PopFont();
	ImGui::PopStyleColor(3);

	ImGui::SameLine();
	ImGui::DragFloat("##Z", &(vector.m128_f32[2]), 0.1f, 0.0f, 0.0f, "%.2f");
	ImGui::PopItemWidth();

	ImGui::PopStyleVar();

	ImGui::Columns(1);

	ImGui::PopID();
}

template<typename T, typename UIFunction>
static void DrawComponent(const std::string& name, std::shared_ptr<Entity>& entity, UIFunction uiFunction)
{
	const ImGuiTreeNodeFlags treeNodeFlags = ImGuiTreeNodeFlags_DefaultOpen | ImGuiTreeNodeFlags_Framed | ImGuiTreeNodeFlags_SpanAvailWidth | ImGuiTreeNodeFlags_AllowItemOverlap | ImGuiTreeNodeFlags_FramePadding;
	if (entity->HasComponent<T>())
	{
		auto& component = entity->GetComponent<T>();
		ImVec2 contentRegionAvailable = ImGui::GetContentRegionAvail();

		ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2{ 4, 4 });
		float lineHeight = GImGui->Font->FontSize + GImGui->Style.FramePadding.y * 2.0f;
		ImGui::Separator();
		bool open = ImGui::TreeNodeEx((void*)typeid(T).hash_code(), treeNodeFlags, name.c_str());
		ImGui::PopStyleVar(
		);
		ImGui::SameLine(contentRegionAvailable.x - lineHeight);
		if (ImGui::Button("+", ImVec2{ lineHeight, lineHeight }))
		{
			ImGui::OpenPopup("ComponentSettings");
		}

		bool removeComponent = false;
		if (ImGui::BeginPopup("ComponentSettings"))
		{
			if (ImGui::MenuItem("Remove component"))
				removeComponent = true;

			ImGui::EndPopup();
		}

		if (open)
		{
			uiFunction(component);
			ImGui::TreePop();
		}

		if (removeComponent)
			entity->RemoveComponent<T>();
	}
}

void EntityPropertiesPanel::OnImGuiRender()
{
	ImGui::Begin("Properties");
	if (!m_Selected_Entity->expired())
	{
		std::shared_ptr<Entity> entity = m_Selected_Entity->lock();
		//const ImGuiTreeNodeFlags treeNodeFlags = ImGuiTreeNodeFlags_DefaultOpen | ImGuiTreeNodeFlags_Framed | ImGuiTreeNodeFlags_SpanAvailWidth | ImGuiTreeNodeFlags_AllowItemOverlap | ImGuiTreeNodeFlags_FramePadding;

		std::string tag = entity->m_tag;
		char buffer[256];
		memset(buffer, 0, sizeof(buffer));
		strncpy_s(buffer, tag.c_str(), sizeof(buffer));
		if (ImGui::InputText("##Tag", buffer, sizeof(buffer)))
		{
			tag = std::string(buffer);
			entity->m_tag = tag;
		}

		ImGui::SameLine();
		ImGui::PushItemWidth(-1);

		if (ImGui::Button("Add Component"))
			ImGui::OpenPopup("AddComponent");

		if (ImGui::BeginPopup("AddComponent"))
		{
			if (ImGui::MenuItem("Transform"))
			{
				if (!entity->HasComponent<TransformComponent>())
					entity->AddComponent<TransformComponent>();
				ImGui::CloseCurrentPopup();
			}

			if (ImGui::MenuItem("Renderer"))
			{
				if (!entity->HasComponent<RendererComponent>())
					entity->AddComponent<RendererComponent>();
				ImGui::CloseCurrentPopup();
			}

			if (ImGui::MenuItem("Box Collider"))
			{
				if (!entity->HasComponent<BoxColliderComponent>())
					entity->AddComponent<BoxColliderComponent>(entity.get());
				ImGui::CloseCurrentPopup();
			}

			if (ImGui::MenuItem("Rigidbody"))
			{
				if (!entity->HasComponent<RigidbodyComponent>())
					entity->AddComponent<RigidbodyComponent>(entity.get(), m_Active_Scene->GetPhysicsWorld());
				ImGui::CloseCurrentPopup();
			}

			ImGui::EndPopup();
		}

		ImGui::PopItemWidth();

		DrawComponent<TransformComponent>("Transform", entity, [](auto& component) {
				DrawVec3Control("Translation", component.Translation);
				//TO FIX: QUATERNION TO EULER
				DirectX::XMVECTOR euler = Math::QuaternionToEuler(component.Rotation);
				DrawVec3Control("Rotation", euler);
				/*DirectX::XMVECTOR delta_rot = DirectX::XMQuaternionMultiply(entity->m_rotation,
					DirectX::XMQuaternionInverse(DirectX::XMQuaternionRotationRollPitchYawFromVector(euler)));
				entity->m_rotation = DirectX::XMQuaternionMultiply(entity->m_rotation, delta_rot);*/
				//entity->m_rotation = DirectX::XMQuaternionRotationRollPitchYawFromVector(euler);
				//entity->m_rotation = DirectX::XMQuaternionRotationMatrix(DirectX::XMMatrixRotationRollPitchYawFromVector(euler));
				DrawVec3Control("Scale", component.Scale, 1.0f);
			});

		DrawComponent<RendererComponent>("Renderer", entity, [](auto& component)
			{
				if (ImGui::Button("Change Shader"))
				{
					std::string filepath = FileDialogs::OpenFile("Shaders (*.hlsl)\0*.hlsl\0");
					std::cout << filepath << std::endl;
					if (!filepath.empty())
					{
						component.shader = std::make_shared<Shader>(std::wstring(filepath.begin(), filepath.end()).c_str());
					}
				}
				if (ImGui::Button("Change Texture"))
				{
					std::string filepath = FileDialogs::OpenFile("Image Files (*.png.jpg)\0*.png;*.jpg\0");
					if (!filepath.empty())
					{
						component.texture = std::make_shared<Texture>(filepath.c_str());
					}
				}
				if (ImGui::Button("Change Mesh"))
				{
					std::string filepath = FileDialogs::OpenFile("Meshes (*.obj)\0*.obj\0");
					if (!filepath.empty())
					{
						auto meshes = MeshTools::LoadObjFile(filepath);
						component.mesh = meshes[0];
					}
				}
			});

		DrawComponent<BoxColliderComponent>("Box Collider", entity, [](auto& component)
			{
				DirectX::XMVECTOR translation = {component.position.x, component.position.y, component.position.z };
				DrawVec3Control("Translation", translation);
				component.position.x = translation.m128_f32[0];
				component.position.y = translation.m128_f32[1];
				component.position.z = translation.m128_f32[2];
				if (!component.rigidbody.expired())
					component.collider->setLocalToBodyTransform(rp3d::Transform(component.position, rp3d::Quaternion::identity()));

				DirectX::XMVECTOR size = { component.size.x, component.size.y, component.size.z };
				DrawVec3Control("Size", size);
				component.size.x = size.m128_f32[0];
				component.size.y = size.m128_f32[1];
				component.size.z = size.m128_f32[2];
				if (component.GetEntity().HasComponent<TransformComponent>()) {
					auto& tc = component.GetEntity().GetComponent<TransformComponent>();
					component.shape->setHalfExtents(rp3d::Vector3(tc.Scale.m128_f32[0] * component.size.x / 2.f, 
						tc.Scale.m128_f32[1] * component.size.y / 2.f, 
						tc.Scale.m128_f32[2] * component.size.z / 2.f));
				}
				else
					component.shape->setHalfExtents(component.size / 2.f);
			});

		DrawComponent<RigidbodyComponent>("Rigidbody", entity, [](auto& component)
			{
				const char* items[] = { "Static", "Kinematic", "Dynamic" };
				const char* combo_preview_value = items[(int)component.body->getType()];
				ImGui::SetNextItemWidth(85);
				if (ImGui::BeginCombo("Body Type", combo_preview_value, ImGuiComboFlags_NoArrowButton))
				{
					for (int n = 0; n < IM_ARRAYSIZE(items); n++)
					{
						const bool is_selected = ((int)component.body->getType() == n);
						if (ImGui::Selectable(items[n], is_selected))
							component.body->setType((rp3d::BodyType)n);

						// Set the initial focus when opening the combo (scrolling + keyboard navigation focus)
						if (is_selected)
							ImGui::SetItemDefaultFocus();
					}
					ImGui::EndCombo();
				}

				bool hasGravity = component.body->isGravityEnabled();
				ImGui::Checkbox("Gravity", &hasGravity);
				component.body->enableGravity(hasGravity);
			});
	}

	ImGui::End();
}
