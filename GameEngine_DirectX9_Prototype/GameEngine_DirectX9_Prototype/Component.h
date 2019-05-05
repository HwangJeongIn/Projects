#ifndef COMPONENT_H
#define COMPONENT_H

#include <cassert>
#include "Utility.h"
#include <vector>
#include "BehaviorTree.h"


class Transform;
class GameObject;
struct GameObjectWithCollision;

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
	virtual void onCollisionStay(GameObjectWithCollision & other){}
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


	//D3DXMATRIX transformMatrix_DX;
	//D3DXMATRIX rotationMatrix_DX;
	//D3DXMATRIX positionMatrix_DX;

	D3DXMATRIX transformMatrix;
	D3DXMATRIX localTransformMatrix;
	D3DXMATRIX rotationMatrix;
	D3DXMATRIX positionMatrix;




	// ���� ��ġ�� �ֽ�ȭ ���θ� �˱� ���� ��Ƽ�÷���
	bool dirty;
protected:
	virtual void start()
	{ 		
		dirty = true;
		D3DXMatrixIdentity(&transformMatrix);
		D3DXMatrixIdentity(&localTransformMatrix);
		D3DXMatrixIdentity(&rotationMatrix);
		D3DXMatrixIdentity(&positionMatrix);

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

	static const D3DXMATRIX IdentityMatrix;
	static const D3DXVECTOR3 WorldRight;
	static const D3DXVECTOR3 WorldUp;
	static const D3DXVECTOR3 WorldForward;
	static const float Pi;

	static float DegreeToRadian(float degree)
	{
		return degree *(Pi / 180.0f);
	}

	static float RadianToDegree(float radian)
	{
		return radian *(180.0f / Pi);
	}


	// ���࿡ ������ǥ���� ���� �������� �θ�ü�� �ִٸ� ������ǥ��� ������ǥ�� ��ǥ�� ǥ���ؼ� �ʱ�ȭ �ؾ��Ѵ�. 
	// �� ���� -> ���÷��� ��ȯ�� �ʿ��ϴ�

	// �׸� ���� ������ǥ�� ������ �θ�ü�� �����Ҷ� ������ǥ�� ������ ��������ִ� ������ �ϴ� �Լ��̴�.




	//// getPosition�� ���� // getPosition�� ������ǥ ��ȯ
	//Vector3 getLocalPosition() const { return{ localTransformMatrix._41, localTransformMatrix._42, localTransformMatrix._43 }; }

	Vector3 getWorldPosition() const { return{ transformMatrix._41, transformMatrix._42, transformMatrix._43 }; }

	// ���濹��/////////////////////////////////////////////////////////////// �ӽ�
	Vector3 getWorldRotation() const
	{
		// ���� transformation matrix�� �и����Ѽ� Quaternion�� ������ش�.
		D3DXQUATERNION q;
		D3DXVECTOR3 scale;
		D3DXVECTOR3 translation;

		D3DXMatrixDecompose(&scale, &q, &translation, &transformMatrix);

		//x
		float pitch = 0.0f;
		//y
		float yaw = 0.0f;
		//z
		float roll = 0.0f;


		float sqw = q.w*q.w;
		float sqx = q.x*q.x;
		float sqy = q.y*q.y;
		float sqz = q.z*q.z;

		pitch = asinf(2.0f *(q.w * q.x - q.y * q.z));
		yaw = atan2f(2.0f *(q.x * q.z + q.w * q.y), (-sqx - sqy + sqz + sqw));
		roll = atan2f(2.0f * (q.x * q.y + q.w * q.z), (-sqx + sqy - sqz + sqw));

		//yaw = atan2(2 * q.y*q.w - 2 * q.x*q.z, 1 - 2 * q.y*q.y - 2 * q.z*q.z);
		//pitch = atan2(2 * q.x*q.w - 2 * q.y*q.z, 1 - 2 * q.x*q.x - 2 * q.z*q.z);
		//roll = asin(2 * q.x*q.y + 2 * q.z*q.w);

		//roll = asin(2 * q.x*q.y + 2 * q.z*q.w);
		//pitch =atan2(2 * q.x*q.w + 2 * q.y*q.z, 1 - 2 * q.x*q.x - 2 * q.z*q.z);
		//yaw = atan2(2 * q.y*q.w + 2 * q.x*q.z, 1 - 2 * q.y*q.y - 2 * q.z*q.z); 

		//roll = atan2(2 * q.y*q.w + 2 * q.x*q.z, 1 - 2 * q.y*q.y - 2 * q.z*q.z);
		//pitch =atan2(2 * q.x*q.w + 2 * q.y*q.z, 1 - 2 * q.x*q.x - 2 * q.z*q.z);
		//yaw = asin(2 * q.x*q.y + 2 * q.z*q.w);


		/*
		pitch = atan2(2.0f * q.x * q.w + 2.0f * q.y * q.z, 1.0f - 2.0f * (sqz + sqw));
		yaw = asin(2.0f * (q.x * q.z - q.w * q.y));
		roll = atan2(2.0f * q.x * q.y + 2.0f * q.z * q.w, 1.0f - 2.0f * (sqy + sqz));
		*/
		

		/*
		pitch = atan2(2.0f * (q.y * q.z + q.x * q.w), (-sqx - sqy + sqz + sqw));
		yaw =  asin(-2.0f * (q.x * q.z - q.y * q.w));
		roll = atan2(2.0f * (q.x * q.y + q.z * q.w), (sqx - sqy - sqz + sqw));
		*/

		/*
		pitch =  asin(2.0f * (q.w * q.x - q.y * q.z));
		yaw =  atan2(2.0f * (q.x * q.z + q.y * q.w), (-sqx - sqy + sqz + sqw));
		roll = atan2(2.0f * (q.x * q.y + q.z * q.w), (-sqx + sqy - sqz + sqw));*/
		

		/*
		pitch = atan2f(2.f * (q.z*q.y + q.x*q.w), 1 - 2 * (sqx + sqy));
		yaw = asinf(-2.f * (q.x*q.z - q.y*q.w));
		roll = atan2f(2.f * (q.x*q.y + q.z*q.w), 1 - 2 * (sqy + sqz));
		*/

		//// atan2 :: -�� ~ ��

		//// roll (x-axis rotation)
		//double sinr_cosp = +2.0 * (q.w * q.x + q.y * q.z);
		//double cosr_cosp = +1.0 - 2.0 * (q.x * q.x + q.y * q.y);
		//roll = atan2(sinr_cosp, cosr_cosp);

		//// pitch (y-axis rotation)
		//double sinp = +2.0 * (q.w * q.y - q.z * q.x);
		//// fabs // �ε� �Ҽ��� ����
		//if (fabs(sinp) >= 1)
		//	pitch = copysign(/*M_PI*/Pi / 2.0f, sinp); // use 90 degrees if out of range
		//else
		//	pitch = asin(sinp);

		//// yaw (z-axis rotation)
		//double siny_cosp = +2.0 * (q.w * q.z + q.x * q.y);
		//double cosy_cosp = +1.0 - 2.0 * (q.y * q.y + q.z * q.z);
		//yaw = atan2(siny_cosp, cosy_cosp);


		pitch = Transform::RadianToDegree(pitch);
		yaw = Transform::RadianToDegree(yaw);
		roll = Transform::RadianToDegree(roll);

		return{ pitch, yaw, roll };
	}
	// ���濹��/////////////////////////////////////////////////////////////// �ӽ�

	const Vector3 & getLocalPosition() const { return position; }
	const Vector3 & getLocalRotation() const { return rotation; }
	const Vector3 & getLocalScale() const { return scale; }

	const Vector3 & getRight() const { return right; }
	const Vector3 & getForward() const { return forward; }
	const Vector3 & getUp() const { return up; }

	bool getDirty() const { return dirty; }
	void setDirty(bool dirty) { this->dirty = dirty; }

	//void transformUpdate(bool dirty, const D3DXMATRIX & parentRotationMatrix, const D3DXMATRIX & parentPositionMatrix);


	//void transformUpdate(bool dirty, const D3DXMATRIX & parentPositionMatrix, const D3DXMATRIX & parentRotationMatrix);

	void transformUpdate(bool dirty, const D3DXMATRIX & parentTransformMatrix, bool toPhysicsSystem = true);
	void rootTransformUpdate(bool toPhysicsSystem = true);

	//const D3DXMATRIX & getTransformMatrix_DX() const { return transformMatrix_DX; }
	//void setTransformMatrix_DX(const D3DXMATRIX & parentPositionMatrix, const D3DXMATRIX & parentRotationMatrix);

	const D3DXMATRIX & getTransformMatrix() const { return transformMatrix; }
	bool getInverseTransformMatrix(D3DXMATRIX & inverseTransformMatrix) const
	{
		float determinant = 0.0f;
		D3DXMatrixInverse(&inverseTransformMatrix, &determinant, &transformMatrix);
		// ���� ������� �������� ������ ������ķ� �ʱ�ȭ
		if (determinant == 0.0f)
		{
			D3DXMatrixIdentity(&inverseTransformMatrix);
			return false;
		}
		return true;
	}
	void setTransformMatrix(const D3DXMATRIX & parentTransformMatrix);

	// setTransformMatrix�� ���ؼ� ���ȴ�.
	const D3DXMATRIX & getLocalTransformMatrix() const { return localTransformMatrix; }
	
	//const D3DXMATRIX & getRotationMatrix_DX() const { return rotationMatrix_DX; }
	//void calcRotationMatrix_DX(const D3DXMATRIX & parentRotationMatrix);

	const D3DXMATRIX & getRotationMatrix() const { return rotationMatrix; }
	void calcRotationMatrix();


	//const D3DXMATRIX & getPositionMatrix_DX() const { return positionMatrix_DX; }
	//void calcPositionMatrix_DX(const D3DXMATRIX & parentPositionMatrix);

	const D3DXMATRIX & getPositionMatrix() const { return positionMatrix; }
	void calcPositionMatrix();




	// �����̼��� �ٲ𶧸��� �ֽ�ȭ �ȴ�.
	void setDirectionVectorWithRotation_DX()
	{
		D3DXMATRIX T;

		D3DXVECTOR3 right_DX{ 1,0,0 };
		D3DXVECTOR3 up_DX{ 0,1,0 };
		D3DXVECTOR3 forward_DX{ 0,0,1 };

		Vector3 worldRot = getWorldRotation();

		/*
		�����̼� ���� �����غ���
		*/

		// ���� ���� ���ͱ������� ������ŭ ������ ȸ�����Ѽ� ��ü�� ���⺤�͸� �����ش�.
		// ���庤�� ���� pitch
		D3DXMatrixRotationAxis(&T, &WorldRight, DegreeToRadian(worldRot.getX()));
		D3DXVec3TransformCoord(&up_DX, &up_DX, &T);
		D3DXVec3TransformCoord(&forward_DX, &forward_DX, &T);

		// ���庤�� ���� yaw
		D3DXMatrixRotationAxis(&T, &WorldUp, DegreeToRadian(worldRot.getY()));
		D3DXVec3TransformCoord(&right_DX, &right_DX, &T);
		D3DXVec3TransformCoord(&forward_DX, &forward_DX, &T);

		// ���庤�� ���� roll
		D3DXMatrixRotationAxis(&T, &WorldForward, DegreeToRadian(worldRot.getZ()));
		D3DXVec3TransformCoord(&up_DX, &up_DX, &T);
		D3DXVec3TransformCoord(&right_DX, &right_DX, &T);

		Vector3::ToVector3(right, right_DX);
		Vector3::ToVector3(up, up_DX);
		Vector3::ToVector3(forward, forward_DX);
	}

	
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
	

	float constrainLessThan180(float angle)
	{
		float value = abs(angle);
		// ������ 360�� �Ѿ�� �����Ѵ�.
		int quotient = (int)((int)value / 360);
		value += -(360 * quotient);
		
		// value�� 360�� ���� ���ѵƴ�.

		// ���� 180�� ���� ũ�ٸ�
		// 180�� ���� �����ش�.
		if (value > 180)
			value = value - 360;

		// ���� �������ٸ� ������ �ٲ��ش�
		if (angle < 0)
			value = -value;

		// -180 ~ 180 �� ���� // ���ʹϾ�� ȣȯ
		return value;
	}



	void convertLocalPositionIfItIsChild(Vector3 & output, const Vector3 & input);
	void convertLocalRotationIfItIsChild(Vector3 & output, const Vector3 & input);

	// ���� ����
	void setWorldPosition_physics(const Vector3 & other);
	void setWorldRotation_physics(const Vector3 & other);
	void setWorldScale_physics(const Vector3 & other);
	
	void rotate(const Vector3 & other);

	void setWorldPosition(const Vector3 & other);
	void setWorldRotation(const Vector3 & other);

	void setLocalScale(const Vector3 & other) { scale = other; }
	// ����� ���ð� ������ ���濹��
	void setWorldScale(const Vector3 & other) { scale = other; }

	void setWorldPosition(float x, float y, float z);
	void setWorldRotation(float x, float y, float z);

	void setLocalScale(float x, float y, float z) { scale = { x,y,z }; }
	// ����� ���ð� ������ ���濹��
	void setWorldScale(float x, float y, float z) { scale = { x,y,z }; }

	void setWorldTransform(const Vector3 & position, const Vector3 & rotation, const Vector3 & scale)
	{
		setWorldPosition(position);
		setWorldRotation(rotation);
		setWorldScale(scale);
	}

	void setWorldTransform(const Transform & other)
	{
		setWorldPosition(other.position);
		setWorldRotation(other.rotation);
		setWorldScale(other.scale);
	}

	// ���� ����
	void setLocalPosition(const Vector3 & other);
	void setLocalRotation(const Vector3 & other);

	void setLocalPosition(float x, float y, float z);
	void setLocalRotation(float x, float y, float z);
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
		if (mesh)
			mesh->Release();
		// �ؽ�ó�� �Ҵ�� ���̰� ������ ����� ���̴�.
		for (int i = 0; i < textures.size(); i++)
		{
			if (textures[i])
				textures[i]->Release();
		}
	}


public:

	MeshRenderer(GameObject * go, Transform * tf);

	virtual ~MeshRenderer()
	{
		onDestroy();
	}


	void loadXFile(const string & fileName);
};

//class AnimationFSM;
class BulletController;
class FbxModelRenderer;
class PlayerScript : public Component
{
private:
	float shootingPower;
	const float chargingSpeed;

	float defaultShootingPower;
	float maxShootingPower;

	float moveSpeed;

	BulletController * bulletController;
	FbxModelRenderer * fbxModelRenderer;

protected:
	virtual void update();
	virtual void start();
	virtual void onCollisionStay(GameObjectWithCollision & other);

public:
	PlayerScript(GameObject * go, Transform * tf)
		: Component(go, tf), shootingPower(0.0f), chargingSpeed(50.0f), defaultShootingPower(200), maxShootingPower(800), bulletController(nullptr), moveSpeed(0.85f),
		fbxModelRenderer(nullptr)
	{
		start();
	}
	
	virtual ~PlayerScript()
	{
		onDestroy();
	}

};

class BasicEnemyAnimationFSM;
class BasicEnemyScript : public Component
{
private:
	const float speed = .2f;
	const float dashSpeed = .3f;
	BasicEnemyAnimationFSM * basicEnemyAnimationFSM;
	GameObject * target;
	BT::Root * basicEnemyBT;
	Vector3 startPoint;
	float power;
protected:
	virtual void fixedUpdate();
	virtual void update();
	virtual void start();
	virtual void onDestroy();
	virtual void onCollisionStay(GameObjectWithCollision & other);

public:
	BasicEnemyScript(GameObject * go, Transform * tf)
		: Component(go, tf), target(nullptr), basicEnemyBT(nullptr), power(10.0f), basicEnemyAnimationFSM(nullptr)
	{
		start();
	}

	virtual ~BasicEnemyScript()
	{
		onDestroy();
	}

	void setTarget(GameObject * target) { this->target = target; }
	GameObject * getTarget() { return target; }


	float getTargetDistance();
	float getPower() const { return power; }
	void setPower(float power) { this->power = power; }
	// ���;��ص��� �Ÿ����� �Ǵ�
	bool isOutOfRange();
	bool isInRange();
	bool isInRange1();
	bool isInRange2();
	bool isInRange3();

	bool isTargeting();
	bool isNotTargeting();

	bool haveToReturn();

	void returnToStartPoint();
	void idle();

	void resetTarget();

	void attackTarget();
	void dashToTarget();
	void chaseTarget();

	void lookAtTarget(const Vector3 & targetDirection);

	void setStartPoint(const Vector3 & other)
	{
		startPoint = other;
	}

	const Vector3 & getStartPoint() { return startPoint; }



};

class BasicEnemySearchRangeScript : public Component
{
private:

	BasicEnemyScript * basicEnemyScript;
protected:
	//virtual void update();
	virtual void start();
	virtual void onCollisionStay(GameObjectWithCollision & other);

public:
	BasicEnemySearchRangeScript(GameObject * go, Transform * tf)
		: Component(go, tf), basicEnemyScript(nullptr)
	{
		start();
	}

	virtual ~BasicEnemySearchRangeScript()
	{
		onDestroy();
	}

};

class DamageableScript : public Component
{
private :
	float maxHp;
	float currentHp;
protected :
	virtual void start();
public :
	DamageableScript(GameObject * go, Transform * tf)
		: Component(go, tf), maxHp(100)
	{
		start();
	}

	virtual ~DamageableScript()
	{
		onDestroy();
	}

	void setMaxHp(float maxHp);

	void setCurrentHp(float hp);

	void getDamaged(float power);

};



// �׽�Ʈ��
class MoveScript_C : public Component
{
private:

protected:
	virtual void start();
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

class BulletController;
class BulletScript : public Component
{
private:
	IDirect3DDevice9 * device;
	ID3DXMesh* bulletMesh;
	IDirect3DTexture9 * texture;
	D3DMATERIAL9 mtrl;

	float power;

	BulletController * bulletController;

protected:
	virtual void start();
	virtual void update();
	virtual void onCollisionStay(GameObjectWithCollision & other);

public:
	BulletScript(GameObject * go, Transform * tf)
		: Component(go, tf), bulletMesh(nullptr), texture(nullptr), power(1.0f), bulletController(nullptr)
	{
		start();
		
	}

	virtual ~BulletScript()
	{
		onDestroy();
	}

	void setPower(float power) { this->power = power; }
	float getPower() const { return power; }

	void setBulletController(BulletController * bulletController)
	{
		this->bulletController = bulletController;
	}
	BulletController * getBulletController() { return bulletController; }
};
class FireExplosion;
class BulletParticle : public Component
{
private :
	FireExplosion * fireExplosion;
	float lagTime;
protected :
	//virtual void start();
	virtual void update();
	virtual void onDestroy();
public:
	BulletParticle(GameObject * go, Transform * tf)
		: Component(go, tf), fireExplosion(nullptr), lagTime(0.0f)
	{
		start();
	}

	virtual ~BulletParticle()
	{
		onDestroy();
	}

	void generateParticle(IDirect3DDevice9 * device, const char * textureFileName, int numOfParticles, int particleSize, const D3DXVECTOR3 & origin);
	void setAmountFactor(float factor);
	void setDuration(float duration);
};

class BulletController : public Component
{
private:
	vector<pair<GameObject *, Vector3 >> bullets;
	float lagTime;
	float creationInterval;
	float maxNumOfBullets;

	float bulletSpeed;
	float bulletSpaceRadius;


	float valueFactor;

	void setValueFactor(float valueFactor) { this->valueFactor = valueFactor; }
	float getValueFactor() const { return valueFactor; }

protected:
	virtual void start();
	virtual void update();
	virtual void onDestroy();
public:
	BulletController(GameObject * go, Transform * tf)
		: Component(go, tf), lagTime(0.0f), maxNumOfBullets(3), bulletSpaceRadius(10), creationInterval(2), bulletSpeed(0.1f), valueFactor(1.0f)
	{
		start();
	}

	virtual ~BulletController()
	{
		onDestroy();
	}

	void setCreationInterval(float interval)
	{
		creationInterval = interval;
	}

	float getCreationInterval() const { return creationInterval; }

	void removeBullet(GameObject * bullet);
	bool addBullet();
	bool shootBullet(float power);




	void unAccelerate() { valueFactor = 1.0f; }
	void accelerate() { valueFactor = 3.5f; }
};


//class Scene;
//enum MainObjTag;
// ���� ī�޶� ������Ʈ�� ������ ������Ʈ�� ������ ������ ���� ������Ʈ ���̺� ��� ������ ���ش�.
class MainCamera : public Component
{
private :
	/*Scene::MainObjTag*/unsigned char mainObjTag;
	virtual void start();
	virtual void update();
protected :
public :
	MainCamera(GameObject * go, Transform * tf);
	virtual ~MainCamera();
	void setViewSpace();
	void getViewMatrix(D3DXMATRIX * V);
};

class GamePlayManager : public Component
{
public :
	enum StageType
	{
		STAGE_NONE,
		STAGE_TUTORIAL,
		STAGE_BOSS,
		STAGE_ONE,
		STAGE_TWO
	};
private:
	// ���������� �̸��� ������Ʈ�� �̸���θ� ���ؼ� ����Ѵ�.
	// ���࿡ ���������� �ٲ�ٸ� ���⼭�� �ٸ� ���������� �ִ� �͵��� ���̻� ���������� �ʵ��� ����ȭ�� �����ش�.
	// ���� �������Ǵ� �͵��� FbxModelRenderer�� render�Լ��ε� �̸� �Ѱ� ���� �ֱ� ������ �� ó���� ���ش�.

	// GameObject * �� mapped value�� �������� ���� ������ �� ��ü�� �����Ǿ����� �����ϱⰡ ��Ʊ� �����̴�.
	// namePath�� ����ؼ� GameObject�� �޾Ƽ� ����Ѵ� // ���� GameObject�� ������ ��Ȳ�̸� unregister�Ѵ� // findWithNamePath :: O(log n)
	multimap<StageType, string> gameObjectsOfStages;
	StageType currentStage;
	GameObject * player;

	// �������� �������� �ʱ� �÷��̾ �ٶ� ����� ȸ������ �־��ش�.
	// start�Լ����� �ϵ��ڵ����� �־��ش�. // ���߿� json�����о �־��ٿ���
	map<StageType, pair<Vector3, Vector3>> stageStartPoints;


	void removeObjectsIfNotExist(StageType stageType);

	void resetCurrentStage();
	void resetStage(StageType stageType);
	void setStage(StageType stageType);
	
	// �������¸� ����
	void resetFbxModelRenderer(StageType stageType);
	// �������¸� Ȱ��ȭ
	void setFbxModelRenderer(StageType stageType);


	void changePlayerPosition(StageType stageType);

protected:
	virtual void start();
	virtual void update();
public:
	GamePlayManager(GameObject * go, Transform * tf)
		: Component(go, tf), currentStage(STAGE_NONE), player(nullptr)
	{
		start();
	}

	~GamePlayManager()
	{
		onDestroy();
	}

	void resetAllStage();
	void registerStageObject(StageType stageType, const string & namePath);
	void unregisterStageObject(StageType stageType, const string & namePath);

	void changeStage(StageType stageType);

	void setPlayer(GameObject * player) { this->player = player; }
	GameObject * getPlayer() { return player; }
};


// AABB�� �̿��� BoxCollider
// ������Ʈ�� �߰��ϸ� ��ǥ�� �߽����� X Y Z ����
/*
Width ����	X����
Depth ����	Z����
Height ����	Y����
���� ����
*/

// ��� ���� 
/*
1. �浹ó���� ��� ��ü�� ���ؼ� Collider�� ã�� ��ü�� ���� ������ŭ �����ؾ��ұ�?
	�浹ü�� ���� ��ü�� ���ؼ��� ó��?
	�浹ü�� ���� ��ü�� ����� ��ü������? // ���߿� ��������(����Ʈ��)�� �ؼ� �� �����ȿ� �ִ� ��ü�� ���ؼ��� �˻����شٸ�?

2. max min���� �׻� �����ؾ��ұ�? �ƴϸ� �浹�˻縦 �Ҷ��� ������ұ�?
	�׻� : ��ü���� ���귮�� ���������� ���� �����ϱ� ������ �浹�˻�� ������.(transform�ֽ�ȭ ��Ƽ�÷��׶� ����)
	�浹�������� : ��ü���� ��ҿ��귮�� ���ξ����� �浹�ÿ� max�� min���� ����� �ǽð����� ����� �ϱ� ������ �浹�˻�� ������.


���� : �浹ü�� ���� ��ü(��Ʈ��ü)�� ���ؼ��� ó���� ���ش� / �浹ü��
       min max���� ���ϴµ� ���� ������ �ʿ��Ѱ��� �ƴϹǷ� �׳� �浹ó���� ����Ѵ�.
*/
class BoxCollider : public Component
{
private :
	float width;
	float height;
	float depth;
	static const float DefaultSize;
protected :
public :
	BoxCollider(GameObject * go, Transform * tf)
		: Component(go, tf), width(DefaultSize), height(DefaultSize), depth(DefaultSize)
	{
		start();
	}

	float calcXMax() const { return transform->getLocalPosition().getX() + width; }
	float calcXMin() const { return transform->getLocalPosition().getX() - width; }
	float calcYMax() const { return transform->getLocalPosition().getY() + height; }
	float calcYMin() const { return transform->getLocalPosition().getY() - height; }
	float calcZMax() const { return transform->getLocalPosition().getZ() + depth; }
	float calcZMin() const { return transform->getLocalPosition().getZ() - depth; }

	const bool checkCollided(const BoxCollider & other) const
	{
		// �߰��� �ϳ��� �� �࿡���ؼ� �浹���� �ʾҴٸ� ����������.
		if (calcXMax() < other.calcXMin() || calcXMin() > other.calcXMax()) return false;
		if (calcYMax() < other.calcYMin() || calcYMin() > other.calcYMax()) return false;
		if (calcZMax() < other.calcZMin() || calcZMin() > other.calcZMax()) return false;
		
		return true;
	}
	
	void physicalReaction(const BoxCollider & other) const
	{
		// �ݹ��Ͽ� 1��ŭ �ݴ�������� Ƣ��´�.
		// ���߿� ������������ ������ ����
		Vector3 direction = transform->getLocalPosition() - other.transform->getLocalPosition();
		Vector3::Normalized(direction, direction);
		transform->setLocalPosition(transform->getLocalPosition() + 1 * direction);
	}


};

class RigidBody : public Component
{
private:
	enum ColliderType
	{
		NONE,
		BOX,
		SPHERE
	};
	struct ColliderInfo
	{
		ColliderType type;

		// ���� sphere�� ��� ��� ����.  // ������ 3�� ��ü : (3,3,3)
		Vector3 size;

		ColliderInfo()
			: type(ColliderType::NONE), size{0,0,0} {}

	};

	ColliderInfo colliderInfo;

protected:
	virtual void start();
	virtual void fixedUpdate();
	virtual void update();
	virtual void onDestroy();
public:



	RigidBody(GameObject * go, Transform * tf)
		: Component(go, tf)
	{
		start();
	}

	virtual ~RigidBody()
	{
		onDestroy();
	}

	void turnOnStaticFlag();
	void turnOffStaticFlag(float mass = 1.0f);

	void turnOnIsTriggerFlag();
	void turnOffIsTriggerFlag();

	void setBoxCollider(const Vector3 & size);
	void setSphereCollider(float radius);

	void setGravity(Vector3 & value);
	void getGravity(Vector3 & output);

	void addForce(const Vector3 & directionWithScalar);

};
class FbxModelAnimations;
class FbxModelSkeletonBones;
class FbxModelMesh;

class FbxModelRenderer : public Component
{
public :
	struct MeshForShading
	{
		IDirect3DVertexBuffer9* vb;
		IDirect3DIndexBuffer9* ib;
		unsigned int vertexCount;
		unsigned int indexCount;
		unsigned int triCount;

		MeshForShading()
			: vb(nullptr), ib(nullptr), vertexCount(0), indexCount(0), triCount(0) {}

		~MeshForShading()
		{
			if (vb)
				vb->Release();
			if (ib)
				ib->Release();
		}


	};
private:
	// �����ϰԸ���� ���ĺ��������� �÷���
	bool isTransparent;
	// ������Ʈ �������� �÷���
	bool updateFlag;

	//// ���̴� ���� ������
	//IDirect3DVertexDeclaration9* declaration;
	//IDirect3DVertexShader9* fbxModelRendererWithAnimationShader;
	//ID3DXConstantTable* constTable;
	//D3DXHANDLE viewProjectionMatrixHandle;
	//D3DXHANDLE worldMatrixHandle;
	//D3DXHANDLE cameraPositionHandle;

	//D3DXHANDLE textureHandle;
	//D3DXCONSTANT_DESC textureDesc;

	//// ������ ���� ����
	////Vector3 scaleFactor;
	//D3DXHANDLE scaleFactorHandle;


	// �������� �޽������� ��� ����
	vector<FbxModelMesh *> fbxMeshes;
	vector<MeshForShading *> meshesForShading;

	//vector<ID3DXMesh *>  meshes;
	map<MeshForShading *,vector<D3DMATERIAL9>> materialsTable;
	map<MeshForShading *,vector<IDirect3DTexture9*>> texturesTable;

	FbxModelAnimations * animations;
	FbxModelSkeletonBones * skeletonBones;


	FbxScene * scene;
	FbxImporter * importer;
	IDirect3DDevice9 * device;
	string fileName;

	static const string filePathToLoadFbxMeshFiles;
	static const string filePathToLoadFbxTextureFiles;
	static const string filePathToLoadFbxAnimationFiles;
	static const unsigned long DefaultOptimizeFlag;

	// �Ӽ����� �ﰢ���� �����ϰ� �Ӽ����̺��� ���� | ������ �ʴ� �ε����� ���ؽ� ���� | ���ؽ� ĳ���� ��Ʈ�� ����
	//static const unsigned long DefaultOptimizeFlag = D3DXMESHOPT_ATTRSORT | D3DXMESHOPT_COMPACT | D3DXMESHOPT_VERTEXCACHE;

	void render();
	// ���̷��� > �޽� ��� ó���Ҷ� ���
	void processAllNodesWithSameAtrributeTypeFromRoot(FbxNode * node, FbxNodeAttribute::EType attributeType);

	// �ִϸ��̼� ó���Ҷ� ���
	void processAnimation(const string & animationFileName, FbxScene * _scene, FbxImporter * _importer);

	// �ִϸ��̼� ó���� ���� ��� Ű������ �ʱ�ȭ�Ҷ� ���
	void processKeyFramesOfAllNodes(FbxNode * node, const string & animationFileName, const string & animationName, float frameRate, float start, float stop);
	//void processKeyFramesOfAllNodes(FbxNode * node, const string & animationName, float frameRate, float start, float stop);

	// �ִϸ��̼��� �������ǰ� ������ ������ �ִϸ��̼� ����� �ֽ�ȭ �ϱ� ���� ���
	void processSkeletonBonesAnimation(FbxModelAnimations * animations);

	// FbxModelMesh -> DXMesh
	void processVertices(MeshForShading * mesh, FbxModelMesh * fbxModelMesh);
	//void processVerticesWithAnimation(ID3DXMesh * mesh, FbxModelMesh * fbxModelMesh);
	void processIndices(MeshForShading * mesh, FbxModelMesh * fbxModelMesh);
	//void processIndices(ID3DXMesh * mesh, FbxModelMesh * fbxModelMesh);
	void processTextures(MeshForShading * mesh, FbxModelMesh * fbxModelMesh);
	//void processTextures(ID3DXMesh * mesh, FbxModelMesh * fbxModelMesh);
	//void processSubsets(MeshForShading * mesh);
	//void optimizeMesh(MeshForShading * mesh);
	//void processSubsets(ID3DXMesh * mesh);
	//void optimizeMesh(ID3DXMesh * mesh);

	void preRenderIfIsTransparent()
	{
		device->SetRenderState(D3DRS_ALPHABLENDENABLE, true);
		device->SetTextureStageState(0, D3DTSS_ALPHAARG1, D3DTA_TEXTURE);
		device->SetTextureStageState(0, D3DTSS_ALPHAOP, D3DTOP_SELECTARG1);
		//device->SetRenderState(D3DRS_CULLMODE, D3DCULL_CCW);
		device->SetRenderState(D3DRS_SRCBLEND, D3DBLEND_ONE);
		device->SetRenderState(D3DRS_DESTBLEND, D3DBLEND_ONE);
	}

	void postRenderIfIsTransparent()
	{
		device->SetRenderState(D3DRS_ALPHABLENDENABLE, false);
	}
	//void optimizeMesh(ID3DXBuffer * adjBuffer, unsigned long optimizeFlag = DefaultOptimizeFlag);
	//void setMtrlsAndTextures(ID3DXBuffer * mtrlBuffer, unsigned long numMtrls);
	struct FbxModelVertex
	{
		float position[3];
		float normal[3];
		float uv[2];
		float animationMatrix[16];
		FbxModelVertex()
			: position{}, normal{}, uv{}, animationMatrix{}
		{
		
		}

		FbxModelVertex(float x, float y, float z,
			float nx, float ny, float nz, float u, float v,
			float m11, float m12, float m13, float m14,
			float m21, float m22, float m23, float m24,
			float m31, float m32, float m33, float m34,
			float m41, float m42, float m43, float m44
		)
		{
			position[0] = x; position[1] = y; position[2] = z;
			normal[0] = nx; normal[1] = ny; normal[2] = nz;
			uv[0] = u; uv[1] = v;
			animationMatrix[0] = m11; animationMatrix[1] = m12; animationMatrix[2] = m13; animationMatrix[3] = m14;
			animationMatrix[4] = m21; animationMatrix[5] = m22; animationMatrix[6] = m23; animationMatrix[7] = m24;
			animationMatrix[8] = m31; animationMatrix[9] = m32; animationMatrix[10] = m33; animationMatrix[11] = m34;
			animationMatrix[12] = m41; animationMatrix[13] = m42; animationMatrix[14] = m43; animationMatrix[15] = m44;
		}

		float& operator[] (unsigned int index) 
		{
			// 0���� �۰ų� 8���� ũ�ų� ������ ���α׷� �����.
			// �׻� 0���� ũ�ų� ���ų� 8���ٴ� �۾ƾ��Ѵ�.
			assert(index >= 0 && index < sizeof(FbxModelVertex) / sizeof(float));
			
			float * result = (float*)this;
			result += index;
			return *result;
		}

		static const unsigned long DefaultFVF;
	};


protected:

	virtual void start();
	virtual void update()
	{
		if (!updateFlag) return;

		render();
		//if (::GetAsyncKeyState('F') & 0x8000f)
		//	setScale(Vector3(1, 1, 1));

		//if (::GetAsyncKeyState('G') & 0x8000f)
		//	setScale(Vector3(1, 3, 1));

		//if (::GetAsyncKeyState('H') & 0x8000f)
		//	setScale(Vector3(3, 1, 3));

		//if (::GetAsyncKeyState('J') & 0x8000f)
		//	setScale(Vector3(3, 3, 3));
	}
	virtual void onDestroy();


public:

	FbxModelRenderer(GameObject * go, Transform * tf);


	virtual ~FbxModelRenderer()
	{
		onDestroy();
	}

	float constrainNegativeNumber(float value)
	{
		if (value < 0)
			return 1.0f;
		return value;
	}

	//void play(const string & animationName)
	//{
	//	if (animations)
	//		animations->play(animationName);
	//}
	void playWithFileName(const string & animationFileName);
	void stop();

	//void setScaleFactor(const Vector3 & scaleFactor);
	//const Vector3 & getScaleFactor() const { return scaleFactor; }


	FbxModelAnimations * getAnimations()
	{
		return animations;
	}

	//void setScale(const Vector3 & value);

	// FbxFile -> FbxModelRednerer(FbxModelMesh, FbxAnimations)

	void loadFbxFile(const string & fileName);

	// �ܺ� �ִϸ��̼� ���� �ҷ��ö� ���
	void loadFbxFileForAnimation(const string & fileName);

	void setUpdateFlag(bool updateFlag) { this->updateFlag = updateFlag; }
	bool getUpdateFlag() const { return updateFlag; }
	void setIsTransparent(bool isTransparent) { this->isTransparent = isTransparent; }
	bool getIsTransparent()const { return isTransparent; }

};


class AnimationFSM : public Component
{

public:
	enum ValueType
	{
		FLOATTYPE,
		BOOLTYPE
	};

	class Transition
	{
	private:
		AnimationFSM * animationFSM;

		string from;
		string to;

		int factor;
		ValueType type;
		string valueName;

		float fValueToCompare;
		bool bValueToCompare;

	public:
		Transition(AnimationFSM * animationFSM, const string & from, const string & to, const string & valueName,
			int factor, ValueType type, float valueToCompare)
			: animationFSM(animationFSM), from(from), to(to), factor(factor), type(type), valueName(valueName), fValueToCompare(valueToCompare), bValueToCompare((bool)valueToCompare)
		{

		}

		const string & getFrom() const { return from; }
		const string & getTo() const { return to; }

		bool operator==(const Transition & transition)
		{
			return (transition.factor == this->factor && transition.to == this->to && transition.from == this->from
				&& transition.type == this->type);
		}

		bool evaluate();

	};
protected:
	//class Transition;
	FbxModelRenderer * fbxModelRenderer;
	FbxModelAnimations * fbxModelAnimations;

	// ����� �̸��� ���¸� �����ϴ� state ���̺�
	map<string, unsigned int> stateTable;

	// ����� float�� �̸��� �����ص� float ���̺�
	map<string, float> floatTable;

	// ����� bool�� �̸��� �����ص� bool ���̺�
	map<string, bool> boolTable;

	// ���̸� ���� ���̺� // A from to B�� ���� a���� b���� ���µ� � ���� ������ �ִ���
	// ������ �̸��� ���������� ��� ���� ���ǵ��� ������Ʈ���� 
	// �˻��ϰ� ���̵� ��Ȳ�̸� animationsŬ������ �̿��ؼ� ����ִϸ��̼��� ��������ش�.
	// Ű�� : from node
	// ����� : to node / Transition * 
	multimap<string, pair<string, Transition *>> transitionTable;


	// ���� ����
	string currentState;
	// �⺻ ����
	string defaultState;

	void updateAllTrasitions(const string & animationFileName);

protected:

	virtual void start();
	virtual void update();
	virtual void onDestroy();

public:

	AnimationFSM(GameObject * go, Transform * tf)
		: Component(go, tf), fbxModelRenderer(nullptr), fbxModelAnimations(nullptr), currentState(""), defaultState("")
	{
		start();
	}

	virtual ~AnimationFSM();

	friend class Transition;

	// �ִϸ��̼� ���
	/*
	Standing Aim Recoil.fbx
	standing idle 01.fbx
	standing run forward.fbx
	standing run forward stop.fbx
	standing run back.fbx
	standing run left.fbx
	standing run right.fbx

	standing walk back.fbx
	standing walk forward.fbx
	standing walk left.fbxComparison value
	standing walk right.fbx
	*/
	void registerAnimation(const string & animationFileName);
	void setDefaultState(const string & animationFileName);

	void registerFloat(const string & floatName);
	void registerBool(const string & boolName);

	void setFloat(const string & floatName, float input);
	bool getFloat(float & output, const string & floatName);

	void setBool(const string & boolName, bool input);
	bool getBool(bool & output, const string & boolName);
	/*
	 - �������� ������ֱ�
	 - ��������� �������� üũ
	 
		from : ���� �ִϸ��̼�
		To : ����üũ�� �ִϸ��̼�'
		valueName : ���ϴ� ��ü�� �Ǵ� ���� �̸�
		factor : -1 = less / 0 = equal / 1 = more
		type : float���̺��� ã���� bool ���̺��� ã����
		valueToCompare : ���� ��
	*/
	// �Լ������ͷ� �޾Ƽ� �������ش�.


	//(AnimationFSM * animationFSM, const string & from, const string & to, const string & valueName,
	//	int factor, ValueType type, float valueToCompare)
	
	void makeTransition(const string & from, const string & to, const string & valueName, int factor, ValueType type, float valueToCompare);

};

class PlayerAnimationFSM : public AnimationFSM
{
protected :
	virtual void start();
	virtual void update();
	virtual void onDestroy(){}
public:
	PlayerAnimationFSM(GameObject * go, Transform * tf)
		: AnimationFSM(go, tf)
	{
		start();
	}

	virtual ~PlayerAnimationFSM()
	{
		onDestroy();
	}
};


class BasicEnemyAnimationFSM : public AnimationFSM
{
protected:
	virtual void start();
	virtual void update();
	virtual void onDestroy() { AnimationFSM::onDestroy(); }
public:
	BasicEnemyAnimationFSM(GameObject * go, Transform * tf)
		: AnimationFSM(go, tf)
	{
		start();
	}

	virtual ~BasicEnemyAnimationFSM()
	{
		onDestroy();
	}
};

class Terrain : public Component
{
private:
	ID3DXMesh*  mesh;
	IDirect3DTexture9 * texture;
	D3DMATERIAL9 mtrl;

	// �ͷ��ο� �������� ��Ƽ����� �ؽ�ó ������ ���� �۾� // �޽�1 : �ؽ�ó1�� ����
	//vector<D3DMATERIAL9> mtrls;
	//vector<IDirect3DTexture9*> textures;
	IDirect3DDevice9 * device;

	string fileName;

	vector<float> heightMap;

	unsigned int verticesPerRow;
	unsigned int verticesPerColumn;

	// verticesPerRow * verticesPerColumn
	unsigned int heightMapSize;

	unsigned int rectsPerRow;
	unsigned int rectsPerColumn;

	// �̰��� ���ؼ� ��ü���� ���̰� �����ȴ�.
	float heightValue;

	float distanceOfVertices;

	// verticesPerRow * distanceOfVertices // x
	float width;

	// verticesPerColumn * distanceOfVertices // z
	float depth;

	static const string filePathToLoadTerrainFiles;
	static const string filePathToLoadTerrainTextureFiles;


	void render();

	bool loadHeightMap(const string & fileName);
	bool generateMeshBuffer();
	void processVertices();
	void processIndices();
	void processSubsets();
	void optimizeMesh();

	struct TerrainVertex
	{
		float position[3];
		float normal[3];
		float uv[2];

		TerrainVertex()
			: position{}, normal{}, uv{}
		{

		}

		TerrainVertex(float x, float y, float z,
			float nx, float ny, float nz, float u, float v)
		{
			position[0] = x; position[1] = y; position[2] = z;
			normal[0] = nx; normal[1] = ny; normal[2] = nz;
			uv[0] = u; uv[1] = v;
		}

		float& operator[] (unsigned int index)
		{
			// 0���� �۰ų� 8���� ũ�ų� ������ ���α׷� �����.
			// �׻� 0���� ũ�ų� ���ų� 8���ٴ� �۾ƾ��Ѵ�.
			assert(index >= 0 && index < sizeof(TerrainVertex) / sizeof(float));

			float * result = (float*)this;
			result += index;
			return *result;
		}

		static const unsigned long DefaultFVF;
	};


protected:
	virtual void start();
	virtual void update() { render(); }
	virtual void onDestroy() 
	{ 
		if (mesh) mesh->Release();
		if (texture) texture->Release();
	}

public:
	Terrain(GameObject * go, Transform * tf)
		: Component(go, tf), mesh(nullptr), device(nullptr), fileName(""), texture(nullptr),
		verticesPerRow(0), verticesPerColumn(0), rectsPerRow(0), rectsPerColumn(0), distanceOfVertices(0), width(0), depth(0), heightValue(0)
	{
		start();
	}

	virtual ~Terrain()
	{
		onDestroy();
	}


	void loadRawFile(const string & fileName, unsigned int verticesPerRow, unsigned int verticesPerColumn, float distanceOfVertices, float heightValue);

	//grass.bmp
	void loadTextureFromFile(const string & fileName);
	bool getHeight(const Vector3 & position, float * output);

	// �ͷ�����ǥ�迡���� ���̸� �����ش�. ���� ��ġ�� ���ؼ� �ͷ��� ��ǥ��� ���� ���� ����� �Ȱ����־ �ȴ�.
	bool getLocalHeight(const Vector3 & position, float * output);

	float getDepth() const { return depth; }
	float getWidth() const { return width; }

	void setMaterial(const D3DMATERIAL9 & mtrl)
	{
		this->mtrl = mtrl;
	}

};

class MoveOnTerrainScript : public Component
{
private:
	Terrain * terrain;
	float objectHeight;
protected:
	virtual void start() {};
	virtual void update();
	virtual void onCollisionStay(GameObjectWithCollision & other) {};

public:
	MoveOnTerrainScript(GameObject * go, Transform * tf)
		: Component(go, tf), terrain(nullptr), objectHeight(0)
	{
		start();
	}

	virtual ~MoveOnTerrainScript()
	{
		onDestroy();
	}

	void setTerrain(Terrain * terrain)
	{
		this->terrain = terrain;
	}

	void setObjectHeight(float objectHeight)
	{
		this->objectHeight = objectHeight;
	}

};



class BillBoard : public Component
{
private:
	GameObject * mainCamera;
	Vector3 currentMainCameraPosition;

	IDirect3DDevice9 * device;

	ID3DXMesh*  mesh;
	IDirect3DTexture9 * texture;
	D3DMATERIAL9 mtrl;

	float width;
	float height;

	static const string filePathToLoadBillBoardTextureFiles;

	void render();
	void rotateIfMainCameraMoves();

	bool generateMeshBuffer();
	void processVertices();
	void processIndices();
	void processSubsets();
	void optimizeMesh();

	struct BillBoardVertex
	{
		float position[3];
		float normal[3];
		float uv[2];

		BillBoardVertex()
			: position{}, normal{}, uv{}
		{

		}

		BillBoardVertex(float x, float y, float z,
			float nx, float ny, float nz, float u, float v)
		{
			position[0] = x; position[1] = y; position[2] = z;
			normal[0] = nx; normal[1] = ny; normal[2] = nz;
			uv[0] = u; uv[1] = v;
		}

		float& operator[] (unsigned int index)
		{
			// 0���� �۰ų� 8���� ũ�ų� ������ ���α׷� �����.
			// �׻� 0���� ũ�ų� ���ų� 8���ٴ� �۾ƾ��Ѵ�.
			assert(index >= 0 && index < sizeof(BillBoardVertex) / sizeof(float));

			float * result = (float*)this;
			result += index;
			return *result;
		}

		static const unsigned long DefaultFVF;
	};

protected:
	virtual void start();
	virtual void update();
	virtual void onCollisionStay(GameObjectWithCollision & other) {};

public:
	BillBoard(GameObject * go, Transform * tf)
		: Component(go, tf), width(100), height(100), mainCamera(nullptr), texture(nullptr), mesh(nullptr), currentMainCameraPosition{}
	{
		start();
	}

	virtual ~BillBoard()
	{
		onDestroy();
	}

	float getWidth() const { return width; }
	float getHeight() const { return height; }

	void setWidth(float width)
	{
		this->width = width; 
		// ���Ӱ� ���ؽ��� �������ش�.
		processVertices();
	}
	void setHeight(float height)
	{
		this->height = height;
		// ���Ӱ� ���ؽ��� �������ش�.
		processVertices();
	}

	void setSize(float width, float height)
	{
		this->width = width;
		this->height = height;

		processVertices();
	}
	void loadTextureFromFile(const string & fileName);

	// ������� ����Ʈ�ʰ� ���� ������ �� �ε��ؾ� ������ �� �ִ� ���� �ƴϴ�.
	// �׷��� ������ ��� ������ �����ִ� �Լ��� �ϳ� �����ߴ�. 
	// ���Լ�ȣ���� loadTextureFromFile�� �ؽ�ó�� �ε��ϸ� �ȴ�.
	void generateBillBoard();


};
class GateEffect;
class GateInScript : public Component
{
private:
	GateEffect * gateEffect;
	Vector3 destination;
protected:
	virtual void onDestroy();
	virtual void start();
	virtual void update();
	virtual void onCollisionStay(GameObjectWithCollision & other);
public:
	GateInScript(GameObject * go, Transform * tf)
		: Component(go, tf), gateEffect(nullptr)
	{

		start();
	}

	virtual ~GateInScript()
	{
		onDestroy();
	}

	void setDestination(const Vector3 & destination)
	{
		this->destination = destination;
	}

	void setDestination(GameObject * other);

	void setOrigin(const Vector3 & origin);
	//fireExplosion = new FireExplosion(device, textureFileName, numOfParticles, particleSize, origin);
};

class GateOutScript : public Component
{
private:
	GateEffect * gateEffect;
	float timeToDestroy;
	float lagTime;
protected:
	virtual void onDestroy();
	virtual void start();
	virtual void update();
	//virtual void onCollisionStay(GameObjectWithCollision & other);
public:
	GateOutScript(GameObject * go, Transform * tf)
		: Component(go, tf), gateEffect(nullptr), timeToDestroy(3.0f), lagTime(0.0f)
	{
		start();
	}

	virtual ~GateOutScript()
	{
		onDestroy();
	}

	void setTimeToDestroy(float time)
	{
		if (time > 0)
			timeToDestroy = time;
	}

	void setOrigin(const Vector3 & origin);

	//fireExplosion = new FireExplosion(device, textureFileName, numOfParticles, particleSize, origin);
};

#endif