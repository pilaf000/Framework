#include "window.h"

#include <memory>

namespace Common
{

Window::Window(Application* app, Description& desc, HINSTANCE hInstance, int nCmdShow)
	: m_App(app)
	, m_Desc(desc)
	, m_Handle(NULL)
{
	///windowクラス作成
	WNDCLASSEX windowClass = {0};
	windowClass.cbSize = sizeof(WNDCLASSEX);
	windowClass.style = CS_HREDRAW | CS_VREDRAW;
	windowClass.lpfnWndProc = WindowProc;
	windowClass.hInstance = hInstance;
	windowClass.hCursor = LoadCursor(NULL, IDC_ARROW);
	windowClass.lpszClassName = L"WindowClass";
	RegisterClassEx(&windowClass);

	RECT windowRect = {0, 0, static_cast<LONG>(desc.Width), static_cast<LONG>(desc.Height)};
	AdjustWindowRect(&windowRect, WS_OVERLAPPEDWINDOW, FALSE);

	///window作成
	m_Handle = CreateWindow(
		windowClass.lpszClassName,
		desc.Title.c_str(),
		WS_OVERLAPPEDWINDOW,
		CW_USEDEFAULT,
		CW_USEDEFAULT,
		windowRect.right - windowRect.left,
		windowRect.bottom - windowRect.top,
		nullptr,
		nullptr,
		hInstance,
		m_App);

	ShowWindow(m_Handle, nCmdShow);
}

///windowsコールバック関数
LRESULT CALLBACK Window::WindowProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	Application* app = reinterpret_cast<Application*>(GetWindowLongPtr(hWnd, GWLP_USERDATA));

	//TODO この中にwindowに対する処理を書く
	switch (message)
	{
		case WM_CREATE:
		{
			LPCREATESTRUCT CreateStruct = reinterpret_cast<LPCREATESTRUCT>(lParam);
			SetWindowLongPtr(hWnd, GWLP_USERDATA, reinterpret_cast<LONG_PTR>(CreateStruct->lpCreateParams));
		}
		return 0;
		case WM_KEYDOWN:
		{
			switch (wParam)
			{
				case VK_ESCAPE:
				{
					app->Terminate();
					break;
				}
			}
		}
		case WM_CLOSE:
		{
			app->Terminate();
			return 0;
		}
	}
	return DefWindowProc(hWnd, message, wParam, lParam);
}

Application* Application::Create(Window::Description& desc, HINSTANCE hInstance, int nCmdShow)
{
	auto app = new Application(desc, hInstance, nCmdShow);
	return app;
}

//コンストラクタ内でwindow作成
Application::Application(Window::Description& desc, HINSTANCE hInstance, int nCmdShow)
	: m_Window(nullptr)
	, m_IsTerminate(false)
{
	m_Window = std::make_unique<Window>(this, desc, hInstance, nCmdShow);
}

void Application::MainLoop()
{
	MSG msg = {0};
	while (!m_IsTerminate)
	{
		if (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE))
		{
			if (msg.message == WM_QUIT) break;
			DispatchMessage(&msg);
		}
	}
}

}