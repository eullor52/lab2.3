#pragma once

#include <iostream>
#include <cstddef>
#include "exceptions.h"
#include <string>
#include <stdexcept>
#include <format>

template <typename T>
class BitProxy {
private:
    T& data;
    size_t index;
public:
    BitProxy(T& value, size_t idx);
    BitProxy& operator=(bool value);
    BitProxy& operator=(const BitProxy& other);
    operator bool() const;
};

template <typename T>
class Bit {
private:
    T value;
protected:
    void CheckIndex(size_t idx) const;
public:
    Bit();
    Bit(T val);
    Bit(const Bit& operand);
    size_t GetBitCount() const;
    T GetValue() const;
    void SetValue(T val);
    bool operator[](size_t idx) const;
    BitProxy<T> operator[](size_t idx);
    operator bool() const;
    Bit& operator=(const Bit& operand);
    Bit operator&(const Bit& operand) const;
    Bit operator|(const Bit& operand) const;
    Bit operator^(const Bit& operand) const;
    Bit operator~() const;
    bool operator==(const Bit& operand) const;
    bool operator!=(const Bit& operand) const;
};

template <typename T>
BitProxy<T>::BitProxy(T& value, size_t idx) : data(value), index(idx) {}

template <typename T>
BitProxy<T>& BitProxy<T>::operator=(bool value) {
    if (value) {
        data = data | (static_cast<T>(1) << index);
    } else {
        data = data & ~(static_cast<T>(1) << index);
    }
    return *this;
}

template <typename T>
BitProxy<T>& BitProxy<T>::operator=(const BitProxy& other) {
    return *this = static_cast<bool>(other);
}

template <typename T>
BitProxy<T>::operator bool() const {
    return (data >> index) & static_cast<T>(1);
}

template <typename T>
void Bit<T>::CheckIndex(size_t idx) const {
    if (idx >= sizeof(T) * 8) {
        throw OutOfRangeException("Index out of range");
    }
}

template <typename T>
Bit<T>::Bit() : value(0) {}

template <typename T>
Bit<T>::Bit(T val) : value(val) {}

template <typename T>
Bit<T>::Bit(const Bit& operand) : value(operand.value) {}

template <typename T>
size_t Bit<T>::GetBitCount() const { 
    return sizeof(T) * 8; 
}

template <typename T>
T Bit<T>::GetValue() const { 
    return value; 
}

template <typename T>
void Bit<T>::SetValue(T val) { 
    value = val; 
}

template <typename T>
bool Bit<T>::operator[](size_t idx) const {
    CheckIndex(idx);
    return (value >> idx) & static_cast<T>(1);
}

template <typename T>
BitProxy<T> Bit<T>::operator[](size_t idx) {
    CheckIndex(idx);
    return BitProxy<T>(value, idx);
}

template <typename T>
Bit<T>::operator bool() const { 
    return value != 0; 
}

template <typename T>
Bit<T>& Bit<T>::operator=(const Bit& operand) {
    if (this != &operand) {
        value = operand.value;
    }
    return *this;
}

template <typename T>
Bit<T> Bit<T>::operator&(const Bit& operand) const { 
    return Bit(value & operand.value); 
}

template <typename T>
Bit<T> Bit<T>::operator|(const Bit& operand) const { 
    return Bit(value | operand.value); 
}

template <typename T>
Bit<T> Bit<T>::operator^(const Bit& operand) const { 
    return Bit(value ^ operand.value); 
}

template <typename T>
Bit<T> Bit<T>::operator~() const { 
    return Bit(~value); 
}

template <typename T>
bool Bit<T>::operator==(const Bit& operand) const { 
    return value == operand.value; 
}

template <typename T>
bool Bit<T>::operator!=(const Bit& operand) const { 
    return value != operand.value; 
}