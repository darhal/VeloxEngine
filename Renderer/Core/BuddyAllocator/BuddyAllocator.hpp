#include <Renderer/Common.hpp>
#include <Renderer/Core/BitmapTree/BitmapTree.hpp>

TRE_NS_START

struct BuddyAllocator
{
    struct Allocation
    {
        uint32 offset;
        uint32 size;
    };

    void Init(uint32 minSize = 256, uint32 maxSize = 4096)
    {
        this->minSize = minSize;
        this->maxSize = maxSize;
        uint32 numBlocks = maxSize / minSize;
        tree.Init(2 * numBlocks - 1);
        uint32 currentLvl = tree.GetLevels();
        printf("Current lvl: %d\n", currentLvl);

        for (uint32 l = 0; l < tree.GetNumNodes(); l++) {
            if (((l+1) & l) == 0) {
                currentLvl--;
            }

            tree.SetNode(l, currentLvl + 1);
        }
    }

    Allocation Allocate(uint32 size)
    {
        uint32 currentNode = 0;
        uint32 offset = 0;
        uint32 realSize = Math::NextPow2(size);
        uint32 level = Math::Log2OfPow2(realSize / minSize) + 1;
        uint32 availSize = maxSize;
        // printf("[max: %d|min:%d] Real size: %d | Searching for level: %d\n", maxSize, minSize, realSize, level);

        while (tree.HaveChild(currentNode)) {
            uint32 leftIdx = tree.GetLeftIdx(currentNode);
            uint32 rightIdx = tree.GetRightIdx(currentNode);

            uint8 currentLevel = tree.GetNode(currentNode);
            uint8 leftNode = tree.GetLeftNode(currentNode);
            uint8 rightNode = tree.GetRightNode(currentNode);

            if (leftNode >= level) {
                currentNode = leftIdx;
                availSize >>= 1;
            }else if (rightNode >= level) {
                currentNode = rightIdx;
                availSize >>= 1;
                offset += availSize;
            }else{
                if (currentLevel < level) {
                    // ASSERTF(true, "Allocator is out of memory");
                    return { UINT32_MAX, UINT32_MAX };
                }
                break;
            }
        }

        // Mark current Node as used and then descend while updating parents
        tree.SetNode(currentNode, 0);
        this->UpdateParentsAlloc(currentNode);
        return { offset, size };
    }

    void Free(const Allocation& alloc)
    {
        uint32 currentNode = 0;
        uint32 realSize = Math::NextPow2(alloc.size);
        uint32 level = Math::Log2OfPow2(realSize / minSize) + 1;
        uint8 currentLevel = tree.GetLevels();
        uint32 currentSize = maxSize >> 1;
        uint32 currentOffset = currentSize;

        while (currentLevel != level) { // condition on level or offset (I think both are equivalent)
            currentSize >>= 1;
            currentLevel--;

            if (alloc.offset >= currentOffset) { // going right
                currentNode = tree.GetRightIdx(currentNode);
                currentOffset += currentSize;
            }else{
                currentNode = tree.GetLeftIdx(currentNode);
                currentOffset -= currentSize;
            }
        }

        tree.SetNode(currentNode, level);
        this->UpdateParentsFree(currentNode);
    }

    void UpdateParentsAlloc(uint32 currentNode)
    {
        // Moving upward updating parents:
        uint32 dummyNode = tree.GetParentIdx(currentNode);

        while (dummyNode != UINT32_MAX) {
            uint8 leftValue = tree.GetLeftNode(dummyNode);
            uint8 rightValue = tree.GetRightNode(dummyNode);
            uint8 dummyLevel = MAX(leftValue, rightValue);

            tree.SetNode(dummyNode, dummyLevel);
            dummyNode = tree.GetParentIdx(dummyNode);
        }
    }

    void UpdateParentsFree(uint32 currentNode)
    {
        // Moving upward updating parents:
        uint32 dummyNode = tree.GetParentIdx(currentNode);

        while (dummyNode != UINT32_MAX) {
            uint8 leftValue = tree.GetLeftNode(dummyNode);
            uint8 rightValue = tree.GetRightNode(dummyNode);
            uint8 dummyLevel = (rightValue == leftValue) ? rightValue + 1 : MAX(leftValue, rightValue);

            tree.SetNode(dummyNode, dummyLevel);
            dummyNode = tree.GetParentIdx(dummyNode);
        }
    }

    void Print()
    {
        tree.Print();
    }

    uint32                minSize;
    uint32                maxSize;
    BitmapTree            tree;
};


/*void testalloc()
{
    BuddyAllocator alloc;
    alloc.Init(256, 1024);

    // FIrst test batch:
    auto a1 = alloc.Allocate(510);
    auto a2 = alloc.Allocate(256);
    auto a3 = alloc.Allocate(256);
    ASSERT(a1.offset != 0);
    ASSERT(a2.offset != 512);
    ASSERT(a3.offset != 512 + 256);
    alloc.Free(a1);
    alloc.Free(a2);
    alloc.Free(a3);

    // Second test batch:
    a1 = alloc.Allocate(256);
    a2 = alloc.Allocate(256);
    a3 = alloc.Allocate(257);
    ASSERT(a1.offset != 0);
    ASSERT(a2.offset != 256);
    ASSERT(a3.offset != 512);
    alloc.Free(a1);
    alloc.Free(a2);
    alloc.Free(a3);

     // Third test batch:
    a1 = alloc.Allocate(256);
    a2 =  alloc.Allocate(257);
    a3 =  alloc.Allocate(256);
    ASSERT(a1.offset != 0);
    ASSERT(a2.offset != 512);
    ASSERT(a3.offset != 256);
    alloc.Free(a2);
    auto a4 = alloc.Allocate(256);
    auto a5 =  alloc.Allocate(256);
    ASSERT(a4.offset != 512);
    ASSERT(a5.offset != 512 + 256);
    alloc.Free(a1);
    alloc.Free(a3);
    alloc.Free(a4);
    alloc.Free(a5);

    {
    INIT_BENCHMARK;
    alloc.Init(64, 1024);

    BENCHMARK("alloc 34", auto a = alloc.Allocate(34););
    BENCHMARK("alloc 66", auto b = alloc.Allocate(66));
    BENCHMARK("alloc 35", auto c = alloc.Allocate(35));
    BENCHMARK("alloc 67", auto d = alloc.Allocate(67));

    ASSERT(a.offset != 0);
    ASSERT(b.offset != 64+64);
    ASSERT(c.offset != 64);
    ASSERT(d.offset != 64 * 4);

    //alloc.Print();

    BENCHMARK("free 66", alloc.Free(b));
    BENCHMARK("free 67", alloc.Free(d));
    BENCHMARK("free 34", alloc.Free(a));
    BENCHMARK("free 35", alloc.Free(c));
    }

    //alloc.Print();
}*/

TRE_NS_END
