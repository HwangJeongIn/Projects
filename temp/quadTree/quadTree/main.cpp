#include <iostream>
#include <vector>

using namespace std;



/*
1. �ͷ����� ��ü ����� �����Ѵ�.
2. ����ؿ� ���ؼ� 4���� ���� �и��Ѵ�.
	// �����?
3. 
*/

class Vector3
{
public :
	float x; 
	float y;
	float z;

	Vector3(float x, float y, float z)
		: x(x), y(y), z(z) {}

	Vector3(const Vector3 & other)
		: x(other.x), y(other.y), z(other.z) {}
	Vector3 & operator=(const Vector3 & other)
	{
		x = other.x;
		y = other.y;
		z = other.z;
		return *this;
	}

	Vector3 operator+(const Vector3 & other)
	{
		return{ other.x + x, other.y + y, other.z + z };
	}

	friend ostream & operator<<(ostream & console, const Vector3 & other)
	{
		console << other.x << " , " << other.y << " , " << other.z;
		return console;
	}

};

class QuadTree;

class QuadTreeNode
{

private :
	vector<QuadTreeNode*> children;
	QuadTreeNode * parent;

	Vector3 startPoint;
	float width;
	float depth;

	// vector<GameObject *> infos;
	// infomation
	/*
	������� �������� �߰��Ѵ�.
	���⼭�� GameObject * �� ������ �ִ� vector
	*/

public :
	friend QuadTree;

	QuadTreeNode(QuadTreeNode * parent, Vector3 startPoint, float width, float depth)
		: startPoint(startPoint), width(width), depth(depth), parent(parent)
	{
		// ó���� �������ͷ� �� �־��ش�.
		for (int i = 0; i < 4; ++i)
		{
			children.push_back(nullptr);
		}
	}

	virtual ~QuadTreeNode()
	{
		release();
	}

	float getWidth() const { return width; }
	float getDepth() const { return depth; }

	void initialize(/*QuadTreeNode * parent*/)
	{
		//if (parent == nullptr)
		//	return;
		//this->parent = parent;

		float halfWidth = width / 2.0f;
		float halfDepth = depth / 2.0f;

		children[0] = new QuadTreeNode(this, startPoint, halfWidth, halfDepth);
		children[1] = new QuadTreeNode(this, startPoint + Vector3{ halfWidth, 0,0}, halfWidth, halfDepth);
		children[2] = new QuadTreeNode(this, startPoint + Vector3(0,0, halfDepth), halfWidth, halfDepth);
		children[3] = new QuadTreeNode(this, startPoint + Vector3{ halfWidth, 0,halfDepth }, halfWidth, halfDepth);
	}

	void release()
	{
		// ��������� ����Ʈ�� ��带 �� �������ش�.
		for (int i = 0; i < 4; ++i)
		{
			if (children[i])
				delete children[i];
			children[i] = nullptr;
		}
	}

	bool inRange(const Vector3 & other)
	{
		float minX = startPoint.x;
		float minZ = startPoint.z;
		float maxX = startPoint.x + width;
		float maxZ = startPoint.z + depth;

		// �ϳ��� �����ٱ����� ����� false
		if (minX >= other.x || maxX <= other.x ||
			minZ >= other.z || maxZ <= other.z)
			return false;
		
		return true;
	}

	void printNodeStartPointRecursively(float height =0)
	{
		cout << startPoint<<" , "<<"height : "<<height << endl;

		++height;
		for (int i = 0; i < 4; ++i)
		{
			if (children[i])
				children[i]->printNodeStartPointRecursively(height);
		}
	}
};

class QuadTree
{
private:
	QuadTreeNode * root;
	static const float defaultModuleSize;
public:
	QuadTree(Vector3 & startPoint, float width, float depth)
		: root(nullptr)
	{
		if (width <= 0 || depth <= 0) return;

		root = new QuadTreeNode(nullptr, startPoint, width, depth);

		divideNode(root);

	}

	virtual ~QuadTree()
	{
		release();
	}

	void release()
	{
		delete root;
		root = nullptr;
	}



	void printNodeStartPointRecursively()
	{
		if (!root) return;

		root->printNodeStartPointRecursively();

	}

	void divideNode(QuadTreeNode * node)
	{
		if (!node) return;

		// ����Ʈ���������ϰ� �ɶ����� �����鼭 �ʱ�ȭ���ش�.
		float width = node->width;
		float depth = node->depth;

		// ���� ���� ����� 2���� ����Ʈ ������� �������ִٸ� ����ؼ� �����ش�.
		if (width >= defaultModuleSize * 2 && depth >= defaultModuleSize * 2)
		{
			node->initialize();
			for (int i = 0; i < 4; ++i)
			{
				divideNode(node->children[i]);
			}
		}

	}
};

const float QuadTree::defaultModuleSize = 50.0f;

int main()
{
	QuadTree * quadTree = new QuadTree(Vector3(0, 0, 0), 200.0f, 200.0f);
	quadTree->printNodeStartPointRecursively();




	delete quadTree;
	


	return 0;
}