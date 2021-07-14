#ifndef HASHMAP_HPP
#define HASHMAP_HPP

#include <math.h>
#include <Core/Misc/Defines/Common.hpp>
#include <Core/Memory/Memory.hpp>
#include <Core/DataStructure/Utils.hpp>
#include <Core/Misc/UtilityConcepts.hpp>
#include <Core/DataStructure/HashMapHelper.hpp>

TRE_NS_START

template<typename K, typename V>
class HashMap : public HashMapHelper<K, V, std::hash<K>, FibonacciHashPolicy, std::equal_to<K>>
{
public:
    using value_type = std::pair<K*, V*>;
    using size_type = size_t;
    using difference_type = std::ptrdiff_t;
    // using hasher = ArgumentHash;
    // using key_equal = ArgumentEqual;
    using reference = value_type&;
    using const_reference = const value_type&;
    using pointer = value_type*;
    using const_pointer = const value_type*;

public:


private:

};


TRE_NS_END

#endif // HASHMAP_HPP
