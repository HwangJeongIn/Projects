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
	// 외부에서 삽입삭제가 일어 날 수 있기 때문에 레퍼런스형으로 하지 않았다. // 좀더 알아봐야함


protected:
	// GameObject클래스내에서 Components를 계속해서 업데이트 시켜줘야 하기 때문에 friend 등록하였다.
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
	{}
	virtual ~Component()
	{
		onDestroy();
	}

	// 내부적으로 계속해서 바꿔야 하기 때문에 const선언을 해주지 않았다.
	// ex) 위치정보 변경 > GameObject정보변경 + Transform정보변경


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



	Vector3 right;
	Vector3 up;
	Vector3 forward;

	static const D3DXVECTOR3 WorldRight_DX;
	static const D3DXVECTOR3 WorldUp_DX;
	static const D3DXVECTOR3 WorldForward_DX;


	// 현재 위치의 최신화 여부를 알기 위한 더티플래그
	bool dirty;
protected:
	// virtual void start() { cout << "Transform start" << endl; }

public:
	Transform(GameObject * go,
		const Vector3 & position = Vector3::Zero, const Vector3 & rotation = Vector3::Zero, const Vector3 & scale = Vector3::One)
		: position(position), rotation(rotation), scale(scale), right{1,0,0}, up{ 0,1,0 }, forward{ 0,0,1 }, dirty(true), Component(go, this)
	{
		start();
		// 절댓값 360 * n 날린다 + 이객체의 right up forward 벡터 계산
		setRotation(rotation);

	}

	// 여기서 const객체를 입력파라미터로 받아올 수 없는 이유는 Component객체를 초기화할때 필요한정보는 계속해서 변경되는 정보이기 때문에
	// const Transform & other로 받으면 Component를 초기화할때 Transform내부의 GameObject와 Transform를 read only로 밖에 받아올 수 없다.
	Transform(Transform & other)
		: position(other.position), rotation(other.rotation), scale(other.scale), Component(other.gameObject, this) {}

	virtual ~Transform()
	{
		onDestroy();
	}

	const Vector3 & getPosition() const { return position; }
	const Vector3 & getRotation() const { return rotation; }
	const Vector3 & getScale() const { return scale; }

	const Vector3 & getRight() const { return right; }
	const Vector3 & getForward() const { return forward; }
	const Vector3 & getUp() const { return up; }

	Vector3 forword() const
	{
		D3DXVECTOR3 forward_DX{ 0,0,1 };
	}

	void transformUpdate(bool dirty);

	void setDirectionVectorWithRotation_DX()
	{
		D3DXMATRIX T;

		D3DXVECTOR3 right_DX{ 1,0,0 };
		D3DXVECTOR3 up_DX{ 0,1,0 };
		D3DXVECTOR3 forward_DX{ 0,0,1 };


		// 월드 방향 벡터기준으로 각도만큼 각각을 회전시켜서 객체의 방향벡터를 구해준다.
		// 월드벡터 기준 pitch
		D3DXMatrixRotationAxis(&T, &WorldRight_DX, rotation.getX());
		D3DXVec3TransformCoord(&up_DX, &up_DX, &T);
		D3DXVec3TransformCoord(&forward_DX, &forward_DX, &T);

		// 월드벡터 기준 yaw
		D3DXMatrixRotationAxis(&T, &WorldUp_DX, rotation.getY());
		D3DXVec3TransformCoord(&right_DX, &right_DX, &T);
		D3DXVec3TransformCoord(&forward_DX, &forward_DX, &T);

		// 월드벡터 기준 roll
		D3DXMatrixRotationAxis(&T, &WorldForward_DX, rotation.getZ());
		D3DXVec3TransformCoord(&up_DX, &up_DX, &T);
		D3DXVec3TransformCoord(&right_DX, &right_DX, &T);

		Vector3::ToVector3(right, right_DX);
		Vector3::ToVector3(up, up_DX);
		Vector3::ToVector3(forward, forward_DX);

	}

	/*
		void pitch_DX(float angle)
	{
		D3DXMATRIX T;
		D3DXVECTOR3 right_DX;
		D3DXVECTOR3 up_DX;
		D3DXVECTOR3 forward_DX;

		Vector3::ToD3DXVECTOR3(right_DX, right);
		Vector3::ToD3DXVECTOR3(up_DX, up);
		Vector3::ToD3DXVECTOR3(forward_DX, forward);

		D3DXMatrixRotationAxis(&T, &right_DX, angle);

		D3DXVec3TransformCoord(&up_DX, &up_DX, &T);
		D3DXVec3TransformCoord(&forward_DX, &forward_DX, &T);

		Vector3::ToVector3(up, up_DX);
		Vector3::ToVector3(forward, forward_DX);
	}

	void yaw_DX(float angle)
	{
		D3DXMATRIX T;
		D3DXVECTOR3 right_DX;
		D3DXVECTOR3 up_DX;
		D3DXVECTOR3 forward_DX;

		Vector3::ToD3DXVECTOR3(right_DX, right);
		Vector3::ToD3DXVECTOR3(up_DX, up);
		Vector3::ToD3DXVECTOR3(forward_DX, forward);

		D3DXMatrixRotationAxis(&T, &up_DX, angle);

		D3DXVec3TransformCoord(&right_DX, &right_DX, &T);
		D3DXVec3TransformCoord(&forward_DX, &forward_DX, &T);

		Vector3::ToVector3(right, right_DX);
		Vector3::ToVector3(forward, forward_DX);
	}

	void roll_DX(float angle)
	{
		D3DXMATRIX T;
		D3DXVECTOR3 right_DX;
		D3DXVECTOR3 up_DX;
		D3DXVECTOR3 forward_DX;

		Vector3::ToD3DXVECTOR3(right_DX, right);
		Vector3::ToD3DXVECTOR3(up_DX, up);
		Vector3::ToD3DXVECTOR3(forward_DX, forward);

		D3DXMatrixRotationAxis(&T, &forward_DX, angle);

		D3DXVec3TransformCoord(&up_DX, &up_DX, &T);
		D3DXVec3TransformCoord(&right_DX, &right_DX, &T);

		Vector3::ToVector3(up, up_DX);
		Vector3::ToVector3(right, right_DX);
	}
	*/



	float constrainLessThan360(float num)
	{
		// 절댓값이 360이 넘어가면 제한한다.
		int quotient = num / 360;
		num += -(360 * quotient);
		return num;
	}

	void setPosition(const Vector3 & other) { position = other; dirty = true; }
	void setRotation(const Vector3 & other) 
	{
		rotation.setX(constrainLessThan360(other.getX())); 
		rotation.setY(constrainLessThan360(other.getY()));
		rotation.setZ(constrainLessThan360(other.getZ()));
		setDirectionVectorWithRotation_DX();
		dirty = true;
	}
	void setScale(const Vector3 & other) { scale = other; }

	void setPosition(float x, float y, float z) 
	{ 
		position.setX(constrainLessThan360(x));
		position.setY(constrainLessThan360(y));
		position.setZ(constrainLessThan360(z));
		dirty = true;
	}
	void setRotation(float x, float y, float z)
	{
		rotation.setX(constrainLessThan360(x));
		rotation.setY(constrainLessThan360(y)); 
		rotation.setZ(constrainLessThan360(z));
		setDirectionVectorWithRotation_DX();
		dirty = true;
	}
	void setScale(float x, float y, float z) { scale = { x,y,z }; }

	void setTransform(const Vector3 & position, const Vector3 & rotation, const Vector3 & scale)
	{
		setPosition(position);
		setRotation(rotation);
		setScale(scale);
	}

	void setTransform(const Transform & other)
	{
		setPosition(other.position);
		setRotation(other.rotation);
		setScale(other.scale);
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

	// 속성으로 삼각형을 정렬하고 속성테이블을 생성 | 사용되지 않는 인덱스와 버텍스 제거 | 버텍스 캐시의 히트율 높임
	static const unsigned long DefaultOptimizeFlag = D3DXMESHOPT_ATTRSORT | D3DXMESHOPT_COMPACT | D3DXMESHOPT_VERTEXCACHE;

protected:
	virtual void update()
	{
		draw();
	}
	virtual void onDestroy()
	{
		d3d::Release<ID3DXMesh*>(mesh);

		// 텍스처만 할당된 값이고 재질은 복사된 값이다.
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

		// X파일을 읽는다.
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

		// 재질이 있을때 내부적으로 처리
		setMtrlsAndTextures(mtrlBuffer, numMtrls);

		// 사용되었던 재질버퍼를 비워준다.
		d3d::Release<ID3DXBuffer*>(mtrlBuffer);

		optimizeMesh(adjBuffer);

		// 사용완료한 인접버퍼 릴리즈
		d3d::Release<ID3DXBuffer*>(adjBuffer); // done w/ buffer
	}

	void setMtrlsAndTextures(ID3DXBuffer * mtrlBuffer, unsigned long numMtrls)
	{
		if (mtrlBuffer == 0 || numMtrls == 0) return;


		// D3DXMATERAL 형식으로 읽으려면 타입캐스팅이 필요하다.
		D3DXMATERIAL* pMtrls = (D3DXMATERIAL*)mtrlBuffer->GetBufferPointer();

		for (int i = 0; i < numMtrls; i++)
		{
			// 로드될때 ambient값을 가지지 않으므로 이를 지정한다.
			pMtrls[i].MatD3D.Ambient = pMtrls[i].MatD3D.Diffuse;

			mtrls.push_back(pMtrls[i].MatD3D);

			if (pMtrls[i].pTextureFilename != 0)
			{
				// 파일이름이 정상적으로 들어가 있을때만 그 파일명으로 파일을 열어서 텍스처를 로드한다.
				IDirect3DTexture9* tex = 0;
				D3DXCreateTextureFromFile(
					Device,
					pMtrls[i].pTextureFilename,
					&tex);

				// 텍스처가 있을때 그 텍스처를 넣어주고
				textures.push_back(tex);
			}
			else
			{
				// 없을때도 서브셋과 같은 인덱스 번호를 맞춰주기 위해서 널값을 넣어준다.
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
		// 만약에 그냥 Component 클래스 생성자 함수바디에서 start()함수 호출?
		// 생성되는 과정이기 때문에 오버라이딩이 적용되지 않는다 // 부모생성완료(이과정에서 자식의 재정의함수를 찾을수없다.) > 자식생성완료
		start();
	}
	
	virtual ~MoveScript()
	{
		onDestroy();
	}

};

//class Scene;
//enum MainObjTag;
// 메인 카메라 오브젝트가 가지는 컴포넌트로 생성과 삭제시 메인 오브젝트 테이블에 등록 삭제를 해준다.
class MainCamera : public Component
{
private :
	/*Scene::MainObjTag*/unsigned char mainObjTag;
	virtual void update();
protected :
public :
	MainCamera(GameObject * go, Transform * tf);
	virtual ~MainCamera();
	void setViewSpace();
	void getViewMatrix(D3DXMATRIX * V);
};


#endif