#include "../headers/spline.hpp"
#include "../headers/piecewisefunction.hpp"
#include "../headers/fragment.hpp"
#include "../headers/arraysequence.hpp"
#include "../headers/exceptions.h"
#include "../headers/complex.hpp"
#include "fixtures.hpp"
#include <iostream>
#include <sstream>
#include <cmath>
#include <type_traits>

using std::cerr;

constexpr double kEps = 1e-9;

template <typename T>
T MakeValue(double value);

template <>
inline double MakeValue<double>(double value) {
    return value;
}

template <>
inline Complex MakeValue<Complex>(double value) {
    return Complex(value, 0.0);
}

template <typename T>
double Magnitude(const T& value);

template <>
inline double Magnitude<double>(const double& value) {
    return std::abs(value);
}

template <>
inline double Magnitude<Complex>(const Complex& value) {
    return abs(value);
}

template <typename T>
std::string DescribeValue(const T& value);

template <>
inline std::string DescribeValue<double>(const double& value) {
    std::ostringstream oss;
    oss << value;
    return oss.str();
}

template <>
inline std::string DescribeValue<Complex>(const Complex& value) {
    return value.toString();
}

template <typename T>
class SplineFixture : public testing::Test {
protected:
    using Node = SplineNode<T>;

    Node linear2[2] = {{0, MakeValue<T>(0)}, {2, MakeValue<T>(4)}};

    Node hat3[3] = {{0, MakeValue<T>(0)}, {1, MakeValue<T>(1)}, {2, MakeValue<T>(0)}};

    Node uneven3[3] = {{-1, MakeValue<T>(2)}, {1, MakeValue<T>(0)}, {4, MakeValue<T>(3)}};

    Node general4[4] = {{0, MakeValue<T>(0)}, {1, MakeValue<T>(1)}, {2, MakeValue<T>(0)}, {3, MakeValue<T>(1)}};

    Node general5[5] = {{0, MakeValue<T>(1)}, {1, MakeValue<T>(3)}, {2, MakeValue<T>(2)}, {3, MakeValue<T>(5)}, {4, MakeValue<T>(4)}};

    Node equalXAtStart3[3] = {{0, MakeValue<T>(0)}, {0, MakeValue<T>(1)}, {1, MakeValue<T>(2)}};

    Node decreasingInMiddle3[3] = {{0, MakeValue<T>(0)}, {1, MakeValue<T>(1)}, {0.5, MakeValue<T>(2)}};
};

using SplineTypes = ::testing::Types<double, Complex>;
TYPED_TEST_SUITE(SplineFixture, SplineTypes);

TYPED_TEST(SplineFixture, throws_on_nullptr) {
    using T = TypeParam;
    bool thrown = false;
    try {
        CubicSplineInterpolation(static_cast<const SplineNode<T>*>(nullptr), 3);
    } catch (const LogicErrorException&) {
        thrown = true;
    }
    EXPECT_TRUE(thrown);
    if (testing::Test::HasFailure()) {
        cerr << "\nТестируем CubicSplineInterpolation с nullptr вместо точек\n";
        cerr << "Ожидаемое поведение: выброс LogicErrorException\n";
        cerr << "Полученное поведение: исключение "
            << (thrown ? "выброшено" : "не выброшено") << "\n";
    }
}

TYPED_TEST(SplineFixture, throws_on_zero_points) {
    bool thrown = false;
    try {
        CubicSplineInterpolation(this->hat3, 0);
    } catch (const LogicErrorException&) {
        thrown = true;
    }
    EXPECT_TRUE(thrown);
    if (testing::Test::HasFailure()) {
        cerr << "\nТестируем CubicSplineInterpolation при n == 0\n";
        cerr << "Ожидаемое поведение: выброс LogicErrorException\n";
        cerr << "Полученное поведение: исключение "
            << (thrown ? "выброшено" : "не выброшено") << "\n";
    }
}

TYPED_TEST(SplineFixture, throws_on_one_point) {
    bool thrown = false;
    try {
        CubicSplineInterpolation(this->hat3, 1);
    } catch (const LogicErrorException&) {
        thrown = true;
    }
    EXPECT_TRUE(thrown);
    if (testing::Test::HasFailure()) {
        cerr << "\nТестируем CubicSplineInterpolation при n == 1\n";
        cerr << "Ожидаемое поведение: выброс LogicErrorException\n";
        cerr << "Полученное поведение: исключение "
            << (thrown ? "выброшено" : "не выброшено") << "\n";
    }
}

TYPED_TEST(SplineFixture, throws_on_equal_x_at_start) {
    bool thrown = false;
    try {
        CubicSplineInterpolation(this->equalXAtStart3, 3);
    } catch (const LogicErrorException&) {
        thrown = true;
    }
    EXPECT_TRUE(thrown);
    if (testing::Test::HasFailure()) {
        cerr << "\nТестируем CubicSplineInterpolation с повторяющимся x в начале\n";
        cerr << "Точки: (0,0), (0,1), (1,2)\n";
        cerr << "Ожидаемое поведение: выброс LogicErrorException\n";
        cerr << "Полученное поведение: исключение "
            << (thrown ? "выброшено" : "не выброшено") << "\n";
    }
}

TYPED_TEST(SplineFixture, throws_on_decreasing_x_in_middle) {
    bool thrown = false;
    try {
        CubicSplineInterpolation(this->decreasingInMiddle3, 3);
    } catch (const LogicErrorException&) {
        thrown = true;
    }
    EXPECT_TRUE(thrown);
    if (testing::Test::HasFailure()) {
        cerr << "\nТестируем CubicSplineInterpolation с убыванием x в середине массива\n";
        cerr << "Точки: (0,0), (1,1), (0.5,2)\n";
        cerr << "Ожидаемое поведение: выброс LogicErrorException\n";
        cerr << "Полученное поведение: исключение "
            << (thrown ? "выброшено" : "не выброшено") << "\n";
    }
}

TYPED_TEST(SplineFixture, two_points_fragment_count) {
    auto result = CubicSplineInterpolation(this->linear2, 2);
    size_t count = result.CountFragments();
    EXPECT_TRUE(count == 1);
    if (testing::Test::HasFailure()) {
        cerr << "\nТестируем количество фрагментов при n == 2\n";
        cerr << "Ожидаемое количество фрагментов: 1\n";
        cerr << "Полученное количество фрагментов: " << count << "\n";
    }
}

TYPED_TEST(SplineFixture, two_points_interpolates_endpoints) {
    using T = TypeParam;
    auto result = CubicSplineInterpolation(this->linear2, 2);
    Fragment<T> frag = result.GetFragment(0);
    T atBegin = frag.Estimation(0);
    T atEnd = frag.Estimation(2);
    T expectedBegin = MakeValue<T>(0);
    T expectedEnd = MakeValue<T>(4);
    bool ok = Magnitude(atBegin - expectedBegin) < kEps && Magnitude(atEnd - expectedEnd) < kEps;
    EXPECT_TRUE(ok);
    if (testing::Test::HasFailure()) {
        cerr << "\nТестируем прохождение сплайна через узлы при n == 2\n";
        cerr << "Точки: (0,0), (2,4)\n";
        cerr << "Ожидаемые значения: f(0)=0, f(2)=4\n";
        cerr << "Полученные значения: f(0)=" << DescribeValue(atBegin)
            << ", f(2)=" << DescribeValue(atEnd) << "\n";
    }
}

TYPED_TEST(SplineFixture, two_points_is_linear_between_nodes) {
    using T = TypeParam;
    auto result = CubicSplineInterpolation(this->linear2, 2);
    Fragment<T> frag = result.GetFragment(0);
    T v05 = frag.Estimation(0.5);
    T v1 = frag.Estimation(1);
    T v15 = frag.Estimation(1.5);
    bool ok = Magnitude(v05 - MakeValue<T>(1.0)) < kEps
        && Magnitude(v1 - MakeValue<T>(2.0)) < kEps
        && Magnitude(v15 - MakeValue<T>(3.0)) < kEps;
    EXPECT_TRUE(ok);
    if (testing::Test::HasFailure()) {
        cerr << "\nТестируем, что при двух точках сплайн вырождается в прямую y=2x\n";
        cerr << "Ожидаемые значения: f(0.5)=1, f(1)=2, f(1.5)=3\n";
        cerr << "Полученные значения: f(0.5)=" << DescribeValue(v05)
            << ", f(1)=" << DescribeValue(v1) << ", f(1.5)=" << DescribeValue(v15) << "\n";
    }
}

TYPED_TEST(SplineFixture, two_points_derivative_is_constant_slope) {
    using T = TypeParam;
    auto result = CubicSplineInterpolation(this->linear2, 2);
    Fragment<T> frag = result.GetFragment(0);
    T d0 = frag.DerivativeAt(0);
    T d1 = frag.DerivativeAt(1);
    T d2 = frag.DerivativeAt(2);
    T expected = MakeValue<T>(2.0);
    bool ok = Magnitude(d0 - expected) < kEps
        && Magnitude(d1 - expected) < kEps
        && Magnitude(d2 - expected) < kEps;
    EXPECT_TRUE(ok);
    if (testing::Test::HasFailure()) {
        cerr << "\nТестируем производную сплайна при n == 2 (должна быть постоянным наклоном)\n";
        cerr << "Ожидаемая производная везде: 2\n";
        cerr << "Полученные значения: f'(0)=" << DescribeValue(d0)
            << ", f'(1)=" << DescribeValue(d1) << ", f'(2)=" << DescribeValue(d2) << "\n";
    }
}

TYPED_TEST(SplineFixture, two_points_is_continuous) {
    auto result = CubicSplineInterpolation(this->linear2, 2);
    bool ok = result.IsContinuous();
    EXPECT_TRUE(ok);
    if (testing::Test::HasFailure()) {
        cerr << "\nТестируем IsContinuous для сплайна из одного фрагмента (n == 2)\n";
        cerr << "Ожидаемый результат: 1\n";
        cerr << "Полученный результат: " << ok << "\n";
    }
}

TYPED_TEST(SplineFixture, hat_fragment_count) {
    auto result = CubicSplineInterpolation(this->hat3, 3);
    size_t count = result.CountFragments();
    EXPECT_TRUE(count == 2);
    if (testing::Test::HasFailure()) {
        cerr << "\nТестируем количество фрагментов при n == 3\n";
        cerr << "Ожидаемое количество фрагментов: 2\n";
        cerr << "Полученное количество фрагментов: " << count << "\n";
    }
}

TYPED_TEST(SplineFixture, hat_interpolates_nodes) {
    using T = TypeParam;
    auto result = CubicSplineInterpolation(this->hat3, 3);
    Fragment<T> left = result.GetFragment(0);
    Fragment<T> right = result.GetFragment(1);
    T f0 = left.Estimation(0);
    T f1left = left.Estimation(1);
    T f1right = right.Estimation(1);
    T f2 = right.Estimation(2);
    bool ok = Magnitude(f0 - MakeValue<T>(0)) < kEps
        && Magnitude(f1left - MakeValue<T>(1)) < kEps
        && Magnitude(f1right - MakeValue<T>(1)) < kEps
        && Magnitude(f2 - MakeValue<T>(0)) < kEps;
    EXPECT_TRUE(ok);
    if (testing::Test::HasFailure()) {
        cerr << "\nТестируем прохождение сплайна через узлы (0,0), (1,1), (2,0)\n";
        cerr << "Ожидаемые значения: f(0)=0, f(1)=1 (с обеих сторон), f(2)=0\n";
        cerr << "Полученные значения: f(0)=" << DescribeValue(f0) << ", f(1-)=" << DescribeValue(f1left)
            << ", f(1+)=" << DescribeValue(f1right) << ", f(2)=" << DescribeValue(f2) << "\n";
    }
}

TYPED_TEST(SplineFixture, hat_value_at_quarter_points) {
    using T = TypeParam;
    auto result = CubicSplineInterpolation(this->hat3, 3);
    Fragment<T> left = result.GetFragment(0);
    Fragment<T> right = result.GetFragment(1);
    T v025 = left.Estimation(0.25);
    T v175 = right.Estimation(1.75);
    T expected = MakeValue<T>(0.3671875);
    bool ok = Magnitude(v025 - expected) < kEps && Magnitude(v175 - expected) < kEps;
    EXPECT_TRUE(ok);
    if (testing::Test::HasFailure()) {
        cerr << "\nТестируем значения сплайна в точках 0.25 и 1.75\n";
        cerr << "Ожидаемые значения (по формуле f(x)=1.5x-0.5x^3 и симметрии): 0.3671875\n";
        cerr << "Полученные значения: f(0.25)=" << DescribeValue(v025) << ", f(1.75)=" << DescribeValue(v175) << "\n";
    }
}

TYPED_TEST(SplineFixture, hat_value_at_half_points) {
    using T = TypeParam;
    auto result = CubicSplineInterpolation(this->hat3, 3);
    Fragment<T> left = result.GetFragment(0);
    Fragment<T> right = result.GetFragment(1);
    T v05 = left.Estimation(0.5);
    T v15 = right.Estimation(1.5);
    T expected = MakeValue<T>(0.6875);
    bool ok = Magnitude(v05 - expected) < kEps && Magnitude(v15 - expected) < kEps;
    EXPECT_TRUE(ok);
    if (testing::Test::HasFailure()) {
        cerr << "\nТестируем значения сплайна в точках 0.5 и 1.5\n";
        cerr << "Ожидаемые значения: 0.6875\n";
        cerr << "Полученные значения: f(0.5)=" << DescribeValue(v05) << ", f(1.5)=" << DescribeValue(v15) << "\n";
    }
}

TYPED_TEST(SplineFixture, hat_is_continuous) {
    auto result = CubicSplineInterpolation(this->hat3, 3);
    bool ok = result.IsContinuous();
    EXPECT_TRUE(ok);
    if (testing::Test::HasFailure()) {
        cerr << "\nТестируем IsContinuous для сплайна из точек (0,0), (1,1), (2,0)\n";
        cerr << "Ожидаемый результат: 1\n";
        cerr << "Полученный результат: " << ok << "\n";
    }
}

TYPED_TEST(SplineFixture, hat_derivative_continuous_at_interior_node) {
    using T = TypeParam;
    auto result = CubicSplineInterpolation(this->hat3, 3);
    Fragment<T> left = result.GetFragment(0);
    Fragment<T> right = result.GetFragment(1);
    T dLeft = left.DerivativeAt(1);
    T dRight = right.DerivativeAt(1);
    bool ok = Magnitude(dLeft) < kEps && Magnitude(dRight) < kEps
        && Magnitude(dLeft - dRight) < kEps;
    EXPECT_TRUE(ok);
    if (testing::Test::HasFailure()) {
        cerr << "\nТестируем непрерывность первой производной во внутреннем узле x=1\n";
        cerr << "По симметрии данных в x=1 находится экстремум, производная должна быть 0\n";
        cerr << "Ожидаемые значения: f'(1-)=0, f'(1+)=0\n";
        cerr << "Полученные значения: f'(1-)=" << DescribeValue(dLeft) << ", f'(1+)=" << DescribeValue(dRight) << "\n";
    }
}

TYPED_TEST(SplineFixture, uneven_grid_interpolates_nodes) {
    using T = TypeParam;
    auto result = CubicSplineInterpolation(this->uneven3, 3);
    Fragment<T> left = result.GetFragment(0);
    Fragment<T> right = result.GetFragment(1);
    T fBegin = left.Estimation(-1);
    T fMidLeft = left.Estimation(1);
    T fMidRight = right.Estimation(1);
    T fEnd = right.Estimation(4);
    bool ok = Magnitude(fBegin - MakeValue<T>(2)) < kEps && Magnitude(fMidLeft - MakeValue<T>(0)) < kEps
        && Magnitude(fMidRight - MakeValue<T>(0)) < kEps && Magnitude(fEnd - MakeValue<T>(3)) < kEps;
    EXPECT_TRUE(ok);
    if (testing::Test::HasFailure()) {
        cerr << "\nТестируем прохождение сплайна через узлы на неравномерной сетке\n";
        cerr << "Точки: (-1,2), (1,0), (4,3)\n";
        cerr << "Ожидаемые значения: f(-1)=2, f(1)=0, f(4)=3\n";
        cerr << "Полученные значения: f(-1)=" << DescribeValue(fBegin) << ", f(1-)=" << DescribeValue(fMidLeft)
            << ", f(1+)=" << DescribeValue(fMidRight) << ", f(4)=" << DescribeValue(fEnd) << "\n";
    }
}

TYPED_TEST(SplineFixture, uneven_grid_values_between_nodes) {
    using T = TypeParam;
    auto result = CubicSplineInterpolation(this->uneven3, 3);
    Fragment<T> left = result.GetFragment(0);
    Fragment<T> right = result.GetFragment(1);
    T vNeg05 = left.Estimation(-0.5);
    T v0 = left.Estimation(0);
    T v25 = right.Estimation(2.5);
    T v3 = right.Estimation(3);
    bool ok = Magnitude(vNeg05 - MakeValue<T>(1.3125)) < kEps
        && Magnitude(v0 - MakeValue<T>(0.7)) < kEps
        && Magnitude(v25 - MakeValue<T>(0.825)) < kEps
        && Magnitude(v3 - MakeValue<T>(22.0 / 15.0)) < kEps;
    EXPECT_TRUE(ok);
    if (testing::Test::HasFailure()) {
        cerr << "\nТестируем промежуточные значения сплайна на неравномерной сетке\n";
        cerr << "Ожидаемые значения: f(-0.5)=1.3125, f(0)=0.7, f(2.5)=0.825, f(3)=22/15\n";
        cerr << "Полученные значения: f(-0.5)=" << DescribeValue(vNeg05) << ", f(0)=" << DescribeValue(v0)
            << ", f(2.5)=" << DescribeValue(v25) << ", f(3)=" << DescribeValue(v3) << "\n";
    }
}

TYPED_TEST(SplineFixture, uneven_grid_derivative_continuous_at_interior_node) {
    using T = TypeParam;
    auto result = CubicSplineInterpolation(this->uneven3, 3);
    Fragment<T> left = result.GetFragment(0);
    Fragment<T> right = result.GetFragment(1);
    T dLeft = left.DerivativeAt(1);
    T dRight = right.DerivativeAt(1);
    bool ok = Magnitude(dLeft - dRight) < kEps;
    EXPECT_TRUE(ok);
    if (testing::Test::HasFailure()) {
        cerr << "\nТестируем непрерывность первой производной во внутреннем узле x=1 "
            << "на неравномерной сетке\n";
        cerr << "Ожидаемое поведение: f'(1-) == f'(1+)\n";
        cerr << "Полученные значения: f'(1-)=" << DescribeValue(dLeft) << ", f'(1+)=" << DescribeValue(dRight) << "\n";
    }
}

TYPED_TEST(SplineFixture, uneven_grid_is_continuous) {
    auto result = CubicSplineInterpolation(this->uneven3, 3);
    bool ok = result.IsContinuous();
    EXPECT_TRUE(ok);
    if (testing::Test::HasFailure()) {
        cerr << "\nТестируем IsContinuous для сплайна на неравномерной сетке\n";
        cerr << "Ожидаемый результат: 1\n";
        cerr << "Полученный результат: " << ok << "\n";
    }
}

TYPED_TEST(SplineFixture, four_points_fragment_count) {
    auto result = CubicSplineInterpolation(this->general4, 4);
    size_t count = result.CountFragments();
    EXPECT_TRUE(count == 3);
    if (testing::Test::HasFailure()) {
        cerr << "\nТестируем количество фрагментов при n == 4\n";
        cerr << "Ожидаемое количество фрагментов: 3\n";
        cerr << "Полученное количество фрагментов: " << count << "\n";
    }
}

TYPED_TEST(SplineFixture, four_points_interpolates_all_nodes) {
    using T = TypeParam;
    auto result = CubicSplineInterpolation(this->general4, 4);
    bool ok = true;
    for (size_t i = 0; i < 4; ++i) {
        double x = this->general4[i].x;
        T expected = this->general4[i].y;
        size_t fragIndex = (i == 3) ? 2 : i;
        Fragment<T> frag = result.GetFragment(fragIndex);
        T got = frag.Estimation(x);
        if (Magnitude(got - expected) >= kEps) {
            ok = false;
            cerr << "\nУзел (" << x << ", " << DescribeValue(expected) << ") не совпал: получено "
                << DescribeValue(got) << "\n";
        }
    }
    EXPECT_TRUE(ok);
    if (testing::Test::HasFailure()) {
        cerr << "\nТестируем прохождение сплайна через все узлы при n == 4\n";
        cerr << "Точки: (0,0), (1,1), (2,0), (3,1)\n";
    }
}

TYPED_TEST(SplineFixture, four_points_is_continuous) {
    auto result = CubicSplineInterpolation(this->general4, 4);
    bool ok = result.IsContinuous();
    EXPECT_TRUE(ok);
    if (testing::Test::HasFailure()) {
        cerr << "\nТестируем IsContinuous для сплайна при n == 4\n";
        cerr << "Ожидаемый результат: 1\n";
        cerr << "Полученный результат: " << ok << "\n";
    }
}

TYPED_TEST(SplineFixture, four_points_derivative_continuous_at_interior_nodes) {
    using T = TypeParam;
    auto result = CubicSplineInterpolation(this->general4, 4);
    Fragment<T> f0 = result.GetFragment(0);
    Fragment<T> f1 = result.GetFragment(1);
    Fragment<T> f2 = result.GetFragment(2);
    T dAt1Left = f0.DerivativeAt(1);
    T dAt1Right = f1.DerivativeAt(1);
    T dAt2Left = f1.DerivativeAt(2);
    T dAt2Right = f2.DerivativeAt(2);
    bool ok = Magnitude(dAt1Left - dAt1Right) < kEps
        && Magnitude(dAt2Left - dAt2Right) < kEps;
    EXPECT_TRUE(ok);
    if (testing::Test::HasFailure()) {
        cerr << "\nТестируем непрерывность первой производной во внутренних узлах x=1 и x=2 "
            << "(n == 4, две внутренние неизвестные M1, M2)\n";
        cerr << "Ожидаемое поведение: f'(1-)==f'(1+) и f'(2-)==f'(2+)\n";
        cerr << "Полученные значения: f'(1-)=" << DescribeValue(dAt1Left) << ", f'(1+)=" << DescribeValue(dAt1Right)
            << ", f'(2-)=" << DescribeValue(dAt2Left) << ", f'(2+)=" << DescribeValue(dAt2Right) << "\n";
    }
}

TYPED_TEST(SplineFixture, five_points_fragment_count) {
    auto result = CubicSplineInterpolation(this->general5, 5);
    size_t count = result.CountFragments();
    EXPECT_TRUE(count == 4);
    if (testing::Test::HasFailure()) {
        cerr << "\nТестируем количество фрагментов при n == 5\n";
        cerr << "Ожидаемое количество фрагментов: 4\n";
        cerr << "Полученное количество фрагментов: " << count << "\n";
    }
}

TYPED_TEST(SplineFixture, five_points_interpolates_all_nodes) {
    using T = TypeParam;
    auto result = CubicSplineInterpolation(this->general5, 5);
    bool ok = true;
    for (size_t i = 0; i < 5; ++i) {
        double x = this->general5[i].x;
        T expected = this->general5[i].y;
        size_t fragIndex = (i == 4) ? 3 : i;
        Fragment<T> frag = result.GetFragment(fragIndex);
        T got = frag.Estimation(x);
        if (Magnitude(got - expected) >= kEps) {
            ok = false;
            cerr << "\nУзел (" << x << ", " << DescribeValue(expected) << ") не совпал: получено "
                << DescribeValue(got) << "\n";
        }
    }
    EXPECT_TRUE(ok);
    if (testing::Test::HasFailure()) {
        cerr << "\nТестируем прохождение сплайна через все узлы при n == 5\n";
        cerr << "Точки: (0,1), (1,3), (2,2), (3,5), (4,4)\n";
    }
}

TYPED_TEST(SplineFixture, five_points_is_continuous) {
    auto result = CubicSplineInterpolation(this->general5, 5);
    bool ok = result.IsContinuous();
    EXPECT_TRUE(ok);
    if (testing::Test::HasFailure()) {
        cerr << "\nТестируем IsContinuous для сплайна при n == 5\n";
        cerr << "Ожидаемый результат: 1\n";
        cerr << "Полученный результат: " << ok << "\n";
    }
}

TYPED_TEST(SplineFixture, integration_is_monotonic_for_increasing_line) {
    using T = TypeParam;
    if constexpr (std::is_same_v<T, double>) {
        auto result = CubicSplineInterpolation(this->linear2, 2);
        bool ok = result.IsMonotonic();
        EXPECT_TRUE(ok);
        if (testing::Test::HasFailure()) {
            cerr << "\nТестируем IsMonotonic для сплайна, построенного по возрастающей прямой\n";
            cerr << "Ожидаемый результат: 1\n";
            cerr << "Полученный результат: " << ok << "\n";
        }
    }
}

TYPED_TEST(SplineFixture, spline_does_not_retain_points_pointer) {
    using T = TypeParam;
    using Node = SplineNode<T>;
    PieceWiseFunction<T> result;
    {
        Node temporary[3] = {{0, MakeValue<T>(0)}, {1, MakeValue<T>(1)}, {2, MakeValue<T>(0)}};
        result = CubicSplineInterpolation(temporary, 3);
    }
    Fragment<T> left = result.GetFragment(0);
    T v05 = left.Estimation(0.5);
    T expected = MakeValue<T>(0.6875);
    EXPECT_TRUE(Magnitude(v05 - expected) < kEps);
    if (testing::Test::HasFailure()) {
        cerr << "\nТестируем, что сплайн не хранит указатель на переданный массив точек\n";
        cerr << "Ожидаемое значение f(0.5) после выхода массива из области видимости: 0.6875\n";
        cerr << "Полученное значение: " << DescribeValue(v05) << "\n";
    }
}