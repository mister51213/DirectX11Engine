#include "System.h"
#include <stdexcept>

using namespace std;

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, PSTR pScmdline, int iCmdshow)
{
	try
	{
		System* system;
		bool result;

		// Create the system object.
		system = new System;
		if (!system)
		{
			return 0;
		}

		// Initialize and run the system object.
		result = system->Initialize();
		if (result)
		{
			system->Run();
		}

		// Shutdown and release the system object.
		system->Shutdown();
		delete system;
		system = 0;
	}
	catch (const RuntimeException& e)
	{
#if defined(UNICODE) || defined(_UNICODE)
	MessageBox(nullptr, e.GetWindowsError().c_str(), L"Runtime error.", MB_OK);
#else
	MessageBox(nullptr, e.GetError().c_str(), "Runtime error.", MB_OK);
#endif
	}
	//catch (const std::runtime_error& e)
	//{
	//	const std::string errMsg = e.what();
	//	const std::string ErrMsg(errMsg.begin(), errMsg.end());

	//	MessageBox(nullptr, ErrMsg.c_str(), "Runtime error.", MB_OK);
	//}

	return 0;
}