#pragma once
#include <Windows.h>

class Window
{
	struct WindowSize { long width, height; };
	
public:
	Window();
	~Window() = default;
	Window operator=(const Window& other) = delete;
	Window(const Window& other) = delete;
	Window operator=(Window&& other) = delete;
	Window(Window&& other) = delete;

private:
	WindowSize m_window_size = { 1024l, 768l };

public:
	HWND m_hwnd;
	WindowSize m_client_size = { 0l, 0l };
	
	bool init();
	bool broadcast();
	void close();

	bool m_is_open = false;
	const WindowSize& GetWindowSize() const { return m_window_size; }
	const HWND& GetHWND() const { return m_hwnd; }
};