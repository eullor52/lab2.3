#include <gtest/gtest.h>
#include "fixtures.hpp"
#include "../headers/piecewisefunction.hpp"
#include "../headers/complex.hpp"

using std::cerr;

// ========== Дополнительные фикстуры для double и Complex ==========

class FragmentFixture : public testing::Test {
protected:
    Complex arr_complex[3] = {Complex(1,0), Complex(0,1), Complex(2,2)};
    MutableArraySequence<Complex>* coeffs_complex;
    Fragment<Complex>* frag_complex;
    double begin_complex = 0.0;
    double end_complex = 1.5;
    double arr_double[4] = {1.5, -2.0, 3.5, 0.5};
    MutableArraySequence<double>* coeffs_double;
    Fragment<double>* frag_double;
    double begin_double = -1.0;
    double end_double = 2.0;

    void SetUp() override {
        coeffs_double = new MutableArraySequence<double>(arr_double, 4);
        frag_double = new Fragment<double>(begin_double, end_double, coeffs_double);
        coeffs_complex = new MutableArraySequence<Complex>(arr_complex, 3);
        frag_complex = new Fragment<Complex>(begin_complex, end_complex, coeffs_complex);
    }

    void TearDown() override {
        delete frag_double;
        delete coeffs_double;
        delete frag_complex;
        delete coeffs_complex;
    }
};

// ========== Тесты для Fragment<int> (используется FragmentFixture) ==========

TEST_F(FragmentFixture, default_constructor) {
    Fragment<int> def_frag;
    EXPECT_EQ(def_frag.GetBegin(), 0);
    EXPECT_EQ(def_frag.GetEnd(), 0);
    if (testing::Test::HasFailure()) {
        cerr << "Тестируем конструктор по умолчанию\n";
        cerr << "Ожидаемое значение begin: 0\n";
        cerr << "Полученное значение begin: " << def_frag.GetBegin() << "\n";
        cerr << "Ожидаемое значение end: 0\n";
        cerr << "Полученное значение end: " << def_frag.GetEnd() << "\n";
    }
}

TEST_F(FragmentFixture, parameters_constructor) {
    EXPECT_EQ(frag_double->GetBegin(), begin_double);
    EXPECT_EQ(frag_double->GetEnd(), end_double);
    if (testing::Test::HasFailure()) {
        cerr << "Тестируем конструктор с параметрами\n";
        cerr << "Ожидаемое значение begin: " << begin_double << "\n";
        cerr << "Полученное значение begin: " << frag_double->GetBegin() << "\n";
        cerr << "Ожидаемое значение end: " << end_double << "\n";
        cerr << "Полученное значение end: " << frag_double->GetEnd() << "\n";
    }
}

TEST_F(FragmentFixture, get_end_begin) {
    EXPECT_EQ(frag_double->GetBegin(), begin_double);
    EXPECT_EQ(frag_double->GetEnd(), end_double);
    if (testing::Test::HasFailure()) {
        cerr << "Тестируем GetEnd/GetBegin\n";
        cerr << "Ожидаемое значение begin: " << begin_double << "\n";
        cerr << "Полученное значение begin: " << frag_double->GetBegin() << "\n";
        cerr << "Ожидаемое значение end: " << end_double << "\n";
        cerr << "Полученное значение end: " << frag_double->GetEnd() << "\n";
    }
}

TEST_F(FragmentFixture, copy_constructor) {
    Fragment<double> copy(*frag_double);
    EXPECT_EQ(copy.GetBegin(), frag_double->GetBegin());
    EXPECT_EQ(copy.GetEnd(), frag->GetEnd());
    EXPECT_DOUBLE_EQ(copy.Estimation(1.5), frag_double->Estimation(1.5));
    if (testing::Test::HasFailure()) {
        cerr << "Тестируем конструктор копирования\n";
        cerr << "Ожидалемое значение begin: " << begin_double << "\n";
        cerr << "Ожидаемое значение end: " << end_double << "\n";
        cerr << "Полученое значение begin: " << copy.GetBegin()  << "\n";
        cerr << "Полученное значение end: " << copy.GetEnd() << "\n";
    }
}

TEST_F(FragmentFixture, assignment_operator) {
    Fragment<double> other;
    other = *frag_double;
    EXPECT_EQ(other.GetBegin(), frag_double->GetBegin());
    EXPECT_EQ(other.GetEnd(), frag_double->GetEnd());
    EXPECT_DOUBLE_EQ(other.Estimation(2.0), frag_double->Estimation(2.0));
    if (testing::Test::HasFailure()) {
        cerr << "Тестируем оператор присваивания\n";
        cerr << "Ожидалемое значение begin: " << begin_double << "\n";
        cerr << "Ожидаемое значение end: " << end_double << "\n";
        cerr << "Полученое значение begin: " << other.GetBegin()  << "\n";
        cerr << "Полученное значение end: " << other.GetEnd() << "\n";
    }
}

TEST_F(FragmentFixture, isIntersecting_true_overlap) {
    Fragment<int> other(2.0, 3.0, coeffs);
    EXPECT_TRUE(frag->isIntersecting(other));
    if (testing::Test::HasFailure()) {
        cerr << "Тестируем isIntersecting для пересекающихся интервалов [1,2] и [2,3]\n";
        cerr << "Ожидалось true\n";
        cerr << "Получено false\n";
    }
}

TEST_F(FragmentFixture, isIntersecting_false_disjoint) {
    Fragment<int> other(3.0, 4.0, coeffs);
    EXPECT_FALSE(frag->isIntersecting(other));
    if (testing::Test::HasFailure()) {
        cerr << "Тестируем isIntersecting для непересекающихся интервалов [1,2] и [3,4]\n";
        cerr << "Ожидалось false\n";
        cerr << "Получено true\n";
    }
}

TEST_F(FragmentFixture, isIntersecting_touch_at_boundary) {
    Fragment<int> other(2.0, 3.0, coeffs);
    EXPECT_FALSE(frag->isIntersecting(other));
    if (testing::Test::HasFailure()) {
        cerr << "Тестируем isIntersecting для касающихся интервалов [1,2] и [2,3]\n";
        cerr << "Ожидалось false (касание не считается пересечением)\n";
        cerr << "Получено true\n";
    }
}

TEST_F(FragmentFixture, isIntersecting_one_inside_other) {
    Fragment<int> other(1.25, 1.75, coeffs);
    EXPECT_TRUE(frag->isIntersecting(other));
    if (testing::Test::HasFailure()) {
        cerr << "Тестируем isIntersecting, когда один интервал внутри другого\n";
        cerr << "Ожидалось true\n";
        cerr << "Получено false\n";
    }
}

TEST_F(FragmentFixture, pruning_changes_bounds) {
    frag->Pruning(2.5, 4.5);
    EXPECT_EQ(frag->GetBegin(), 2.5);
    EXPECT_EQ(frag->GetEnd(), 4.5);
    if (testing::Test::HasFailure()) {
        cerr << "Тестируем Pruning\n";
        cerr << "Ожидались новые границы 2.5 и 4.5\n";
        cerr << "Получено: begin = " << frag->GetBegin() 
            << ", end = " << frag->GetEnd() << "\n";
    }
}

TEST_F(FragmentFixture, pruning_throws_on_invalid_bounds) {
    EXPECT_THROW(frag->Pruning(3.0, 2.0), LogicErrorException);
    if (testing::Test::HasFailure()) {
        cerr << "Тестируем Pruning с некорректными границами (newBeg >= newEnd)\n";
        cerr << "Ожидалось исключение LogicErrorException\n";
        cerr << "Исключение не было выброшено\n";
    }
}

TEST_F(FragmentFixture, redefinition_replaces_coefficients) {
    int newArr[3] = {5, 6, 7};
    MutableArraySequence<int>* newCoeffs = new MutableArraySequence<int>(newArr, 3);
    frag->Redefinition(newCoeffs);
    EXPECT_DOUBLE_EQ(frag->Estimation(1.0), 18.0);
    delete newCoeffs;
    if (testing::Test::HasFailure()) {
        cerr << "Тестируем Redefinition\n";
        cerr << "Ожидалось новое значение полинома 18.0 при point=1\n";
        cerr << "Получено: " << frag->Estimation(1.0) << "\n";
    }
}

TEST_F(FragmentFixture, redefinition_throws_on_null) {
    EXPECT_THROW(frag->Redefinition(nullptr), LogicErrorException);
    if (testing::Test::HasFailure()) {
        cerr << "Тестируем Redefinition с nullptr\n";
        cerr << "Ожидалось исключение LogicErrorException\n";
        cerr << "Исключение не было выброшено\n";
    }
}

TEST_F(FragmentFixture, is_interior_point_true) {
    EXPECT_TRUE(frag->IsInteriorPoint(1.5));
    if (testing::Test::HasFailure()) {
        cerr << "Тестируем IsInteriorPoint для точки внутри интервала\n";
        cerr << "Ожидалось true\n";
        cerr << "Получено false\n";
    }
}

TEST_F(FragmentFixture, is_interior_point_false_on_boundary) {
    EXPECT_FALSE(frag->IsInteriorPoint(begin));
    EXPECT_FALSE(frag->IsInteriorPoint(end));
    if (testing::Test::HasFailure()) {
        cerr << "Тестируем IsInteriorPoint для граничных точек\n";
        cerr << "Ожидалось false для begin и end\n";
        cerr << "Получено: begin = " << frag->IsInteriorPoint(begin) 
            << ", end = " << frag->IsInteriorPoint(end) << "\n";
    }
}

TEST_F(FragmentFixture, estimation_int) {
    EXPECT_DOUBLE_EQ(frag->Estimation(2.0), 1 + 2*2 + 3*4 + 4*8);
    EXPECT_DOUBLE_EQ(frag->Estimation(0.0), 1.0);
    if (testing::Test::HasFailure()) {
        cerr << "Тестируем Estimation для int\n";
        cerr << "Ожидалось значение 49.0 при x=2\n";
        cerr << "Получено: " << frag->Estimation(2.0) << "\n";
    }
}

TEST_F(FragmentFixture, derivative_at_int) {
    EXPECT_DOUBLE_EQ(frag->DerivativeAt(2.0), 2 + 6*2 + 12*4);
    EXPECT_DOUBLE_EQ(frag->DerivativeAt(0.0), 2.0);
    if (testing::Test::HasFailure()) {
        cerr << "Тестируем DerivativeAt для int\n";
        cerr << "Ожидалось значение 62.0 при x=2\n";
        cerr << "Получено: " << frag->DerivativeAt(2.0) << "\n";
    }
}

TEST_F(DoubleFragmentFixture, estimation_double) {
    double x = 1.0;
    double expected = 1.5 - 2.0*1 + 3.5*1 + 0.5*1;
    EXPECT_NEAR(frag->Estimation(x), expected, 1e-9);
    if (testing::Test::HasFailure()) {
        cerr << "Тестируем Estimation для double\n";
        cerr << "Ожидалось значение " << expected << " при x=" << x << "\n";
        cerr << "Получено: " << frag->Estimation(x) << "\n";
    }
}

TEST_F(DoubleFragmentFixture, derivative_at_double) {
    double x = 2.0;
    double expected = -2 + 7*2 + 1.5*4; // -2+14+6=18
    EXPECT_NEAR(frag->DerivativeAt(x), expected, 1e-9);
    if (testing::Test::HasFailure()) {
        cerr << "Тестируем DerivativeAt для double\n";
        cerr << "Ожидалось значение " << expected << " при x=" << x << "\n";
        cerr << "Получено: " << frag->DerivativeAt(x) << "\n";
    }
}

TEST_F(ComplexFragmentFixture, estimation_complex) {
    double x = 1.0;
    Complex expected(1 + 0*1 + 2*1, 0 + 1*1 + 2*1); // (3, 3)
    Complex result = frag->Estimation(x);
    EXPECT_DOUBLE_EQ(result.GetReal(), expected.GetReal());
    EXPECT_DOUBLE_EQ(result.GetImag(), expected.GetImag());
    if (testing::Test::HasFailure()) {
        cerr << "Тестируем Estimation для Complex\n";
        cerr << "Ожидалось значение (3,3) при x=1\n";
        cerr << "Получено: (" << result.GetReal() << ", " << result.GetImag() << ")\n";
    }
}

TEST_F(ComplexFragmentFixture, derivative_at_complex) {
    // Производная: (0+1i) + 2*(2+2i)x = (0+1i) + (4+4i)x
    double x = 0.5;
    Complex expected(0 + 4*0.5, 1 + 4*0.5); // (2, 3)
    Complex result = frag->DerivativeAt(x);
    EXPECT_DOUBLE_EQ(result.GetReal(), expected.GetReal());
    EXPECT_DOUBLE_EQ(result.GetImag(), expected.GetImag());
    if (testing::Test::HasFailure()) {
        cerr << "Тестируем DerivativeAt для Complex\n";
        cerr << "Ожидалось значение (2,3) при x=0.5\n";
        cerr << "Получено: (" << result.GetReal() << ", " << result.GetImag() << ")\n";
    }
}


TEST_F(FragmentConstructorTest, throws_when_beg_ge_end) {
    int arr[1] = {1};
    MutableArraySequence<int>* coeffs = new MutableArraySequence<int>(arr, 1);
    EXPECT_THROW(Fragment<int>(2.0, 1.0, coeffs), LogicErrorException);
    EXPECT_THROW(Fragment<int>(1.0, 1.0, coeffs), LogicErrorException);
    delete coeffs;
    if (testing::Test::HasFailure()) {
        cerr << "Тестируем конструктор Fragment с begin >= end\n";
        cerr << "Ожидалось исключение LogicErrorException\n";
        cerr << "Исключение не было выброшено\n";
    }
}

TEST_F(FragmentConstructorTest, throws_when_null_sequence) {
    EXPECT_THROW(Fragment<int>(1.0, 2.0, nullptr), LogicErrorException);
    if (testing::Test::HasFailure()) {
        cerr << "Тестируем конструктор Fragment с nullptr последовательностью\n";
        cerr << "Ожидалось исключение LogicErrorException\n";
        cerr << "Исключение не было выброшено\n";
    }
}