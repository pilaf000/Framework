#pragma once
#include <Windows.h>

#include <cstdint>
#include <memory>
#include <string>

namespace Common
{

using namespace std::string_literals;

class Application;

class Window
{
public:
    struct Description final
    {
        std::wstring title = L"hello, window!"s;
        std::uint32_t width = 1280U;
        std::uint32_t height = 720U;
    };

public:
    Window(Application* app, Description& desc, HINSTANCE hInstance, int nCmdShow);
    ~Window() { }

public:
    const Description& GetDescription() const
    {
        return m_desc;
    }
    const HWND GetWindowHandle() const
    {
        return m_handle;
    }

protected:
    static LRESULT CALLBACK WindowProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);

private:
    Description m_desc;
    HWND m_handle;
    Application* m_app;
};

class Application
{
public:
    static Application* Create(Window::Description& desc, HINSTANCE hInstance, int nCmdShow);
    ~Application() = default;

public:
    const void Terminate()
    {
        m_isTerminate = true;
    }
    void MainLoop();

protected:
    std::unique_ptr<Window> m_window;
    bool m_isTerminate;

private:
    Application(Window::Description& desc, HINSTANCE hInstance, int nCmdShow);
};

} ///end of namespace Common