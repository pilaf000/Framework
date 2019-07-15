#include "window.h"

#include <memory>

namespace Common
{

Window::Window(Application* app, Description& desc, HINSTANCE hInstance, int nCmdShow)
	: m_app(app)
	, m_desc(desc)
	, m_handle(NULL)
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

	RECT windowRect = {0, 0, static_cast<LONG>(desc.width), static_cast<LONG>(desc.height)};
	AdjustWindowRect(&windowRect, WS_OVERLAPPEDWINDOW, FALSE);

	///window作成
	m_handle = CreateWindow(
		windowClass.lpszClassName,
		desc.title.c_str(),
		WS_OVERLAPPEDWINDOW,
		CW_USEDEFAULT,
		CW_USEDEFAULT,
		windowRect.right - windowRect.left,
		windowRect.bottom - windowRect.top,
		nullptr,
		nullptr,
		hInstance,
		m_app);

	ShowWindow(m_handle, nCmdShow);
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
	: m_window(nullptr)
	, m_isTerminate(false)
{
	m_window = std::make_unique<Window>(this, desc, hInstance, nCmdShow);
}

void Application::MainLoop()
{
	MSG msg = {0};
	while (!m_isTerminate)
	{
		if (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE))
		{
			if (msg.message == WM_QUIT) break;
			DispatchMessage(&msg);
		}
	}
}

}