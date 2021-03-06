////////////////////////////////////////////////////////////////////////////////
// Filename: model.cpp
////////////////////////////////////////////////////////////////////////////////
#include "Model.h"
#include "Shlwapi.h"

using namespace GfxUtil;

Model::Model()
	:
	Name("Model"),
	bPerVertexCollision(false),
	bCastShadow(true),
	bVisibleInRefraction(false)
{
	_scale = XMFLOAT3(1.f, 1.f, 1.f);
	_orientation = XMFLOAT3(0.f, 0.f, 0.f);
	_position = XMFLOAT3(0.f, 0.f, 0.f);
}

Model::Model(XMFLOAT3 scale, XMFLOAT3 orientation, XMFLOAT3 translation, string name, bool castShadow, bool visibleInRefraction)
{
	_scale = scale;
	_orientation = orientation;
	_position = translation;
	Name = name;
	bCastShadow = castShadow;
	bVisibleInRefraction = visibleInRefraction;
}

//Model::Model(const Model& other)
//{}

bool Model::Initialize(ID3D11Device* const device, ID3D11DeviceContext* const deviceContext, const string modelFilename, vector<string> texFileNames, EShaderType shaderType)
{
	// Load in the model data
	LoadVerticesFromFile(modelFilename);

	// Calculate the normal, tangent, and binormal vectors for the model.
	if (shaderType == EShaderType::ELIGHT_SPECULAR)
	{
		CalculateModelVectors();
	}

	// Initialize the vertex and index buffers.
	InitializeBuffers(device);

	// Loads the textures in the material. Material now holds pointer to texture class
	_material.reset(new Material);

	_material->Initialize(device, deviceContext, shaderType, texFileNames);

	// TESTING AREA for different string functions
	//WCHAR* wstr = (WCHAR*)fileName1; // not working
	//WCHAR* wstr2 = reinterpret_cast<WCHAR*>(fileName1); // not working
	//const WCHAR *pwcsName2 = charToWChar(fileName1); // works even though Microsoft doesnt like it
	//const WCHAR *pwcsName3 = charToWChar_S(fileName1); // BAD
	// TESTING AREA

	return true;
}

Material* Model::GetMaterial()
{
	return _material.get();
}

void Model::SetMaterial(Material* const mat)
{
	_material.reset(mat);
}

int Model::GetIndexCount()
{
	return _indexCount;
}

bool Model::InitializeBuffers(ID3D11Device* const device)
{
	VertexType* vertices;
	unsigned long* indices;
	D3D11_BUFFER_DESC vertexBufferDesc, indexBufferDesc;
	D3D11_SUBRESOURCE_DATA vertexData, indexData;
	HRESULT result;

	// Create the vertex array.
	vertices = new VertexType[_vertexCount];
	if (!vertices)
	{
		return false;
	}
	//_Vertices = new VertexType[_vertexCount];
	//if (!_Vertices)
	//{
	//	return false;
	//}

	// Create the index array.
	indices = new unsigned long[_indexCount];
	if (!indices)
	{
		return false;
	}

	// Load the vertex array and index array with data.
	for (int i = 0; i < _vertexCount; i++)
	{
		vertices[i].position = XMFLOAT3(_model.get()[i].x, _model.get()[i].y, _model.get()[i].z);
		vertices[i].texture = XMFLOAT2(_model.get()[i].tu, _model.get()[i].tv);
		vertices[i].normal = XMFLOAT3(_model.get()[i].nx, _model.get()[i].ny, _model.get()[i].nz);
		vertices[i].tangent = XMFLOAT3(_model.get()[i].tx, _model.get()[i].ty, _model.get()[i].tz);
		vertices[i].binormal = XMFLOAT3(_model.get()[i].bx, _model.get()[i].by, _model.get()[i].bz);

		indices[i] = i;
	}

	// Set up the description of the static vertex buffer.
	vertexBufferDesc.Usage = D3D11_USAGE_DEFAULT;
	vertexBufferDesc.ByteWidth = sizeof(VertexType) * _vertexCount;
	vertexBufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	vertexBufferDesc.CPUAccessFlags = 0;
	vertexBufferDesc.MiscFlags = 0;
	vertexBufferDesc.StructureByteStride = 0;

	// Give the subresource structure a pointer to the vertex data.
	//vertexData.pSysMem = _Vertices;
	vertexData.pSysMem = vertices;
	vertexData.SysMemPitch = 0;
	vertexData.SysMemSlicePitch = 0;

	// Now create the vertex buffer.
	ThrowHResultIf(device->CreateBuffer(&vertexBufferDesc, &vertexData, &_vertexBuffer));

	// Set up the description of the static index buffer.
	indexBufferDesc.Usage = D3D11_USAGE_DEFAULT;
	indexBufferDesc.ByteWidth = sizeof(unsigned long) * _indexCount;
	indexBufferDesc.BindFlags = D3D11_BIND_INDEX_BUFFER;
	indexBufferDesc.CPUAccessFlags = 0;
	indexBufferDesc.MiscFlags = 0;
	indexBufferDesc.StructureByteStride = 0;

	// Give the subresource structure a pointer to the index data.
	indexData.pSysMem = indices;
	indexData.SysMemPitch = 0;
	indexData.SysMemSlicePitch = 0;

	// Create the index buffer.
	ThrowHResultIf(device->CreateBuffer(&indexBufferDesc, &indexData, _indexBuffer.GetAddressOf()));

	// If using per vertex collision, store an array of positions with w value 1 for positional calculations and array of indices
	if (bPerVertexCollision)
	{
		VertexType* localVert = vertices;
		for (int i = 0; i < _vertexCount; ++i)
		{
			_Vertices.emplace_back(vertices->position.x, vertices->position.y, vertices->position.z, 1.f);
			localVert++;
		}

		unsigned long* localIndex = indices;
		for (int i = 0; i < _vertexCount; ++i)
		{
			_Indices.push_back((static_cast<unsigned int>(*localIndex)));
			localIndex++;
		}
	}

	// Release the arrays now that the vertex and index buffers have been created and loaded.
	delete[] vertices;
	vertices = 0;

	delete[] indices;
	indices = 0;

	return true;
}

void Model::PutVerticesOnPipeline(ID3D11DeviceContext* const deviceContext)
{
	// Set vertex buffer stride and offset.
	unsigned int stride = sizeof(VertexType);
	unsigned int offset = 0;

	// Set the vertex buffer to active in the input assembler so it can be rendered.
	deviceContext->IASetVertexBuffers(0, 1, _vertexBuffer.GetAddressOf(), &stride, &offset);

	// Set the index buffer to active in the input assembler so it can be rendered.
	deviceContext->IASetIndexBuffer(_indexBuffer.Get(), DXGI_FORMAT_R32_UINT, 0);

	// Set the type of primitive that should be rendered from this vertex buffer, in this case triangles.
	deviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
}

void Model::Draw(ID3D11DeviceContext* const deviceContext)
{
	//// Set vertex buffer stride and offset.
	//unsigned int stride = sizeof(VertexType);
	//unsigned int offset = 0;

	//// Set the vertex buffer to active in the input assembler so it can be rendered.
	//deviceContext->IASetVertexBuffers(0, 1, _vertexBuffer.GetAddressOf(), &stride, &offset);

	//// Set the index buffer to active in the input assembler so it can be rendered.
	//deviceContext->IASetIndexBuffer(_indexBuffer.Get(), DXGI_FORMAT_R32_UINT, 0);

	//// Set the type of primitive that should be rendered from this vertex buffer, in this case triangles.
	//deviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

	deviceContext->DrawIndexed(_indexCount, 0, 0);
}

void Model::CalculateModelVectors()
{
	int faceCount, index;
	TempVertexType vertex1, vertex2, vertex3;
	VectorType tangent, binormal, normal;

	// Calculate the number of faces in the model.
	faceCount = _vertexCount / 3;

	// Initialize the index to the model data.
	index = 0;

	// Go through all the faces and calculate the the tangent, binormal, and normal vectors.
	for (int i = 0; i < faceCount; i++)
	{
		// Get the three vertices for this face from the model.
		vertex1.x = _model.get()[index].x;
		vertex1.y = _model.get()[index].y;
		vertex1.z = _model.get()[index].z;
		vertex1.tu = _model.get()[index].tu;
		vertex1.tv = _model.get()[index].tv;
		vertex1.nx = _model.get()[index].nx;
		vertex1.ny = _model.get()[index].ny;
		vertex1.nz = _model.get()[index].nz;
		index++;

		vertex2.x = _model.get()[index].x;
		vertex2.y = _model.get()[index].y;
		vertex2.z = _model.get()[index].z;
		vertex2.tu = _model.get()[index].tu;
		vertex2.tv = _model.get()[index].tv;
		vertex2.nx = _model.get()[index].nx;
		vertex2.ny = _model.get()[index].ny;
		vertex2.nz = _model.get()[index].nz;
		index++;

		vertex3.x = _model.get()[index].x;
		vertex3.y = _model.get()[index].y;
		vertex3.z = _model.get()[index].z;
		vertex3.tu = _model.get()[index].tu;
		vertex3.tv = _model.get()[index].tv;
		vertex3.nx = _model.get()[index].nx;
		vertex3.ny = _model.get()[index].ny;
		vertex3.nz = _model.get()[index].nz;
		index++;

		// Calculate the tangent and binormal of that face.
		CalculateTangentBinormal(vertex1, vertex2, vertex3, tangent, binormal);

		// Calculate the new normal using the tangent and binormal.
		CalculateNormal(tangent, binormal, normal);

		// Store the normal, tangent, and binormal for this face back in the model structure.
		_model.get()[index - 1].nx = normal.x;
		_model.get()[index - 1].ny = normal.y;
		_model.get()[index - 1].nz = normal.z;
		_model.get()[index - 1].tx = tangent.x;
		_model.get()[index - 1].ty = tangent.y;
		_model.get()[index - 1].tz = tangent.z;
		_model.get()[index - 1].bx = binormal.x;
		_model.get()[index - 1].by = binormal.y;
		_model.get()[index - 1].bz = binormal.z;

		_model.get()[index - 2].nx = normal.x;
		_model.get()[index - 2].ny = normal.y;
		_model.get()[index - 2].nz = normal.z;
		_model.get()[index - 2].tx = tangent.x;
		_model.get()[index - 2].ty = tangent.y;
		_model.get()[index - 2].tz = tangent.z;
		_model.get()[index - 2].bx = binormal.x;
		_model.get()[index - 2].by = binormal.y;
		_model.get()[index - 2].bz = binormal.z;

		_model.get()[index - 3].nx = normal.x;
		_model.get()[index - 3].ny = normal.y;
		_model.get()[index - 3].nz = normal.z;
		_model.get()[index - 3].tx = tangent.x;
		_model.get()[index - 3].ty = tangent.y;
		_model.get()[index - 3].tz = tangent.z;
		_model.get()[index - 3].bx = binormal.x;
		_model.get()[index - 3].by = binormal.y;
		_model.get()[index - 3].bz = binormal.z;
	}
}

void Model::CalculateTangentBinormal(TempVertexType vertex1, TempVertexType vertex2, TempVertexType vertex3,
	VectorType& tangent, VectorType& binormal)
{
	float vector1[3], vector2[3];
	float tuVector[2], tvVector[2];
	float den;
	float length;


	// Calculate the two vectors for this face.
	vector1[0] = vertex2.x - vertex1.x;
	vector1[1] = vertex2.y - vertex1.y;
	vector1[2] = vertex2.z - vertex1.z;

	vector2[0] = vertex3.x - vertex1.x;
	vector2[1] = vertex3.y - vertex1.y;
	vector2[2] = vertex3.z - vertex1.z;

	// Calculate the tu and tv texture space vectors.
	tuVector[0] = vertex2.tu - vertex1.tu;
	tvVector[0] = vertex2.tv - vertex1.tv;

	tuVector[1] = vertex3.tu - vertex1.tu;
	tvVector[1] = vertex3.tv - vertex1.tv;

	// Calculate the denominator of the tangent/binormal equation.
	den = 1.0f / (tuVector[0] * tvVector[1] - tuVector[1] * tvVector[0]);

	// Calculate the cross products and multiply by the coefficient to get the tangent and binormal.
	tangent.x = (tvVector[1] * vector1[0] - tvVector[0] * vector2[0]) * den;
	tangent.y = (tvVector[1] * vector1[1] - tvVector[0] * vector2[1]) * den;
	tangent.z = (tvVector[1] * vector1[2] - tvVector[0] * vector2[2]) * den;

	binormal.x = (tuVector[0] * vector2[0] - tuVector[1] * vector1[0]) * den;
	binormal.y = (tuVector[0] * vector2[1] - tuVector[1] * vector1[1]) * den;
	binormal.z = (tuVector[0] * vector2[2] - tuVector[1] * vector1[2]) * den;

	// Calculate the length of this normal.
	length = sqrt((tangent.x * tangent.x) + (tangent.y * tangent.y) + (tangent.z * tangent.z));

	// Normalize the normal and then store it
	tangent.x = tangent.x / length;
	tangent.y = tangent.y / length;
	tangent.z = tangent.z / length;

	// Calculate the length of this normal.
	length = sqrt((binormal.x * binormal.x) + (binormal.y * binormal.y) + (binormal.z * binormal.z));

	// Normalize the normal and then store it
	binormal.x = binormal.x / length;
	binormal.y = binormal.y / length;
	binormal.z = binormal.z / length;
}

void Model::CalculateNormal(VectorType tangent, VectorType binormal, VectorType& normal)
{
	float length;


	// Calculate the cross product of the tangent and binormal which will give the normal vector.
	normal.x = (tangent.y * binormal.z) - (tangent.z * binormal.y);
	normal.y = (tangent.z * binormal.x) - (tangent.x * binormal.z);
	normal.z = (tangent.x * binormal.y) - (tangent.y * binormal.x);

	// Calculate the length of the normal.
	length = sqrt((normal.x * normal.x) + (normal.y * normal.y) + (normal.z * normal.z));

	// Normalize the normal.
	normal.x = normal.x / length;
	normal.y = normal.y / length;
	normal.z = normal.z / length;
}

void Model::SetResourceView(const int index, ID3D11ShaderResourceView * view)
{
	if (_material)
	{
		_material->GetTextureObject()->GetTextureArray()[index] = view;
	}
}

bool Model::LoadVerticesFromFile(const string filename)
{
	ifstream fin;
	char input;
	int i;

	// TODO: check file extension first and use difft lambdas for bin mesh and txt
	string extension = PathFindExtension(filename.data());
	if (extension == ".txt")
	{
		// Open the model file.
		fin.open(filename);

		// If it could not open the file then exit.
		if (fin.fail())
		{
			ThrowRuntime("Could not open " + filename + ".");
		}

		// Read up to the value of vertex count.
		fin.get(input);
		while (input != ':')
		{
			fin.get(input);
		}

		// Read in the vertex count.
		fin >> _vertexCount;

		// Set the number of indices to be the same as the vertex count.
		_indexCount = _vertexCount;

		// Create the model using the vertex count that was read in.
		_model.reset(new ModelType[_vertexCount]);
		if (!_model)
		{
			ThrowRuntime("Could not create the model - " + filename);
		}

		// Read up to the beginning of the data.
		fin.get(input);
		while (input != ':')
		{
			fin.get(input);
		}
		fin.get(input);
		fin.get(input);

		// Read in the vertex data.
		for (i = 0; i < _vertexCount; i++)
		{
			fin >> _model.get()[i].x >> _model.get()[i].y >> _model.get()[i].z;
			fin >> _model.get()[i].tu >> _model.get()[i].tv;
			fin >> _model.get()[i].nx >> _model.get()[i].ny >> _model.get()[i].nz;
		}

		// Close the model file.
		fin.close();

		return true;
	}
	else
	{
		// BINARY MESH LOADER VERSION // BINARY MESH LOADER VERSION // BINARY MESH LOADER VERSION // BINARY MESH LOADER VERSION
		ID3D11Buffer* pVertexBuffer, *pIndexBuffer;
		int vertexCount, indexCount;
		ModelType* pModel;

		ifstream file(filename, std::ios::binary); // open file in binary mode

		if (file.fail())
		{
			return false;
		}

		// read first 32 bits into vertexCount
		vertexCount = 0; 
		//file.read(reinterpret_cast<char*>(&vertexCount), sizeof(int));
		file.read(reinterpret_cast<char*>(&_vertexCount), sizeof(int));

		// pack into vector of structs
		vector<VertexType> vertList(_vertexCount);
		unsigned long long dataSize = sizeof(VertexType);
		dataSize*= _vertexCount;
		file.read(reinterpret_cast<char*>(vertList.data()), dataSize/*sizeof(VertexType)*vertexCount*/);

		_indexCount = _vertexCount;

		// Create the model using the vertex count that was read in.
		_model.reset(new ModelType[_vertexCount]);
		if (!_model)
		{
			ThrowRuntime("Could not create the model - " + filename);
		}

		// TEMP FOR TESTING (SEE IF THE BINARY MESH FILE HAS THE RIGHT DATA)
		for (i = 0; i < _vertexCount; i++)
		{
			_model.get()[i].x = vertList[i].position.x;
			_model.get()[i].y = vertList[i].position.y;
			_model.get()[i].z = vertList[i].position.z;

			_model.get()[i].tu = vertList[i].texture.x;
			_model.get()[i].tv = vertList[i].texture.y;

			_model.get()[i].nx = vertList[i].normal.x;
			_model.get()[i].ny = vertList[i].normal.y;
			_model.get()[i].nz = vertList[i].normal.z;
		}

		file.close();

		return true;
		// BINARY MESH LOADER VERSION // BINARY MESH LOADER VERSION // BINARY MESH LOADER VERSION // BINARY MESH LOADER VERSION
	}
}