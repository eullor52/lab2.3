#pragma once

#include "piecewisefunction.hpp"
#include "fragment.hpp"
#include "arraysequence.hpp"
#include "exceptions.h"
#include "complex.hpp"
#include <cstddef>
#include <type_traits>

template <typename T>
struct SplineNode {
    double x = 0;
    T y = T();
};

using Point = SplineNode<double>;
using ComplexPoint = SplineNode<Complex>;

template <typename T>
inline T ScalarMultiply(double scalar, const T& value) {
    return scalar * value;
}

inline Complex ScalarMultiply(double scalar, const Complex& value) {
    return Complex(scalar, 0.0) * value;
}

template <typename T>
inline T ScalarDivide(const T& value, double scalar) {
    return value / scalar;
}

inline Complex ScalarDivide(const Complex& value, double scalar) {
    return value * Complex(1.0 / scalar, 0.0);
}

template <typename T>
inline void ShiftPolynomialCoeffs(const T* localCoeffs, size_t n, double x0, T* result) {
    if (n == 0) return;
    for (size_t i = 0; i < n; ++i) result[i] = T();
    result[0] = localCoeffs[n - 1];
    for (size_t k = 1; k < n; ++k) {
        T nextCoeff = localCoeffs[n - 1 - k];
        for (size_t mi = 0; mi <= k; ++mi) {
            size_t m = k - mi;
            T prev = (m > 0) ? result[m - 1] : T();
            result[m] = prev - ScalarMultiply(x0, result[m]);
        }
        result[0] += nextCoeff;
    }
}

template <typename T, template <typename> class Container = MutableArraySequence>
PieceWiseFunction<T, Container> CubicSplineInterpolationImpl(const SplineNode<T>* points, size_t n) {
    static_assert(std::is_same_v<T, double> || std::is_same_v<T, Complex>,
                  "Cubic spline interpolation supports only double and Complex ordinates");

    if (!points) throw LogicErrorException("Nullptr in argument");
    if (n < 2) throw LogicErrorException("At least 2 points are required for spline interpolation");
    for (size_t i = 0; i + 1 < n; ++i) {
        if (points[i].x >= points[i + 1].x) {
            throw LogicErrorException("Points must be strictly increasing by x");
        }
    }
    double* h = new double[n - 1];
    T* M = new T[n];
    double* alpha = nullptr;
    T* beta = nullptr;
    try {
        for (size_t i = 0; i + 1 < n; ++i) 
            h[i] = points[i + 1].x - points[i].x;
        for (size_t i = 0; i < n; ++i) 
            M[i] = T();
        if (n > 2) {
            alpha = new double[n];
            beta = new T[n];
            for (size_t i = 0; i < n; ++i) {
                alpha[i] = 0.0;
                beta[i] = T(); 
            }

            for (size_t i = 1; i + 1 < n; ++i) {
                double a = h[i - 1];
                double b = 2.0 * (h[i - 1] + h[i]);
                double c = h[i];
                T d = ScalarMultiply(6.0,
                        ScalarDivide(points[i + 1].y - points[i].y, h[i])
                      - ScalarDivide(points[i].y - points[i - 1].y, h[i - 1]));

                double denom = b - a * alpha[i - 1];
                alpha[i] = c / denom;
                beta[i] = ScalarDivide(d - ScalarMultiply(a, beta[i - 1]), denom);
            }
            for (long i = static_cast<long>(n) - 2; i >= 1; --i) {
                size_t ui = static_cast<size_t>(i);
                M[ui] = beta[ui] - ScalarMultiply(alpha[ui], M[ui + 1]);
            }

            delete[] alpha;
            delete[] beta;
            alpha = nullptr;
            beta = nullptr;
        }

        PieceWiseFunction<T, Container> result;
        for (size_t i = 0; i + 1 < n; ++i) {
            T localCoeffs[4];
            localCoeffs[0] = points[i].y;
            localCoeffs[1] = ScalarDivide(points[i + 1].y - points[i].y, h[i])
                           - ScalarMultiply(h[i] / 6.0, ScalarMultiply(2.0, M[i]) + M[i + 1]);
            localCoeffs[2] = ScalarDivide(M[i], 2.0);
            localCoeffs[3] = ScalarDivide(M[i + 1] - M[i], 6.0 * h[i]);

            T globalCoeffs[4];
            ShiftPolynomialCoeffs(localCoeffs, 4, points[i].x, globalCoeffs);

            MutableArraySequence<T> coeffSeq(globalCoeffs, 4);
            Fragment<T> fragment(points[i].x, points[i + 1].x, &coeffSeq);
            result.FurtherSpecification(fragment);
        }

        delete[] h;
        delete[] M;
        return result;
    } catch (...) {
        delete[] h;
        delete[] M;
        delete[] alpha;
        delete[] beta;
        throw;
    }
}

template <template <typename> class Container = MutableArraySequence>
PieceWiseFunction<double, Container> CubicSplineInterpolation(const Point* points, size_t n) {
    return CubicSplineInterpolationImpl<double, Container>(points, n);
}

template <template <typename> class Container = MutableArraySequence>
PieceWiseFunction<Complex, Container> CubicSplineInterpolation(const ComplexPoint* points, size_t n) {
    return CubicSplineInterpolationImpl<Complex, Container>(points, n);
}