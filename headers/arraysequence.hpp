#pragma once

#include "array.hpp"
#include "exceptions.h"
#include "sequence.hpp"
#include "list.hpp" 
#include <stdexcept>
#include <utility>

template <typename T>
class ArraySequence : public Sequence<T> {
private:
    DynamicArray<T>* array;
protected:
    virtual ArraySequence<T>* EmptyClone() const = 0;
    virtual ArraySequence<T>* Clone() const = 0;
    virtual ArraySequence<T>* Instance() = 0;
    void AppendInternal(T item);
    void PrependInternal(T item);
    void InsertAtInternal(T item, size_t index);

    class ArraySeqIterator : public Sequence<T>::IEnumerator {
    private:
        typename DynamicArray<T>::Iterator current;
    public:
        ArraySeqIterator(const typename DynamicArray<T>::Iterator& other)
            : current(other) {}
        T operator*() const override {
            return *current;
        }
        ArraySeqIterator& operator++() override {
            ++current;
            return *this;
        }
        bool equals(const typename Sequence<T>::IEnumerator& other) const override {
            const auto* o = dynamic_cast<const ArraySeqIterator*>(&other);
            if (!o) return false;
            return current == o->current;
        }
        typename Sequence<T>::IEnumerator* clone() const override {
            return new ArraySeqIterator(current);
        }
    };
public:
    ArraySequence(T* items, size_t count);
    ArraySequence();
    ArraySequence(const LinkedList<T>& list);
    ArraySequence(const ArraySequence<T>& copy);
    ~ArraySequence() override;
    T GetFirst() const override;
    T GetLast() const override;
    T Get(size_t index) const override;
    size_t GetLength() const override;
    ArraySequence<T>& operator=(const ArraySequence<T>& other);
    ArraySequence<T>* Append(T item) override;
    ArraySequence<T>* Prepend(T item) override;
    ArraySequence<T>* InsertAt(T item, size_t index) override;
    Sequence<T>* GetSubsequence(size_t startIndex, size_t endIndex) const override;
    Sequence<T>* Concat(const Sequence<T>& other) override;
    typename Sequence<T>::Iterator begin() const override;
    typename Sequence<T>::Iterator end() const override;
};

template <typename T>
ArraySequence<T>::ArraySequence(T* items, size_t count) 
    : array(new DynamicArray<T>(items, count)) {}

template <typename T>
ArraySequence<T>::ArraySequence() 
    : array(new DynamicArray<T>()) {}

template <typename T>
ArraySequence<T>::ArraySequence(const LinkedList<T>& list) 
    : array(new DynamicArray<T>(list.GetLength())) {
    for (size_t i = 0; i < list.GetLength(); ++i)
        array->Set(i, list.Get(i));
}

template <typename T>
ArraySequence<T>::ArraySequence(const ArraySequence<T>& copy) 
    : array(new DynamicArray<T>(*copy.array)) {}

template <typename T>
ArraySequence<T>::~ArraySequence() {
    delete array;
}

template <typename T>
T ArraySequence<T>::GetFirst() const {
    if (array->GetSize() == 0)
        throw OutOfRangeException("GetFirst on empty ArraySequence");
    return array->Get(0);
}

template <typename T>
T ArraySequence<T>::GetLast() const {
    if (array->GetSize() == 0)
        throw OutOfRangeException("GetLast on empty ArraySequence");
    return array->Get(array->GetSize() - 1);
}

template <typename T>
T ArraySequence<T>::Get(size_t index) const {
    if (index >= array->GetSize())
        throw OutOfRangeException("Index out of range in ArraySequence::Get");
    return array->Get(index);
}

template <typename T>
size_t ArraySequence<T>::GetLength() const {
    return array->GetSize();
}

template <typename T>
ArraySequence<T>& ArraySequence<T>::operator=(const ArraySequence<T>& other) {
    if (this != &other) {
        ArraySequence<T>* copy = other.Clone();
        std::swap(this->array, copy->array);
        delete copy;          
    }
    return *this;
}

template <typename T>
void ArraySequence<T>::PrependInternal(T item) {
    size_t newSize = array->GetSize() + 1;
    array->Resize(newSize);
    for (size_t i = newSize - 1; i > 0; --i)
        array->Set(i, array->Get(i - 1));
    array->Set(0, item);
}

template <typename T>
void ArraySequence<T>::AppendInternal(T item) {
    size_t newSize = array->GetSize() + 1;
    array->Resize(newSize);
    array->Set(newSize - 1, item);
}

template <typename T>
void ArraySequence<T>::InsertAtInternal(T item, size_t index) {
    size_t len = array->GetSize();
    if (index > len)
        throw OutOfRangeException("InsertAtInternal: index out of range");
    size_t newSize = len + 1;
    array->Resize(newSize);
    for (size_t i = newSize - 1; i > index; --i)
        array->Set(i, array->Get(i - 1));
    array->Set(index, item);
}

template <typename T>
ArraySequence<T>* ArraySequence<T>::Append(T item) {
    ArraySequence<T>* result = this->Instance();
    result->AppendInternal(item);
    return result;
}

template <typename T>
ArraySequence<T>* ArraySequence<T>::Prepend(T item) {
    ArraySequence<T>* result = this->Instance();
    result->PrependInternal(item);
    return result;
}

template <typename T>
ArraySequence<T>* ArraySequence<T>::InsertAt(T item, size_t index) {
    ArraySequence<T>* result = this->Instance();
    result->InsertAtInternal(item, index);
    return result;
}

template <typename T>
Sequence<T>* ArraySequence<T>::GetSubsequence(size_t startIndex, size_t endIndex) const {
    size_t len = array->GetSize();
    if (startIndex > endIndex || startIndex >= len || endIndex > len)
        throw OutOfRangeException("GetSubsequence: invalid indices");
    size_t newSize = endIndex - startIndex;
    ArraySequence<T>* result = this->EmptyClone();
    result->array->Resize(newSize);
    for (size_t i = startIndex, p = 0; i < endIndex; ++i, ++p)
        result->array->Set(p, array->Get(i));
    return result;
}

template <typename T>
Sequence<T>* ArraySequence<T>::Concat(const Sequence<T>& other) {
    ArraySequence<T>* result = this->EmptyClone();
    size_t newLen = GetLength() + other.GetLength();
    result->array->Resize(newLen);
    size_t pos = 0;
    for (size_t i = 0; i < GetLength(); ++i)
        result->array->Set(pos++, Get(i));
    for (size_t i = 0; i < other.GetLength(); ++i)
        result->array->Set(pos++, other.Get(i));
    return result;
}

template <typename T>
class MutableArraySequence : public ArraySequence<T> {
protected:
    MutableArraySequence<T>* EmptyClone() const override {
        return new MutableArraySequence<T>();
    }
    MutableArraySequence<T>* Clone() const override {
        return new MutableArraySequence<T>(*this);
    }
    MutableArraySequence<T>* Instance() override {
        return this;
    }
public:
    using ArraySequence<T>::ArraySequence;
    using ArraySequence<T>::operator=;
};

template <typename T>
class ImmutableArraySequence : public ArraySequence<T> {
protected:
    ImmutableArraySequence<T>* EmptyClone() const override {
        return new ImmutableArraySequence<T>();
    }
    ImmutableArraySequence<T>* Clone() const override {
        return new ImmutableArraySequence<T>(*this);
    }
    ImmutableArraySequence<T>* Instance() override {
        return this->Clone();
    }
public:
    using ArraySequence<T>::ArraySequence;
    using ArraySequence<T>::operator=;
};

template <typename T>
typename Sequence<T>::Iterator ArraySequence<T>::begin() const {
    return typename Sequence<T>::Iterator(new ArraySeqIterator(array->begin()));
}

template <typename T>
typename Sequence<T>::Iterator ArraySequence<T>::end() const {
    return typename Sequence<T>::Iterator(new ArraySeqIterator(array->end()));
}

template <typename T, typename U>
ArraySequence<U>* Map(const ArraySequence<T>& seq, U (*func)(T)) {
    ArraySequence<U>* result = new MutableArraySequence<U>();
    for (auto it = seq.begin(); it != seq.end(); ++it) {
        result->Append(func(*it));
    }
    return result;
}