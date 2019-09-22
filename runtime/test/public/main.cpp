#include <window.h>

namespace
{

class Test {
public:
Test() = default;
~Test() = default;

void update(){
	
}
private:
};

}


int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE, LPSTR, int nCmdShow)
{
	Common::Window::Description windowDesc = {};
	windowDesc.title = L"Hello, World";
	windowDesc.width = 640;
	windowDesc.height = 360;

	Test t;
	t.update();

	auto app = Common::Application::Create(windowDesc, hInstance, nCmdShow);
	app->MainLoop();
	return 0;
}