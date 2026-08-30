#pragma once

#include <memory>
#include <stdexcept>

template <typename T>
class Sequence {
public:
    class IEnumerator {
    public:
        virtual ~IEnumerator() = default;
        virtual T operator*() const = 0;
        virtual IEnumerator& operator++() = 0;
        virtual bool equals(const IEnumerator& other) const = 0;
        virtual IEnumerator* clone() const = 0;
    };

    class Iterator {
    private:
        std::unique_ptr<IEnumerator> impl;
    public:
        Iterator() = default;
        explicit Iterator(IEnumerator* p) : impl(p) {}
        Iterator(const Iterator& other)
            : impl(other.impl ? other.impl->clone() : nullptr) {}
        Iterator& operator=(const Iterator& other) {
            if (this != &other) {
                impl.reset(other.impl ? other.impl->clone() : nullptr);
            }
            return *this;
        }
        T operator*() const { return **impl; }
        Iterator& operator++() { ++(*impl); return *this; }

        bool operator!=(const Iterator& other) const {
            if (!impl && !other.impl) return false;
            if (!impl || !other.impl) return true;
            return !impl->equals(*other.impl);
        }
        bool operator==(const Iterator& other) const {
            return !(*this != other);
        }
    };

    virtual Iterator begin() const = 0;
    virtual Iterator end() const = 0;
    virtual Sequence<T>* EmptyClone() const = 0;
    virtual ~Sequence() = default;
    virtual T GetFirst() const = 0;
    virtual T GetLast() const = 0;
    virtual T Get(size_t index) const = 0;
    virtual size_t GetLength() const = 0;
    virtual Sequence<T>* Append(T item) = 0;
    virtual Sequence<T>* Prepend(T item) = 0;
    virtual Sequence<T>* InsertAt(T item, size_t index) = 0;
    virtual Sequence<T>* GetSubsequence(size_t startIndex, size_t endIndex) const = 0;
    virtual Sequence<T>* Concat(const Sequence<T>& other) = 0;
};
