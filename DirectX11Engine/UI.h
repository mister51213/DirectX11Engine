#pragma once


#include "FpsClass.h" // put in UI
#include "CpuClass.h" // put in UI
#include <memory>

using namespace std;

class UI
{
public:
	UI();
	~UI();
	bool Initialize();
	void Tick();

	unique_ptr<FpsClass> _Fps;
	unique_ptr<CpuClass> _Cpu;
};

