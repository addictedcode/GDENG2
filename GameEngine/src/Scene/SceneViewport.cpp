#include "stdafx.h"
#include "SceneViewport.h"


#include "Core/Input.h"
#include "imgui/imgui.h"
#include "ImGuizmo/ImGuizmo.h"

#include "Rendering/Renderer.h"

#include "Editing/EditMousePicking.h"

SceneViewport::SceneViewport():
	m_Active_Camera(nullptr),
	m_Active_Scene(nullptr),
	m_Selected_Entity(nullptr),
	m_frame_buffer(nullptr)
{
}

void SceneViewport::Init(const uint32_t width, const uint32_t height)
{
	m_frame_buffer = std::make_unique<FrameBuffer>(2, width, height);
	float color[] = { 0.2f, 0.1f, 0.2f, 1.0f };
	m_frame_buffer->AddBuffer(DXGI_FORMAT_B8G8R8A8_UNORM, D3D11_USAGE_DEFAULT, D3D11_BIND_SHADER_RESOURCE | D3D11_BIND_RENDER_TARGET, color);
	m_frame_buffer->AddBuffer(DXGI_FORMAT_R32_UINT);
	
	m_frame_buffer->CreateRenderTargetViews();
	m_frame_buffer->CreateDepthStencilView();
	CreateShaderResourceView();
	
	m_Active_Camera->UpdateRatio((float)width / (float)height);

	EditMousePicking::Initialize(width, height);
}

void SceneViewport::BeginScene()
{
	if (m_frame_buffer)
		m_frame_buffer->Begin();
}

void SceneViewport::EndScene()
{
	if (m_frame_buffer)
		m_frame_buffer->End();
}

enum class SceneMode
{
	Object, Edit
};

void SceneViewport::OnImGuiRender()
{
	static float t_snap = 0.5f;
	static float r_snap = 15.f;
	static float s_snap = 0.5f;
	ImGui::Begin("Snapping", nullptr, ImGuiWindowFlags_AlwaysAutoResize | ImGuiWindowFlags_NoResize);

	ImGui::DragFloat("Translate", &t_snap, 0.05f, 0.f, 64.f, "%.2f");
	ImGui::DragFloat("Rotate", &r_snap, 5.f, 0.f, 180.f, "%.0f");
	ImGui::DragFloat("Scale", &s_snap, 0.05f, 0.f, 64.f, "%.2f");
	
	ImGui::End();
	
	static SceneMode scene_mode = SceneMode::Object;
	static uint32_t sel_face_index = 0;

	if (Input::IsKeyDown(9)) {
		if (scene_mode == SceneMode::Object) scene_mode = SceneMode::Edit;
		else {
			scene_mode = SceneMode::Object;
			sel_face_index = 0;
		}
	}
	
	m_Active_Camera->OnImGuiRender();
	
	ImGui::Begin("Scene", nullptr, ImGuiWindowFlags_NoCollapse);

	//UI for Scene mode
	ImGui::SetCursorPos(ImVec2(ImGui::GetWindowContentRegionMin().x + 15.f, ImGui::GetWindowContentRegionMin().y + 15.f));
	ImGui::BeginChild("SceneMode", ImVec2(85, 20));
	const char* items[] = { "Object Mode", "Edit Mode" };
	const char* combo_preview_value = items[(int)scene_mode];
	ImGui::SetNextItemWidth(85);
	if (ImGui::BeginCombo("", combo_preview_value, ImGuiComboFlags_NoArrowButton))
	{
		for (int n = 0; n < IM_ARRAYSIZE(items); n++)
		{
			const bool is_selected = ((int)scene_mode == n);
			if (ImGui::Selectable(items[n], is_selected))
				scene_mode = (SceneMode)n;

			// Set the initial focus when opening the combo (scrolling + keyboard navigation focus)
			if (is_selected)
				ImGui::SetItemDefaultFocus();
		}
		ImGui::EndCombo();
	}
	ImGui::EndChild();
	
	ImGuiStyle& style = ImGui::GetStyle();
	style.WindowPadding = { 0.0f, 0.0f };

	ImGui::SetCursorPos(ImGui::GetWindowContentRegionMin());
	static ImVec2 window_size = ImGui::GetContentRegionAvail();

	//Initialize DirectX Resources
	static bool m_first_time = true;
	if (m_first_time) {
		Init((uint32_t)window_size.x, (uint32_t)window_size.y);
		m_first_time = false;
	}

	//Check if the window has changed sizes
	const ImVec2 new_window_size = ImGui::GetContentRegionAvail();
	if (new_window_size.x != window_size.x || new_window_size.y != window_size.y)
	{
		window_size = new_window_size;
		OnWindowResize((uint32_t)window_size.x, (uint32_t)window_size.y);
	}

	if (m_camera_need_update)
	{
		m_Active_Camera->UpdateRatio(window_size.x / window_size.y);
		m_camera_need_update = false;
	}

	if (ImGui::IsWindowFocused())
		m_Active_Camera->OnUpdate();
	
	//Mouse Picking
	if (ImGui::IsMouseDown(0) && ImGui::IsWindowHovered() && !ImGui::IsItemHovered() && 
		(!ImGuizmo::IsOver() || (scene_mode == SceneMode::Edit && sel_face_index == 0) || m_Selected_Entity->expired()))
	{
		ImVec2 mouse_pos = ImGui::GetMousePos();
		ImVec2 window_pos = ImGui::GetWindowPos();
		ImVec2 cursor_pos = ImGui::GetCursorPos();

		ImVec2 local_mouse_pos = { mouse_pos.x - window_pos.x - cursor_pos.x , mouse_pos.y - window_pos.y - cursor_pos.y };

		switch (scene_mode) {
		case SceneMode::Object:
			uint32_t entity_id;
			m_frame_buffer->ReadPixel(1, (int)local_mouse_pos.x, (int)local_mouse_pos.y, DXGI_FORMAT_R32_UINT, &entity_id);
			if (entity_id) //If Entity ID is not 0 (not an entity)
				for (auto& entity : m_Active_Scene->m_Registry)
				{
					if (*entity == entity_id)
					{
						*m_Selected_Entity = entity;
						break;
					}
				}
			else m_Selected_Entity->reset();
			break;
		case SceneMode::Edit:
			if (!m_Selected_Entity->expired())
				sel_face_index = EditMousePicking::Begin(m_Active_Camera, m_Selected_Entity->lock(), (int)local_mouse_pos.x, (int)local_mouse_pos.y);
			break;
		}
	}

	ImGui::Image(m_shader_resource_view.Get(), window_size);

	//Gizmo
	if (!m_Selected_Entity->expired())
	{
		ImGuizmo::SetOrthographic(!m_Active_Camera->m_is_perspective);
		ImGuizmo::SetDrawlist();
		ImGuizmo::SetRect(ImGui::GetWindowPos().x + ImGui::GetWindowContentRegionMin().x, 
			ImGui::GetWindowPos().y + ImGui::GetWindowContentRegionMin().y, 
			window_size.x, window_size.y);

		auto entity = m_Selected_Entity->lock();

		if (entity->HasComponent<TransformComponent>()) {

			auto& transformComponent = entity->GetComponent<TransformComponent>();
			DirectX::XMMATRIX transform = transformComponent.GetTransform();

			static int guizmo_operation = -1;
			if (Input::IsKeyDown(81)) guizmo_operation = -1;
			else if (Input::IsKeyDown(87)) guizmo_operation = ImGuizmo::OPERATION::TRANSLATE;
			else if (Input::IsKeyDown(69)) guizmo_operation = ImGuizmo::OPERATION::ROTATE;
			else if (Input::IsKeyDown(82)) guizmo_operation = ImGuizmo::OPERATION::SCALE;

			bool snap = Input::IsKeyPressed(17);
			float snapValue = 0.5f;
			switch (guizmo_operation)
			{
			case ImGuizmo::OPERATION::TRANSLATE:
				snapValue = t_snap; break;
			case ImGuizmo::OPERATION::ROTATE:
				snapValue = r_snap; break;
			case ImGuizmo::OPERATION::SCALE:
				snapValue = s_snap; break;
			}

			float snapValues[3] = { snapValue, snapValue, snapValue };

			if (guizmo_operation != -1) {
				switch (scene_mode) {
				case SceneMode::Object:
					ImGuizmo::Manipulate(m_Active_Camera->m_view.r[0].m128_f32, m_Active_Camera->m_projection->r[0].m128_f32,
						(ImGuizmo::OPERATION)guizmo_operation, ImGuizmo::LOCAL, transform.r[0].m128_f32, nullptr, snap ? snapValues : nullptr);

					if (ImGuizmo::IsUsing()) {
						DirectX::XMVECTOR translation;
						DirectX::XMVECTOR rotation;
						DirectX::XMVECTOR scale;
						DirectX::XMMatrixDecompose(&scale, &rotation, &translation, transform);

						transformComponent.Translation = translation;
						transformComponent.Rotation = rotation;
						transformComponent.Scale = scale;
					}
					break;
				}
			}
			switch (scene_mode) {
			case SceneMode::Edit:
				if (!entity->HasComponent<RendererComponent>()) break;
				ImGuizmo::FaceManipulate(m_Active_Camera->m_view.r[0].m128_f32, m_Active_Camera->m_projection->r[0].m128_f32,
					transform.r[0].m128_f32, entity->GetComponent<RendererComponent>().mesh.get(), 
					(ImGuizmo::OPERATION)guizmo_operation, sel_face_index, snap ? snapValues : nullptr);
				break;
			}
		}
	}
	
	ImGui::End();
}

void SceneViewport::CreateShaderResourceView()
{
	D3D11_SHADER_RESOURCE_VIEW_DESC shaderResourceViewDesc;
	shaderResourceViewDesc.Format = DXGI_FORMAT_B8G8R8A8_UNORM;
	shaderResourceViewDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
	shaderResourceViewDesc.Texture2D.MostDetailedMip = 0;
	shaderResourceViewDesc.Texture2D.MipLevels = 1;

	HRESULT hr = Renderer::GetDevice()->CreateShaderResourceView(m_frame_buffer->GetBuffer(0), &shaderResourceViewDesc, m_shader_resource_view.GetAddressOf());
	assert(SUCCEEDED(hr));
}

void SceneViewport::OnWindowResize(const uint32_t width, const uint32_t height)
{
	m_frame_buffer->Release();
	m_shader_resource_view->Release();
	m_frame_buffer->Resize(width, height);
	m_frame_buffer->CreateRenderTargetViews();
	m_frame_buffer->CreateDepthStencilView();
	CreateShaderResourceView();

	m_Active_Camera->UpdateRatio((float)width / (float)height);

	EditMousePicking::OnWindowResize(width, height);
}
