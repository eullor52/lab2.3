#pragma once
#include "complex.hpp"
#include "sequence.hpp"
#include "exceptions.h"
#include "arraysequence.hpp"
#include "bitsequence.hpp"
#include "listsequence.hpp"
#include <string>
#include <sstream>

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
inline std::string FormatValue(const T& value) {
    std::ostringstream oss;
    oss << value;
    return oss.str();
}

inline std::string FormatValue(const Complex& value) {
    return value.toString();
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

    double GetBegin() const;
    double GetEnd() const;

    bool isIntersecting(const Fragment<T>& other) const;

    Fragment(const Fragment<T>& other);
    Fragment& operator=(const Fragment& other);

    void Pruning(double newBeg, double newEnd);

    void Redefinition(Sequence<T>* newCoeffs);
    bool IsInteriorPoint(double point) const;

    auto Estimation(double point) const;

    auto DerivativeAt(double point) const;

    std::string toString() const;
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
double Fragment<T>::GetBegin() const {return begin;}

template <typename T>
double Fragment<T>::GetEnd() const {return end;}

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
    return this->IsInteriorPoint(other.end) && other.end != begin 
        && other.end != end 
        || this->IsInteriorPoint(other.begin) && other.begin != begin 
        && other.begin != end
        || other.IsInteriorPoint(begin) && other.begin != begin 
        && other.end != begin
        || other.IsInteriorPoint(end) && other.begin != end
        && other.end != end;
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
    return point >= begin && point <= end;
}

template <>
inline auto Fragment<int>::Estimation(double point)  const {
    double result = 0.0;
    if (!coeffs) return result;
    for (size_t i = 0; i < coeffs->GetLength(); ++i) {
        double coef = static_cast<double>(coeffs->Get(i));
        result += coef * power(point, i);
    }
    return result;
}


template <>
inline auto Fragment<double>::Estimation(double point) const {
    double result = 0.0;
    if (!coeffs) return result;
    for (size_t i = 0; i < coeffs->GetLength(); ++i) {
        result += coeffs->Get(i) * power(point, i);
    }
    return result;
}

template <>
inline auto Fragment<Complex>::Estimation(double point) const {
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
inline auto Fragment<int>::DerivativeAt(double point) const {
    double result = 0.0;
    if (!coeffs) return result;
    for (size_t i = 1; i < coeffs->GetLength(); ++i) {
        double coef = static_cast<double>(coeffs->Get(i));
        result += i * coef * power(point, i - 1);
    }
    return result;
}

template <>
inline auto Fragment<double>::DerivativeAt(double point) const {
    double result = 0.0;
    if (!coeffs) return result;
    for (size_t i = 1; i < coeffs->GetLength(); ++i) {
        result += i * coeffs->Get(i) * power(point, i - 1);
    }
    return result;
}

template <>
inline auto Fragment<Complex>::DerivativeAt(double point) const {
    Complex result;
    if (!coeffs) return result;
    for (size_t i = 1; i < coeffs->GetLength(); ++i) {
        Complex coef = coeffs->Get(i);
        Complex factor(i * power(point, i - 1), 0.0);
        result += coef * factor;
    }
    return result;
}

template <typename T>
std::string Fragment<T>::toString() const {
    std::ostringstream oss;
    oss << "[" << begin << ", " << end << "]: ";

    if (!coeffs || coeffs->GetLength() == 0) {
        oss << "0";
        return oss.str();
    }

    bool first = true;
    for (size_t i = 0; i < coeffs->GetLength(); ++i) {
        T coef = coeffs->Get(i);
        if (coef == T()) continue;

        if (!first) oss << " + ";
        if (i == 0) {
            oss << FormatValue(coef);
        } else if (i == 1) {
            oss << FormatValue(coef) << "*x";
        } else {
            oss << FormatValue(coef) << "*x^" << i;
        }
        first = false;
    }

    if (first) oss << "0";
    return oss.str();
}