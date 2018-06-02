#include "stdafx.h"

#include <queue>
#include <string>

const int MAX_LEVELS = 5;
const int NUM_CHILDREN = 12;

class TreeNode
{
public:
    char m_squares[24];
    char m_moves[MAX_LEVELS];
    TreeNode *m_pChildren;

public:
    TreeNode(const char *squares, bool bottom)
        : m_pChildren(nullptr)
    {
        memcpy(m_squares, squares, sizeof(m_squares));
        memset(m_moves, 0, sizeof(m_moves));
        setChildren(1, 0, bottom);
    }

    TreeNode(const TreeNode &parent, void (TreeNode::*pTransform)(void), char move, int level, bool bottom)
        : m_pChildren(nullptr)
    {
        memcpy(m_squares, parent.m_squares, sizeof(m_squares));
        (*this.*pTransform)();
        memcpy(m_moves, parent.m_moves, sizeof(m_moves));
        setChildren(level, move, bottom);
    }

    ~TreeNode()
    {
        if (m_pChildren == nullptr)
        {
            return;
        }
        for (int t = 0; t < NUM_CHILDREN; t++)
        {
            m_pChildren[t].~TreeNode();
        }
        free(m_pChildren);
    }

    std::string getSquares()
    {
        return std::string(m_squares, sizeof(m_squares));
    }

    TreeNode *getChild(int index)
    {
        return m_pChildren ? m_pChildren + index : nullptr;
    }

    std::string getMove()
    {
        std::string moves;
        for (char *p = m_moves; *p; p++)
        {
            const char *move;
            switch (*p)
            {
            case 'u':
                move = "Up\n";
                break;
            case 'U':
                move = "Up'\n";
                break;
            case 'd':
                move = "Down\n";
                break;
            case 'D':
                move = "Down'\n";
                break;
            case 'r':
                move = "Right\n";
                break;
            case 'R':
                move = "Right'\n";
                break;
            case 'l':
                move = "Left\n";
                break;
            case 'L':
                move = "Left'\n";
                break;
            case 'f':
                move = "Front\n";
                break;
            case 'F':
                move = "Front'\n";
                break;
            case 'b':
                move = "Back\n";
                break;
            case 'B':
                move = "Back'\n";
                break;
            }
            moves += move;
        }
        return moves;
    }

    void setChildren(int level, char move, bool bottom)
    {
        if (bottom)
        {
            // Prepend move to m_move
            if (level > 1)
            {
                memmove(m_moves + 1, m_moves, level - 1);
            }
            m_moves[0] = move;
        }
        else
        {
            // Append move to m_move
            if (level != 1)
            {
                m_moves[level - 2] = move;
            }
        }

        if (level < MAX_LEVELS)
        {
            m_pChildren = static_cast<TreeNode *>(malloc(NUM_CHILDREN * sizeof(TreeNode)));
            TreeNode *children = m_pChildren;
            if (bottom)
            {
                new (children + 0) TreeNode(*this, &TreeNode::up, 'U'/*"Up'"*/, level + 1, bottom);
                new (children + 1) TreeNode(*this, &TreeNode::upInverted, 'u'/*"Up"*/, level + 1, bottom);
                new (children + 2) TreeNode(*this, &TreeNode::down, 'D'/*"Down'"*/, level + 1, bottom);
                new (children + 3) TreeNode(*this, &TreeNode::downInverted, 'd'/*"Down\n"*/, level + 1, bottom);
                new (children + 4) TreeNode(*this, &TreeNode::right, 'R'/*"Right'\n"*/, level + 1, bottom);
                new (children + 5) TreeNode(*this, &TreeNode::rightInverted, 'r'/*"Right\n"*/, level + 1, bottom);
                new (children + 6) TreeNode(*this, &TreeNode::left, 'L'/*"Left'\n"*/, level + 1, bottom);
                new (children + 7) TreeNode(*this, &TreeNode::leftInverted, 'l'/*"Left\n"*/, level + 1, bottom);
                new (children + 8) TreeNode(*this, &TreeNode::front, 'F'/*"Front'\n"*/, level + 1, bottom);
                new (children + 9) TreeNode(*this, &TreeNode::frontInverted, 'f'/*"Front\n"*/, level + 1, bottom);
                new (children + 10) TreeNode(*this, &TreeNode::back, 'B'/*"Back'\n"*/, level + 1, bottom);
                new (children + 11) TreeNode(*this, &TreeNode::backInverted, 'b'/*"Back\n"*/, level + 1, bottom);
            }
            else
            {
                new (children + 0) TreeNode(*this, &TreeNode::up, 'u'/*"Up\n"*/, level + 1, bottom);
                new (children + 1) TreeNode(*this, &TreeNode::upInverted, 'U'/*"Up'\n"*/, level + 1, bottom);
                new (children + 2) TreeNode(*this, &TreeNode::down, 'd'/*"Down\n"*/, level + 1, bottom);
                new (children + 3) TreeNode(*this, &TreeNode::downInverted, 'D'/*"Down'\n"*/, level + 1, bottom);
                new (children + 4) TreeNode(*this, &TreeNode::right, 'r'/*"Right\n"*/, level + 1, bottom);
                new (children + 5) TreeNode(*this, &TreeNode::rightInverted, 'R'/*"Right'\n"*/, level + 1, bottom);
                new (children + 6) TreeNode(*this, &TreeNode::left, 'l'/*"Left\n"*/, level + 1, bottom);
                new (children + 7) TreeNode(*this, &TreeNode::leftInverted, 'L'/*"Left'\n"*/, level + 1, bottom);
                new (children + 8) TreeNode(*this, &TreeNode::front, 'f'/*"Front\n"*/, level + 1, bottom);
                new (children + 9) TreeNode(*this, &TreeNode::frontInverted, 'F'/*"Front'\n"*/, level + 1, bottom);
                new (children + 10) TreeNode(*this, &TreeNode::back, 'b'/*"Back\n"*/, level + 1, bottom);
                new (children + 11) TreeNode(*this, &TreeNode::backInverted, 'B'/*"Back'\n"*/, level + 1, bottom);
            }
        }
    }

    void up()
    {
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

    void upInverted()
    {
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

    void down()
    {
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

    void downInverted()
    {
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

    void right()
    {
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

    void rightInverted()
    {
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

    void left()
    {
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

    void leftInverted()
    {
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

    void front()
    {
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

    void frontInverted()
    {
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

    void back()
    {
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

    void backInverted()
    {
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

    bool equals(TreeNode *t)
    {
        return memcmp(m_squares, t->m_squares, sizeof(m_squares)) == 0;
    }
};

static char solved[24] =
{
    'r', 'r', 'r', 'r',
    'b', 'b', 'b', 'b',
    'w', 'w', 'w', 'w',
    'g', 'g', 'g', 'g',
    'y', 'y', 'y', 'y',
    'o', 'o', 'o', 'o',
};

static TreeNode *top;
static TreeNode *bottom;

std::string traverse();
TreeNode *traverse2(TreeNode *comp);

// Testcase: rrrrwwbbggwwyyggbbyyoooo
// Solution Up'

// Testcase: rrwwwobbggworyggbbryoyoy
// Solution: Up', Right'

// Testcase: yrbwbwrorgborywoboggwgyy
// Solution: Up', Front', Down', Right' [MaxLevel=3]
// Solution: Down', Left', Up', Right' [MaxLevel=8]

// Testcase: ywgroryowbbowoygggrbwbry
// Solution: Left', Down, Back', Right, Front', Right', Up [MaxLevel=8]
// (the Java version had a different solution, we checked that both were same len & both worked)

void solve(const char *start)
{
    top = new TreeNode(start, false);
    bottom = new TreeNode(solved, true);
    printf("Trees constructed\n");

    printf(traverse().c_str());

    delete top;
    top = nullptr;
    delete bottom;
    bottom = nullptr;
}

std::string traverse()
{
    TreeNode *current;
    TreeNode *t2;

    auto queue = std::queue<TreeNode *>();
    queue.push(top);

    while (!queue.empty())
    {
        current = queue.front();
        queue.pop();
        t2 = traverse2(current);
        if (t2 != nullptr)
        {
            return "START\n" + current->getMove() + t2->getMove() + "END\n";
        }

        if (current->getChild(0) != nullptr)
        {
            for (int i = 0; i < 12; i++)
            {
                queue.push(current->getChild(i));
            }
        }
    }

    return "No path found\n";
}

TreeNode *traverse2(TreeNode *comp)
{
    TreeNode *current;

    auto queue = std::queue<TreeNode *>();
    queue.push(bottom);

    while (!queue.empty())
    {
        current = queue.front();
        queue.pop();
        if (current->equals(comp))
        {
            return current;
        }

        if (current->getChild(0) != nullptr)
        {
            for (int i = 0; i < 12; i++)
            {
                queue.push(current->getChild(i));
            }
        }
    }

    return nullptr;
}

int main(int argc, char *argv[])
{
    char squares[24];

    if (argc == 2)
    {
        if (strlen(argv[1]) != 24)
        {
            printf("Error, the cube state must be 24 characters of colors: roygbw\n");
            return 1;
        }
        for (int t = 0; t < 24; t++)
        {
            char c = argv[1][t];
            if (c != 'r' && c != 'o' && c != 'y' && c != 'g' && c != 'b' && c != 'w')
            {
                printf("Error, the cube state must be 24 characters of colors: roygbw\n");
                return 1;
            }
            squares[t] = c;
        }
    }
    else
    {
        printf("Solving a solved cube\n");
        memcpy(squares, solved, sizeof(solved));
    }

    solve(squares);

    return 0;
}
