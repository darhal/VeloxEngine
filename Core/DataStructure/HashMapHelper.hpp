#ifndef HASHMAPHELPER_HPP
#define HASHMAPHELPER_HPP

#include <math.h>
#include <Core/Misc/Defines/Common.hpp>
#include <Core/Memory/Memory.hpp>
#include <Core/DataStructure/Utils.hpp>
#include <Core/Misc/UtilityConcepts.hpp>
#include <Core/DataStructure/HashMapIterator.hpp>
#include <Core/DataStructure/HashMapLinkedList.hpp>

TRE_NS_START

template<typename K, typename V, typename H, typename HP, typename KE>
class HashMapHelper : private H, private KE
{
public:
    using HashPolicy = HP;
    using Hasher = H;
    using KeyEqual = KE;

    // using ValueType = std::pair<K*, V*>;
    using KeyType = K;
    using ValueType = V;
    using value_type = std::pair<K*, V*>;
    using value_ref  = std::pair<K&, V&>;
    using const_value_type = std::pair<const K*, const V*>;
    using const_value_ref  = std::pair<const K&, const V&>;
    using size_type = size_t;
    using difference_type = std::ptrdiff_t;
    using hasher = H;
    using key_equal = KE;
    using reference = value_type&;
    using const_reference = const value_type&;
    using pointer = value_type*;
    using const_pointer = const value_type*;

public:
    constexpr static usize BLOCK_SIZE = 16;
    constexpr static float MAX_LOAD_FACTOR = 0.9375f;

    struct Constants
    {
        static constexpr uint8 MAGIC_FOR_EMPTY      = int8_t(0b11111111);
        static constexpr uint8 MAGIC_FOR_RESERVED   = int8_t(0b11111110);
        static constexpr uint8 BITS_FOR_DIRECT_HIT  = int8_t(0b10000000);
        static constexpr uint8 MAGIC_FOR_DIRECT_HIT = int8_t(0b00000000);
        static constexpr uint8 MAGIC_FOR_LIST_ENTRY = int8_t(0b10000000);
        static constexpr uint8 BITS_FOR_DISTANCE    = int8_t(0b01111111);
        static constexpr uint32 NUM_JUMP_DISTANCES  = 126;

        FORCEINLINE static uint8 DistanceFromMetadata(uint8 metadata)
        {
            return metadata & BITS_FOR_DISTANCE;
        }

        // jump distances chosen like this:
        // 1. pick the first 16 integers to promote staying in the same block
        // 2. add the next 66 triangular numbers to get even jumps when
        // the hash table is a power of two
        // 3. add 44 more triangular numbers at a much steeper growth rate
        // to get a sequence that allows large jumps so that a table
        // with 10000 sequential numbers doesn't endlessly re-allocate
        static constexpr usize JUMP_DISTANCES[NUM_JUMP_DISTANCES]
        {
            0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15,

            21, 28, 36, 45, 55, 66, 78, 91, 105, 120, 136, 153, 171, 190, 210, 231,
            253, 276, 300, 325, 351, 378, 406, 435, 465, 496, 528, 561, 595, 630,
            666, 703, 741, 780, 820, 861, 903, 946, 990, 1035, 1081, 1128, 1176,
            1225, 1275, 1326, 1378, 1431, 1485, 1540, 1596, 1653, 1711, 1770, 1830,
            1891, 1953, 2016, 2080, 2145, 2211, 2278, 2346, 2415, 2485, 2556,

            3741, 8385, 18915, 42486, 95703, 215496, 485605, 1091503, 2456436,
            5529475, 12437578, 27986421, 62972253, 141700195, 318819126, 717314626,
            1614000520, 3631437253, 8170829695, 18384318876, 41364501751,
            93070021080, 209407709220, 471167588430, 1060127437995, 2385287281530,
            5366895564381, 12075513791265, 27169907873235, 61132301007778,
            137547673121001, 309482258302503, 696335090510256, 1566753939653640,
            3525196427195653, 7931691866727775, 17846306747368716,
            40154190394120111, 90346928493040500, 203280588949935750,
            457381324898247375, 1029107980662394500, 2315492957028380766,
            5209859150892887590,
        };
    };

    template<usize BlockSize>
    struct Block
    {
        uint8 controlBytes[BlockSize];
        alignas(K) uint8 keys[BlockSize];
        alignas(V) uint8 values[BlockSize];

        static Block* EmptyBlock()
        {
            static std::array<uint8, BlockSize> emptyBytes = []
            {
                std::array<uint8, BlockSize> result;
                result.fill(Constants::MAGIC_FOR_EMPTY);
                return result;
            }();

            return reinterpret_cast<Block*>(&emptyBytes);
        }

        constexpr FORCEINLINE usize GetFirstEmptyIndex() const
        {
            for (int i = 0; i < BlockSize; i++) {
                if (controlBytes[i] == Constants::magic_for_empty)
                    return i;
            }
            return -1;
        }

        constexpr FORCEINLINE void FillControlBytes(uint8 value)
        {
            Utils::MemSet(controlBytes, value, BlockSize);
        }

        constexpr FORCEINLINE void Destroy(uint32 idx)
        {
            Utils::Destroy(keys + idx);
            Utils::Destroy(values + idx);
        }

        template<typename... Args>
        constexpr static FORCEINLINE void Construct(const value_type& adr, K&& key, Args&&... args)
        {
            new (adr.first) K(std::forward<K>(key));
            new (adr.second) V(std::forward<Args>(args)...);
        }
    };

    using BlockType      = Block<BLOCK_SIZE>;
    using BlockPointer   = BlockType*;
    using iterator       = TemplatedIterator<HashMapHelper, value_ref>;
    using const_iterator = TemplatedIterator<HashMapHelper, const value_ref>;
    using LinkedList     = LinkedListItr<HashMapHelper>;

    friend LinkedList;
public:
    constexpr HashMapHelper();

    constexpr ~HashMapHelper();

    template<typename... Args>
    constexpr std::pair<iterator, bool> Emplace(K&& key, Args&&... args);

    template<typename... Args>
    constexpr std::pair<iterator, bool> EmplaceDirectHit(LinkedList block, K&& key, Args&&... args);

    template<typename... Args>
    constexpr std::pair<iterator, bool> EmplaceNewKey(LinkedList parent, K&& key, Args&&... args);

    constexpr iterator Find(const K& key) const noexcept;

    constexpr void Reinit(usize count);

    constexpr FORCEINLINE void Reserve(usize size);

    FORCEINLINE constexpr usize Size() const noexcept { return m_ElementsCount; }

    FORCEINLINE constexpr usize BucketCount() const noexcept { return m_SlotsCount ? m_SlotsCount + 1 : 0 ; }

    FORCEINLINE constexpr float LoadFactor() const noexcept { return static_cast<float>(m_ElementsCount) / (m_SlotsCount + 1); }

    FORCEINLINE constexpr float GetMaxLoadFactor() const noexcept { return MAX_LOAD_FACTOR; }

    FORCEINLINE constexpr bool Empty() const noexcept { return m_ElementsCount == 0; }

    FORCEINLINE constexpr bool IsFull() const noexcept
    {
        if (!m_SlotsCount)
            return true;
        else
            return m_ElementsCount + 1 > (m_SlotsCount + 1) * static_cast<double>(MAX_LOAD_FACTOR);
    }

    FORCEINLINE constexpr iterator begin() noexcept
    {
        size_t slots = m_SlotsCount ? m_SlotsCount + 1 : 0;
        return ++iterator{ m_Entries + slots / BLOCK_SIZE, slots };
    }

    FORCEINLINE constexpr iterator begin() const noexcept
    {
        size_t slots = m_SlotsCount ? m_SlotsCount + 1 : 0;
        return ++iterator{ m_Entries + slots / BLOCK_SIZE, slots };
    }

    FORCEINLINE constexpr const_iterator cbegin() const noexcept
    {
        return this->begin();
    }

    FORCEINLINE constexpr iterator end() noexcept
    {
        return { m_Entries - 1, std::numeric_limits<usize>::max() };
    }

    FORCEINLINE constexpr iterator end() const noexcept
    {
        return { m_Entries - 1, std::numeric_limits<usize>::max() };
    }

    FORCEINLINE constexpr const_iterator cend() const noexcept
    {
        return this->end();
    }

private:
    constexpr void DeallocateData(BlockPointer begin, usize slotsCount) noexcept;

    constexpr FORCEINLINE void ResetToEmpty() noexcept;

    template<typename U>
    FORCEINLINE constexpr usize HashObject(const U& key) noexcept
    {
        return static_cast<Hasher&>(*this)(key);
    }

    template<typename U>
    FORCEINLINE constexpr usize HashObject(const U& key) const noexcept
    {
        // return std::hash<U>{}(key);
        return static_cast<const Hasher&>(*this)(key);
    }

    template<typename L, typename R>
    FORCEINLINE constexpr bool ComparesEqual(const L& lhs, const R& rhs) const noexcept
    {
        return lhs == rhs;
    }

    FORCEINLINE constexpr void Grow()
    {
        return this->Reinit(std::max(usize(10), 2 * this->BucketCount()));
    }

    FORCEINLINE constexpr usize NumBucketsForReserve(usize size) const noexcept
    {
        return static_cast<usize>(std::ceil(size / static_cast<double>(MAX_LOAD_FACTOR)));
    }

    FORCEINLINE constexpr usize CalculateMemorySize(usize blockCount) const noexcept
    {
        usize memRequired = sizeof(BlockType) * blockCount;
        memRequired += BLOCK_SIZE; // for metadata of past-the-end pointer
        return memRequired;
    }

    FORCEINLINE constexpr std::pair<uint8, LinkedList> FindFreeIndex(LinkedList parent) const noexcept
    {
        for (uint8 jumpIndex = 1; jumpIndex < Constants::NUM_JUMP_DISTANCES; ++jumpIndex) {
            size_t index = m_HashPolicy.KeepInRange(parent.index + Constants::JUMP_DISTANCES[jumpIndex], m_SlotsCount);
            BlockPointer block = m_Entries + (index / BLOCK_SIZE);

            if (block->controlBytes[index % BLOCK_SIZE] == Constants::MAGIC_FOR_EMPTY)
                return { jumpIndex, { index, block } };
        }

        return { 0, {} };
    }

    FORCEINLINE constexpr LinkedList FindDirectHit(LinkedList child) const noexcept
    {
        auto keyValuePtr = *child;
        usize toMoveHash = HashObject(*keyValuePtr.first);
        usize toMoveIndex = m_HashPolicy.IndexForHash(toMoveHash, m_SlotsCount);
        return { toMoveIndex, m_Entries + toMoveIndex / BLOCK_SIZE };
    }

    FORCEINLINE constexpr LinkedList FindParentBlock(LinkedList child) noexcept
    {
        LinkedList parentBlock = FindDirectHit(child);
        while (1) {
            LinkedList next = parentBlock.Next(*this);
            if (next == child)
                return parentBlock;
            parentBlock = next;
        }
    }
private:
    BlockPointer m_Entries;
    usize m_SlotsCount;
    usize m_ElementsCount;
    HashPolicy m_HashPolicy;
};

template<typename K, typename V, typename H, typename HP, typename KE>
constexpr HashMapHelper<K, V, H, HP , KE>::HashMapHelper()
    : m_Entries{BlockType::EmptyBlock()}, m_SlotsCount{0}, m_ElementsCount{0}, m_HashPolicy{}
{

}

template<typename K, typename V, typename H, typename HP, typename KE>
constexpr HashMapHelper<K, V, H, HP , KE>::~HashMapHelper()
{

}

template<typename K, typename V, typename H, typename HP, typename KE>
template<typename... Args>
constexpr auto HashMapHelper<K, V, H, HP , KE>::Emplace(K&& key, Args&&... args) -> std::pair<iterator, bool>
{
    usize index = this->HashObject(key);
    usize slots = this->m_SlotsCount;
    BlockPointer entries = this->m_Entries;
    index = m_HashPolicy.IndexForHash(index, slots);
    bool first = true;

    while (1) {
        usize blockIndex = index / BLOCK_SIZE;
        uint32 indexInBlock = index % BLOCK_SIZE;
        BlockPointer block = entries + blockIndex;
        uint8 metadata = block->controlBytes[indexInBlock];

        if (first) {
            if ((metadata & Constants::BITS_FOR_DIRECT_HIT) != Constants::MAGIC_FOR_DIRECT_HIT)
                return EmplaceDirectHit({ index, block }, std::forward<K>(key), std::forward<Args>(args)...);
            first = false;
        }

        const K& inMemKey = *reinterpret_cast<const K*>(block->keys + indexInBlock);
        if (ComparesEqual(key, inMemKey))
            return { { block, index }, false };

        uint8 toNextIndex = metadata & Constants::BITS_FOR_DISTANCE;
        if (toNextIndex == 0)
            return EmplaceNewKey({ index, block }, std::forward<K>(key), std::forward<Args>(args)...);
        index += Constants::JUMP_DISTANCES[toNextIndex];
        index = m_HashPolicy.KeepInRange(index, slots);
    }
}

template<typename K, typename V, typename H, typename HP, typename KE>
template<typename... Args>
constexpr auto HashMapHelper<K, V, H, HP , KE>::EmplaceDirectHit(LinkedList block, K&& key, Args&&... args) -> std::pair<iterator, bool>
{
    if (this->IsFull()) {
        this->Grow();
        return this->Emplace(std::forward<K>(key), std::forward<Args>(args)...);
    }

    if (block.GetMetadata() == Constants::MAGIC_FOR_EMPTY){
        // AllocatorTraits::construct(*this, std::addressof(*block), std::forward<Args>(args)...);
        BlockType::Construct(*block, std::forward<K>(key), std::forward<Args>(args)...);
        block.SetMetadata(Constants::MAGIC_FOR_DIRECT_HIT);
        ++m_ElementsCount;
        return { block.Iterator(), true };
    }else{
        LinkedList parentBlock = this->FindParentBlock(block);
        std::pair<uint8, LinkedList> freeBlock = this->FindFreeIndex(parentBlock);

        if (!freeBlock.first) {
            this->Grow();
            return this->Emplace(std::forward<K>(key), std::forward<Args>(args)...);
        } // ValueType newValue(std::forward<Args>(args)...);

        for (LinkedList it = block;;) {
            // AllocatorTraits::construct(*this, std::addressof(*freeBlock.second), std::move(*it));
            // AllocatorTraits::destroy(*this, std::addressof(*it));
            auto keyValuePairPtr = *it;
            BlockType::Construct(*freeBlock.second, std::move(*keyValuePairPtr.first), std::move(*keyValuePairPtr.second));
            parentBlock.SetNext(freeBlock.first);
            freeBlock.second.SetMetadata(Constants::MAGIC_FOR_LIST_ENTRY);

            if (!it.HasNext()) {
                it.SetMetadata(Constants::MAGIC_FOR_EMPTY);
                break;
            }

            LinkedList next = it.Next(*this);
            it.SetMetadata(Constants::MAGIC_FOR_EMPTY);
            block.SetMetadata(Constants::MAGIC_FOR_RESERVED);
            it = next;
            parentBlock = freeBlock.second;
            freeBlock = this->FindFreeIndex(freeBlock.second);

            if (!freeBlock.first) {
                this->Grow();
                return this->Emplace(std::forward<K>(key), std::forward<Args>(args)...);
            }
        }

        // AllocatorTraits::construct(*this, std::addressof(*block), std::move(newValue));
        BlockType::Construct(*block, std::forward<K>(key), std::forward<Args>(args)...);
        block.SetMetadata(Constants::MAGIC_FOR_DIRECT_HIT);
        ++m_ElementsCount;
        return { block.Iterator(), true };
    }
}

template<typename K, typename V, typename H, typename HP, typename KE>
template<typename... Args>
constexpr auto HashMapHelper<K, V, H, HP , KE>::EmplaceNewKey(LinkedList parent, K&& key, Args&&... args) -> std::pair<iterator, bool>
{
    if (this->IsFull()) {
        this->Grow();
        return this->Emplace(std::forward<K>(key), std::forward<Args>(args)...);
    }

    std::pair<uint8, LinkedList> freeBlock = this->FindFreeIndex(parent);

    if (!freeBlock.first) {
        this->Grow();
        return this->Emplace(std::forward<K>(key), std::forward<Args>(args)...);
    }

    // AllocatorTraits::construct(*this, std::addressof(*freeBlock.second), std::forward<Args>(args)...);
    BlockType::Construct(*freeBlock.second, std::forward<K>(key), std::forward<Args>(args)...);
    freeBlock.second.SetMetadata(Constants::MAGIC_FOR_LIST_ENTRY);
    parent.SetNext(freeBlock.first);
    ++m_ElementsCount;
    return { freeBlock.second.Iterator(), true };
}


template<typename K, typename V, typename H, typename HP, typename KE>
constexpr auto HashMapHelper<K, V, H, HP , KE>::Find(const K& key) const noexcept -> iterator
{
    usize index = HashObject(key);
    usize slots = this->m_SlotsCount;
    BlockPointer entries = this->m_Entries;
    index = m_HashPolicy.IndexForHash(index, slots);
    bool first = true;

    for (;;) {
        usize blockIndex = index / BLOCK_SIZE;
        uint32 indexInBlock = index % BLOCK_SIZE;
        BlockPointer block = entries + blockIndex;
        uint8 metadata = block->controlBytes[indexInBlock];

        if (first) {
            if ((metadata & Constants::BITS_FOR_DIRECT_HIT) != Constants::MAGIC_FOR_DIRECT_HIT)
                return this->end();
            first = false;
        }

        const K& inMemKey = *reinterpret_cast<const K*>(block->keys + indexInBlock);
        if (this->ComparesEqual(key, inMemKey))
            return { block, index };

        uint8 toNextIndex = metadata & Constants::BITS_FOR_DISTANCE;
        if (toNextIndex == 0)
            return this->end();
        index += Constants::JUMP_DISTANCES[toNextIndex];
        index = m_HashPolicy.KeepInRange(index, slots);
    }

    return this->end();
}

template<typename K, typename V, typename H, typename HP, typename KE>
constexpr void HashMapHelper<K, V, H, HP , KE>::Reinit(usize count)
{
    count = std::max(count, static_cast<usize>(std::ceil(m_ElementsCount / static_cast<double>(MAX_LOAD_FACTOR))));
    if (count == 0) {
        this->ResetToEmpty();
        return;
    }

    if (count == m_SlotsCount + 1)
        return;
    auto newIndex = m_HashPolicy.NextSize(count);
    usize blocksCount = count / BLOCK_SIZE;
    if (count % BLOCK_SIZE)
        ++blocksCount;
    auto memSize = this->CalculateMemorySize(blocksCount);
    void* newMemory = Utils::AllocateBytes(memSize);
    BlockPointer newBuckets = reinterpret_cast<BlockPointer>(newMemory);
    BlockPointer endItem = newBuckets + blocksCount;
    for (BlockPointer ptr = newBuckets; ptr <= endItem; ptr++)
        ptr->FillControlBytes(Constants::MAGIC_FOR_EMPTY);
    std::swap(m_Entries, newBuckets);
    std::swap(m_SlotsCount, count);
    --m_SlotsCount;
    m_HashPolicy.Commit(newIndex);
    m_ElementsCount = 0;
    if (count)
        ++count;
    usize oldBlocksCount = count / BLOCK_SIZE;
    if (count % BLOCK_SIZE)
        ++oldBlocksCount;

    for (BlockPointer itr = newBuckets, end = newBuckets + oldBlocksCount; itr != end; ++itr) {
        for (uint32 i = 0; i < BLOCK_SIZE; ++i) {
            uint8 metadata = itr->controlBytes[i];

            if (metadata != Constants::MAGIC_FOR_EMPTY && metadata != Constants::MAGIC_FOR_RESERVED) {
                KeyType* key = reinterpret_cast<KeyType*>(itr->keys + i);
                ValueType* value = reinterpret_cast<ValueType*>(itr->values + i);
                this->Emplace(std::move(*key), std::move(*value));
                itr->Destroy(i);
            }
        }
    }

    this->DeallocateData(newBuckets, count - 1);
}

template<typename K, typename V, typename H, typename HP, typename KE>
constexpr FORCEINLINE void HashMapHelper<K, V, H, HP , KE>::Reserve(usize size)
{
    usize requiredBuckets =this->NumBucketsForReserve(size);
    if (requiredBuckets > this->BucketCount())
        return this->Reinit(requiredBuckets);
}

template<typename K, typename V, typename H, typename HP, typename KE>
constexpr FORCEINLINE void HashMapHelper<K, V, H, HP , KE>::ResetToEmpty() noexcept
{
    this->DeallocateData(m_Entries, m_SlotsCount);
    m_Entries = BlockType::EmptyBlock();
    m_SlotsCount = 0;
    m_HashPolicy.Reset();
}

template<typename K, typename V, typename H, typename HP, typename KE>
constexpr void HashMapHelper<K, V, H, HP , KE>::DeallocateData(HashMapHelper::BlockPointer begin, usize slotsCount) noexcept
{
    if (begin == BlockType::EmptyBlock())
        return;
    ++slotsCount;
    usize blocksCount = slotsCount / BLOCK_SIZE;
    if (slotsCount % BLOCK_SIZE)
        ++blocksCount;
    // usize mem = this->CalculateMemorySize(blocksCount);
    Utils::FreeMemory(begin);
}

TRE_NS_END

#endif // HASHMAPHELPER_HPP
