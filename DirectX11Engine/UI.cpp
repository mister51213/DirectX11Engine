#include "UI.h"



UI::UI()
{
	_Fps.reset(new FpsClass);
	_Cpu.reset(new CpuClass);
}

UI::~UI()
{
}

bool UI::Initialize()
{
	_Fps->Initialize();
	_Cpu->Initialize();

	return true;
}

void UI::Tick()
{
	_Fps->Frame(); 
	_Cpu->Frame();
}