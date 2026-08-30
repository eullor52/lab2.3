#pragma once

#include <stdexcept>
#include "exceptions.h"
#include <utility>

template <typename T>
class LinkedList {
private:
    struct ListElem {
        ListElem* prev = nullptr;
        T value;
        ListElem* next = nullptr;
    };
    using elem = ListElem;
    elem* head = nullptr;
    elem* tail = nullptr;

public:
    class Iterator {
    private:
        ListElem* current;
    public:
        Iterator(ListElem* ptr) : current(ptr) {}
        T operator*() const {
            return current->value;
        }
        Iterator& operator++() {
            if (current) current = current->next;
            return *this;
        }
        bool operator!=(const Iterator& other) const {
            return current != other.current;
        }
        bool operator==(const Iterator& other) const {
            return !(*this != other);
        }
    };
    LinkedList() = default;
    LinkedList(T* items, size_t count);
    LinkedList(const LinkedList<T>& list);
    ~LinkedList();
    LinkedList<T>& operator=(const LinkedList<T>& other);
    T GetFirst() const;
    T GetLast() const;
    T Get(size_t index) const;
    size_t GetLength() const;
    LinkedList<T> GetSubList(size_t startIndex, size_t endIndex) const;
    void Append(T item);
    void Prepend(T item);
    void InsertAt(T item, size_t index);
    void Clear();
    LinkedList<T> Concat(const LinkedList<T>* list) const;
    Iterator begin() const;
    Iterator end() const;
};

template <typename T>
LinkedList<T>::LinkedList(T* items, size_t count) : head(nullptr), tail(nullptr) {
    if (count == 0) return;
    if (!items) throw InvalidArgumentException("Items pointer cannot be null");
    try {
        for (size_t i = 0; i < count; ++i) {
            Append(items[i]);
        }
    } catch (...) {
        Clear();
        throw;
    }
}

template <typename T>
LinkedList<T>::LinkedList(const LinkedList<T>& list) : head(nullptr), tail(nullptr) {
    try {
        elem* current = list.head;
        while (current) {
            Append(current->value);
            current = current->next;
        }
    } catch (...) {
        Clear();
        throw;
    }
}

template <typename T>
LinkedList<T>::~LinkedList() {
    Clear();
}

template <typename T>
void LinkedList<T>::Clear() {
    elem* current = head;
    while (current) {
        elem* next = current->next;
        delete current;
        current = next;
    }
    head = nullptr;
    tail = nullptr;
}

template <typename T>
LinkedList<T>& LinkedList<T>::operator=(const LinkedList<T>& other) {
    if (this != &other) {
        LinkedList<T> temp(other);
        std::swap(head, temp.head);
        std::swap(tail, temp.tail);
    }
    return *this;
}

template <typename T>
T LinkedList<T>::GetFirst() const {
    if (!head) throw OutOfRangeException("List is empty");
    return head->value;
}

template <typename T>
T LinkedList<T>::GetLast() const {
    if (!tail) throw OutOfRangeException("List is empty");
    return tail->value;
}

template <typename T>
T LinkedList<T>::Get(size_t index) const {
    elem* current = head;
    size_t i = 0;
    while (current && i < index) {
        current = current->next;
        ++i;
    }
    if (!current) throw OutOfRangeException("Index out of range");
    return current->value;
}

template <typename T>
size_t LinkedList<T>::GetLength() const {
    size_t length = 0;
    elem* current = head;
    while (current) {
        ++length;
        current = current->next;
    }
    return length;
}

template <typename T>
void LinkedList<T>::Append(T item) {
    elem* newElem = new elem{tail, std::move(item), nullptr};
    if (tail) {
        tail->next = newElem;
    } else {
        head = newElem;
    }
    tail = newElem;
}

template <typename T>
void LinkedList<T>::Prepend(T item) {
    elem* newElem = new elem{nullptr, std::move(item), head};
    if (head) {
        head->prev = newElem;
    } else {
        tail = newElem;
    }
    head = newElem;
}

template <typename T>
void LinkedList<T>::InsertAt(T item, size_t index) {
    if (index == 0) {
        Prepend(std::move(item));
        return;
    }
    
    elem* current = head;
    size_t i = 0;
    while (current && i < index) {
        current = current->next;
        ++i;
    }
    
    if (i == index && !current) {
        Append(std::move(item));
        return;
    }
    
    if (!current) {
        throw OutOfRangeException("Index out of range");
    }
    
    elem* newElem = new elem{current->prev, std::move(item), current};
    if (current->prev) {
        current->prev->next = newElem;
    }
    current->prev = newElem;
}

template <typename T>
LinkedList<T> LinkedList<T>::GetSubList(size_t startIndex, size_t endIndex) const {
    size_t len = GetLength();
    if (len == 0) {
        if (startIndex == 0 && endIndex == 0)
            return LinkedList<T>();
        else
            throw OutOfRangeException("Invalid sublist indices");
    }
    if (startIndex > endIndex || endIndex >= len)
        throw OutOfRangeException("Invalid sublist indices");
    LinkedList<T> result;
    elem* current = head;
    size_t i = 0;
    while (current && i < startIndex) {
        current = current->next;
        ++i;
    }
    try {
        while (current && i <= endIndex) {
            result.Append(current->value);
            current = current->next;
            ++i;
        }
    } catch (...) {
        result.Clear();
        throw;
    }
    return result;
}

template <typename T>
LinkedList<T> LinkedList<T>::Concat(const LinkedList<T>* list) const {
    LinkedList<T> result;
    try {
        elem* current = head;
        while (current) {
            result.Append(current->value);
            current = current->next;
        }
        if (list) {
            current = list->head;
            while (current) {
                result.Append(current->value);
                current = current->next;
            }
        }
    } catch (...) {
        result.Clear();
        throw;
    }
    return result;
}

template <typename T>
typename LinkedList<T>::Iterator LinkedList<T>::begin() const {
    return Iterator(head);
}

template <typename T>
typename LinkedList<T>::Iterator LinkedList<T>::end() const {
    return Iterator(nullptr);
} 