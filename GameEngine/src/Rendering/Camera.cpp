#include "stdafx.h"
#include "Camera.h"

#include "imgui/imgui.h"

#include "Core/Input.h"

Camera::Camera() :
	m_position({ -5.f, 3.5f, -5.0f }),
	m_rotation({ 3.14f/6.f, 3.14f / 4.f ,0.f }),
	m_view(DirectX::XMMatrixIdentity()),
	m_projection(&m_perspective_proj),
	m_is_perspective(true),
	m_perspective_proj(DirectX::XMMatrixPerspectiveFovLH(45.0f, 2.0f / 1.5f, 0.1f, 100.0f)),
	m_orthographic_proj(DirectX::XMMatrixOrthographicLH(2.0f, 1.5f, 0.0f, 100.0f)),
	m_ratio(2.0f / 1.5f),
	persdata({45.0f, 0.1f, 100.0f}),
	orthodata({10.0f, 0.0f, 100.0f})
{
}

void Camera::OnUpdate()
{
	if (Input::IsKeyDown(90)) { // Z Key
		m_is_perspective = !m_is_perspective;
		m_is_perspective ? m_projection = &m_perspective_proj : m_projection = &m_orthographic_proj;
	}

	//If Right Click is Pressed
	if (Input::IsKeyPressed(2)) {

		//Go Left
		if (Input::IsKeyPressed(37))
		{
			DirectX::XMVECTOR right = { m_view.r[0].m128_f32[0], m_view.r[1].m128_f32[0], m_view.r[2].m128_f32[0] };
			right = DirectX::XMVectorMultiply(DirectX::XMVector3Normalize(right), { -0.2f, -0.2f, -0.2f });
			m_position = DirectX::XMVectorAdd(m_position, right);
		}
		//Go Right
		if (Input::IsKeyPressed(39))
		{
			DirectX::XMVECTOR right = { m_view.r[0].m128_f32[0], m_view.r[1].m128_f32[0], m_view.r[2].m128_f32[0] };
			right = DirectX::XMVectorMultiply(DirectX::XMVector3Normalize(right), { 0.2f, 0.2f, 0.2f });
			m_position = DirectX::XMVectorAdd(m_position, right);
		}
		//Go Forward
		if (Input::IsKeyPressed(38))
		{
			DirectX::XMVECTOR forward = { m_view.r[0].m128_f32[2], m_view.r[1].m128_f32[2], m_view.r[2].m128_f32[2] };
			forward = DirectX::XMVectorMultiply(DirectX::XMVector3Normalize(forward), { 0.2f, 0.2f, 0.2f });
			m_position = DirectX::XMVectorAdd(m_position, forward);
		}
		//Go Backward
		if (Input::IsKeyPressed(40))
		{
			DirectX::XMVECTOR forward = { m_view.r[0].m128_f32[2], m_view.r[1].m128_f32[2], m_view.r[2].m128_f32[2] };
			forward = DirectX::XMVectorMultiply(DirectX::XMVector3Normalize(forward), { -0.2f, -0.2f, -0.2f });
			m_position = DirectX::XMVectorAdd(m_position, forward);
		}

	}

	static DirectX::XMFLOAT2 old_mouse_pos = Input::GetMousePosition();
	const DirectX::XMFLOAT2 new_mouse_pos = Input::GetMousePosition();

	if (Input::IsKeyPressed(2)) {
		const float delta_mouse_x = (new_mouse_pos.x - old_mouse_pos.x) * 0.01f;
		const float delta_mouse_y = (new_mouse_pos.y - old_mouse_pos.y) * 0.01f;

		m_rotation = DirectX::XMVectorAdd(m_rotation, { delta_mouse_y, delta_mouse_x, 0.0f });
	}
		
	old_mouse_pos = new_mouse_pos;
}

void Camera::OnImGuiRender()
{
	ImGui::Begin("Editor Camera");

	ImGui::Text("Is Perspective?");
	ImGui::SameLine();
	if (ImGui::Checkbox("##IsPerspective", &m_is_perspective))
		m_is_perspective ? m_projection = &m_perspective_proj : m_projection = &m_orthographic_proj;
	
	if (m_is_perspective)
	{
		bool hasChanged = false;
		ImGui::Text("Perspective");
		
		ImGui::Columns(2);
		ImGui::SetColumnWidth(0, 50);
		ImGui::Text("FOV");
		ImGui::NextColumn();
		if (ImGui::DragFloat("##FOV", &(persdata.FOV), 0.1f, 0.1f, 0.0f, "%.2f")) hasChanged = true;;

		ImGui::NextColumn();
		ImGui::Text("NearZ");
		ImGui::NextColumn();
		if (ImGui::DragFloat("##NearZ", &(persdata.nearZ), 0.1f, 0.1f, 0.0f, "%.2f")) hasChanged = true;
		if (persdata.nearZ < 0.1f) persdata.nearZ = 0.1f;

		ImGui::NextColumn();
		ImGui::Text("FarZ");
		ImGui::NextColumn();
		if(ImGui::DragFloat("##FarZ", &(persdata.farZ), 0.1f, 0.1f, 0.0f, "%.2f")) hasChanged = true;
		if (persdata.farZ < 0.1f) persdata.farZ = 0.1f;

		if (hasChanged)
			m_perspective_proj = DirectX::XMMatrixPerspectiveFovLH(persdata.FOV * 3.1415f / 180.0f, m_ratio, persdata.nearZ, persdata.farZ);
	}
	else
	{
		bool hasChanged = false;
		ImGui::Text("Orthographic");
		
		ImGui::Columns(2);
		ImGui::SetColumnWidth(0, 100);
		ImGui::Text("View Scale");
		ImGui::NextColumn();
		if (ImGui::DragFloat("##ViewScale", &(orthodata.view_scale), 0.1f, 0.1f, 0.0f, "%.2f")) hasChanged = true;
		if (orthodata.view_scale < 0.1f) orthodata.view_scale = 0.1f;

		ImGui::NextColumn();
		ImGui::Text("NearZ");
		ImGui::NextColumn();
		if (ImGui::DragFloat("##NearZ", &(orthodata.nearZ), 0.1f, 0.0f, 0.0f, "%.2f")) hasChanged = true;
		if (orthodata.nearZ < 0.0f) orthodata.nearZ = 0.0f;

		ImGui::NextColumn();
		ImGui::Text("FarZ");
		ImGui::NextColumn();
		if (ImGui::DragFloat("##FarZ", &(orthodata.farZ), 0.1f, 0.1f, 0.0f, "%.2f")) hasChanged = true;
		if (orthodata.farZ < 0.1f) orthodata.farZ = 0.1f;

		if (hasChanged)
			m_orthographic_proj = DirectX::XMMatrixOrthographicLH(m_ratio * orthodata.view_scale, orthodata.view_scale, orthodata.nearZ, orthodata.farZ);
	}
	
	ImGui::End();

	
}

void Camera::UpdateRatio(float ratio)
{
	m_ratio = ratio;
	m_perspective_proj = DirectX::XMMatrixPerspectiveFovLH(persdata.FOV * 3.1415f / 180.0f, m_ratio, persdata.nearZ, persdata.farZ);
	m_orthographic_proj = DirectX::XMMatrixOrthographicLH(m_ratio * orthodata.view_scale, orthodata.view_scale, orthodata.nearZ, orthodata.farZ);
}
