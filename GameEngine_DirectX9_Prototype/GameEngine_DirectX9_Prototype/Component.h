#ifndef COMPONENT_H
#define COMPONENT_H

#include <cassert>
#include "Utility.h"
#include <vector>
#include "d3dUtility.h"
#include "BehaviorTree.h"


class Transform;
class GameObject;
struct GameObjectWithCollision;

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
	// 3개의 값은 만약에 root객체라면 월드좌표계 기준으로 설정되어있고
	// child객체라면 parent객체기준으로 설정되어있다.
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




	// 현재 위치의 최신화 여부를 알기 위한 더티플래그
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

	// 여기서 const객체를 입력파라미터로 받아올 수 없는 이유는 Component객체를 초기화할때 필요한정보는 계속해서 변경되는 정보이기 때문에
	// const Transform & other로 받으면 Component를 초기화할때 Transform내부의 GameObject와 Transform를 read only로 밖에 받아올 수 없다.
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


	// 만약에 월드좌표계의 값이 들어왔을때 부모객체가 있다면 로컬좌표계로 월드좌표계 좌표를 표현해서 초기화 해야한다. 
	// 즉 월드 -> 로컬로의 변환이 필요하다

	// 그를 위해 월드좌표가 들어오면 부모객체가 존재할때 로컬좌표계 값으로 변경시켜주는 역할을 하는 함수이다.




	//// getPosition과 동일 // getPosition도 로컬좌표 반환
	//Vector3 getLocalPosition() const { return{ localTransformMatrix._41, localTransformMatrix._42, localTransformMatrix._43 }; }

	Vector3 getWorldPosition() const { return{ transformMatrix._41, transformMatrix._42, transformMatrix._43 }; }

	// 변경예정/////////////////////////////////////////////////////////////// 임시
	Vector3 getWorldRotation() const
	{
		// 먼저 transformation matrix를 분리시켜서 Quaternion을 만들어준다.
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

		//// atan2 :: -π ~ π

		//// roll (x-axis rotation)
		//double sinr_cosp = +2.0 * (q.w * q.x + q.y * q.z);
		//double cosr_cosp = +1.0 - 2.0 * (q.x * q.x + q.y * q.y);
		//roll = atan2(sinr_cosp, cosr_cosp);

		//// pitch (y-axis rotation)
		//double sinp = +2.0 * (q.w * q.y - q.z * q.x);
		//// fabs // 부동 소수점 절댓값
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
	// 변경예정/////////////////////////////////////////////////////////////// 임시

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
		// 만약 역행렬이 존재하지 않으면 단위행렬로 초기화
		if (determinant == 0.0f)
		{
			D3DXMatrixIdentity(&inverseTransformMatrix);
			return false;
		}
		return true;
	}
	void setTransformMatrix(const D3DXMATRIX & parentTransformMatrix);

	// setTransformMatrix에 의해서 계산된다.
	const D3DXMATRIX & getLocalTransformMatrix() const { return localTransformMatrix; }
	
	//const D3DXMATRIX & getRotationMatrix_DX() const { return rotationMatrix_DX; }
	//void calcRotationMatrix_DX(const D3DXMATRIX & parentRotationMatrix);

	const D3DXMATRIX & getRotationMatrix() const { return rotationMatrix; }
	void calcRotationMatrix();


	//const D3DXMATRIX & getPositionMatrix_DX() const { return positionMatrix_DX; }
	//void calcPositionMatrix_DX(const D3DXMATRIX & parentPositionMatrix);

	const D3DXMATRIX & getPositionMatrix() const { return positionMatrix; }
	void calcPositionMatrix();




	// 로테이션이 바뀔때마다 최신화 된다.
	void setDirectionVectorWithRotation_DX()
	{
		D3DXMATRIX T;

		D3DXVECTOR3 right_DX{ 1,0,0 };
		D3DXVECTOR3 up_DX{ 0,1,0 };
		D3DXVECTOR3 forward_DX{ 0,0,1 };

		/*
		로테이션 순서 생각해보기
		*/

		// 월드 방향 벡터기준으로 각도만큼 각각을 회전시켜서 객체의 방향벡터를 구해준다.
		// 월드벡터 기준 pitch
		D3DXMatrixRotationAxis(&T, &WorldRight, DegreeToRadian(rotation.getX()));
		D3DXVec3TransformCoord(&up_DX, &up_DX, &T);
		D3DXVec3TransformCoord(&forward_DX, &forward_DX, &T);

		// 월드벡터 기준 yaw
		D3DXMatrixRotationAxis(&T, &WorldUp, DegreeToRadian(rotation.getY()));
		D3DXVec3TransformCoord(&right_DX, &right_DX, &T);
		D3DXVec3TransformCoord(&forward_DX, &forward_DX, &T);

		// 월드벡터 기준 roll
		D3DXMatrixRotationAxis(&T, &WorldForward, DegreeToRadian(rotation.getZ()));
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
		// 절댓값이 360이 넘어가면 제한한다.
		int quotient = (int)((int)value / 360);
		value += -(360 * quotient);
		
		// value는 360도 내로 제한됐다.

		// 만약 180도 보다 크다면
		// 180도 내로 돌려준다.
		if (value > 180)
			value = value - 360;

		// 만약 음수였다면 음수로 바꿔준다
		if (angle < 0)
			value = -value;

		// -180 ~ 180 로 제한 // 쿼터니언과 호환
		return value;
	}



	void convertLocalPositionIfItIsChild(Vector3 & output, const Vector3 & input);
	void convertLocalRotationIfItIsChild(Vector3 & output, const Vector3 & input);

	// 월드 기준
	void setWorldPosition_physics(const Vector3 & other);
	void setWorldRotation_physics(const Vector3 & other);
	void setWorldScale_physics(const Vector3 & other);
	
	void rotate(const Vector3 & other);

	void setWorldPosition(const Vector3 & other);
	void setWorldRotation(const Vector3 & other);

	void setWorldScale(const Vector3 & other) { scale = other; }

	void setWorldPosition(float x, float y, float z);
	void setWorldRotation(float x, float y, float z);

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

	// 로컬 기준
	void setLocalPosition(const Vector3 & other);
	void setLocalRotation(const Vector3 & other);

	void setLocalPosition(float x, float y, float z);
	void setLocalRotation(float x, float y, float z);
};

// X파일을 통한 메쉬생성 컴포넌트
// 나중에 FBX를 통해 받아올수 있게 만들예정 FBX SDK 학습중
class MeshRenderer : public Component
{
private:
	ID3DXMesh*  mesh;
	vector<D3DMATERIAL9> mtrls;
	vector<IDirect3DTexture9*> textures;
	IDirect3DDevice9 * device;
	string fileName;

	// 속성으로 삼각형을 정렬하고 속성테이블을 생성 | 사용되지 않는 인덱스와 버텍스 제거 | 버텍스 캐시의 히트율 높임
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

		// 텍스처만 할당된 값이고 재질은 복사된 값이다.
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

//class AnimationFSM;

class PlayerScript : public Component
{
private:
	float shootingPower;
	const float chargingSpeed;
protected:
	virtual void update();
	virtual void start();
	virtual void onCollisionStay(GameObjectWithCollision & other);

public:
	PlayerScript(GameObject * go, Transform * tf)
		: Component(go, tf), shootingPower(0.0f), chargingSpeed(6.0f)
	{
		start();
	}
	
	virtual ~PlayerScript()
	{
		onDestroy();
	}

};


class BasicEnemyScript : public Component
{
private:
	const float speed = .2f;
	const float dashSpeed = .3f;

	GameObject * target;
	BT::Root * basicEnemyBT;
	Vector3 startPoint;
protected:
	virtual void fixedUpdate();
	virtual void update();
	virtual void start();
	virtual void onDestroy();
	//virtual void onCollisionStay(GameObjectWithCollision & other);

public:
	BasicEnemyScript(GameObject * go, Transform * tf)
		: Component(go, tf), target(nullptr), basicEnemyBT(nullptr)
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

	// 복귀안해도될 거리인지 판단
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



// 테스트용
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
		// 만약에 그냥 Component 클래스 생성자 함수바디에서 start()함수 호출?
		// 생성되는 과정이기 때문에 오버라이딩이 적용되지 않는다 // 부모생성완료(이과정에서 자식의 재정의함수를 찾을수없다.) > 자식생성완료
		start();
	}

	virtual ~MoveScript_C()
	{
		onDestroy();
	}

};

class BulletScript : public Component
{
private:
	IDirect3DDevice9 * device;
	ID3DXMesh* bulletMesh;
	IDirect3DTexture9 * texture;
	D3DMATERIAL9 mtrl;

protected:
	virtual void start();
	virtual void update();
	virtual void onCollisionStay(GameObjectWithCollision & other);

public:
	BulletScript(GameObject * go, Transform * tf)
		: Component(go, tf), bulletMesh(nullptr), texture(nullptr)
	{
		start();
		
	}

	virtual ~BulletScript()
	{
		onDestroy();
	}
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


//class Scene;
//enum MainObjTag;
// 메인 카메라 오브젝트가 가지는 컴포넌트로 생성과 삭제시 메인 오브젝트 테이블에 등록 삭제를 해준다.
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


// AABB를 이용한 BoxCollider
// 컴포넌트를 추가하면 좌표를 중심으로 X Y Z 방향
/*
Width 가로	X방향
Depth 세로	Z방향
Height 높이	Y방향
으로 정의
*/

// 고려 사항 
/*
1. 충돌처리는 모든 객체에 대해서 Collider를 찾고 객체의 수의 제곱만큼 수행해야할까?
	충돌체를 가진 객체에 대해서만 처리?
	충돌체를 가진 객체중 가까운 객체에서만? // 나중에 공간분할(쿼드트리)을 해서 그 공간안에 있는 객체에 대해서만 검사해준다면?

2. max min값은 항상 유지해야할까? 아니면 충돌검사를 할때만 해줘야할까?
	항상 : 전체적인 연산량은 많아지지만 값을 유지하기 때문에 충돌검사는 빠르다.(transform최신화 더티플래그랑 연계)
	충돌했을때만 : 전체적인 평소연산량은 별로없지만 충돌시에 max와 min값을 계산을 실시간으로 해줘야 하기 때문에 충돌검사는 느리다.


현재 : 충돌체를 가진 객체(루트객체)에 대해서만 처리를 해준다 / 충돌체를
       min max값을 구하는데 많은 연산이 필요한것이 아니므로 그냥 충돌처리시 계산한다.
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
		// 중간에 하나라도 한 축에대해서 충돌하지 않았다면 빠져나간다.
		if (calcXMax() < other.calcXMin() || calcXMin() > other.calcXMax()) return false;
		if (calcYMax() < other.calcYMin() || calcYMin() > other.calcYMax()) return false;
		if (calcZMax() < other.calcZMin() || calcZMin() > other.calcZMax()) return false;
		
		return true;
	}
	
	void physicalReaction(const BoxCollider & other) const
	{
		// 반발하여 1만큼 반대방향으로 튀어나온다.
		// 나중에 물리현상으로 수정할 예정
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

		// 만약 sphere의 경우 모두 같다.  // 반지름 3인 구체 : (3,3,3)
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
private:
	// 여러가지 메쉬정보를 담고 있음
	vector<FbxModelMesh *> fbxMeshes;

	vector<ID3DXMesh *>  meshes;
	map<ID3DXMesh *,vector<D3DMATERIAL9>> materialsTable;
	map<ID3DXMesh *,vector<IDirect3DTexture9*>> texturesTable;

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

	// 속성으로 삼각형을 정렬하고 속성테이블을 생성 | 사용되지 않는 인덱스와 버텍스 제거 | 버텍스 캐시의 히트율 높임
	//static const unsigned long DefaultOptimizeFlag = D3DXMESHOPT_ATTRSORT | D3DXMESHOPT_COMPACT | D3DXMESHOPT_VERTEXCACHE;

	void render();
	// 스켈레톤 > 메쉬 노드 처리할때 사용
	void processAllNodesWithSameAtrributeTypeFromRoot(FbxNode * node, FbxNodeAttribute::EType attributeType);

	// 애니메이션 처리할때 사용
	void processAnimation(const string & animationFileName, FbxScene * _scene, FbxImporter * _importer);

	// 애니메이션 처리시 본의 모든 키프레임 초기화할때 사용
	void processKeyFramesOfAllNodes(FbxNode * node, const string & animationFileName, const string & animationName, float frameRate, float start, float stop);
	//void processKeyFramesOfAllNodes(FbxNode * node, const string & animationName, float frameRate, float start, float stop);

	// 애니메이션이 렌더링되고 있을때 본들의 애니메이션 행렬을 최신화 하기 위해 사용
	void processSkeletonBonesAnimation(FbxModelAnimations * animations);

	// FbxModelMesh -> DXMesh
	void processVertices(ID3DXMesh * mesh, FbxModelMesh * fbxModelMesh);
	void processVerticesWithAnimation(ID3DXMesh * mesh, FbxModelMesh * fbxModelMesh);
	void processIndices(ID3DXMesh * mesh, FbxModelMesh * fbxModelMesh);
	void processTextures(ID3DXMesh * mesh, FbxModelMesh * fbxModelMesh);
	void processSubsets(ID3DXMesh * mesh);
	void optimizeMesh(ID3DXMesh * mesh);

	//void optimizeMesh(ID3DXBuffer * adjBuffer, unsigned long optimizeFlag = DefaultOptimizeFlag);
	//void setMtrlsAndTextures(ID3DXBuffer * mtrlBuffer, unsigned long numMtrls);
	struct FbxModelVertex
	{
		float position[3];
		float normal[3];
		float uv[2];

		FbxModelVertex()
			: position{}, normal{}, uv{}
		{
		
		}

		FbxModelVertex(float x, float y, float z,
			float nx, float ny, float nz, float u, float v)
		{
			position[0] = x; position[1] = y; position[2] = z;
			normal[0] = nx; normal[1] = ny; normal[2] = nz;
			uv[0] = u; uv[1] = v;
		}

		float& operator[] (unsigned int index) 
		{
			// 0보다 작거나 8보다 크거나 같으면 프로그램 멈춘다.
			// 항상 0보다 크거나 같거나 8보다는 작아야한다.
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


	FbxModelAnimations * getAnimations()
	{
		return animations;
	}

	void setScale(const Vector3 & value);

	// FbxFile -> FbxModelRednerer(FbxModelMesh, FbxAnimations)

	void loadFbxFile(const string & fileName);

	// 외부 애니메이션 파일 불러올때 사용
	void loadFbxFileForAnimation(const string & fileName);



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
private:
	//class Transition;
	FbxModelRenderer * fbxModelRenderer;
	FbxModelAnimations * fbxModelAnimations;

	// 등록한 이름과 상태를 저장하는 state 테이블
	map<string, unsigned int> stateTable;

	// 사용할 float의 이름을 설정해둔 float 테이블
	map<string, float> floatTable;

	// 사용할 bool의 이름을 설정해둔 bool 테이블
	map<string, bool> boolTable;

	// 전이를 위한 테이블 // A from to B로 지원 a에서 b까지 가는데 어떤 전이 조건이 있는지
	// 한파일 이름을 기준으로한 모든 전이 조건들을 업데이트에서 
	// 검사하고 전이될 상황이면 animations클래스를 이용해서 현재애니메이션을 변경시켜준다.
	// 키값 : from node
	// 밸류값 : to node / Transition * 
	multimap<string, pair<string, Transition *>> transitionTable;


	// 현재 상태
	string currentState;
	// 기본 상태
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

	~AnimationFSM();

	friend class Transition;

	// 애니메이션 목록
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
	 - 전이조건 만들어주기
	 - 현재상태의 전이조건 체크
	 
		from : 현재 애니메이션
		To : 전이체크할 애니메이션'
		valueName : 비교하는 주체가 되는 값의 이름
		factor : -1 = less / 0 = equal / 1 = more
		type : float테이블에서 찾을지 bool 테이블에서 찾을지
		valueToCompare : 비교할 값
	*/
	// 함수포인터로 받아서 실행해준다.


	//(AnimationFSM * animationFSM, const string & from, const string & to, const string & valueName,
	//	int factor, ValueType type, float valueToCompare)
	
	void makeTransition(const string & from, const string & to, const string & valueName, int factor, ValueType type, float valueToCompare);

};


class Terrain : public Component
{
private:
	ID3DXMesh*  mesh;
	IDirect3DTexture9 * texture;
	D3DMATERIAL9 mtrl;

	// 터레인에 여러가지 머티리얼과 텍스처 적용은 나중 작업 // 메쉬1 : 텍스처1로 대응
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

	// 이값에 의해서 전체적인 높이가 결정된다.
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
			// 0보다 작거나 8보다 크거나 같으면 프로그램 멈춘다.
			// 항상 0보다 크거나 같거나 8보다는 작아야한다.
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

	// 터레인좌표계에서의 높이를 구해준다. 들어온 위치에 대해서 터레인 좌표계로 들어가기 위한 행렬을 안곱해주어도 된다.
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
			// 0보다 작거나 8보다 크거나 같으면 프로그램 멈춘다.
			// 항상 0보다 크거나 같거나 8보다는 작아야한다.
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
		// 새롭게 버텍스를 설정해준다.
		processVertices();
	}
	void setHeight(float height)
	{
		this->height = height;
		// 새롭게 버텍스를 설정해준다.
		processVertices();
	}

	void setSize(float width, float height)
	{
		this->width = width;
		this->height = height;

		processVertices();
	}
	void loadTextureFromFile(const string & fileName);

	// 빌보드는 하이트맵과 같이 파일을 꼭 로드해야 생성할 수 있는 것은 아니다.
	// 그렇기 때문에 모든 과정을 묶어주는 함수를 하나 정의했다. 
	// 이함수호출후 loadTextureFromFile로 텍스처를 로딩하면 된다.
	void generateBillBoard();


};

#endif