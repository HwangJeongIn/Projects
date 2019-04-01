#ifndef COMPONENT_H
#define COMPONENT_H

#include <cassert>
#include "Utility.h"
#include <vector>
#include "d3dUtility.h"


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


	D3DXMATRIX transformMatrix_DX;
	D3DXMATRIX rotationMatrix_DX;
	D3DXMATRIX positionMatrix_DX;

	static const D3DXVECTOR3 WorldRight_DX;
	static const D3DXVECTOR3 WorldUp_DX;
	static const D3DXVECTOR3 WorldForward_DX;


	// 현재 위치의 최신화 여부를 알기 위한 더티플래그
	bool dirty;
protected:
	virtual void start()
	{ 		
		// 절댓값 360 * n 날린다 + 이객체의 right up forward 벡터 계산
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

	static const D3DXMATRIX IdentityMatrix_DX;

	const Vector3 & getPosition() const { return position; }
	const Vector3 & getRotation() const { return rotation; }
	const Vector3 & getScale() const { return scale; }

	const Vector3 & getRight() const { return right; }
	const Vector3 & getForward() const { return forward; }
	const Vector3 & getUp() const { return up; }

	bool getDirty() const { return dirty; }
	void setDirty(bool dirty) { this->dirty = dirty; }

	//void transformUpdate(bool dirty, const D3DXMATRIX & parentRotationMatrix, const D3DXMATRIX & parentPositionMatrix);


	void transformUpdate(bool dirty, const D3DXMATRIX & parentPositionMatrix, const D3DXMATRIX & parentRotationMatrix);
	const D3DXMATRIX & getTransformMatrix_DX() const { return transformMatrix_DX; }
	void setTransformMatrix_DX(const D3DXMATRIX & parentPositionMatrix, const D3DXMATRIX & parentRotationMatrix)
	{
		// 중요 :: 부모에의해 부모의 로테이션만큼 더 돌게 되어있다. (부모 + 자식 로테이션) > 부모기준(forward up right) 자식 위치 + 부모위치
		calcRotationMatrix_DX(parentRotationMatrix); calcPositionMatrix_DX(parentPositionMatrix);
		transformMatrix_DX = rotationMatrix_DX* positionMatrix_DX;
	}

	
	const D3DXMATRIX & getRotationMatrix_DX() const { return rotationMatrix_DX; }
	void calcRotationMatrix_DX(const D3DXMATRIX & parentRotationMatrix);

	const D3DXMATRIX & getPositionMatrix_DX() const { return positionMatrix_DX; }
	void calcPositionMatrix_DX(const D3DXMATRIX & parentPositionMatrix);




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

	void setPosition_physics(const Vector3 & other);
	void setRotation_physics(const Vector3 & other);
	void setScale_physics(const Vector3 & other);

	void setPosition(const Vector3 & other);
	void setRotation(const Vector3 & other);

	void setScale(const Vector3 & other) { scale = other; }

	void setPosition(float x, float y, float z);
	void setRotation(float x, float y, float z);

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

class MoveScript : public Component
{
private:
	//AnimationFSM * animationFSM;
protected:
	virtual void update();
	virtual void start();
	virtual void onCollisionStay(GameObjectWithCollision & other);
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

	float calcXMax() const { return transform->getPosition().getX() + width; }
	float calcXMin() const { return transform->getPosition().getX() - width; }
	float calcYMax() const { return transform->getPosition().getY() + height; }
	float calcYMin() const { return transform->getPosition().getY() - height; }
	float calcZMax() const { return transform->getPosition().getZ() + depth; }
	float calcZMin() const { return transform->getPosition().getZ() - depth; }

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
		Vector3 direction = transform->getPosition() - other.transform->getPosition();
		Vector3::Normalized(direction, direction);
		transform->setPosition(transform->getPosition() + 1 * direction);
	}


};

class RigidBody : public Component
{
private:

protected:
	virtual void start();
	virtual void fixedUpdate();
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
		// 속성으로 삼각형을 정렬하고 속성테이블을 생성 | 사용되지 않는 인덱스와 버텍스 제거 | 버텍스 캐시의 히트율 높임
	};


protected:


	virtual void update()
	{
		render();
		if (::GetAsyncKeyState('F') & 0x8000f)
			setScale(Vector3(1, 1, 1));

		if (::GetAsyncKeyState('G') & 0x8000f)
			setScale(Vector3(1, 3, 1));

		if (::GetAsyncKeyState('H') & 0x8000f)
			setScale(Vector3(3, 1, 3));

		if (::GetAsyncKeyState('J') & 0x8000f)
			setScale(Vector3(3, 3, 3));
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


	void updateAllTrasitions(const string & animationFileName);


};

#endif