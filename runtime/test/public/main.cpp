#include <window.h>

namespace
{

void Test()
{

	auto x = 1.f;
	auto y = 2u;
	auto z = 0.;

	auto f = [&]() 
	{
		double result = 0.;

		result = (double)x + (double)y;

		return result;
	};

	auto a = f();

}

}


int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE, LPSTR, int nCmdShow)
{
	Common::Window::Description windowDesc = {};
	windowDesc.Title = L"Hello, World";
	windowDesc.Width = 640;
	windowDesc.Height = 360;

	Test();

	auto app = Common::Application::Create(windowDesc, hInstance, nCmdShow);
	app->MainLoop();
	return 0;
}