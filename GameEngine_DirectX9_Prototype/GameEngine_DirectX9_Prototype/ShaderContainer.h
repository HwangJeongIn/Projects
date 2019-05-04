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
	// ���̴��� ������ ����ִ� ����ü�̴�.
	struct ShaderInfo
	{
		// ���� �ȳ������� ������ �ܺο��� ���ΰ����� �ٲٰ���������� device�� �Ѱ�����ҰͰ��Ƽ�
		// �׳� ���������� ������ �ִ� ������ ó�����־���.

		// ������������ ������ ���۷��������� ������ �������ֱ�
		// ���������϶��� �ٸ�ó���� ���ټ� �����Ͱ��Ƽ� �ϴ� ������������ ����
		IDirect3DDevice9 * device;

		IDirect3DVertexDeclaration9* declaration;
		IDirect3DVertexShader9* shader;
		ID3DXConstantTable* constTable;

		// ���������� �ʿ��� �ڵ��� ��� �ִ� ���̺��̴�.
		// ���̴� ���� �̸��� Ű������ ������ �ִ�.
		map<string, D3DXHANDLE> handleTable;

		// description ������ ������ �ִ� ���̺� 
		// �ڵ�� ���������Ƿ� �ڵ��� Ű������ �޴´�.
		map<D3DXHANDLE, D3DXCONSTANT_DESC> descriptionTable;

		// ó������ ������ nullptr�� �������ش�.
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

			// ���� ��ϵǾ������� ����Եǰ� / ��ϵǾ����� �������� ���� �����ȴ�.
			handleTable[variableName] = variableHandle;
		}

		void addDescription(const string & variableName)
		{
			if (!device) return;

			// ���� �ڵ��� ��ϵǾ��ִ��� Ȯ���Ѵ�.
			map<string, D3DXHANDLE>::iterator it = handleTable.find(variableName);
			// �ڵ��� ��ϵǾ� ���� �ʴٸ� �����Ѵ�.
			if (it == handleTable.end()) return;

			D3DXHANDLE variableHandle = (*it).second;

			D3DXCONSTANT_DESC textureDesc{};
			unsigned int uint;
			constTable->GetConstantDesc(variableHandle, &textureDesc, &uint);

			device->SetSamplerState(textureDesc.RegisterIndex, D3DSAMP_MAGFILTER, D3DTEXF_LINEAR);
			device->SetSamplerState(textureDesc.RegisterIndex, D3DSAMP_MINFILTER, D3DTEXF_LINEAR);
			device->SetSamplerState(textureDesc.RegisterIndex, D3DSAMP_MIPFILTER, D3DTEXF_LINEAR);

			// ���������� �־��ش�.
			// ���� ��ϵǾ������� ����Եǰ� / ��ϵǾ����� �������� ���� �����ȴ�.
			descriptionTable[variableHandle] = textureDesc;
		}

	};
private :
	
	static IDirect3DDevice9 * Device;
	// ���� ���̴� ���ϵ��� �̸��� �������ش�.
	static vector<string> ShaderFileNames;
	// �����̸����� ���̴� ������ ��� �ִ� ���̴� ���̺�
	static map<string, ShaderInfo *> ShaderTable;

	static D3DXHANDLE * GetHandle(const string & shaderFileName, const string & variableName)
	{
		map<string, ShaderInfo *>::iterator it = ShaderTable.find(shaderFileName);
		// ���࿡ ���̴� �����̸����� ���̴��� ��ϵǾ� ���� �ʴٸ� ����
		if (it == ShaderTable.end()) return nullptr;

		map<string, D3DXHANDLE>::iterator it2 = (*it).second->handleTable.find(variableName);
		// ���࿡ ���̴� �������� �����̸����� �ڵ��� ã������ ���ٸ� ����
		if (it2 == (*it).second->handleTable.end()) return nullptr;

		return &(*it2).second;
	}

	static D3DXCONSTANT_DESC * GetDescription(const string & shaderFileName, const string & variableName)
	{
		map<string, ShaderInfo *>::iterator it = ShaderTable.find(shaderFileName);
		// ���࿡ ���̴� �����̸����� ���̴��� ��ϵǾ� ���� �ʴٸ� ����
		if (it == ShaderTable.end()) return nullptr;

		map<string, D3DXHANDLE>::iterator it2 = (*it).second->handleTable.find(variableName);
		// ���࿡ ���̴� �������� �����̸����� �ڵ��� ã������ ���ٸ� ����
		if (it2 == (*it).second->handleTable.end()) return nullptr;

		map<D3DXHANDLE, D3DXCONSTANT_DESC>::iterator it3 = (*it).second->descriptionTable.find((*it2).second);
		// ���࿡ ���̴� �������� �ڵ�� ��ũ������ ã������ ���ٸ� ����
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
		// ���������� ���ؽ� ����� ���̴������� ���ش�.
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
		// ���α׷��� ������ ���Լ��� ȣ���ؼ� ���̴��� ��� ������ ���ش�.
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

		// �ؽ�ó ����
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

		// ���������� ����
		D3DXHANDLE * handle = GetHandle(shaderFileName, "ViewProjectionMatrix");
		if (handle)
		{
			(*it).second->constTable->SetMatrix(
				ShaderContainer::Device,
				*handle,
				&viewProjectionMatrix);
		}
		// �ؽ�ó ����
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