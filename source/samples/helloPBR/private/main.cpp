#include <stdafx.h>
#include <iostream>
#include <fstream>
#include <D3D12HelloWorld.h>

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE, LPSTR, int nCmdShow)
{
	try
	{
		D3D12HelloWindow sample(1280, 720, L"D3D12 Hello Window");
		return Win32Application::Run(&sample, hInstance, nCmdShow);
	}
	catch (const std::exception& e)
	{
		std::ofstream file("log.txt");
		file << e.what();
		file.close();
		throw(e.what());
	}
	return -1;
}
