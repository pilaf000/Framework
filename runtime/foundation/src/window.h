#pragma once
#define NOMINMAX
#include <Windows.h>
#include <cstdint>
#include <string>

namespace Common
{

using namespace std::string_literals;

class Application;

///windowクラス
class Window
{
public:
	struct Description final
	{
		std::wstring Title = L"hello, window!"s;
		std::uint32_t Width = 1280U;
		std::uint32_t Height = 720U;
	};
public:
	Window(Application* app, Description& desc, HINSTANCE hInstance, int nCmdShow);
	~Window(){}
public:
	const Description& GetDescription() const
	{
		return m_Desc;
	}
	const HWND GetWindowHandle() const
	{
		return m_Handle;
	}
protected:
	static LRESULT CALLBACK WindowProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);
private:
	Description m_Desc;
	HWND m_Handle;
	Application* m_App;
};

///applicationとして扱うクラス
class Application
{
public:
	static Application* Create(Window::Description& desc, HINSTANCE hInstance, int nCmdShow);
	~Application() = default;
public:
	const void Terminate()
	{
		m_IsTerminate = true;
	}
	void MainLoop();
protected:
	std::unique_ptr<Window> m_Window;
	bool m_IsTerminate;
private:
	Application(Window::Description& desc, HINSTANCE hInstance, int nCmdShow);
};

} ///end of namespace Common