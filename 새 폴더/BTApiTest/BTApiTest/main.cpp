#include <cstdarg>
#include <iostream>
#include <vector>

using namespace std;

/*
BTNode를 상속받은 객체들을 업캐스팅 해서 사용한다.
*/

enum class BTNodeState
{
	Success,
	Failure,
	Continue
};

// 기본이 되는 BTNode // 업캐스팅해서 사용한다.
class BTNode
{
public : 
	// 최종적으로 사용할 노드에서 보통 오버라이딩 된다.
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

// 가지를 만들어서 노드를 추가할 수 있는 클래스
// 이 클래스를 상속받은 경우 자식 노드들을 추가할 수 있다. // openBranch
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
			if(child != nullptr)
				this->children.push_back(child);
		}

		va_end(children);
	}

};


// 블록단위로 실행시킬 수 있는 클래스
// 내부 노드에 상관없이 모두 실행시킨다. // 단 노드 상태가 Continue일 경우 리턴한다.
// Success 와 Failure일 경우 모두 실행한다.
//  조건이 참이라면 내부는 항상 실행하는 노드를 정의하거나 항상 실행시켜야하는 노드를 실행하기에 알맞다. // If / While / Root
class Block : public Branch
{
public :
	Block()
	//	:
	{ }


	// 자식노드가 하나라도 Continue를 반환하지 않으면 모두실행후 Success 반환
	virtual BTNodeState tick()
	{
		BTNodeState state;
		for (auto it = children.begin(); it != children.end(); ++it)
		{
			state = (*it)->tick();

			// 만약의 내부 자식 노드들이 하나라도 Continue를 반환한다면 Continue를 반환한다.
			if (state == BTNodeState::Continue)
				return BTNodeState::Continue;
		}

		// 반복문이 모두 정상적으로 끝난다면 Continue를 반환하지 않았다는 것이 된다.
		// 즉 Success를 반환한다.
		return BTNodeState::Success;
	}


};

class Root : public Block
{
public:
	Root()
		//	:
	{ }

	// 루트 노드는 기본적으로 정의된 Block의 tick함수를 따른다.
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

	// 내부 함수포인터로 등록한 함수의 평가값에 따라서 다른 실행을 해준다.
	/*
	true : 내부 자식 노드들을 tick()해주고 (물론 중간에 Continue나오면 return Coninue) Continue 반환
	false : Failure반환
	*/
	virtual BTNodeState tick()
	{
		bool result = fn();

		if(result)
		{
			// 기본적인 부모클래스에 의해서 중간에 Continue 자식노드가 있을때 Continue를 반환한다. // 중간에 멈춘다.
			Block::tick();

			// 항상 함수의 평가값이 true이면 Continue를 반환하는데 이때 트리 순회가 멈추게 된다.
			return BTNodeState::Continue;
		}
		
		// 함수의 평가값이 false라면 Failure반환
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

	// 내부 함수포인터로 등록한 함수의 평가값에 따라서 다른 실행을 해준다.
	/*
	true : 내부 자식 노드들을 tick()해주고 (물론 중간에 Continue나오면 return Coninue) Continue 반환
	false : Failure반환
	*/
	virtual BTNodeState tick()
	{
		bool result = fn();

		if (result)
		{
			// 기본적인 부모클래스에 의해서 중간에 Continue 자식노드가 있을때 Continue를 반환한다. // 중간에 멈춘다.
			BTNodeState state = Block::tick();

			// 보통 자식노드의 Continue가 없을때 Success를 반환한다.
			return state;
		}

		// 함수의 평가값이 false라면 Failure반환
		return BTNodeState::Failure;
	}
};

// 만약 자식노드가 하나라도 Success를 반환하면 바로 Continue 반환
// 모두 Failure이면 Failure 반환
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

		// 함수의 평가값이 false라면 Failure반환
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