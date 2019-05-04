#ifndef SHADERCONTAINER_H
#define SHADERCONTAINER_H

#include <iostream>
#include <map>
#include <vector>
#include <string>
#include <d3dx9.h>
#include "Utility.h"

using namespace std;

class ShaderContainer
{
public :
	// 쉐이더의 정보를 담아주는 구조체이다.
	struct ShaderInfo
	{
		// 원래 안넣으려고 했지만 외부에서 내부값들을 바꾸고싶을때마다 device를 넘겨줘야할것같아서
		// 그냥 내부적으로 가지고 있는 것으로 처리해주었다.

		// 포인터형으로 해줄지 레퍼런스형으로 해줄지 결정해주기
		// 널포인터일때도 다른처리를 해줄수 있을것같아서 일단 포인터형으로 설정
		IDirect3DDevice9 * device;

		IDirect3DVertexDeclaration9* declaration;
		IDirect3DVertexShader9* shader;
		ID3DXConstantTable* constTable;

		// 내부적으로 필요한 핸들을 담고 있는 테이블이다.
		// 쉐이더 변수 이름을 키값으로 가지고 있다.
		map<string, D3DXHANDLE> handleTable;

		// description 정보를 가지고 있는 테이블 
		// 핸들과 관련있으므로 핸들을 키값으로 받는다.
		map<D3DXHANDLE, D3DXCONSTANT_DESC> descriptionTable;

		// 처음값은 무조건 nullptr로 설정해준다.
		ShaderInfo()
			: declaration(nullptr), shader(nullptr), constTable(nullptr), device(nullptr) {}
		~ShaderInfo()
		{
			if (declaration)
				declaration->Release();

			if (constTable)
				constTable->Release();

			if (shader)
				shader->Release();
		}

		void selectShader()
		{
			if (!device) return;

			device->SetVertexDeclaration(declaration);
			device->SetVertexShader(shader);

		}

		void addHandle(const string & variableName)
		{
			if (!device) return;

			D3DXHANDLE variableHandle = constTable->GetConstantByName(0, variableName.c_str());

			// 만약 등록되어있으면 덮어쓰게되고 / 등록되어있지 않은경우는 새로 생성된다.
			handleTable[variableName] = variableHandle;
		}

		void addDescription(const string & variableName)
		{
			if (!device) return;

			// 먼저 핸들이 등록되어있는지 확인한다.
			map<string, D3DXHANDLE>::iterator it = handleTable.find(variableName);
			// 핸들이 등록되어 있지 않다면 리턴한다.
			if (it == handleTable.end()) return;

			D3DXHANDLE variableHandle = (*it).second;

			D3DXCONSTANT_DESC textureDesc{};
			unsigned int uint;
			constTable->GetConstantDesc(variableHandle, &textureDesc, &uint);

			device->SetSamplerState(textureDesc.RegisterIndex, D3DSAMP_MAGFILTER, D3DTEXF_LINEAR);
			device->SetSamplerState(textureDesc.RegisterIndex, D3DSAMP_MINFILTER, D3DTEXF_LINEAR);
			device->SetSamplerState(textureDesc.RegisterIndex, D3DSAMP_MIPFILTER, D3DTEXF_LINEAR);

			// 최종적으로 넣어준다.
			// 만약 등록되어있으면 덮어쓰게되고 / 등록되어있지 않은경우는 새로 생성된다.
			descriptionTable[variableHandle] = textureDesc;
		}

	};
private :
	
	static IDirect3DDevice9 * Device;
	// 먼저 쉐이더 파일들의 이름을 정의해준다.
	static vector<string> ShaderFileNames;
	// 파일이름으로 쉐이더 정보를 담고 있는 쉐이더 테이블
	static map<string, ShaderInfo *> ShaderTable;

	static D3DXHANDLE * GetHandle(const string & shaderFileName, const string & variableName)
	{
		map<string, ShaderInfo *>::iterator it = ShaderTable.find(shaderFileName);
		// 만약에 쉐이더 파일이름으로 쉐이더가 등록되어 있지 않다면 리턴
		if (it == ShaderTable.end()) return nullptr;

		map<string, D3DXHANDLE>::iterator it2 = (*it).second->handleTable.find(variableName);
		// 만약에 쉐이더 정보에서 변수이름으로 핸들을 찾았지만 없다면 리턴
		if (it2 == (*it).second->handleTable.end()) return nullptr;

		return &(*it2).second;
	}

	static D3DXCONSTANT_DESC * GetDescription(const string & shaderFileName, const string & variableName)
	{
		map<string, ShaderInfo *>::iterator it = ShaderTable.find(shaderFileName);
		// 만약에 쉐이더 파일이름으로 쉐이더가 등록되어 있지 않다면 리턴
		if (it == ShaderTable.end()) return nullptr;

		map<string, D3DXHANDLE>::iterator it2 = (*it).second->handleTable.find(variableName);
		// 만약에 쉐이더 정보에서 변수이름으로 핸들을 찾았지만 없다면 리턴
		if (it2 == (*it).second->handleTable.end()) return nullptr;

		map<D3DXHANDLE, D3DXCONSTANT_DESC>::iterator it3 = (*it).second->descriptionTable.find((*it2).second);
		// 만약에 쉐이더 정보에서 핸들로 디스크립션을 찾았지만 없다면 리턴
		if (it3 == (*it).second->descriptionTable.end()) return nullptr;

		return &(*it3).second;
	}

	static void DestroyAllShader()
	{
		for (auto it = ShaderTable.begin(); it != ShaderTable.end(); ++it)
		{
			if ((*it).second != nullptr)
			{
				delete (*it).second;

			}
			(*it).second = nullptr;

		}

		ShaderFileNames.clear();
		ShaderTable.clear();
	}
	static bool SetShader(const string & shaderFileName)
	{
		map<string, ShaderInfo *>::iterator it = ShaderTable.find(shaderFileName);
		if (it == ShaderTable.end()) return false;
		// 내부적으로 버텍스 선언과 쉐이더설정을 해준다.
		(*it).second->selectShader();
		return true;
	}
	static void InitFbxModelRendererWithAnimationShader(IDirect3DDevice9 * device);
	static void InitFireExplosionShader(IDirect3DDevice9 * device);
public :


	static void InitAllShader(IDirect3DDevice9 * device)
	{
		if (!device) return;

		ShaderContainer::Device = device;
		// 프로그램이 끝나면 이함수를 호출해서 쉐이더를 모두 릴리즈 해준다.
		atexit(DestroyAllShader);

		InitFbxModelRendererWithAnimationShader(device);
		InitFireExplosionShader(device);
	}


	static bool SetFbxModelRendererWithAnimationShaderParameters(IDirect3DTexture9* texture, const D3DXMATRIX & viewProjectionMatrix,
		const D3DXMATRIX & worldMatrix, const Vector3 & cameraPosition, const Vector3 & scaleFactor)
	{
		string shaderFileName = "FbxModelRendererWithAnimation.vs";
		map<string, ShaderInfo *>::iterator it = ShaderTable.find(shaderFileName);
		if (it == ShaderTable.end()) return false;

		D3DXHANDLE * handle = GetHandle(shaderFileName, "ViewProjectionMatrix");
		if (handle)
		{
			(*it).second->constTable->SetMatrix(
				ShaderContainer::Device,
				*handle,
				&viewProjectionMatrix);
		}

		handle = GetHandle(shaderFileName, "WorldMatrix");
		if (handle)
		{
			(*it).second->constTable->SetMatrix(
				ShaderContainer::Device,
				*handle,
				&worldMatrix);
		}

		handle = GetHandle(shaderFileName, "CameraPosition");
		if (handle)
		{
			(*it).second->constTable->SetVector
			(
				ShaderContainer::Device,
				*handle,
				&D3DXVECTOR4(cameraPosition.getX(), cameraPosition.getY(), cameraPosition.getZ(), 1)
			);
		}

		handle = GetHandle(shaderFileName, "ScaleFactor");
		if (handle)
		{
			(*it).second->constTable->SetVector
			(
				ShaderContainer::Device,
				*handle,
				&D3DXVECTOR4(scaleFactor.getX(), scaleFactor.getY(), scaleFactor.getZ(), 0)
			);
		}

		// 텍스처 설정
		D3DXCONSTANT_DESC * textureDesc = GetDescription(shaderFileName, "Texture");
		if (textureDesc && texture)
		{
			Device->SetTexture(textureDesc->RegisterIndex, texture);
		}
		return true;

	}

	static bool SetFbxModelRendererWithAnimationShader()
	{
		string shaderFileName = "FbxModelRendererWithAnimation.vs";
		return SetShader(shaderFileName);
	}
	static bool SetFireExplosionShaderParameters(IDirect3DTexture9* texture, const D3DXMATRIX & viewProjectionMatrix)
	{
		string shaderFileName = "FireExplosion.vs";
		map<string, ShaderInfo *>::iterator it = ShaderTable.find(shaderFileName);
		if (it == ShaderTable.end()) return false;

		// 뷰프로젝션 설정
		D3DXHANDLE * handle = GetHandle(shaderFileName, "ViewProjectionMatrix");
		if (handle)
		{
			(*it).second->constTable->SetMatrix(
				ShaderContainer::Device,
				*handle,
				&viewProjectionMatrix);
		}
		// 텍스처 설정
		D3DXCONSTANT_DESC * textureDesc = GetDescription(shaderFileName, "Texture");
		if (textureDesc && texture)
		{
			Device->SetTexture(textureDesc->RegisterIndex, texture);
		}
		return true;
	}

	static bool SetFireExplosionShader()
	{
		string shaderFileName = "FireExplosion.vs";
		return SetShader(shaderFileName);
	}


};

#endif