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
	void setDirty(bool dirty) { this->dirty = dirty; }

	//void transformUpdate(bool dirty, const D3DXMATRIX & parentRotationMatrix, const D3DXMATRIX & parentPositionMatrix);


	void transformUpdate(bool dirty, const D3DXMATRIX & parentPositionMatrix, const D3DXMATRIX & parentRotationMatrix);
	const D3DXMATRIX & getTransformMatrix_DX() const { return transformMatrix_DX; }
	void setTransformMatrix_DX(const D3DXMATRIX & parentPositionMatrix, const D3DXMATRIX & parentRotationMatrix)
	{
		// �߿� :: �θ����� �θ��� �����̼Ǹ�ŭ �� ���� �Ǿ��ִ�. (�θ� + �ڽ� �����̼�) > �θ����(forward up right) �ڽ� ��ġ + �θ���ġ
		calcRotationMatrix_DX(parentRotationMatrix); calcPositionMatrix_DX(parentPositionMatrix);
		transformMatrix_DX = rotationMatrix_DX* positionMatrix_DX;
	}

	
	const D3DXMATRIX & getRotationMatrix_DX() const { return rotationMatrix_DX; }
	void calcRotationMatrix_DX(const D3DXMATRIX & parentRotationMatrix);

	const D3DXMATRIX & getPositionMatrix_DX() const { return positionMatrix_DX; }
	void calcPositionMatrix_DX(const D3DXMATRIX & parentPositionMatrix);




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
		// ���࿡ �׳� Component Ŭ���� ������ �Լ��ٵ𿡼� start()�Լ� ȣ��?
		// �����Ǵ� �����̱� ������ �������̵��� ������� �ʴ´� // �θ�����Ϸ�(�̰������� �ڽ��� �������Լ��� ã��������.) > �ڽĻ����Ϸ�
		start();
	}
	
	virtual ~MoveScript()
	{
		onDestroy();
	}

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

	float calcXMax() const { return transform->getPosition().getX() + width; }
	float calcXMin() const { return transform->getPosition().getX() - width; }
	float calcYMax() const { return transform->getPosition().getY() + height; }
	float calcYMin() const { return transform->getPosition().getY() - height; }
	float calcZMax() const { return transform->getPosition().getZ() + depth; }
	float calcZMin() const { return transform->getPosition().getZ() - depth; }

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
	// �������� �޽������� ��� ����
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

	// �Ӽ����� �ﰢ���� �����ϰ� �Ӽ����̺��� ���� | ������ �ʴ� �ε����� ���ؽ� ���� | ���ؽ� ĳ���� ��Ʈ�� ����
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
			// 0���� �۰ų� 8���� ũ�ų� ������ ���α׷� �����.
			// �׻� 0���� ũ�ų� ���ų� 8���ٴ� �۾ƾ��Ѵ�.
			assert(index >= 0 && index < sizeof(FbxModelVertex) / sizeof(float));
			
			float * result = (float*)this;
			result += index;
			return *result;
		}

		static const unsigned long DefaultFVF;
		// �Ӽ����� �ﰢ���� �����ϰ� �Ӽ����̺��� ���� | ������ �ʴ� �ε����� ���ؽ� ���� | ���ؽ� ĳ���� ��Ʈ�� ����
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

	// �ܺ� �ִϸ��̼� ���� �ҷ��ö� ���
	void loadFbxFileForAnimation(const string & fileName);

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


	void updateAllTrasitions(const string & animationFileName);


};

#endif