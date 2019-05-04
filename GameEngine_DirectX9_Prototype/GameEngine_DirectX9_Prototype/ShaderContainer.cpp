#include "ShaderContainer.h"

vector<string> ShaderContainer::ShaderFileNames{};
map<string, ShaderContainer::ShaderInfo *> ShaderContainer::ShaderTable{};
IDirect3DDevice9 * ShaderContainer::Device = nullptr;

void ShaderContainer::InitFbxModelRendererWithAnimationShader(IDirect3DDevice9 * device)
{
	if (!device) return;

	// 쉐이더 목록 // 파일이름으로 저장해준다. 여기서는 쓰지않지만 그냥해준다.
	string shaderFileName = "FbxModelRendererWithAnimation.vs";
	ShaderFileNames.push_back(shaderFileName);


	ShaderInfo * tempShader = new ShaderInfo;
	tempShader->device = device;


	HRESULT hr = 0;

	D3DVERTEXELEMENT9 decl[] =
	{
		/*
		나중에 셰이더 코드의 구조체
		struct VS_INPUT
		{
		vector positi : POSITION // POSITION0
		: NORMAL0
		: NORMAL1
		: NORMAL2
		}
		과 매핑된다.
		/*
		float position[3];
		float normal[3];
		float uv[2];
		float animationMatrix[4][4];
		*/
		// offsets in bytes
		{ 0,  0, D3DDECLTYPE_FLOAT3, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_POSITION, 0 },
		{ 0, 12, D3DDECLTYPE_FLOAT3, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_NORMAL,   0 },
		{ 0, 24, D3DDECLTYPE_FLOAT2, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_TEXCOORD, 0 },

		// 나머지 매트릭스
		{ 0, 32, D3DDECLTYPE_FLOAT4, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_NORMAL, 1 },
		{ 0, 48, D3DDECLTYPE_FLOAT4, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_NORMAL, 2 },
		{ 0, 64, D3DDECLTYPE_FLOAT4, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_NORMAL, 3 },
		{ 0, 80, D3DDECLTYPE_FLOAT4, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_NORMAL, 4 },

		D3DDECL_END()
	};


	hr = device->CreateVertexDeclaration(decl, &tempShader->declaration);
	if (FAILED(hr))
	{
		::MessageBox(0, "CreateVertexDeclaration() - FAILED", 0, 0);
		return;
	}


	// 쉐이더 컴파일 / 핸들 받아오기
	//------------------------------------------------------------------
	ID3DXBuffer* compiledCode = 0;
	ID3DXBuffer* errorBuffer = 0;

	hr = D3DXCompileShaderFromFile(
		shaderFileName.c_str(),
		0,
		0,
		"Main", // entry point function name
		"vs_2_0",   //"vs_2_sw",//"vs_2_0",//"vs_1_1",

		D3DXSHADER_ENABLE_BACKWARDS_COMPATIBILITY | D3DXSHADER_DEBUG,
		&compiledCode,
		&errorBuffer,
		&tempShader->constTable);

	// output any error messages
	if (errorBuffer)
	{
		::MessageBox(0, (char*)errorBuffer->GetBufferPointer(), 0, 0);
		errorBuffer->Release();
	}

	if (FAILED(hr))
	{
		::MessageBox(0, "D3DXCompileShaderFromFile() - FAILED", 0, 0);
		return;
	}

	hr = device->CreateVertexShader(
		(DWORD*)compiledCode->GetBufferPointer(),
		&tempShader->shader);

	if (FAILED(hr))
	{
		::MessageBox(0, "CreateVertexShader - FAILED", 0, 0);
		return;
	}

	if (compiledCode)
		compiledCode->Release();

	tempShader->addHandle("ViewProjectionMatrix");
	tempShader->addHandle("WorldMatrix");
	tempShader->addHandle("CameraPosition");
	tempShader->addHandle("ScaleFactor");
	tempShader->addHandle("Texture");
	tempShader->addDescription("Texture");
	//viewProjectionMatrixHandle = constTable->GetConstantByName(0, "ViewProjectionMatrix");
	//worldMatrixHandle = constTable->GetConstantByName(0, "WorldMatrix");
	//cameraPositionHandle = constTable->GetConstantByName(0, "CameraPosition");
	//scaleFactorHandle = constTable->GetConstantByName(0, "ScaleFactor");
	//textureHandle = constTable->GetConstantByName(0, "Texture");
	//unsigned int uint;
	//constTable->GetConstantDesc(textureHandle, &textureDesc, &uint);


	// 최종적으로 등록

	// 만약 이미 등록되어있다면 지워주고 리턴
	if (ShaderTable.find(shaderFileName) != ShaderTable.end())
	{
		delete tempShader;
		return;
	}

	// 등록되어있지 않다면 등록해준다. // 처음 등록하는 것이기 때문에 새로 할당된다.
	ShaderTable[shaderFileName] = tempShader;

}

void ShaderContainer::InitFireExplosionShader(IDirect3DDevice9 * device)
{
	if (!device) return;

	// 쉐이더 목록 // 파일이름으로 저장해준다. 여기서는 쓰지않지만 그냥해준다.
	string shaderFileName = "FireExplosion.vs";
	ShaderFileNames.push_back(shaderFileName);


	ShaderInfo * tempShader = new ShaderInfo;
	tempShader->device = device;

	HRESULT hr = 0;

	D3DVERTEXELEMENT9 decl[] =
	{
		{ 0,  0, D3DDECLTYPE_FLOAT3, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_POSITION, 0 },
		{ 0, 12, D3DDECLTYPE_FLOAT4, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_COLOR,   0 },
		D3DDECL_END()
	};


	hr = device->CreateVertexDeclaration(decl, &tempShader->declaration);
	if (FAILED(hr))
	{
		::MessageBox(0, "CreateVertexDeclaration() - FAILED", 0, 0);
		return;
	}


	// 쉐이더 컴파일 / 핸들 받아오기
	ID3DXBuffer* compiledCode = 0;
	ID3DXBuffer* errorBuffer = 0;

	hr = D3DXCompileShaderFromFile(
		shaderFileName.c_str(),
		0,
		0,
		"Main", // entry point function name
		"vs_2_0",   //"vs_2_sw",//"vs_2_0",//"vs_1_1",

		D3DXSHADER_ENABLE_BACKWARDS_COMPATIBILITY | D3DXSHADER_DEBUG,
		&compiledCode,
		&errorBuffer,
		&tempShader->constTable);

	// output any error messages
	if (errorBuffer)
	{
		::MessageBox(0, (char*)errorBuffer->GetBufferPointer(), 0, 0);
		errorBuffer->Release();
	}

	if (FAILED(hr))
	{
		::MessageBox(0, "D3DXCompileShaderFromFile() - FAILED", 0, 0);
		return;
	}

	hr = device->CreateVertexShader(
		(DWORD*)compiledCode->GetBufferPointer(),
		&tempShader->shader);

	if (FAILED(hr))
	{
		::MessageBox(0, "CreateVertexShader - FAILED", 0, 0);
		return;
	}

	if (compiledCode)
		compiledCode->Release();

	tempShader->addHandle("ViewProjectionMatrix");
	tempShader->addHandle("Texture");
	tempShader->addDescription("Texture");


	// 최종적으로 등록

	// 만약 이미 등록되어있다면 지워주고 리턴
	if (ShaderTable.find(shaderFileName) != ShaderTable.end())
	{
		delete tempShader;
		return;
	}

	// 등록되어있지 않다면 등록해준다. // 처음 등록하는 것이기 때문에 새로 할당된다.
	ShaderTable[shaderFileName] = tempShader;
}
