#include "../include/station_iterator.h"
#include <stdexcept>

template <bool IsConst>
StationIteratorT<IsConst>::StationIteratorT(VecIterator current, VecIterator end)
    : current_(current), end_(end) {}

template <bool IsConst>
StationIteratorT<IsConst>::ReferenceType StationIteratorT<IsConst>::operator*() const {
    return *(*current_);
}

template <bool IsConst>
StationIteratorT<IsConst>::PointerType StationIteratorT<IsConst>::operator->() const {
    return current_->get();
}

template <bool IsConst>
StationIteratorT<IsConst>& StationIteratorT<IsConst>::operator++() {
    if (current_ == end_) {
        throw std::out_of_range("Ошибка: выход за пределы станции");
    }
    ++current_;
    return *this;
}

template <bool IsConst>
StationIteratorT<IsConst> StationIteratorT<IsConst>::operator++(int) {
    StationIteratorT temp = *this;
    ++(*this);
    return temp;
}

template <bool IsConst>
bool StationIteratorT<IsConst>::operator!=(const StationIteratorT& other) const {
    return current_ != other.current_;
}

template <bool IsConst>
bool StationIteratorT<IsConst>::operator==(const StationIteratorT& other) const {
    return current_ == other.current_;
}

template class StationIteratorT<false>;
template class StationIteratorT<true>;