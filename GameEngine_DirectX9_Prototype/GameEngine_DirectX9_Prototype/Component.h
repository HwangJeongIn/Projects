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
	{}
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
	// 3���� ���� ���࿡ root��ü��� ������ǥ�� �������� �����Ǿ��ְ�
	// child��ü��� parent��ü�������� �����Ǿ��ִ�.
	Vector3 position;
	Vector3 rotation;
	Vector3 scale;

	Vector3 right;
	Vector3 up;
	Vector3 forward;


	D3DXMATRIX transformMatrix_DX;
	D3DXMATRIX rotationMatrix_DX;
	D3DXMATRIX positionMatrix_DX;

	static const D3DXVECTOR3 WorldRight_DX;
	static const D3DXVECTOR3 WorldUp_DX;
	static const D3DXVECTOR3 WorldForward_DX;


	// ���� ��ġ�� �ֽ�ȭ ���θ� �˱� ���� ��Ƽ�÷���
	bool dirty;
protected:
	virtual void start()
	{ 		
		// ���� 360 * n ������ + �̰�ü�� right up forward ���� ���
		setRotation(rotation);
		D3DXMatrixIdentity(&transformMatrix_DX);
		D3DXMatrixIdentity(&rotationMatrix_DX);
		D3DXMatrixIdentity(&positionMatrix_DX);
	}

public:
	Transform(GameObject * go,
		const Vector3 & position = Vector3::Zero, const Vector3 & rotation = Vector3::Zero, const Vector3 & scale = Vector3::One)
		: position(position), rotation(rotation), scale(scale), right{1,0,0}, up{ 0,1,0 }, forward{ 0,0,1 }, dirty(true), Component(go, this)
	{
		start();
	}

	// ���⼭ const��ü�� �Է��Ķ���ͷ� �޾ƿ� �� ���� ������ Component��ü�� �ʱ�ȭ�Ҷ� �ʿ��������� ����ؼ� ����Ǵ� �����̱� ������
	// const Transform & other�� ������ Component�� �ʱ�ȭ�Ҷ� Transform������ GameObject�� Transform�� read only�� �ۿ� �޾ƿ� �� ����.
	Transform(Transform & other)
		: position(other.position), rotation(other.rotation), scale(other.scale), Component(other.gameObject, this) 
	{
		start();		
	}

	virtual ~Transform()
	{
		onDestroy();
	}

	static const D3DXMATRIX IdentityMatrix_DX;

	const Vector3 & getPosition() const { return position; }
	const Vector3 & getRotation() const { return rotation; }
	const Vector3 & getScale() const { return scale; }

	const Vector3 & getRight() const { return right; }
	const Vector3 & getForward() const { return forward; }
	const Vector3 & getUp() const { return up; }

	bool getDirty() const { return dirty; }

	void transformUpdate(bool dirty, const D3DXMATRIX & parentRotationMatrix, const D3DXMATRIX & parentPositionMatrix);

	const D3DXMATRIX & getTransformMatrix_DX() const { return transformMatrix_DX; }
	void setTransformMatrix_DX()
	{
		transformMatrix_DX = rotationMatrix_DX * positionMatrix_DX;
	}
	
	const D3DXMATRIX & getRotationMatrix_DX() const { return rotationMatrix_DX; }
	void setRotationMatrix_DX(const D3DXMATRIX & parentRotationMatrix)
	{
		rotationMatrix_DX = parentRotationMatrix * calcLocalRotationMatrix_DX();
	}

	const D3DXMATRIX & getPositionMatrix_DX() const { return positionMatrix_DX; }
	void setPositionMatrix_DX(const D3DXMATRIX & parentPositionMatrix)
	{
		positionMatrix_DX = parentPositionMatrix * calcLocalPositionMatrix_DX();
	}

	// �Ƹ������̼ǰ� �������� ������� �ҰͰ����� �׳� �ѹ� �غ�����
	// �� ������ �θ��� �����̼� > �������̵� > �ڽ��� �����̼� > �������̵� 
	// �ڽ��� �����̼ǿ��� ���� 0���� �߽����� ȸ���ҵ��ϴ�
	// �θ��� �����̼� ��İ��� ���� �θ��� ������ ��İ��� ���� �޾ƿ;��Ѵ�.
	D3DXMATRIX calcLocalRotationMatrix_DX()
	{
		// �� �Լ��� �ʿ��� ������ ���ñ������� ����� ���ְ� ����������� ����ϱ� ���ؼ�
		// WorldMatrix = �θ�ü�� Matrix * LocalMatrix ���ָ� �ȴ�.

		// ���࿡ ��Ʈ ��ü���
		// WorldMatrix = ������� * LocalMatrix = LocalMatrix

		// �����̼� > ������

		D3DXMATRIX T;
		D3DXMATRIX temp;
		D3DXMatrixIdentity(&T);
		D3DXMatrixMultiply(&T, &T, D3DXMatrixRotationX(&temp, rotation.getX()));
		D3DXMatrixMultiply(&T, &T, D3DXMatrixRotationY(&temp, rotation.getY()));
		D3DXMatrixMultiply(&T, &T, D3DXMatrixRotationZ(&temp, rotation.getZ()));

		return T;
	}

	D3DXMATRIX calcLocalPositionMatrix_DX()
	{
		D3DXMATRIX T;
		D3DXMatrixTranslation(&T, getPosition().getX(), getPosition().getY(), getPosition().getZ());
		return T;
	}

	// �����̼��� �ٲ𶧸��� �ֽ�ȭ �ȴ�.
	void setDirectionVectorWithRotation_DX()
	{
		D3DXMATRIX T;

		D3DXVECTOR3 right_DX{ 1,0,0 };
		D3DXVECTOR3 up_DX{ 0,1,0 };
		D3DXVECTOR3 forward_DX{ 0,0,1 };

		/*
		�����̼� ���� �����غ���
		*/

		// ���� ���� ���ͱ������� ������ŭ ������ ȸ�����Ѽ� ��ü�� ���⺤�͸� �����ش�.
		// ���庤�� ���� pitch
		D3DXMatrixRotationAxis(&T, &WorldRight_DX, rotation.getX());
		D3DXVec3TransformCoord(&up_DX, &up_DX, &T);
		D3DXVec3TransformCoord(&forward_DX, &forward_DX, &T);

		// ���庤�� ���� yaw
		D3DXMatrixRotationAxis(&T, &WorldUp_DX, rotation.getY());
		D3DXVec3TransformCoord(&right_DX, &right_DX, &T);
		D3DXVec3TransformCoord(&forward_DX, &forward_DX, &T);

		// ���庤�� ���� roll
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
		// ������ 360�� �Ѿ�� �����Ѵ�.
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

// X������ ���� �޽����� ������Ʈ
// ���߿� FBX�� ���� �޾ƿü� �ְ� ���鿹�� FBX SDK �н���
class MeshRenderer : public Component
{
private:
	ID3DXMesh*  mesh;
	vector<D3DMATERIAL9> mtrls;
	vector<IDirect3DTexture9*> textures;
	IDirect3DDevice9 * device;
	string fileName;

	// �Ӽ����� �ﰢ���� �����ϰ� �Ӽ����̺��� ���� | ������ �ʴ� �ε����� ���ؽ� ���� | ���ؽ� ĳ���� ��Ʈ�� ����
	static const unsigned long DefaultOptimizeFlag = D3DXMESHOPT_ATTRSORT | D3DXMESHOPT_COMPACT | D3DXMESHOPT_VERTEXCACHE;

	void render();
	void optimizeMesh(ID3DXBuffer * adjBuffer, unsigned long optimizeFlag = DefaultOptimizeFlag);
	void setMtrlsAndTextures(ID3DXBuffer * mtrlBuffer, unsigned long numMtrls);


protected:
	virtual void update()
	{
		render();
	}
	virtual void onDestroy()
	{
		d3d::Release<ID3DXMesh*>(mesh);

		// �ؽ�ó�� �Ҵ�� ���̰� ������ ����� ���̴�.
		for (int i = 0; i < textures.size(); i++)
			d3d::Release<IDirect3DTexture9*>(textures[i]);
	}


public:

	MeshRenderer(GameObject * go, Transform * tf);

	virtual ~MeshRenderer()
	{
		onDestroy();
	}


	void loadXFile(const string & fileName);
};

class MoveScript : public Component
{
private:

protected:
	virtual void update();

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

class MoveScript_C : public Component
{
private:

protected:
	virtual void update();

public:
	MoveScript_C(GameObject * go, Transform * tf)
		: Component(go, tf)
	{
		// ���࿡ �׳� Component Ŭ���� ������ �Լ��ٵ𿡼� start()�Լ� ȣ��?
		// �����Ǵ� �����̱� ������ �������̵��� ������� �ʴ´� // �θ�����Ϸ�(�̰������� �ڽ��� �������Լ��� ã��������.) > �ڽĻ����Ϸ�
		start();
	}

	virtual ~MoveScript_C()
	{
		onDestroy();
	}

};

//class Scene;
//enum MainObjTag;
// ���� ī�޶� ������Ʈ�� ������ ������Ʈ�� ������ ������ ���� ������Ʈ ���̺� ��� ������ ���ش�.
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