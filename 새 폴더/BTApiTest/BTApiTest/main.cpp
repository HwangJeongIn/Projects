#include <cstdarg>
#include <iostream>
#include <vector>

using namespace std;

/*
BTNode�� ��ӹ��� ��ü���� ��ĳ���� �ؼ� ����Ѵ�.

*/

enum class NodeState
{
	Success,
	Failure,
	Continue
};

class BTNode
{
public : 
	// ���������� ����� ��忡�� ���� �������̵� �ȴ�.
	virtual NodeState tick() = 0;
};

class Action : public BTNode
{
private :
	void(*fn)();
public:
	Action(void (* fn)())
		: fn(fn)
	{}

	virtual NodeState tick()
	{
		fn();
		return NodeState::Success;
	}
};

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
			if(child)
				this->children.push_back(child);
		}

		va_end(children);
	}

};

class Block : public Branch
{

};

class Root : public Block
{

};

class While : public Block
{

};


class If : public Block
{

};

class Selector : public Branch
{

};

class Suquence : public Branch
{

};

class Condition : public BTNode
{

};

int main()
{


	return 0;
}