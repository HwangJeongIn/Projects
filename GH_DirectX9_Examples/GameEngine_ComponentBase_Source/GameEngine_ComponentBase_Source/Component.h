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
	// GameObjectŬ���������� Components�� ����ؼ� ������Ʈ ������� �ϱ� ������ friend ����Ͽ���.
	friend class GameObject;
	virtual void update() {}
	virtual void start() { cout << "Component start" << endl; }

public:
	Component(GameObject * go, Transform * tf)
		: gameObject(go), transform(tf) 
	{
		start();
	}

	// ���������� ����ؼ� �ٲ�� �ϱ� ������ const������ ������ �ʾҴ�.
	// ex) ��ġ���� ���� > GameObject�������� + Transform��������
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

	// ���⼭ const��ü�� �Է��Ķ���ͷ� �޾ƿ� �� ���� ������ Component��ü�� �ʱ�ȭ�Ҷ� �ʿ��������� ����ؼ� ����Ǵ� �����̱� ������
	// const Transform & other�� ������ Component�� �ʱ�ȭ�Ҷ� Transform������ GameObject�� Transform�� read only�� �ۿ� �޾ƿ� �� ����.
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
		// ���࿡ �׳� Component Ŭ���� ������ �Լ��ٵ𿡼� start()�Լ� ȣ��?
		// �����Ǵ� �����̱� ������ �������̵��� ������� �ʴ´� // �θ�����Ϸ�(�̰������� �ڽ��� �������Լ��� ã��������.) > �ڽĻ����Ϸ�
		start();
	}

};

#endif