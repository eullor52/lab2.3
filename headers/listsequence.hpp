#pragma once

#include "sequence.hpp"
#include "list.hpp"
#include <utility>
#include <stdexcept>

template <typename T>
class ListSequence : public Sequence<T> {
private:
    LinkedList<T>* list;
protected:
    virtual ListSequence<T>* EmptyClone() const = 0;
    virtual ListSequence<T>* Clone() const = 0;
    virtual ListSequence<T>* Instance() = 0;
    void AppendInternal(T item);
    void PrependInternal(T item);
    void InsertAtInternal(T item, size_t index);

    class ListSeqIterator : public Sequence<T>::IEnumerator {
    private:
        typename LinkedList<T>::Iterator current;
    public:
        ListSeqIterator (const typename LinkedList<T>::Iterator& other)
            : current(other) {} 
        T operator*() const override {
            return *current;
        }
        ListSeqIterator& operator++() override {
            ++current;
            return *this;
        }
        bool equals(const typename Sequence<T>::IEnumerator& other) const override {
            const auto* o = dynamic_cast<const ListSeqIterator*>(&other);
            if (!o) return false;
            return current == o->current;
        }
        typename Sequence<T>::IEnumerator* clone() const override {
            return new ListSeqIterator(current);
        }
    };
public:
    ListSequence(T* items, size_t count);
    ListSequence();
    ListSequence(const LinkedList<T>& copy);
    ListSequence(const ListSequence<T>& copy);
    ~ListSequence() override;
    T GetFirst() const override;
    T GetLast() const override;
    T Get(size_t index) const override;
    size_t GetLength() const override;
    ListSequence<T>& operator=(const ListSequence<T>& other);
    ListSequence<T>* Append(T item) override;
    ListSequence<T>* Prepend(T item) override;
    ListSequence<T>* InsertAt(T item, size_t index) override;
    Sequence<T>* GetSubsequence(size_t startIndex, size_t endIndex) const override;
    Sequence<T>* Concat(const Sequence<T>& other) override;
    typename Sequence<T>::Iterator begin() const override;
    typename Sequence<T>::Iterator end() const override;
};

template <typename T>
ListSequence<T>::ListSequence(T* items, size_t count)
    : list(new LinkedList<T>(items, count)) {}

template <typename T>
ListSequence<T>::ListSequence()
    : list(new LinkedList<T>()) {}

template <typename T>
ListSequence<T>::ListSequence(const LinkedList<T>& copy)
    : list(new LinkedList<T>(copy)) {}

template <typename T>
ListSequence<T>::ListSequence(const ListSequence<T>& copy)
    : list(new LinkedList<T>(*copy.list)) {}

template <typename T>
ListSequence<T>::~ListSequence() {
    delete list;
}

template <typename T>
T ListSequence<T>::GetFirst() const {
    return list->GetFirst();
}

template <typename T>
T ListSequence<T>::GetLast() const {
    return list->GetLast();
}

template <typename T>
T ListSequence<T>::Get(size_t index) const {
    return list->Get(index);
}

template <typename T>
size_t ListSequence<T>::GetLength() const {
    return list->GetLength();
}

template <typename T>
ListSequence<T>& ListSequence<T>::operator=(const ListSequence<T>& other) {
    if (this != &other) {
        ListSequence<T>* copy = other.Clone();
        std::swap(this->list, copy->list);
        delete copy;
    }
    return *this;
}

template <typename T>
void ListSequence<T>::AppendInternal(T item) {
    list->Append(item);
}

template <typename T>
void ListSequence<T>::PrependInternal(T item) {
    list->Prepend(item);
}

template <typename T>
void ListSequence<T>::InsertAtInternal(T item, size_t index) {
    list->InsertAt(item, index);
}

template <typename T>
ListSequence<T>* ListSequence<T>::Append(T item) {
    ListSequence<T>* result = this->Instance();
    result->AppendInternal(item);
    return result;
}

template <typename T>
ListSequence<T>* ListSequence<T>::Prepend(T item) {
    ListSequence<T>* result = this->Instance();
    result->PrependInternal(item);
    return result;
}

template <typename T>
ListSequence<T>* ListSequence<T>::InsertAt(T item, size_t index) {
    ListSequence<T>* result = this->Instance();
    result->InsertAtInternal(item, index);
    return result;
}

template <typename T>
Sequence<T>* ListSequence<T>::GetSubsequence(size_t startIndex, size_t endIndex) const {
    size_t len = this->GetLength();
    if (startIndex > endIndex || startIndex >= len || endIndex > len)
        throw OutOfRangeException("GetSubsequence: invalid indices");
    ListSequence<T>* result = this->EmptyClone();
    for (size_t i = startIndex; i <= endIndex; ++i)
        result->Append(this->Get(i));
    return result;
}

template <typename T>
Sequence<T>* ListSequence<T>::Concat(const Sequence<T>& other) {
    ListSequence<T>* result = this->EmptyClone();
    size_t myLen = this->GetLength();
    for (size_t i = 0; i < myLen; ++i)
        result->Append(this->Get(i));
    size_t otherLen = other.GetLength();
    for (size_t i = 0; i < otherLen; ++i)
        result->Append(other.Get(i));
    return result;
}

template <typename T>
class MutableListSequence : public ListSequence<T> {
protected:
    MutableListSequence<T>* EmptyClone() const override {
        return new MutableListSequence<T>();
    }
    MutableListSequence<T>* Clone() const override {
        return new MutableListSequence<T>(*this);
    }
    MutableListSequence<T>* Instance() override {
        return this;
    }
public:
    using ListSequence<T>::ListSequence;
    using ListSequence<T>::operator=;
};

template <typename T>
class ImmutableListSequence : public ListSequence<T> {
protected:
    ImmutableListSequence<T>* EmptyClone() const override {
        return new ImmutableListSequence<T>();
    }
    ImmutableListSequence<T>* Clone() const override {
        return new ImmutableListSequence<T>(*this);
    }
    ImmutableListSequence<T>* Instance() override {
        return this->Clone();
    }
public:
    using ListSequence<T>::ListSequence;
    using ListSequence<T>::operator=;
};

template <typename T>
typename Sequence<T>::Iterator ListSequence<T>::begin() const {
    return typename Sequence<T>::Iterator(new ListSeqIterator(list->begin()));
}

template <typename T>
typename Sequence<T>::Iterator ListSequence<T>::end() const {
    return typename Sequence<T>::Iterator(new ListSeqIterator(list->end()));
}

template <typename T, typename U>
ListSequence<U>* Map(const ListSequence<T>& seq, U (*func)(T)) {
    ListSequence<U>* result = new MutableListSequence<U>();
    for (auto it = seq.begin(); it != seq.end(); ++it) {
        result->Append(func(*it));
    }
    return result;
}