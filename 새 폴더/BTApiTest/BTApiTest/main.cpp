#include <cstdarg>
#include <iostream>
#include <vector>
#include <string>

// OpenBranch이후에 마지막인것을 알려주기 위한 NULL값 마지막 파라미터로 넘겨줘야한다.
// 문자열이 마지막 NULL값으로 하는 것처럼 
// 여기서도 여러가지 파라미터들을 받을때 마지막 파라미터가 NULL일때 까지 읽는다.
#define BTNodeBranchEnd NULL

using namespace std;



enum class BTNodeState
{
	Success,
	Failure,
	Continue
};

/*
BTNode를 상속받은 객체들을 업캐스팅 해서 사용한다.
기본이 되는 BTNode // 업캐스팅해서 사용한다.
*/
class BTNode
{
private :
	string name;
public : 
	BTNode(const string & name)
		: name(name) {}

	virtual ~BTNode() {}
	// 최종적으로 사용할 노드에서 보통 오버라이딩 된다.
	virtual BTNodeState tick() = 0;
	const string & getName() { return name; }

	// Branch타입일 때만 사용하기 때문에 항상 오버라이딩 되지 않는다 // 순수가상함수로 할 수 없었다.
	// 그냥 Branch만의 멤버함수로 사용하고 싶었지만, BTNode타입이 Branch타입인지 확인하는 과정 dynamic_cast가 필요하다.
	// 연산이 많을것같아서 기본적인 구현이된 가상함수를 사용했다.
	virtual BTNode * openBranch(BTNode * child, ...) { return this; }

};

class Action : public BTNode
{
private :
	void(*fn)();
public:
	Action(void (* fn)())
		: BTNode("Action"), fn(fn)
	{}
	virtual ~Action() {}
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
		: BTNode("Condition"), fn(fn)
	{}
	virtual ~Condition() {}
	virtual BTNodeState tick()
	{
		bool result = fn();
		if (result)
			return BTNodeState::Success;

		return BTNodeState::Failure;
	}
};


/*
 가지를 만들어서 노드를 추가할 수 있는 클래스
 이 클래스를 상속받은 경우 자식 노드들을 추가할 수 있다. // openBranch
*/
// 이클래스만 독립적으로 사용하는 경우는 없다. // 아직 제한해주지 않았음
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
			// 내부적으로 Branch타입인 경우 재귀적으로 삭제

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

		// 처음 값은 들어온 그대로 받아서 넣어주고 그다음 부터는 child다음 argument들을 차례대로 받아서 넣어준다.
		// 만약 NULL값이 나온다면 멈춰준다. // 마지막을 의미 // 넘겨줄때 주의해줘야 한다. // BTNodeEnd
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
 블록단위로 실행시킬 수 있는 클래스
 내부 노드에 상관없이 모두 실행시킨다. // 단 노드 상태가 Continue일 경우 리턴한다.
 Success 와 Failure일 경우 모두 실행한다.
 조건이 참이라면 내부는 항상 실행하는 노드를 정의하거나 항상 실행시켜야하는 노드를 실행하기에 알맞다. // If / While / Root
*/
// 이클래스만 독립적으로 사용하는 경우는 없다. // 아직 제한해주지 않았음
class Block : public Branch
{
public :
	Block(const string & name)
		: Branch(name)
	{ }
	virtual ~Block(){}

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
		: Block("Root")
	{ }

	virtual ~Root() {}
	// 루트 노드는 기본적으로 정의된 Block의 tick함수를 따른다.
	//	virtual BTNodeState tick()

};

class While : public Block
{
private :
	bool(*fn)();

public :
	While(bool(*fn)())
		: Block("While"), fn(fn)
	{}

	virtual ~While() {}
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
		: Block("If"), fn(fn)
	{}

	virtual ~If() {}
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


/*
하나의 노드만 선택되어 실행된다. // 모든 노드가 실행이 안되는 경우도 있음
만약 자식노드가 하나라도 Success를 반환하면 바로 Success 반환
모두 Failure이면 Failure 반환
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

		// 모두 순회했지만 Continue나 Success를 반환하지 못했다면
		// Failure 반환
		return BTNodeState::Failure;
	}
};

/*
모든 노드가 Success를 반환할때 Success를 반환
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

		// 모두 순회했지만 Continue나 Failure를 반환하지 않았다면
		// 모두 실행 되었다는 뜻 Success 반환 
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

#include <Windows.h>
#include <conio.h>

int main()
{
	BTNode * root = new Root();
	root->openBranch
	(
		(new Selector())->openBranch
		(
			(new Suquence())->openBranch
			(
				// 타겟검사
				new Condition([]()-> bool {return false; }),
				(new Selector())->openBranch
				(
					// Range1일때
					(new If([]()-> bool {return false; }))->openBranch
					(
						new Action([]()->void {cout << "action in Range1" << endl; }),
						BTNodeBranchEnd
					),

					// Range2일때
					(new If([]()-> bool {return false; }))->openBranch
					(
						new Action([]()->void {cout << "action in Range2" << endl; }),
						BTNodeBranchEnd
					),

					// Range3일때
					(new If([]()-> bool {return false; }))->openBranch
					(
						new Action([]()->void {cout << "action in Range3" << endl; }),
						BTNodeBranchEnd
					),
					BTNodeBranchEnd
				),
				BTNodeBranchEnd
			),

			// 복귀검사
			(new If([]()-> bool {return false; }))->openBranch
			(
				new Action([]()->void {cout << "action in return"<<endl; }),
				BTNodeBranchEnd
			),

			// 대기할때 할일 정해주기
			(new While([]()-> bool {return true; }))->openBranch
			(
				new Action([]()->void {cout << "action in idle" << endl; }),
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