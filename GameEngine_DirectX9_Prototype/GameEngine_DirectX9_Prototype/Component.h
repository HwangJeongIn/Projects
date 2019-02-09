#ifndef COMPONENT_H
#define COMPONENT_H

#include "Utility.h"
#include <vector>
#include "d3dUtility.h"

class Transform;
class GameObject;

class Component
{
private:
	// �ܺο��� ���Ի����� �Ͼ� �� �� �ֱ� ������ ���۷��������� ���� �ʾҴ�. // ���� �˾ƺ�����


protected:
	// GameObjectŬ���������� Components�� ����ؼ� ������Ʈ ������� �ϱ� ������ friend ����Ͽ���.
	friend class GameObject;
	virtual void update() {}
	virtual void fixedUpdate() {}
	virtual void start() { cout << "Component start" << endl; }
	virtual void onDestroy(){}

	GameObject * gameObject;
	Transform * transform;

public:
	Component(GameObject * go, Transform * tf)
		: gameObject(go), transform(tf) 
	{
		start();
	}

	virtual ~Component()
	{
		onDestroy();
	}

	// ���������� ����ؼ� �ٲ�� �ϱ� ������ const������ ������ �ʾҴ�.
	// ex) ��ġ���� ���� > GameObject�������� + Transform��������


	// read only version
	/*
	const Transform * getTransform_c() const { return transform; }
	const GameObject * getGameObject_c() const { return gameObject; }
	*/


};

class Transform : public Component
{
private:
	Vector3 position;
	Vector3 rotation;
	Vector3 scale;

protected:
	virtual void start() { cout << "Transform start" << endl; }

public:
	Transform(GameObject * go, 
		const Vector3 & position = Vector3::Zero, const Vector3 & rotation = Vector3::Zero, const Vector3 & scale = Vector3::One )
		: position(position), rotation(rotation), scale(scale), Component(go, this)
	{
		start();
	}

	// ���⼭ const��ü�� �Է��Ķ���ͷ� �޾ƿ� �� ���� ������ Component��ü�� �ʱ�ȭ�Ҷ� �ʿ��������� ����ؼ� ����Ǵ� �����̱� ������
	// const Transform & other�� ������ Component�� �ʱ�ȭ�Ҷ� Transform������ GameObject�� Transform�� read only�� �ۿ� �޾ƿ� �� ����.
	Transform(Transform & other)
		: position(other.position), rotation(other.rotation), scale(other.scale), Component(other.gameObject, this) {}

	virtual ~Transform()
	{
		onDestroy();
	}

	const Vector3 & getPosition() const { return position; }
	const Vector3 & getRotation() const { return rotation; }
	const Vector3 & getScale() const { return scale; }

	void setPostition(const Vector3 & other) { position = other; }
	void setRotation(const Vector3 & other) { rotation = other; }
	void setScale(const Vector3 & other) { scale = other; }
	void setTransform(const Vector3 & position, const Vector3 & rotation, const Vector3 & scale)
	{
		this->position = position;
		this->rotation = rotation;
		this->scale = scale;
	}

	void setTransform(const Transform & other)
	{
		position = other.position;
		rotation = other.rotation;
		scale = other.scale;
	}

};

class MeshRenderer : public Component
{
private:
	ID3DXMesh*  mesh;
	vector<D3DMATERIAL9> mtrls;
	vector<IDirect3DTexture9*> textures;
	IDirect3DDevice9* Device;

	void draw()
	{
		for (int i = 0; i < mtrls.size(); ++i)
		{
			Device->SetMaterial(&mtrls[i]);
			Device->SetTexture(0, textures[i]);
			mesh->DrawSubset(i);
		}

	}

	// �Ӽ����� �ﰢ���� �����ϰ� �Ӽ����̺��� ���� | ������ �ʴ� �ε����� ���ؽ� ���� | ���ؽ� ĳ���� ��Ʈ�� ����
	static const unsigned long DefaultOptimizeFlag = D3DXMESHOPT_ATTRSORT | D3DXMESHOPT_COMPACT | D3DXMESHOPT_VERTEXCACHE;

protected:
	virtual void update()
	{
		draw();
	}
	virtual void onDestroy()
	{
		d3d::Release<ID3DXMesh*>(mesh);

		// �ؽ�ó�� �Ҵ�� ���̰� ������ ����� ���̴�.
		for (int i = 0; i < textures.size(); i++)
			d3d::Release<IDirect3DTexture9*>(textures[i]);
	}


public:

	MeshRenderer(GameObject * go, Transform * tf, IDirect3DDevice9* Device, const string & fbxFileName)
		: Component(go, tf)
	{
		if (!Device) return;

		HRESULT hr = 0;

		ID3DXBuffer* adjBuffer = 0;
		ID3DXBuffer* mtrlBuffer = 0;
		unsigned long numMtrls = 0;

		// X������ �д´�.
		hr = D3DXLoadMeshFromX(
			fbxFileName.c_str(),
			D3DXMESH_MANAGED,
			Device,
			&adjBuffer,
			&mtrlBuffer,
			0,
			&numMtrls,
			&mesh);

		if (FAILED(hr))
		{
			::MessageBox(0, "GenMesh - FAILED", 0, 0);
			return;
		}

		// ������ ������ ���������� ó��
		setMtrlsAndTextures(mtrlBuffer, numMtrls);

		// ���Ǿ��� �������۸� ����ش�.
		d3d::Release<ID3DXBuffer*>(mtrlBuffer);

		optimizeMesh(adjBuffer);

		// ���Ϸ��� �������� ������
		d3d::Release<ID3DXBuffer*>(adjBuffer); // done w/ buffer
	}

	void setMtrlsAndTextures(ID3DXBuffer * mtrlBuffer, unsigned long numMtrls)
	{
		if (mtrlBuffer == 0 || numMtrls == 0) return;


		// D3DXMATERAL �������� �������� Ÿ��ĳ������ �ʿ��ϴ�.
		D3DXMATERIAL* pMtrls = (D3DXMATERIAL*)mtrlBuffer->GetBufferPointer();

		for (int i = 0; i < numMtrls; i++)
		{
			// �ε�ɶ� ambient���� ������ �����Ƿ� �̸� �����Ѵ�.
			pMtrls[i].MatD3D.Ambient = pMtrls[i].MatD3D.Diffuse;

			mtrls.push_back(pMtrls[i].MatD3D);

			if (pMtrls[i].pTextureFilename != 0)
			{
				// �����̸��� ���������� �� �������� �� ���ϸ����� ������ ��� �ؽ�ó�� �ε��Ѵ�.
				IDirect3DTexture9* tex = 0;
				D3DXCreateTextureFromFile(
					Device,
					pMtrls[i].pTextureFilename,
					&tex);

				// �ؽ�ó�� ������ �� �ؽ�ó�� �־��ְ�
				textures.push_back(tex);
			}
			else
			{
				// �������� ����°� ���� �ε��� ��ȣ�� �����ֱ� ���ؼ� �ΰ��� �־��ش�.
				textures.push_back(0);
			}
		}

	}

	void optimizeMesh(ID3DXBuffer * adjBuffer, unsigned long optimizeFlag = DefaultOptimizeFlag)
	{
		HRESULT hr;
		hr = mesh->OptimizeInplace(
			optimizeFlag,
			(unsigned long*)adjBuffer->GetBufferPointer(),
			0, 0, 0);

		if (FAILED(hr))
			::MessageBox(0, "OptimizeMesh - FAILED", 0, 0);
			
		
	}


	virtual ~MeshRenderer()
	{
		onDestroy();
	}
};

class MoveScript : public Component
{
private:

protected:
	virtual void update();
	virtual void start();

public:
	MoveScript(GameObject * go, Transform * tf)
		: Component(go, tf)
	{
		// ���࿡ �׳� Component Ŭ���� ������ �Լ��ٵ𿡼� start()�Լ� ȣ��?
		// �����Ǵ� �����̱� ������ �������̵��� ������� �ʴ´� // �θ�����Ϸ�(�̰������� �ڽ��� �������Լ��� ã��������.) > �ڽĻ����Ϸ�
		start();
	}

	virtual ~MoveScript()
	{
		onDestroy();
	}

};




#endif