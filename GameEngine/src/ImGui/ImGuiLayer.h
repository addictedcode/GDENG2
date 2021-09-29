#pragma once

class ImGuiLayer
{
public:
	ImGuiLayer(void* hwnd);
	~ImGuiLayer();
	ImGuiLayer operator=(const ImGuiLayer & other) = delete;
	ImGuiLayer(const ImGuiLayer & other) = delete;
	ImGuiLayer operator=(ImGuiLayer && other) = delete;
	ImGuiLayer(ImGuiLayer && other) = delete;

public:
	void Begin();
	void End();
};
