#ifndef COMPONENT_H
#define COMPONENT_H

#include "Utility.h"
#include <vector>

class Transform;
class GameObject;

class Component
{
private:
	GameObject * gameObject;
	Transform * transform;

protected:
	// GameObject클래스내에서 Components를 계속해서 업데이트 시켜줘야 하기 때문에 friend 등록하였다.
	friend class GameObject;
	virtual void update() {}
	virtual void start() { cout << "Component start" << endl; }

public:
	Component(GameObject * go, Transform * tf)
		: gameObject(go), transform(tf) 
	{
		start();
	}

	// 내부적으로 계속해서 바꿔야 하기 때문에 const선언을 해주지 않았다.
	// ex) 위치정보 변경 > GameObject정보변경 + Transform정보변경
	Transform * getTransform() { return transform; }
	GameObject * getGameObject() { return gameObject; }

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

	// 여기서 const객체를 입력파라미터로 받아올 수 없는 이유는 Component객체를 초기화할때 필요한정보는 계속해서 변경되는 정보이기 때문에
	// const Transform & other로 받으면 Component를 초기화할때 Transform내부의 GameObject와 Transform를 read only로 밖에 받아올 수 없다.
	Transform(Transform & other)
		: position(other.position), rotation(other.rotation), scale(other.scale), Component(other.getGameObject(), this) {}

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

class MoveScript : public Component
{
private :

protected :
	virtual void update();
	virtual void start();

public :
	MoveScript(GameObject * go, Transform * tf)
		: Component(go, tf) 
	{
		// 만약에 그냥 Component 클래스 생성자 함수바디에서 start()함수 호출?
		// 생성되는 과정이기 때문에 오버라이딩이 적용되지 않는다 // 부모생성완료(이과정에서 자식의 재정의함수를 찾을수없다.) > 자식생성완료
		start();
	}

};

#endif