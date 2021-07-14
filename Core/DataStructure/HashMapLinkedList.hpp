#ifndef HASHMAPLINKEDLIST_HPP
#define HASHMAPLINKEDLIST_HPP

#include <algorithm>
#include <Core/Misc/Defines/Common.hpp>
#include <Core/Math/MathUtils.hpp>

TRE_NS_START

struct FibonacciHashPolicy
{
    usize IndexForHash(usize hash, usize /*slotsCount*/) const noexcept
    {
        return (11400714819323198485ull * hash) >> shift;
    }

    usize KeepInRange(usize index, usize slotsCount) const noexcept
    {
        return index & slotsCount;
    }

    uint8 NextSize(usize& size) const noexcept
    {
        size = std::max(usize(2), Math::NextPow2(size));
        return 64 - Math::Log2OfPow2(size);
    }

    void Commit(uint8 shift) noexcept
    {
        this->shift = shift;
    }

    void Reset() noexcept
    {
        shift = 63;
    }

private:
    uint8 shift = 63;
};

template<typename HashMapType>
struct LinkedListItr
{
    using KeyType = typename HashMapType::KeyType;
    using ValueType = typename HashMapType::ValueType;
    using value_type = typename HashMapType::value_type;
    using iterator = typename HashMapType::iterator;
    using BlockPointer = typename HashMapType::BlockPointer;
    using Constants = typename HashMapType::Constants;
    constexpr static auto BLOCK_SIZE = HashMapType::BLOCK_SIZE;

    usize index = 0;
    BlockPointer block = nullptr;

    LinkedListItr()
    {
    }

    LinkedListItr(usize index, BlockPointer block)
        : index(index), block(block)
    {
    }

    iterator Iterator() const
    {
        return { block, index };
    }

    uint32 IndexInBlock() const
    {
        return index % BLOCK_SIZE;
    }

    bool IsDirectHit() const
    {
        return (this->GetMetadata() & Constants::BITS_FOR_DIRECT_HIT) == Constants::MAGIC_FOR_DIRECT_HIT;
    }

    bool IsEmpty() const
    {
        return this->GetMetadata() == Constants::MAGIC_FOR_EMPTY;
    }

    bool HasNext() const
    {
        return this->JumpIndex() != 0;
    }

    uint8 JumpIndex() const
    {
        return Constants::DistanceFromMetadata(this->GetMetadata());
    }

    uint8 GetMetadata() const
    {
        return block->controlBytes[this->IndexInBlock()];
    }

    void SetMetadata(uint8 metadata)
    {
        block->controlBytes[this->IndexInBlock()] = metadata;
    }

    LinkedListItr Next(HashMapType& table) const
    {
        uint8 distance = this->JumpIndex();
        usize nextIdx = table.m_HashPolicy.KeepInRange(index + Constants::JUMP_DISTANCES[distance], table.m_SlotsCount);
        return { nextIdx, table.m_Entries + nextIdx / BLOCK_SIZE };
    }

    void SetNext(uint8 jumpIdx)
    {
        uint8& metadata = block->controlBytes[this->IndexInBlock()];
        metadata = (metadata & ~Constants::BITS_FOR_DISTANCE) | jumpIdx;
    }

    void ClearNext()
    {
        this->SetNext(0);
    }

    value_type operator*() const
    {
        auto idx = this->IndexInBlock();
        KeyType* key = reinterpret_cast<KeyType*>(block->keys + idx);
        ValueType* value = reinterpret_cast<ValueType*>(block->values + idx);
        return { key, value };
    }

    bool operator!() const
    {
        return !block;
    }

    explicit operator bool() const
    {
        return block != nullptr;
    }

    bool operator==(const LinkedListItr & other) const
    {
        return index == other.index;
    }

    bool operator!=(const LinkedListItr & other) const
    {
        return !(*this == other);
    }
};

TRE_NS_END

#endif // HASHMAPLINKEDLIST_HPP
