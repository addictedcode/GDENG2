#include "stdafx.h"
#include "Application.h"

#include "Editor/EditorLayer.h"

#include "Rendering/Renderer.h"

static Window* current_window;

Application::Application()
{
	m_Window.init();
	Renderer::Initialize(m_Window);

	current_window = &m_Window;
	
	m_ImGuiLayer = std::make_unique<ImGuiLayer>(m_Window.m_hwnd);

	m_LayerStack.PushOverlay(new EditorLayer());
}

void Application::run()
{
	while (m_Window.m_is_open) {
		if (!m_Window.broadcast()) break;
		
		std::chrono::steady_clock::time_point time = std::chrono::steady_clock::now();
		const float delta_time = std::chrono::duration<float>(time - m_last_frame_time).count();
		m_last_frame_time = time;

		//Fixed Update
		static float accumulator = 0.f;
		accumulator += delta_time;
		while (accumulator >= m_FixedTimeStep)
		{
			for (auto* layer : m_LayerStack)
			{
				layer->OnFixedUpdate(m_FixedTimeStep);
			}
			accumulator -= m_FixedTimeStep;
		}
		
		//Per frame Update
		for (auto* layer : m_LayerStack)
		{
			layer->OnUpdate(delta_time);
		}

		//Render ImGui
		Renderer::Clear();
		m_ImGuiLayer->Begin();

		for (auto* layer : m_LayerStack)
		{
			layer->OnImGuiRender();
		}

		m_ImGuiLayer->End();
		Renderer::SwapBuffer();
	}
}

Window* Application::GetCurrentWindow()
{
	return current_window;
}
