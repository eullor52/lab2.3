#pragma once

#include "sequence.hpp"
#include "bit.hpp"
#include "array.hpp"
#include <stdexcept>
#include <cstddef>
#include <utility>

template <typename T>
class BitSequence : public Sequence<Bit<T>> {
private:
    DynamicArray<T>* data;
    size_t bits_in_T;
    size_t length_bits;
    size_t GetByteIndex(size_t bitIndex) const;
    size_t GetBitInByteIndex(size_t bitIndex) const;
    void SetBit(size_t bitIndex, bool value);
    bool GetBit(size_t bitIndex) const;
protected:
    virtual BitSequence<T>* EmptyClone() const;
    virtual BitSequence<T>* Clone() const;
    virtual BitSequence<T>* Instance();
    void AppendInternal(const Bit<T>& item);
    void PrependInternal(const Bit<T>& item);
    void InsertAtInternal(const Bit<T>& item, size_t index);
    void ConcatInternal(const Sequence<Bit<T>>* list);
    class BitSeqIterator : public Sequence<Bit<T>>::IEnumerator {
    private:
        const BitSequence<T>* seq;
        size_t index;
    public:
        BitSeqIterator(const BitSequence<T>* s, size_t idx)
            : seq(s), index(idx) {}
        Bit<T> operator*() const override {
            return seq->Get(index);
        }
        BitSeqIterator& operator++() override {
            ++index;
            return *this;
        }
        bool equals(const typename Sequence<Bit<T>>::IEnumerator& other) const override {
            const auto* o = dynamic_cast<const BitSeqIterator*>(&other);
            if (!o) return false;
            return seq == o->seq && index == o->index;
        }
        typename Sequence<Bit<T>>::IEnumerator* clone() const override {
            return new BitSeqIterator(seq, index);
        }
    };

public:
    BitSequence();
    BitSequence(Bit<T>* new_items, size_t count);
    BitSequence(const BitSequence<T>& operand);
    ~BitSequence() override;
    Bit<T> GetFirst() const override;
    Bit<T> GetLast() const override;
    Bit<T> Get(size_t index) const override;
    size_t GetLength() const override;
    BitSequence<T>* GetSubsequence(size_t startIndex, size_t endIndex) const override;
    BitSequence<T>* Append(Bit<T> item) override;
    BitSequence<T>* Prepend(Bit<T> item) override;
    BitSequence<T>* InsertAt(Bit<T> item, size_t index) override;
    BitSequence<T>* Concat(const Sequence<Bit<T>>& list) override;
    BitSequence<T> operator&(const BitSequence<T>& operand) const;
    BitSequence<T> operator|(const BitSequence<T>& operand) const;
    BitSequence<T> operator^(const BitSequence<T>& operand) const;
    BitSequence<T> operator~() const;
    typename Sequence<Bit<T>>::Iterator begin() const override;
    typename Sequence<Bit<T>>::Iterator end() const override; 
};

template <typename T>
size_t BitSequence<T>::GetByteIndex(size_t bitIndex) const {
    return bitIndex / bits_in_T;
}

template <typename T>
size_t BitSequence<T>::GetBitInByteIndex(size_t bitIndex) const {
    size_t offset = bitIndex % bits_in_T;
    return (bits_in_T - 1) - offset;
}

template <typename T>
void BitSequence<T>::SetBit(size_t bitIndex, bool value) {
    size_t byteIdx = GetByteIndex(bitIndex);
    size_t bitIdx = GetBitInByteIndex(bitIndex);
    T byte = data->Get(byteIdx);
    T mask = static_cast<T>(1) << bitIdx;
    if (value) {
        data->Set(byteIdx, byte | mask);
    } else {
        data->Set(byteIdx, byte & ~mask);
    }
}

template <typename T>
bool BitSequence<T>::GetBit(size_t bitIndex) const {
    size_t byteIdx = GetByteIndex(bitIndex);
    size_t bitIdx = GetBitInByteIndex(bitIndex);
    T byte = data->Get(byteIdx);
    return (byte >> bitIdx) & static_cast<T>(1);
}

template <typename T>
BitSequence<T>* BitSequence<T>::EmptyClone() const {
    return new BitSequence<T>();
}

template <typename T>
BitSequence<T>* BitSequence<T>::Clone() const {
    return new BitSequence<T>(*this);
}

template <typename T>
BitSequence<T>* BitSequence<T>::Instance() {
    return this;
}

template <typename T>
void BitSequence<T>::AppendInternal(const Bit<T>& item) {
    bool val = static_cast<bool>(item);
    size_t oldLen = length_bits;
    size_t oldByteCount = (oldLen + bits_in_T - 1) / bits_in_T;
    size_t newByteCount = (oldLen + bits_in_T) / bits_in_T;
    if (newByteCount > oldByteCount) {
        data->Resize(newByteCount);
        data->Set(newByteCount - 1, 0);
    }
    SetBit(oldLen, val);
    ++length_bits;
}

template <typename T>
void BitSequence<T>::PrependInternal(const Bit<T>& item) {
    InsertAtInternal(item, 0);
}

template <typename T>
void BitSequence<T>::InsertAtInternal(const Bit<T>& item, size_t index) {
    if (index > length_bits) {
        throw OutOfRangeException("Index out of range");
    }
    if (index == length_bits) {
        AppendInternal(item);
        return;
    }
    size_t oldLen = length_bits;
    size_t oldByteCount = (oldLen + bits_in_T - 1) / bits_in_T;
    size_t newByteCount = (oldLen + bits_in_T) / bits_in_T;
    if (newByteCount > oldByteCount) {
        data->Resize(newByteCount);
        data->Set(newByteCount - 1, 0);
    }
    for (size_t i = oldLen; i > index; --i) {
        bool bitVal = GetBit(i - 1);
        SetBit(i, bitVal);
    }
    SetBit(index, static_cast<bool>(item));
    ++length_bits;
}

template <typename T>
void BitSequence<T>::ConcatInternal(const Sequence<Bit<T>>* list) {
    if (!list) return;
    size_t len = list->GetLength();
    for (size_t i = 0; i < len; ++i) {
        AppendInternal(list->Get(i));
    }
}

template <typename T>
BitSequence<T>::BitSequence()
    : data(new DynamicArray<T>())
    , bits_in_T(sizeof(T) * 8)
    , length_bits(0) {}

template <typename T>
BitSequence<T>::BitSequence(Bit<T>* new_items, size_t count)
    : bits_in_T(sizeof(T) * 8) {
    if (count > 0 && !new_items) {
        throw InvalidArgumentException("BitSequence: new_items is null");
    }
    size_t bytesNeeded = (count + bits_in_T - 1) / bits_in_T;
    data = new DynamicArray<T>(bytesNeeded);
    for (size_t i = 0; i < bytesNeeded; ++i) {
        data->Set(i, 0);
    }
    for (size_t i = 0; i < count; ++i) {
        SetBit(i, static_cast<bool>(new_items[i]));
    }
    length_bits = count;
}

template <typename T>
BitSequence<T>::BitSequence(const BitSequence<T>& operand)
    : data(new DynamicArray<T>(*operand.data))
    , bits_in_T(operand.bits_in_T)
    , length_bits(operand.length_bits) {}

template <typename T>
BitSequence<T>::~BitSequence() {
    delete data;
}

template <typename T>
Bit<T> BitSequence<T>::GetFirst() const {
    if (length_bits == 0) throw OutOfRangeException("BitSequence is empty");
    return Get(0);
}

template <typename T>
Bit<T> BitSequence<T>::GetLast() const {
    if (length_bits == 0) throw OutOfRangeException("BitSequence is empty");
    return Get(length_bits - 1);
}

template <typename T>
Bit<T> BitSequence<T>::Get(size_t index) const {
    if (index >= length_bits) throw OutOfRangeException("Index out of range");
    return Bit<T>(static_cast<T>(GetBit(index)));
}

template <typename T>
size_t BitSequence<T>::GetLength() const {
    return length_bits;
}

template <typename T>
BitSequence<T>* BitSequence<T>::GetSubsequence(size_t startIndex, size_t endIndex) const {
    if (startIndex > endIndex || startIndex >= length_bits || endIndex >= length_bits) {
        throw OutOfRangeException("Invalid subsequence indices");
    }
    BitSequence<T>* result = this->EmptyClone();
    for (size_t i = startIndex; i <= endIndex; ++i) {
        result->AppendInternal(Get(i));
    }
    return result;
}

template <typename T>
BitSequence<T>* BitSequence<T>::Append(Bit<T> item) {
    BitSequence<T>* result = this->Instance();
    result->AppendInternal(item);
    return result;
}

template <typename T>
BitSequence<T>* BitSequence<T>::Prepend(Bit<T> item) {
    BitSequence<T>* result = this->Instance();
    result->PrependInternal(item);
    return result;
}

template <typename T>
BitSequence<T>* BitSequence<T>::InsertAt(Bit<T> item, size_t index) {
    BitSequence<T>* result = this->Instance();
    result->InsertAtInternal(item, index);
    return result;
}

template <typename T>
BitSequence<T>* BitSequence<T>::Concat(const Sequence<Bit<T>>& list) {
    BitSequence<T>* result = this->Instance();
    result->ConcatInternal(&list);
    return result;
}

template <typename T>
BitSequence<T> BitSequence<T>::operator&(const BitSequence<T>& operand) const {
    if (length_bits != operand.length_bits) {
        throw LengthErrorException("BitSequence lengths differ for operator&");
    }
    BitSequence<T> result;
    for (size_t i = 0; i < length_bits; ++i) {
        result.AppendInternal(Get(i) & operand.Get(i));
    }
    return result;
}

template <typename T>
BitSequence<T> BitSequence<T>::operator|(const BitSequence<T>& operand) const {
    if (length_bits != operand.length_bits) {
        throw LengthErrorException("BitSequence lengths differ for operator|");
    }
    BitSequence<T> result;
    for (size_t i = 0; i < length_bits; ++i) {
        result.AppendInternal(Get(i) | operand.Get(i));
    }
    return result;
}

template <typename T>
BitSequence<T> BitSequence<T>::operator^(const BitSequence<T>& operand) const {
    if (length_bits != operand.length_bits) {
        throw LengthErrorException("BitSequence lengths differ for operator^");
    }
    BitSequence<T> result;
    for (size_t i = 0; i < length_bits; ++i) {
        result.AppendInternal(Get(i) ^ operand.Get(i));
    }
    return result;
}

template <typename T>
BitSequence<T> BitSequence<T>::operator~() const {
    BitSequence<T> result;
    for (size_t i = 0; i < length_bits; ++i) {
        bool cur = static_cast<bool>(Get(i));
        result.AppendInternal(Bit<T>(static_cast<T>(!cur)));
    }
    return result;
}

template <typename T>
class MutableBitSequence : public BitSequence<T> {
protected:
    BitSequence<T>* EmptyClone() const override {
        return new MutableBitSequence<T>();
    }
    BitSequence<T>* Clone() const override {
        return new MutableBitSequence<T>(*this);
    }
    BitSequence<T>* Instance() override {
        return this;
    }
public:
    using BitSequence<T>::BitSequence;
};

template <typename T>
class ImmutableBitSequence : public BitSequence<T> {
protected:
    BitSequence<T>* EmptyClone() const override {
        return new ImmutableBitSequence<T>();
    }
    BitSequence<T>* Clone() const override {
        return new ImmutableBitSequence<T>(*this);
    }
    BitSequence<T>* Instance() override {
        return this->Clone();
    }
public:
    using BitSequence<T>::BitSequence;
};

template <typename T>
typename Sequence<Bit<T>>::Iterator BitSequence<T>::begin() const {
    return typename Sequence<Bit<T>>::Iterator(new BitSeqIterator(this, 0));
}

template <typename T>
typename Sequence<Bit<T>>::Iterator BitSequence<T>::end() const {
    return typename Sequence<Bit<T>>::Iterator(new BitSeqIterator(this, length_bits));
}