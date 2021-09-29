#include "stdafx.h"
#include "Window.h"

#include <windowsx.h>

#include "Renderer.h"
#include "imgui/imgui.h"

static Window* window = nullptr;

#ifndef WM_DPICHANGED
#define WM_DPICHANGED 0x02E0 // From Windows SDK 8.1+ headers
#endif

extern IMGUI_IMPL_API LRESULT ImGui_ImplWin32_WndProcHandler(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);

LRESULT CALLBACK WndProc(HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam)
{
	if (ImGui_ImplWin32_WndProcHandler(hwnd, msg, wparam, lparam))
		return true;
	
	switch (msg)
	{
	case WM_KEYDOWN:
		//std::cout << "Keydown " << wparam << std::endl;
		break;
	case WM_MOUSEMOVE:
		//std::cout << "Mousemove " << GET_X_LPARAM(lparam) << " " << GET_Y_LPARAM(lparam) << std::endl;
		break;
	case WM_SIZE:
		window->m_client_size = { LOWORD(lparam), HIWORD(lparam) };
		Renderer::OnWindowResize(window->m_client_size.width, window->m_client_size.height);
		return 0;
	case WM_SYSCOMMAND:
		if ((wparam & 0xfff0) == SC_KEYMENU) // Disable ALT application menu
			return 0;
		break;
	case WM_CREATE:
		window->m_hwnd = hwnd;
		break;
	case WM_DESTROY:
		window->close();
		::PostQuitMessage(0);
		break;
	case WM_DPICHANGED:
		if (ImGui::GetIO().ConfigFlags & ImGuiConfigFlags_DpiEnableScaleViewports)
		{
			//const int dpi = HIWORD(wParam);
			//printf("WM_DPICHANGED to %d (%.0f%%)\n", dpi, (float)dpi / 96.0f * 100.0f);
			const RECT* suggested_rect = (RECT*)lparam;
			::SetWindowPos(window->m_hwnd, NULL, suggested_rect->left, suggested_rect->top, suggested_rect->right - suggested_rect->left, suggested_rect->bottom - suggested_rect->top, SWP_NOZORDER | SWP_NOACTIVATE);
		}
		break;
	}
	return ::DefWindowProc(hwnd, msg, wparam, lparam);
}

Window::Window():
	m_hwnd()
{
}

bool Window::init()
{
	
    WNDCLASSEX windowClass = {};

    windowClass.cbSize = sizeof(WNDCLASSEX);
    windowClass.style = CS_HREDRAW | CS_VREDRAW;
    windowClass.lpfnWndProc = &WndProc;
    windowClass.cbClsExtra = 0;
    windowClass.cbWndExtra = 0;
    windowClass.hInstance = nullptr;
    windowClass.hIcon = ::LoadIcon(nullptr, IDI_APPLICATION);
    windowClass.hCursor = ::LoadCursor(nullptr, IDC_ARROW);
    windowClass.hbrBackground = reinterpret_cast<HBRUSH>(COLOR_WINDOW + 1);
    windowClass.lpszMenuName = L"";
    windowClass.lpszClassName = L"WindowClass";
    windowClass.hIconSm = ::LoadIcon(nullptr, IDI_APPLICATION);

    static ATOM atom = ::RegisterClassEx(&windowClass);
	
    assert(atom > 0);

	if (!window)
		window = this;

	//Creation of the window
	m_hwnd = ::CreateWindowEx(
		WS_EX_OVERLAPPEDWINDOW,
		windowClass.lpszClassName, L"DirectX Game Engine",
		WS_OVERLAPPEDWINDOW,
		CW_USEDEFAULT, CW_USEDEFAULT,
		m_window_size.width, m_window_size.height,
		NULL, NULL, windowClass.hInstance,
		NULL);

	//If creation fail return false
	if (!m_hwnd)
		return false;

	::ShowWindow(m_hwnd, SW_SHOWDEFAULT);
	::UpdateWindow(m_hwnd);
	
	RECT client_rect;
	::GetClientRect(m_hwnd, &client_rect);
	m_client_size = { client_rect.right, client_rect.bottom };
	
	m_is_open = true;

	return true;
}

bool Window::broadcast()
{
	MSG msg;

	while (::PeekMessage(&msg, NULL, 0, 0, PM_REMOVE) > 0)
	{
		TranslateMessage(&msg);
		DispatchMessage(&msg);
		if (msg.message == WM_QUIT)
			return false;
	}

	return true;
}

void Window::close()
{
	m_is_open = false;
	::DestroyWindow(m_hwnd);
}
