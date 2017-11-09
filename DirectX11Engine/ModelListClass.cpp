///////////////////////////////////////////////////////////////////////////////
// Filename: modellistclass.cpp
///////////////////////////////////////////////////////////////////////////////
#include "modellistclass.h"

ModelListClass::ModelListClass()
	:
	_ModelInfoList(0)
{}

ModelListClass::ModelListClass(const ModelListClass& other)
{
}

ModelListClass::~ModelListClass()
{
}

bool ModelListClass::Initialize(int numModels)
{
	float red, green, blue;

	// Store the number of models.
	_modelCount = numModels;

	// Create a list array of the model information.
	_ModelInfoList = new ModelInfoType[_modelCount];
	if (!_ModelInfoList)
	{
		return false;
	}

	// Seed the random generator with the current time.
	srand((unsigned int)time(NULL));

	// Go through all the models and randomly generate the model color and position.
	for (int i = 0; i<_modelCount; i++)
	{
		// Generate a random color for the model.
		red = (float)rand() / RAND_MAX;
		green = (float)rand() / RAND_MAX;
		blue = (float)rand() / RAND_MAX;

		_ModelInfoList[i].color = XMFLOAT4(red, green, blue, 1.0f);

		// Generate a random position in front of the viewer for the mode.
		_ModelInfoList[i].positionX = (((float)rand() - (float)rand()) / RAND_MAX) * 10.0f;
		_ModelInfoList[i].positionY = (((float)rand() - (float)rand()) / RAND_MAX) * 10.0f;
		_ModelInfoList[i].positionZ = ((((float)rand() - (float)rand()) / RAND_MAX) * 10.0f) + 5.0f;
	}

	return true;
}

void ModelListClass::Shutdown()
{
	// Release the model information list.
	if (_ModelInfoList)
	{
		delete[] _ModelInfoList;
		_ModelInfoList = 0;
	}

	return;
}

int ModelListClass::GetModelCount()
{
	return _modelCount;
}

void ModelListClass::GetData(int index, float& positionX, float& positionY, float& positionZ, XMFLOAT4& color)
{
	positionX = _ModelInfoList[index].positionX;
	positionY = _ModelInfoList[index].positionY;
	positionZ = _ModelInfoList[index].positionZ;

	color = _ModelInfoList[index].color;

	return;
}
