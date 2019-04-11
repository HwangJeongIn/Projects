#include <cstdarg>
#include <iostream>
#include <vector>

using namespace std;

/*
BTNode�� ��ӹ��� ��ü���� ��ĳ���� �ؼ� ����Ѵ�.
*/

enum class BTNodeState
{
	Success,
	Failure,
	Continue
};

// �⺻�� �Ǵ� BTNode // ��ĳ�����ؼ� ����Ѵ�.
class BTNode
{
public : 
	// ���������� ����� ��忡�� ���� �������̵� �ȴ�.
	virtual BTNodeState tick() = 0;
};

class Action : public BTNode
{
private :
	void(*fn)();
public:
	Action(void (* fn)())
		: fn(fn)
	{}

	virtual BTNodeState tick()
	{
		fn();
		return BTNodeState::Success;
	}
};


class Condition : public BTNode
{
private:
	bool(*fn)();
public:
	Condition(bool(*fn)())
		: fn(fn)
	{}

	virtual BTNodeState tick()
	{
		bool result = fn();
		if (result)
			return BTNodeState::Success;

		return BTNodeState::Failure;
	}
};

// ������ ���� ��带 �߰��� �� �ִ� Ŭ����
// �� Ŭ������ ��ӹ��� ��� �ڽ� ������ �߰��� �� �ִ�. // openBranch
class Branch : public BTNode
{
protected : 
	vector<BTNode *> children;
	unsigned int activeChildren;

public :
	Branch()
		: activeChildren(0)
	{}

	virtual ~Branch()
	{
		for (auto it = children.begin(); it != children.end(); ++it)
		{
			// ���������� BranchŸ���� ��� ��������� ����
			if ((*it) != nullptr)
			{
				delete (*it);
				(*it) = nullptr;
			}
			children.clear();
		}
	}
	void openBranch(BTNode * child, ...)
	{
		va_list children;

		for (va_start(children, child); child != NULL; child = va_arg(children, BTNode *)) 
		{
			if(child != nullptr)
				this->children.push_back(child);
		}

		va_end(children);
	}

};


// ��ϴ����� �����ų �� �ִ� Ŭ����
// ���� ��忡 ������� ��� �����Ų��. // �� ��� ���°� Continue�� ��� �����Ѵ�.
// Success �� Failure�� ��� ��� �����Ѵ�.
//  ������ ���̶�� ���δ� �׻� �����ϴ� ��带 �����ϰų� �׻� ������Ѿ��ϴ� ��带 �����ϱ⿡ �˸´�. // If / While / Root
class Block : public Branch
{
public :
	Block()
	//	:
	{ }


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
		//	:
	{ }

	// ��Ʈ ���� �⺻������ ���ǵ� Block�� tick�Լ��� ������.
	//	virtual BTNodeState tick()

};

class While : public Block
{
private :
	bool(*fn)();

public :
	While(bool(*fn)())
		: fn(fn)
	{}

	// ���� �Լ������ͷ� ����� �Լ��� �򰡰��� ���� �ٸ� ������ ���ش�.
	/*
	true : ���� �ڽ� ������ tick()���ְ� (���� �߰��� Continue������ return Coninue) Continue ��ȯ
	false : Failure��ȯ
	*/
	virtual BTNodeState tick()
	{
		bool result = fn();

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

class If : public Block
{
private:
	bool(*fn)();

public:
	If(bool(*fn)())
		: fn(fn)
	{}

	// ���� �Լ������ͷ� ����� �Լ��� �򰡰��� ���� �ٸ� ������ ���ش�.
	/*
	true : ���� �ڽ� ������ tick()���ְ� (���� �߰��� Continue������ return Coninue) Continue ��ȯ
	false : Failure��ȯ
	*/
	virtual BTNodeState tick()
	{
		bool result = fn();

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

// ���� �ڽĳ�尡 �ϳ��� Success�� ��ȯ�ϸ� �ٷ� Continue ��ȯ
// ��� Failure�̸� Failure ��ȯ
class Selector : public Branch
{
public:
	Selector()
	//	: 
	{}

	virtual BTNodeState tick()
	{
		BTNodeState state;

		for (auto it = children.begin(); it != children.end(); ++it)
		{
			state = (*it)->tick();
			if (state == BTNodeState::Success)
				return BTNodeState::Continue;
		}

		// �Լ��� �򰡰��� false��� Failure��ȯ
		return BTNodeState::Failure;
	}
};

class Suquence : public Branch
{

};


int main()
{


	return 0;
}