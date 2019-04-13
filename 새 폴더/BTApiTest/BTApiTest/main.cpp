#include <cstdarg>
#include <iostream>
#include <vector>
#include <string>

// OpenBranch���Ŀ� �������ΰ��� �˷��ֱ� ���� NULL�� ������ �Ķ���ͷ� �Ѱ�����Ѵ�.
// ���ڿ��� ������ NULL������ �ϴ� ��ó�� 
// ���⼭�� �������� �Ķ���͵��� ������ ������ �Ķ���Ͱ� NULL�϶� ���� �д´�.
#define BTNodeBranchEnd NULL


//template <typename T, T> struct proxy;
//
//template <typename T, typename R, typename ...Args, R(T::*mf)(Args...)>
//struct proxy<R(T::*)(Args...), mf>
//{
//	static R call(T & obj, Args &&... args)
//	{
//		return (obj.*mf)(std::forward<Args>(args)...);
//	}
//};


using namespace std;

/* 
�Լ� ��ü ����
*/

// �տ��� ��������� �ν�
template <typename T, T> struct BoolFunc;

template <typename T, bool(T::*fn)() /* typename R, typename ...Args, R(T::*mf)(Args...)*/>
struct BoolFunc<bool (T::*)(), fn>
{
	T * obj;
	BoolFunc(T * obj)
		: obj(obj) 
	{}

	bool call(/*T & obj, Args &&... args*/)
	{
		if (!obj) return false;
		return (obj->*fn)();
	}
};

// �տ��� ��������� �ν�
template <typename T, T> struct VoidFunc;

template <typename T, void(T::*fn)() /* typename R, typename ...Args, R(T::*mf)(Args...)*/>
struct VoidFunc<void (T::*)(), fn>
{
	T * obj;
	VoidFunc(T * obj)
		: obj(obj)
	{}

	void call(/*T & obj, Args &&... args*/)
	{
		if (!obj) return;
		(obj->*fn)();
	}
};




enum class BTNodeState
{
	Success,
	Failure,
	Continue
};

/*
BTNode�� ��ӹ��� ��ü���� ��ĳ���� �ؼ� ����Ѵ�.
�⺻�� �Ǵ� BTNode // ��ĳ�����ؼ� ����Ѵ�.
*/
class BTNode
{
private :
	string name;
public : 
	BTNode(const string & name)
		: name(name) {}

	virtual ~BTNode() {}
	// ���������� ����� ��忡�� ���� �������̵� �ȴ�.
	virtual BTNodeState tick() = 0;
	const string & getName() { return name; }

	// BranchŸ���� ���� ����ϱ� ������ �׻� �������̵� ���� �ʴ´� // ���������Լ��� �� �� ������.
	// �׳� Branch���� ����Լ��� ����ϰ� �;�����, BTNodeŸ���� BranchŸ������ Ȯ���ϴ� ���� dynamic_cast�� �ʿ��ϴ�.
	// ������ �����Ͱ��Ƽ� �⺻���� �����̵� �����Լ��� ����ߴ�.
	virtual BTNode * openBranch(BTNode * child, ...) { return this; }

};

template <typename T, void(T::*fn)()>
class Action : public BTNode
{
private :
	//void(*fn)();
	struct VoidFunc<void (T::*)(), fn> functionObj;
public:
	Action(T * obj)
		: BTNode("Action"), functionObj(obj)
	{}
	virtual ~Action() {}
	virtual BTNodeState tick()
	{
		functionObj.call();
		return BTNodeState::Success;
	}
};


template <typename T, bool(T::*fn)()>
class Condition : public BTNode
{
private:
	//bool(*fn)();

	struct BoolFunc<bool (T::*)(), fn> functionObj;

public:
	Condition(T * obj)
		: BTNode("Condition"), functionObj(obj)
	{}
	virtual ~Condition() {}


	virtual BTNodeState tick()
	{
		bool result = functionObj.call();

		if (result)
			return BTNodeState::Success;

		return BTNodeState::Failure;
	}
};


/*
 ������ ���� ��带 �߰��� �� �ִ� Ŭ����
 �� Ŭ������ ��ӹ��� ��� �ڽ� ������ �߰��� �� �ִ�. // openBranch
*/
// ��Ŭ������ ���������� ����ϴ� ���� ����. // ���� ���������� �ʾ���


class Branch : public BTNode
{
protected : 
	vector<BTNode *> children;
	//unsigned int activeChildren;

public :
	Branch(const string & name)
		: BTNode(name) //activeChildren(0)
	{}

	virtual ~Branch()
	{
		cout << "delete branch children size :: " <<children.size()<< endl;
		Branch * targetBranch = nullptr;
		for (auto it = children.begin(); it != children.end(); ++it)
		{
			// ���������� BranchŸ���� ��� ��������� ����

			if ((*it) != nullptr)
			{
				delete (*it);
				(*it) = nullptr;
			}

		}
		children.clear();
	}
	virtual BTNode * openBranch(BTNode * child, ...)
	{
		va_list childList;

		va_start(childList, child);

		// ó�� ���� ���� �״�� �޾Ƽ� �־��ְ� �״��� ���ʹ� child���� argument���� ���ʴ�� �޾Ƽ� �־��ش�.
		// ���� NULL���� ���´ٸ� �����ش�. // �������� �ǹ� // �Ѱ��ٶ� ��������� �Ѵ�. // BTNodeEnd
		for (va_start(childList, child); child != NULL; child = va_arg(childList, BTNode *))
		{
			if(child != nullptr)
				this->children.push_back(child);
		}

		va_end(childList);
		return this;
	}

};



/*
 ��ϴ����� �����ų �� �ִ� Ŭ����
 ���� ��忡 ������� ��� �����Ų��. // �� ��� ���°� Continue�� ��� �����Ѵ�.
 Success �� Failure�� ��� ��� �����Ѵ�.
 ������ ���̶�� ���δ� �׻� �����ϴ� ��带 �����ϰų� �׻� ������Ѿ��ϴ� ��带 �����ϱ⿡ �˸´�. // If / While / Root
*/
// ��Ŭ������ ���������� ����ϴ� ���� ����. // ���� ���������� �ʾ���
class Block : public Branch
{
public :
	Block(const string & name)
		: Branch(name)
	{ }
	virtual ~Block(){}

	// �ڽĳ�尡 �ϳ��� Continue�� ��ȯ���� ������ ��ν����� Success ��ȯ
	virtual BTNodeState tick()
	{
		BTNodeState state;
		for (auto it = children.begin(); it != children.end(); ++it)
		{
			state = (*it)->tick();

			// ������ ���� �ڽ� ������ �ϳ��� Continue�� ��ȯ�Ѵٸ� Continue�� ��ȯ�Ѵ�.
			if (state == BTNodeState::Continue)
				return BTNodeState::Continue;
		}

		// �ݺ����� ��� ���������� �����ٸ� Continue�� ��ȯ���� �ʾҴٴ� ���� �ȴ�.
		// �� Success�� ��ȯ�Ѵ�.
		return BTNodeState::Success;
	}


};

class Root : public Block
{
public:
	Root()
		: Block("Root")
	{ }

	virtual ~Root() {}
	// ��Ʈ ���� �⺻������ ���ǵ� Block�� tick�Լ��� ������.
	//	virtual BTNodeState tick()

};



template <typename T, bool(T::*fn)()>
class While : public Block
{
private :
	//bool(*fn)();
	struct BoolFunc<bool (T::*)(), fn> functionObj;
public :
	While(T * obj)
		: Block("While"), functionObj(obj)
	{}

	virtual ~While() {}
	// ���� �Լ������ͷ� ����� �Լ��� �򰡰��� ���� �ٸ� ������ ���ش�.
	/*
	true : ���� �ڽ� ������ tick()���ְ� (���� �߰��� Continue������ return Coninue) Continue ��ȯ
	false : Failure��ȯ
	*/
	virtual BTNodeState tick()
	{
		bool result = functionObj.call();

		if(result)
		{
			// �⺻���� �θ�Ŭ������ ���ؼ� �߰��� Continue �ڽĳ�尡 ������ Continue�� ��ȯ�Ѵ�. // �߰��� �����.
			Block::tick();

			// �׻� �Լ��� �򰡰��� true�̸� Continue�� ��ȯ�ϴµ� �̶� Ʈ�� ��ȸ�� ���߰� �ȴ�.
			return BTNodeState::Continue;
		}
		
		// �Լ��� �򰡰��� false��� Failure��ȯ
		return BTNodeState::Failure;
	}

};


template <typename T, bool(T::*fn)()>
class If : public Block
{
private:
	//bool(*fn)();
	struct BoolFunc<bool (T::*)(), fn> functionObj;
public:
	If(T * obj)
		: Block("If"), functionObj(obj)
	{}

	virtual ~If() {}
	// ���� �Լ������ͷ� ����� �Լ��� �򰡰��� ���� �ٸ� ������ ���ش�.
	/*
	true : ���� �ڽ� ������ tick()���ְ� (���� �߰��� Continue������ return Coninue) Continue ��ȯ
	false : Failure��ȯ
	*/
	virtual BTNodeState tick()
	{
		bool result = functionObj.call();

		if (result)
		{
			// �⺻���� �θ�Ŭ������ ���ؼ� �߰��� Continue �ڽĳ�尡 ������ Continue�� ��ȯ�Ѵ�. // �߰��� �����.
			BTNodeState state = Block::tick();

			// ���� �ڽĳ���� Continue�� ������ Success�� ��ȯ�Ѵ�.
			return state;
		}

		// �Լ��� �򰡰��� false��� Failure��ȯ
		return BTNodeState::Failure;
	}
};


/*
�ϳ��� ��常 ���õǾ� ����ȴ�. // ��� ��尡 ������ �ȵǴ� ��쵵 ����
���� �ڽĳ�尡 �ϳ��� Success�� ��ȯ�ϸ� �ٷ� Success ��ȯ
��� Failure�̸� Failure ��ȯ
*/
class Selector : public Branch
{
public:
	Selector()
		: Branch("Selector")
	{}

	virtual ~Selector() {}
	virtual BTNodeState tick()
	{
		BTNodeState state;

		for (auto it = children.begin(); it != children.end(); ++it)
		{
			state = (*it)->tick();

			if (state == BTNodeState::Continue)
				return BTNodeState::Continue;

			if (state == BTNodeState::Success)
				return BTNodeState::Success;
		}

		// ��� ��ȸ������ Continue�� Success�� ��ȯ���� ���ߴٸ�
		// Failure ��ȯ
		return BTNodeState::Failure;
	}
};

/*
��� ��尡 Success�� ��ȯ�Ҷ� Success�� ��ȯ
*/
class Suquence : public Branch
{
public:
	Suquence()
		: Branch("Suquence")
	{}

	virtual ~Suquence() {}
	virtual BTNodeState tick()
	{
		BTNodeState state;

		for (auto it = children.begin(); it != children.end(); ++it)
		{
			state = (*it)->tick();

			if (state == BTNodeState::Continue)
				return BTNodeState::Continue;

			if (state == BTNodeState::Failure)
				return BTNodeState::Failure;
		}

		// ��� ��ȸ������ Continue�� Failure�� ��ȯ���� �ʾҴٸ�
		// ��� ���� �Ǿ��ٴ� �� Success ��ȯ 
		return BTNodeState::Success;
	}
};

bool isTargeting()
{
	return true;
}

bool inRange1()
{
	return false;
}

bool inRange2()
{
	return false;
}

bool inRange3()
{
	return true;
}

float distance = 3.0f;


class TempClass
{
public :
	void func() { cout << "hello world!" << endl; }
	bool boolTrueFunc() { return true; }
	bool boolFalseFunc() { return false; }
};

#include <Windows.h>
#include <conio.h>




int main()
{
	TempClass temp;

	BTNode * root = new Root();
	root->openBranch
	(
		(new Selector())->openBranch
		(
			(new Suquence())->openBranch
			(
				// Ÿ�ٰ˻�
				new Condition<TempClass, &TempClass::boolTrueFunc>(&temp),
				(new Selector())->openBranch
				(
					// Range1�϶�
					(new If<TempClass, &TempClass::boolFalseFunc>(&temp))->openBranch
					(
						//new Action([]()->void {cout << "action in Range1" << endl; }),
						BTNodeBranchEnd
					),

					// Range2�϶�
					(new If<TempClass, &TempClass::boolFalseFunc>(&temp))->openBranch
					(
						//new Action([]()->void {cout << "action in Range2" << endl; }),
						BTNodeBranchEnd
					),

					// Range3�϶�
					(new If<TempClass, &TempClass::boolFalseFunc>(&temp))->openBranch
					(
						//new Action([]()->void {cout << "action in Range3" << endl; }),
						BTNodeBranchEnd
					),
					BTNodeBranchEnd
				),
				BTNodeBranchEnd
			),

			// ���Ͱ˻�
			(new If<TempClass, &TempClass::boolFalseFunc>(&temp))->openBranch
			(
				//new Action([]()->void {cout << "action in return"<<endl; }),
				BTNodeBranchEnd
			),

			// ����Ҷ� ���� �����ֱ�
			(new While<TempClass, &TempClass::boolTrueFunc>(&temp))->openBranch
			(
				//new Action([]()->void {cout << "action in Range3" << endl; }),
				new Action<TempClass, &TempClass::func>(&temp),
				BTNodeBranchEnd
			),
			BTNodeBranchEnd
		),
		BTNodeBranchEnd
	);

	char input;
	BTNodeState state;
	while (true)
	{
		state =  root->tick();
		switch (state)
		{
		case BTNodeState::Continue:
			cout << "Continue" << endl;
			break;
		case BTNodeState::Success:
			cout << "Success" << endl;
			break;
		case BTNodeState::Failure:
			cout << "Failure" << endl;
			break;
		}
		if (_kbhit())
		{
			input = _getch();
			if (input == 'a')
				break;
		}

		Sleep(300);
	}

	delete root;


	return 0;
}