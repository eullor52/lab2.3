#pragma once
#include "complex.hpp"
#include "c:/projects/lab-2/headers/sequence.hpp"
#include "c:/projects/lab-2/headers/exceptions.h"
#include "c:/projects/lab-2/headers/arraysequence.hpp"
#include "c:/projects/lab-2/headers/bitsequence.hpp"
#include "c:/projects/lab-2/headers/listsequence.hpp"

template <template <typename> class Container>
struct is_correct_sequence : std::true_type {};

template <>
struct is_correct_sequence<MutableArraySequence> : std::false_type {};

template <>
struct is_correct_sequence<MutableListSequence> : std::false_type {};

inline double power(double base, unsigned int deg) {
    double result = 1.0;
    for (unsigned int i = 0; i < deg; ++i) result *= base;
    return result;
}

template <typename T>
class Fragment {
private:
    static_assert(
        std::is_same_v<T, int> || std::is_same_v<T, double> || std::is_same_v<T, Complex>,
        "Fragment supports only int, double, and Complex types"
    );  
    Sequence<T>* coeffs = nullptr;
    double begin = 0;
    double end = 0;

public:
    Fragment() = default;
    Fragment(double beg, double en, Sequence<T>* coef);
    ~Fragment();

    double GetBegin();
    double GetEnd();

    bool isIntersecting(const Fragment<T>& other) const;

    Fragment(const Fragment<T>& other);
    Fragment& operator=(const Fragment& other);

    void Pruning(double newBeg, double newEnd);

    void Redefinition(Sequence<T>* newCoeffs);
    bool IsInteriorPoint(double point) const;

    auto Estimation(double point);

    auto DerivativeAt(double point);
};

template <typename T>
Fragment<T>::Fragment(double beg, double en, Sequence<T>* coefs) {
    if (!coefs) throw LogicErrorException("Nullptr in argument");
    if (beg >= en) {
        throw LogicErrorException("Begin must be less than end");
    }
    begin = beg;
    end = en;
    coeffs = coefs->EmptyClone();
    try {
        for (auto item : *coefs) coeffs->Append(item);
    } catch (...) {
        delete coeffs;
        throw;
    }
}

template <typename T>
Fragment<T>::~Fragment() {
    delete coeffs;
}

template <typename T>
Fragment<T>::Fragment(const Fragment<T>& other) {
    if (!other.coeffs) {
        coeffs = nullptr;
        begin = other.begin;
        end = other.end;
        return;
    }
    coeffs = other.coeffs->EmptyClone();
    try {
        for (const auto& item : *other.coeffs) coeffs->Append(item);
    } catch (...) {
        delete coeffs;
        throw;
    }
    begin = other.begin;
    end = other.end;
}

template <typename T>
double Fragment<T>::GetBegin() {return begin;}

template <typename T>
double Fragment<T>::GetEnd() {return end;}

template <typename T>
Fragment<T>& Fragment<T>::operator=(const Fragment<T>& other) {
    if (this != &other) {
        if (!other.coeffs) {
            delete coeffs;
            coeffs = nullptr;
        } else {
            Sequence<T>* newCoeffs = other.coeffs->EmptyClone();
            try {
                for (const auto& item : *other.coeffs)  newCoeffs->Append(item);
            } catch (...){
                delete newCoeffs;
                throw;
            }
            delete coeffs;
            coeffs = newCoeffs;
        }
        begin = other.begin;
        end = other.end;
    }
    return *this;
}

template <typename T>
bool Fragment<T>::isIntersecting(const Fragment<T>& other) const {
    return this->IsInteriorPoint(other.end) || this->IsInteriorPoint(other.begin) 
        || other.IsInteriorPoint(begin) || other.IsInteriorPoint(end);
}

template <typename T>
void Fragment<T>::Pruning(double newBeg, double newEnd) {
    if (newBeg >= newEnd) {
        throw LogicErrorException("Begin must be less than end");
    }
    end = newEnd;
    begin = newBeg;
}

template <typename T>
void Fragment<T>::Redefinition(Sequence<T>* newCoeffs) {
    if (!newCoeffs) throw LogicErrorException("Nullptr in argument");
    Sequence<T>* newClone = newCoeffs->EmptyClone();
    try {
        for (auto item : *newCoeffs) newClone->Append(item);
    } catch (...) {
        delete newClone;
        throw;
    }
    delete coeffs;
    coeffs = newClone;
}

template <typename T>
bool Fragment<T>::IsInteriorPoint(double point) const {
    return point > begin && point < end;
}

template <>
inline auto Fragment<int>::Estimation(double point) {
    double result = 0.0;
    if (!coeffs) return result;
    for (size_t i = 0; i < coeffs->GetLength(); ++i) {
        double coef = static_cast<double>(coeffs->Get(i));
        result += coef * power(point, i);
    }
    return result;
}


template <>
inline auto Fragment<double>::Estimation(double point) {
    double result = 0.0;
    if (!coeffs) return result;
    for (size_t i = 0; i < coeffs->GetLength(); ++i) {
        result += coeffs->Get(i) * power(point, i);
    }
    return result;
}

template <>
inline auto Fragment<Complex>::Estimation(double point) {
    Complex result;
    if (!coeffs) return result;
    for (size_t i = 0; i < coeffs->GetLength(); ++i) {
        Complex coef = coeffs->Get(i);
        Complex factor(power(point, i), 0.0);
        result += coef * factor;
    }
    return result;
}

template <>
inline auto Fragment<int>::DerivativeAt(double point) {
    double result = 0.0;
    if (!coeffs) return result;
    for (size_t i = 1; i < coeffs->GetLength(); ++i) {
        double coef = static_cast<double>(coeffs->Get(i));
        result += i * coef * power(point, i - 1);
    }
    return result;
}

template <>
inline auto Fragment<double>::DerivativeAt(double point) {
    double result = 0.0;
    if (!coeffs) return result;
    for (size_t i = 1; i < coeffs->GetLength(); ++i) {
        result += i * coeffs->Get(i) * power(point, i - 1);
    }
    return result;
}

template <>
inline auto Fragment<Complex>::DerivativeAt(double point) {
    Complex result;
    if (!coeffs) return result;
    for (size_t i = 1; i < coeffs->GetLength(); ++i) {
        Complex coef = coeffs->Get(i);
        Complex factor(i * power(point, i - 1), 0.0);
        result += coef * factor;
    }
    return result;
}