#pragma once

#include <Renderer/Common.hpp>
#include <Engine/Core/Misc/Maths/Maths.hpp>

TRE_NS_START

struct BitmapTree
{
    BitmapTree() : tree(NULL), numNodes(0)
    {}

    BitmapTree(const BitmapTree& other) : numNodes(other.numNodes)
    {
        tree = new uint8[numNodes];
        memcpy(this->tree, other.tree, sizeof(uint8) * numNodes);
    }

    BitmapTree(BitmapTree&& other) : tree(other.tree), numNodes(other.numNodes)
    {
        other.tree = NULL;
    }

    BitmapTree& operator=(const BitmapTree& other)
    {
        if (tree)
            delete[] tree;

        numNodes = other.numNodes;
        tree = new uint8[numNodes];
        memcpy(tree, other.tree, sizeof(uint8) * numNodes);
        return *this;
    }

    BitmapTree& operator=(BitmapTree&& other)
    {
        if (tree)
            delete[] tree;

        tree = other.tree;
        numNodes = other.numNodes;
        other.tree = NULL;
        return *this;
    }

    ~BitmapTree()
    {
        if (tree) {
            delete[] tree;
            tree = NULL;
        }
    }

    void Init(uint32 numNodes)
    {
        if (tree)
            delete[] tree;

        this->numNodes = numNodes;
        this->tree = new uint8[numNodes];
    }

    uint32 GetLevelElementsCount(uint32 lvl)
    {
        return 1 << lvl;
    }

    uint8* GetLevel(uint32 lvl)
    {
        return &tree[(1 << lvl) - 1];
    }

    uint32 GetLeftIdx(uint32 nodeIdx)
    {
        return (nodeIdx << 1) | 1;
    }

    uint32 GetRightIdx(uint32 nodeIdx)
    {
        return (nodeIdx << 1) + 2;
    }

    bool HaveChild(uint32 nodeIdx)
    {
        return this->GetRightIdx(nodeIdx) < numNodes || this->GetLeftIdx(nodeIdx) < numNodes;
    }

    uint32 GetParentIdx(uint32 nodeIdx)
    {
        if (int64(nodeIdx) - 1 < 0)
            return UINT32_MAX;
        return (nodeIdx - 1) >> 1;
    }

    uint8 GetLeftNode(uint32 nodeIdx)
    {
        return GetNode(GetLeftIdx(nodeIdx));
    }

    uint8 GetRightNode(uint32 nodeIdx)
    {
        return GetNode(GetRightIdx(nodeIdx));
    }

    uint8 GetParentNode(uint32 nodeIdx)
    {
        return GetNode(GetParentIdx(nodeIdx));
    }

    uint8 GetNode(uint32 nodeIdx)
    {
        return tree[nodeIdx];
    }

    void SetNode(uint32 nodeIdx, uint8 value)
    {
        tree[nodeIdx] = value;
    }

    void SetNodeChildren(uint32 nodeIdx, uint8 value)
    {
        this->SetNode(GetLeftIdx(nodeIdx), value);
        this->SetNode(GetRightIdx(nodeIdx), value);
    }

    uint8* GetData()
    {
        return tree;
    }

    uint32 GetLevels() {
        return Math::Log2OfPow2(numNodes) + 1;
    }

    uint32 GetNumNodes()
    {
        return numNodes;
    }

    void Print()
    {
        for (uint32 i = 0; i < numNodes; i++) {
            if (((i+1) & (i)) == 0) {
                printf( "\n");
            }

            printf("%d ", tree[i]);
        }
        printf("\n");

        // structure(0, 0);
    }

    void padding(char ch, int n) {
      for (int i = 0; i < n; i++)
        putchar(ch);
    }

    void structure(uint32 root, int level) {
      if (root >= numNodes) {
        padding('\t', level);
        puts ( "~" );
      } else {
        structure(GetRightIdx(root), level + 1);
        padding('\t', level);
        printf("%d\n", GetNode(root));
        structure(GetLeftIdx(root), level + 1);
      }
    }

    uint8* tree;
    uint32 numNodes;
};

TRE_NS_END
