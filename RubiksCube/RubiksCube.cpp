#include "stdafx.h"

#include <queue>
#include <string>
#include <unordered_map>
#include "MainDlg.h"

//#define WITH_TREES

const int MAX_LEVELS = 8;
const int NUM_CHILDREN = 12;

typedef unsigned long long uint64;

// When using trees we store the cube states as characters
// When using the map we store them as integers (0-5) for easier hashing
#ifdef WITH_TREES
const char COLOR_R = 'r';
const char COLOR_B = 'b';
const char COLOR_W = 'w';
const char COLOR_G = 'g';
const char COLOR_Y = 'y';
const char COLOR_O = 'o';
#else
const unsigned char COLOR_R = 0;
const unsigned char COLOR_B = 1;
const unsigned char COLOR_W = 2;
const unsigned char COLOR_G = 3;
const unsigned char COLOR_Y = 4;
const unsigned char COLOR_O = 5;
#endif

class Node
{
protected:
	char m_moves[MAX_LEVELS];

public:
	Node()
	{
		memset(m_moves, 0, sizeof(m_moves));
	}

	void copyMoves(const Node &parent)
	{
		memcpy(m_moves, parent.m_moves, sizeof(m_moves));
	}

	size_t movesLength() const
	{
		if (m_moves[MAX_LEVELS - 1] != 0)
		{
			return MAX_LEVELS;
		}
		return strlen(m_moves);
	}

	std::string getMoves() const
	{
		std::string moves;
		for (const char *p = m_moves; *p; p++)
		{
			const char *move = nullptr;
			switch (*p)
			{
			case 'u':
				move = "Up\r\n";
				break;
			case 'U':
				move = "Up'\r\n";
				break;
			case 'd':
				move = "Down\r\n";
				break;
			case 'D':
				move = "Down'\r\n";
				break;
			case 'r':
				move = "Right\r\n";
				break;
			case 'R':
				move = "Right'\r\n";
				break;
			case 'l':
				move = "Left\r\n";
				break;
			case 'L':
				move = "Left'\r\n";
				break;
			case 'f':
				move = "Front\r\n";
				break;
			case 'F':
				move = "Front'\r\n";
				break;
			case 'b':
				move = "Back\r\n";
				break;
			case 'B':
				move = "Back'\r\n";
				break;
			}
			moves += move;
		}
		return moves;
	}
};

typedef std::unordered_map<uint64, Node> BottomMap;

class TreeNode
	: public Node
{
	typedef Node Parent;

private:
	char m_squares[24];
	TreeNode *m_pChildren;

public:
	TreeNode(const char *squares)
		: m_pChildren(nullptr)
	{
		//NumConstruct++;
		memcpy(m_squares, squares, sizeof(m_squares));
	}

	~TreeNode()
	{
		//NumDestruct++;
		delete[] m_pChildren;
	}

	void buildTopTree()
	{
		buildChildrenForTopTree(nullptr, nullptr, 0, 1);
	}

	void buildBottomTree()
	{
		buildChildrenForBottomTree(nullptr, nullptr, 0, 1);
	}

	BottomMap *buildBottomMap()
	{
		BottomMap *pMap = new BottomMap;
		buildChildrenForBottomMap(pMap, nullptr, nullptr, 0, 1);
		return pMap;
	}

	std::string getSquares() const
	{
		return std::string(m_squares, sizeof(m_squares));
	}

	uint64 getSquareHash() const
	{
		uint64 hash = 0;
		const unsigned char *s = reinterpret_cast<const unsigned char *>(m_squares);
		for (int t = 0; t < sizeof(m_squares); t++, s++)
		{
			hash = hash * 6;
			hash += *s;
		}
		return hash;
	}

	TreeNode *getChild(int index) const
	{
		return m_pChildren ? m_pChildren + index : nullptr;
	}

	bool equals(const TreeNode *t) const
	{
		return memcmp(m_squares, t->m_squares, sizeof(m_squares)) == 0;
	}

private:
	TreeNode()
		: m_pChildren(nullptr)
	{
		//NumConstruct++;
	}

	void buildChildrenForTopTree(const TreeNode *pParent, void (TreeNode::*pTransform)(const TreeNode &), char move, int level)
	{
		// Build the current node's moves & squares (from the parent's)
		if (level > 1)
		{
			(*this.*pTransform)(*pParent);
			copyMoves(*pParent);
			m_moves[level - 2] = move;
		}

		if (level < MAX_LEVELS)
		{
			m_pChildren = new TreeNode[NUM_CHILDREN];

			m_pChildren[0].buildChildrenForTopTree(this, &TreeNode::up, 'u'/*"Up\n"*/, level + 1);
			m_pChildren[1].buildChildrenForTopTree(this, &TreeNode::upInverted, 'U'/*"Up'\n"*/, level + 1);
			m_pChildren[2].buildChildrenForTopTree(this, &TreeNode::down, 'd'/*"Down\n"*/, level + 1);
			m_pChildren[3].buildChildrenForTopTree(this, &TreeNode::downInverted, 'D'/*"Down'\n"*/, level + 1);
			m_pChildren[4].buildChildrenForTopTree(this, &TreeNode::right, 'r'/*"Right\n"*/, level + 1);
			m_pChildren[5].buildChildrenForTopTree(this, &TreeNode::rightInverted, 'R'/*"Right'\n"*/, level + 1);
			m_pChildren[6].buildChildrenForTopTree(this, &TreeNode::left, 'l'/*"Left\n"*/, level + 1);
			m_pChildren[7].buildChildrenForTopTree(this, &TreeNode::leftInverted, 'L'/*"Left'\n"*/, level + 1);
			m_pChildren[8].buildChildrenForTopTree(this, &TreeNode::front, 'f'/*"Front\n"*/, level + 1);
			m_pChildren[9].buildChildrenForTopTree(this, &TreeNode::frontInverted, 'F'/*"Front'\n"*/, level + 1);
			m_pChildren[10].buildChildrenForTopTree(this, &TreeNode::back, 'b'/*"Back\n"*/, level + 1);
			m_pChildren[11].buildChildrenForTopTree(this, &TreeNode::backInverted, 'B'/*"Back'\n"*/, level + 1);
		}
	}

	void buildChildrenForBottomTree(const TreeNode *pParent, void (TreeNode::*pTransform)(const TreeNode &), char move, int level)
	{
		// Build the current node's moves & squares (from the parent's)
		if (level > 1)
		{
			(*this.*pTransform)(*pParent);
			copyMoves(*pParent);
			memmove(m_moves + 1, m_moves, level - 1);
			m_moves[0] = move;
		}

		if (level < MAX_LEVELS)
		{
			m_pChildren = new TreeNode[NUM_CHILDREN];

			m_pChildren[0].buildChildrenForBottomTree(this, &TreeNode::up, 'U'/*"Up'"*/, level + 1);
			m_pChildren[1].buildChildrenForBottomTree(this, &TreeNode::upInverted, 'u'/*"Up"*/, level + 1);
			m_pChildren[2].buildChildrenForBottomTree(this, &TreeNode::down, 'D'/*"Down'"*/, level + 1);
			m_pChildren[3].buildChildrenForBottomTree(this, &TreeNode::downInverted, 'd'/*"Down\n"*/, level + 1);
			m_pChildren[4].buildChildrenForBottomTree(this, &TreeNode::right, 'R'/*"Right'\n"*/, level + 1);
			m_pChildren[5].buildChildrenForBottomTree(this, &TreeNode::rightInverted, 'r'/*"Right\n"*/, level + 1);
			m_pChildren[6].buildChildrenForBottomTree(this, &TreeNode::left, 'L'/*"Left'\n"*/, level + 1);
			m_pChildren[7].buildChildrenForBottomTree(this, &TreeNode::leftInverted, 'l'/*"Left\n"*/, level + 1);
			m_pChildren[8].buildChildrenForBottomTree(this, &TreeNode::front, 'F'/*"Front'\n"*/, level + 1);
			m_pChildren[9].buildChildrenForBottomTree(this, &TreeNode::frontInverted, 'f'/*"Front\n"*/, level + 1);
			m_pChildren[10].buildChildrenForBottomTree(this, &TreeNode::back, 'B'/*"Back'\n"*/, level + 1);
			m_pChildren[11].buildChildrenForBottomTree(this, &TreeNode::backInverted, 'b'/*"Back\n"*/, level + 1);
		}
	}

	void buildChildrenForBottomMap(BottomMap *pMap, const TreeNode *pParent, void (TreeNode::*pTransform)(const TreeNode &), char move, int level)
	{
		// Build the current node's moves & squares (from the parent's)
		if (level > 1)
		{
			(*this.*pTransform)(*pParent);
			copyMoves(*pParent);
			memmove(m_moves + 1, m_moves, level - 1);
			m_moves[0] = move;
		}
		auto emplaceResult = pMap->try_emplace(getSquareHash(), *this);
		if (!emplaceResult.second)
		{
			if (this->movesLength() < emplaceResult.first->second.movesLength())
			{
				emplaceResult.first->second = *this;
			}
		}

		if (level < MAX_LEVELS)
		{
			TreeNode child;

			child.buildChildrenForBottomMap(pMap, this, &TreeNode::up, 'U'/*"Up'"*/, level + 1);
			child.buildChildrenForBottomMap(pMap, this, &TreeNode::upInverted, 'u'/*"Up"*/, level + 1);
			child.buildChildrenForBottomMap(pMap, this, &TreeNode::down, 'D'/*"Down'"*/, level + 1);
			child.buildChildrenForBottomMap(pMap, this, &TreeNode::downInverted, 'd'/*"Down\n"*/, level + 1);
			child.buildChildrenForBottomMap(pMap, this, &TreeNode::right, 'R'/*"Right'\n"*/, level + 1);
			child.buildChildrenForBottomMap(pMap, this, &TreeNode::rightInverted, 'r'/*"Right\n"*/, level + 1);
			child.buildChildrenForBottomMap(pMap, this, &TreeNode::left, 'L'/*"Left'\n"*/, level + 1);
			child.buildChildrenForBottomMap(pMap, this, &TreeNode::leftInverted, 'l'/*"Left\n"*/, level + 1);
			child.buildChildrenForBottomMap(pMap, this, &TreeNode::front, 'F'/*"Front'\n"*/, level + 1);
			child.buildChildrenForBottomMap(pMap, this, &TreeNode::frontInverted, 'f'/*"Front\n"*/, level + 1);
			child.buildChildrenForBottomMap(pMap, this, &TreeNode::back, 'B'/*"Back'\n"*/, level + 1);
			child.buildChildrenForBottomMap(pMap, this, &TreeNode::backInverted, 'b'/*"Back\n"*/, level + 1);
		}
	}

	void up(const TreeNode &parent)
	{
		memcpy(m_squares, parent.m_squares, sizeof(m_squares));
		char *c = m_squares;

		// spin the whole row
		char temp = c[4]; // 2 temps are needed to store the values of the first two squares overwritten
		char temp2 = c[5];
		c[4] = c[8];
		c[5] = c[9];
		c[8] = c[12];
		c[9] = c[13];
		c[12] = c[16];
		c[13] = c[17];
		c[16] = temp;
		c[17] = temp2;

		// rotate the 4 sides on the top face
		temp = c[0];
		c[0] = c[2];
		c[2] = c[3];
		c[3] = c[1];
		c[1] = temp;
	}

	void upInverted(const TreeNode &parent)
	{
		memcpy(m_squares, parent.m_squares, sizeof(m_squares));
		char *c = m_squares;

		// spin the whole row
		char temp = c[17]; // 2 temps are needed to store the values of the first two squares overwritten
		char temp2 = c[16];
		c[17] = c[13];
		c[16] = c[12];
		c[13] = c[9];
		c[12] = c[8];
		c[9] = c[5];
		c[8] = c[4];
		c[5] = temp;
		c[4] = temp2;

		// rotate the 4 sides on the top face
		temp = c[0];
		c[0] = c[1];
		c[1] = c[3];
		c[3] = c[2];
		c[2] = temp;
	}

	void down(const TreeNode &parent)
	{
		memcpy(m_squares, parent.m_squares, sizeof(m_squares));
		char *c = m_squares;

		// spin the whole row
		char temp = c[19]; // 2 temps are needed to store the values of the first two squares overwritten
		char temp2 = c[18];
		c[19] = c[15];
		c[18] = c[14];
		c[15] = c[11];
		c[14] = c[10];
		c[11] = c[7];
		c[10] = c[6];
		c[7] = temp;
		c[6] = temp2;

		// rotate the 4 sides on the top face
		temp = c[20];
		c[20] = c[22];
		c[22] = c[23];
		c[23] = c[21];
		c[21] = temp;
	}

	void downInverted(const TreeNode &parent)
	{
		memcpy(m_squares, parent.m_squares, sizeof(m_squares));
		char *c = m_squares;

		// spin the whole row
		char temp = c[6]; // 2 temps are needed to store the values of the first two squares overwritten
		char temp2 = c[7];
		c[6] = c[10];
		c[7] = c[11];
		c[10] = c[14];
		c[11] = c[15];
		c[14] = c[18];
		c[15] = c[19];
		c[18] = temp;
		c[19] = temp2;

		// rotate the 4 sides on the top face
		temp = c[20];
		c[20] = c[21];
		c[21] = c[23];
		c[23] = c[22];
		c[22] = temp;
	}

	void right(const TreeNode &parent)
	{
		memcpy(m_squares, parent.m_squares, sizeof(m_squares));
		char *c = m_squares;

		// spin the whole row
		char temp = c[18]; // 2 temps are needed to store the values of the first two squares overwritten
		char temp2 = c[16];
		c[18] = c[1];
		c[16] = c[3];
		c[1] = c[9];
		c[3] = c[11];
		c[9] = c[21];
		c[11] = c[23];
		c[21] = temp;
		c[23] = temp2;

		// rotate the 4 sides on the top face
		temp = c[12];
		c[12] = c[14];
		c[14] = c[15];
		c[15] = c[13];
		c[13] = temp;
	}

	void rightInverted(const TreeNode &parent)
	{
		memcpy(m_squares, parent.m_squares, sizeof(m_squares));
		char *c = m_squares;

		// spin the whole row
		char temp = c[16]; // 2 temps are needed to store the values of the first two squares overwritten
		char temp2 = c[18];
		c[16] = c[23];
		c[18] = c[21];
		c[23] = c[11];
		c[21] = c[9];
		c[11] = c[3];
		c[9] = c[1];
		c[3] = temp;
		c[1] = temp2;

		// rotate the 4 sides on the top face
		temp = c[12];
		c[12] = c[13];
		c[13] = c[15];
		c[15] = c[14];
		c[14] = temp;
	}

	void left(const TreeNode &parent)
	{
		memcpy(m_squares, parent.m_squares, sizeof(m_squares));
		char *c = m_squares;

		// spin the whole row
		char temp = c[17]; // 2 temps are needed to store the values of the first two squares overwritten
		char temp2 = c[19];
		c[17] = c[22];
		c[19] = c[20];
		c[22] = c[10];
		c[20] = c[8];
		c[10] = c[2];
		c[8] = c[0];
		c[2] = temp;
		c[0] = temp2;

		// rotate the 4 sides on the top face
		temp = c[4];
		c[4] = c[6];
		c[6] = c[7];
		c[7] = c[5];
		c[5] = temp;
	}

	void leftInverted(const TreeNode &parent)
	{
		memcpy(m_squares, parent.m_squares, sizeof(m_squares));
		char *c = m_squares;

		// spin the whole row
		char temp = c[19]; // 2 temps are needed to store the values of the first two squares overwritten
		char temp2 = c[17];
		c[19] = c[0];
		c[17] = c[2];
		c[0] = c[8];
		c[2] = c[10];
		c[8] = c[20];
		c[10] = c[22];
		c[20] = temp;
		c[22] = temp2;

		// rotate the 4 sides on the top face
		temp = c[4];
		c[4] = c[5];
		c[5] = c[7];
		c[7] = c[6];
		c[6] = temp;
	}

	void front(const TreeNode &parent)
	{
		memcpy(m_squares, parent.m_squares, sizeof(m_squares));
		char *c = m_squares;

		// spin the whole row
		char temp = c[3]; // 2 temps are needed to store the values of the first two squares overwritten
		char temp2 = c[2];
		c[3] = c[5];
		c[2] = c[7];
		c[5] = c[20];
		c[7] = c[21];
		c[20] = c[14];
		c[21] = c[12];
		c[14] = temp;
		c[12] = temp2;

		// rotate the 4 sides on the top face
		temp = c[8];
		c[8] = c[10];
		c[10] = c[11];
		c[11] = c[9];
		c[9] = temp;
	}

	void frontInverted(const TreeNode &parent)
	{
		memcpy(m_squares, parent.m_squares, sizeof(m_squares));
		char *c = m_squares;

		// spin the whole row
		char temp = c[2]; // 2 temps are needed to store the values of the first two squares overwritten
		char temp2 = c[3];
		c[2] = c[12];
		c[3] = c[14];
		c[12] = c[21];
		c[14] = c[20];
		c[21] = c[7];
		c[20] = c[5];
		c[7] = temp;
		c[5] = temp2;

		// rotate the 4 sides on the top face
		temp = c[8];
		c[8] = c[9];
		c[9] = c[11];
		c[11] = c[10];
		c[10] = temp;
	}

	void back(const TreeNode &parent)
	{
		memcpy(m_squares, parent.m_squares, sizeof(m_squares));
		char *c = m_squares;

		// spin the whole row
		char temp = c[0]; // 2 temps are needed to store the values of the first two squares overwritten
		char temp2 = c[1];
		c[0] = c[13];
		c[1] = c[15];
		c[13] = c[23];
		c[15] = c[22];
		c[23] = c[6];
		c[22] = c[4];
		c[6] = temp;
		c[4] = temp2;

		// rotate the 4 sides on the top face
		temp = c[16];
		c[16] = c[18];
		c[18] = c[19];
		c[19] = c[17];
		c[17] = temp;
	}

	void backInverted(const TreeNode &parent)
	{
		memcpy(m_squares, parent.m_squares, sizeof(m_squares));
		char *c = m_squares;

		// spin the whole row
		char temp = c[1]; // 2 temps are needed to store the values of the first two squares overwritten
		char temp2 = c[0];
		c[1] = c[4];
		c[0] = c[6];
		c[4] = c[22];
		c[6] = c[23];
		c[22] = c[15];
		c[23] = c[13];
		c[15] = temp;
		c[13] = temp2;

		// rotate the 4 sides on the top face
		temp = c[16];
		c[16] = c[17];
		c[17] = c[19];
		c[19] = c[18];
		c[18] = temp;
	}
};

static char solved[24] =
{
	COLOR_R, COLOR_R, COLOR_R, COLOR_R,
	COLOR_B, COLOR_B, COLOR_B, COLOR_B,
	COLOR_W, COLOR_W, COLOR_W, COLOR_W,
	COLOR_G, COLOR_G, COLOR_G, COLOR_G,
	COLOR_Y, COLOR_Y, COLOR_Y, COLOR_Y,
	COLOR_O, COLOR_O, COLOR_O, COLOR_O,
};

static TreeNode *top;
static TreeNode *bottom;
static BottomMap *pBottomMap;

static std::string traverse();
static bool traverse2(TreeNode *comp, Node *found);

static void solve(const char *start)
{
	top = new TreeNode(start);
	top->buildTopTree();
	//write_line("NumConstructed = %d", NumConstruct);
	bottom = new TreeNode(solved);
#ifdef WITH_TREES
	bottom->buildBottomTree();
#else
	pBottomMap = bottom->buildBottomMap();
#endif
	//write_line("NumConstructed = %d", NumConstruct);
	write_line("Trees constructed");

	write_line(traverse().c_str());

	delete pBottomMap;
	pBottomMap = nullptr;
	delete top;
	top = nullptr;
	//write_line("NumDestructed = %d", NumDestruct);
	delete bottom;
	bottom = nullptr;
	//write_line("NumDestructed = %d", NumDestruct);
}

static std::string traverse()
{
	TreeNode *current;
	Node t2;

	auto queue = std::queue<TreeNode *>();
	queue.push(top);

	while (!queue.empty())
	{
		current = queue.front();
		queue.pop();
		if (traverse2(current, &t2))
		{
			return "START\r\n" + current->getMoves() + t2.getMoves() + "END";
		}

		if (current->getChild(0) != nullptr)
		{
			for (int i = 0; i < 12; i++)
			{
				queue.push(current->getChild(i));
			}
		}
	}

	return "No path found";
}

static bool traverse2(TreeNode *comp, Node *found)
{
#ifdef WITH_TREES
	TreeNode *current;

	auto queue = std::queue<TreeNode *>();
	queue.push(bottom);

	while (!queue.empty())
	{
		current = queue.front();
		queue.pop();
		if (current->equals(comp))
		{
			*found = *current;
			return true;
		}

		if (current->getChild(0) != nullptr)
		{
			for (int i = 0; i < 12; i++)
			{
				queue.push(current->getChild(i));
			}
		}
	}

	return false;
#else
	auto it = pBottomMap->find(comp->getSquareHash());
	if (it == pBottomMap->end())
	{
		return false;
	}
	*found = it->second;
	return true;
#endif
}

static void translate(char *squares)
{
#ifndef WITH_TREES
	unsigned char *s = reinterpret_cast<unsigned char *>(squares);
	for (int t = 0; t < 24; t++, s++)
	{
		switch (*s)
		{
		case 'r':
			*s = COLOR_R;
			break;
		case 'b':
			*s = COLOR_B;
			break;
		case 'w':
			*s = COLOR_W;
			break;
		case 'g':
			*s = COLOR_G;
			break;
		case 'y':
			*s = COLOR_Y;
			break;
		case 'o':
			*s = COLOR_O;
			break;
		}
	}
#endif
}

// Testcase: rrrrwwbbggwwyyggbbyyoooo
// Solution Up'

// Testcase: rrwwwobbggworyggbbryoyoy
// Solution: Up', Right'

// Testcase: yrbwbwrorgborywoboggwgyy
// Solution: Up', Front', Down', Right' [MaxLevel=3]
// Solution: Down', Left', Up', Right' [MaxLevel=8]
//           (note, for this one you get a different result if the map is built strictly depth first)

// Testcase: ywgroryowbbowoygggrbwbry
// Solution: Right, Right, Front, Down, Left', Back, Left' [MaxLevel=5]
// Solution: Right, Right, Back, Right, Down', Front, Left' [MaxLevel=6,7]
// Solution: Left', Down, Back', Right, Front', Right', Up [MaxLevel=8]
// (the Java version [MaxLevel unknown] had a different solution, we checked that both were same # moves & both worked)

void start(const char *cubeState)
{
	char squares[24];

	if (strlen(cubeState) != 24)
	{
		write_line("Error, the cube state must be 24 characters of colors: rbwgyo");
	}
	for (int t = 0; t < 24; t++)
	{
		char c = cubeState[t];
		if (c != 'r' && c != 'b' && c != 'w' && c != 'g' && c != 'y' && c != 'o')
		{
			write_line("Error, the cube state must be 24 characters of colors: rbwgyo");
		}
		squares[t] = c;
	}
	translate(squares);

	solve(squares);
}
