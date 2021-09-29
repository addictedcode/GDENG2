#pragma once
#include "Rendering/Window.h"
#include "ImGui/ImGuiLayer.h"
#include "LayerStack.h"

class Application
{
public:
	Application();
	~Application() = default;
	Application operator=(const Application& other) = delete;
	Application (const Application& other) = delete;
	Application operator=(Application&& other) = delete;
	Application(Application&& other) = delete;

	void run();
private:
	Window m_Window;
	std::unique_ptr<ImGuiLayer> m_ImGuiLayer;
	LayerStack m_LayerStack;

	const float m_FixedTimeStep = 1.f / 60.f;
	std::chrono::steady_clock::time_point m_last_frame_time = std::chrono::steady_clock::now();

public:
	static Window* GetCurrentWindow();
};
