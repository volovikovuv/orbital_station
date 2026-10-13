#pragma once
#include "module.h"
#include <type_traits>
#include <vector>
#include <memory>

template <bool IsConst>
class StationIteratorT {
public:
    using VecIterator = std::conditional_t<IsConst,
                        std::vector<std::unique_ptr<Module>>::const_iterator,
                        std::vector<std::unique_ptr<Module>>::iterator>;

    using ReferenceType = std::conditional_t<IsConst, const Module&, Module&>;
    using PointerType   = std::conditional_t<IsConst, const Module*, Module*>;
private:
    VecIterator current_;
    VecIterator end_;
public:
    StationIteratorT(VecIterator current, VecIterator end);

    ReferenceType operator*() const;
    PointerType operator->() const;

    StationIteratorT& operator++();
    StationIteratorT operator++(int);
    bool operator!=(const StationIteratorT& other) const;
    bool operator==(const StationIteratorT& other) const;
};

using StationIterator = StationIteratorT<false>;
using ConstStationIterator = StationIteratorT<true>;