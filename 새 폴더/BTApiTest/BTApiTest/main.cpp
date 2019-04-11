#include <cstdarg>
#include <iostream>
#include <vector>

using namespace std;

/*
BTNode를 상속받은 객체들을 업캐스팅 해서 사용한다.

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
	// 최종적으로 사용할 노드에서 보통 오버라이딩 된다.
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
			// 내부적으로 Branch타입인 경우 재귀적으로 삭제
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