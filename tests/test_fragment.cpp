#include "../headers/fragment.hpp"
#include "../headers/arraysequence.hpp"
#include "../headers/exceptions.h"
#include "fixtures.hpp"
#include <iostream>

using std::cerr;

TEST_F(FragmentFixture, constructor_valid) {
    EXPECT_TRUE(frag->GetBegin() == begin && frag->GetEnd() == end);
    if (testing::Test::HasFailure()) {
        cerr << "\nТестируем конструктор с параметрами\n";
        cerr << "Ожидаемый отрезок: [1, 2]\n";
        cerr << "Полученный отрезок: [" << frag->GetBegin() << ", " << frag->GetEnd() << "]\n";
    }
}

TEST_F(FragmentFixture, constructor_nullptr_throws) {
    bool thrown = false;
    try {
        Fragment<int> bad(begin, end, nullptr);
    } catch (const LogicErrorException&) {
        thrown = true;
    }
    EXPECT_TRUE(thrown);
    if (testing::Test::HasFailure()) {
        cerr << "\nТестируем конструктор с nullptr вместо коэффициентов\n";
        cerr << "Ожидаемое поведение: выброс LogicErrorException\n";
        cerr << "Полученное поведение: исключение " 
            << (thrown ? "выброшено" : "не выброшено") << "\n";
    }
}

TEST_F(FragmentFixture, constructor_begin_greater_than_end_throws) {
    bool thrown = false;
    try {
        Fragment<int> bad(end, begin, coeffsInt);
    } catch (const LogicErrorException&) {
        thrown = true;
    }
    EXPECT_TRUE(thrown);
    if (testing::Test::HasFailure()) {
        cerr << "\nТестируем конструктор при begin > end\n";
        cerr << "Ожидаемое поведение: выброс LogicErrorException\n";
        cerr << "Полученное поведение: исключение " 
            << (thrown ? "выброшено" : "не выброшено") << "\n";
    }
}

TEST_F(FragmentFixture, constructor_begin_equal_end_throws) {
    bool thrown = false;
    try {
        Fragment<int> bad(begin, begin, coeffsInt);
    } catch (const LogicErrorException&) {
        thrown = true;
    }
    EXPECT_TRUE(thrown);
    if (testing::Test::HasFailure()) {
        cerr << "\nТестируем конструктор при begin == end\n";
        cerr << "Ожидаемое поведение: выброс LogicErrorException\n";
        cerr << "Полученное поведение: исключение " 
            << (thrown ? "выброшено" : "не выброшено") << "\n";
    }
}

TEST_F(FragmentFixture, default_constructor) {
    Fragment<int> empty;
    EXPECT_TRUE(empty.GetBegin() == 0 && empty.GetEnd() == 0);
    if (testing::Test::HasFailure()) {
        cerr << "\nТестируем конструктор по умолчанию\n";
        cerr << "Ожидаемый отрезок: [0, 0]\n";
        cerr << "Полученный отрезок: [" << empty.GetBegin() << ", " << empty.GetEnd() << "]\n";
    }
}

TEST_F(FragmentFixture, get_begin) {
    EXPECT_TRUE(frag->GetBegin() == 1);
    if (testing::Test::HasFailure()) {
        cerr << "\nТестируем метод GetBegin\n";
        cerr << "Ожидаемое значение: 1\n";
        cerr << "Полученное значение: " << frag->GetBegin() << "\n";
    }
}

TEST_F(FragmentFixture, get_end) {
    EXPECT_TRUE(frag->GetEnd() == 2);
    if (testing::Test::HasFailure()) {
        cerr << "\nТестируем метод GetEnd\n";
        cerr << "Ожидаемое значение: 2\n";
        cerr << "Полученное значение: " << frag->GetEnd() << "\n";
    }
}

TEST_F(FragmentFixture, copy_constructor_deep_copy) {
    Fragment<int> copy(*frag);
    EXPECT_TRUE(copy.GetBegin() == frag->GetBegin()
        && copy.GetEnd() == frag->GetEnd()
        && copy.Estimation(1) == frag->Estimation(1));
    if (testing::Test::HasFailure()) {
        cerr << "\nТестируем конструктор копирования (непустые коэффициенты)\n";
        cerr << "Ожидаемый отрезок: [" << frag->GetBegin() << ", " << frag->GetEnd() << "]\n";
        cerr << "Полученный отрезок: [" << copy.GetBegin() << ", " << copy.GetEnd() << "]\n";
    }
}

TEST_F(FragmentFixture, copy_constructor_independent_from_original) {
    Fragment<int> copy(*frag);
    copy.Pruning(5, 10);
    EXPECT_TRUE(copy.GetBegin() == 5 && frag->GetBegin() == 1);
    if (testing::Test::HasFailure()) {
        cerr << "\nТестируем независимость копии от оригинала после конструктора копирования\n";
        cerr << "Ожидаемые начала отрезков: копия 5, оригинал 1\n";
        cerr << "Полученные начала отрезков: копия " 
            << copy.GetBegin() << ", оригинал " << frag->GetBegin() << "\n";
    }
}

TEST_F(FragmentFixture, copy_constructor_from_empty) {
    Fragment<int> empty;
    Fragment<int> copy(empty);
    EXPECT_TRUE(copy.GetBegin() == 0 && copy.GetEnd() == 0 && copy.Estimation(1) == 0);
    if (testing::Test::HasFailure()) {
        cerr << "\nТестируем конструктор копирования от "
            << "фрагмента с пустыми (nullptr) коэффициентами\n";
        cerr << "Ожидаемый результат Estimation: 0\n";
        cerr << "Полученный результат: " << copy.Estimation(1) << "\n";
    }
}

TEST_F(FragmentFixture, copy_operator_deep_copy) {
    Fragment<int> copy;
    copy = *frag;
    EXPECT_TRUE(copy.GetBegin() == frag->GetBegin()
        && copy.GetEnd() == frag->GetEnd()
        && copy.Estimation(1) == frag->Estimation(1));
    if (testing::Test::HasFailure()) {
        cerr << "\nТестируем оператор присваивания копированием\n";
        cerr << "Ожидаемый отрезок: [" 
            << frag->GetBegin() << ", " << frag->GetEnd() << "]\n";
        cerr << "Полученный отрезок: [" 
            << copy.GetBegin() << ", " << copy.GetEnd() << "]\n";
    }
}

TEST_F(FragmentFixture, copy_operator_independent_from_original) {
    Fragment<int> copy;
    copy = *frag;
    copy.Pruning(5, 10);
    EXPECT_TRUE(copy.GetBegin() == 5 && frag->GetBegin() == 1);
    if (testing::Test::HasFailure()) {
        cerr << "\nТестируем независимость копии от оригинала после оператора присваивания\n";
        cerr << "Ожидаемые начала отрезков: копия 5, оригинал 1\n";
        cerr << "Полученные начала отрезков: копия " 
            << copy.GetBegin() << ", оригинал " << frag->GetBegin() << "\n";
    }
}

TEST_F(FragmentFixture, copy_operator_from_empty) {
    Fragment<int> empty;
    *frag = empty;
    EXPECT_TRUE(frag->GetBegin() == 0 && frag->GetEnd() == 0 && frag->Estimation(1) == 0);
    if (testing::Test::HasFailure()) {
        cerr << "\nТестируем присваивание фрагмента с пустыми (nullptr) коэффициентами\n";
        cerr << "Ожидаемый отрезок: [0, 0], Estimation: 0\n";
        cerr << "Полученный отрезок: [" << frag->GetBegin() << ", " << frag->GetEnd()
             << "], Estimation: " << frag->Estimation(1) << "\n";
    }
}

TEST_F(FragmentFixture, copy_operator_self_assignment) {
    double expectedBegin = frag->GetBegin();
    double expectedEnd = frag->GetEnd();
    double expectedEstimation = frag->Estimation(1);
    *frag = *frag;
    EXPECT_TRUE(frag->GetBegin() == expectedBegin
        && frag->GetEnd() == expectedEnd
        && frag->Estimation(1) == expectedEstimation);
    if (testing::Test::HasFailure()) {
        cerr << "\nТестируем присваивание фрагмента самому себе\n";
        cerr << "Ожидаемый отрезок: [" 
            << expectedBegin << ", " << expectedEnd << "]\n";
        cerr << "Полученный отрезок: [" 
            << frag->GetBegin() << ", " << frag->GetEnd() << "]\n";
    }
}

TEST_F(FragmentFixture, is_interior_point_inside) {
    bool result = frag->IsInteriorPoint(1.5);
    EXPECT_TRUE(result);
    if (testing::Test::HasFailure()) {
        cerr << "\nТестируем метод IsInteriorPoint для точки внутри отрезка\n";
        cerr << "Точка: 1.5, отрезок: [1, 2]\n";
        cerr << "Ожидаемый результат: 1\n";
        cerr << "Полученный результат: " << result << "\n";
    }
}

TEST_F(FragmentFixture, is_interior_point_on_begin_border) {
    bool result = frag->IsInteriorPoint(1);
    EXPECT_TRUE(result);
    if (testing::Test::HasFailure()) {
        cerr << "\nТестируем метод IsInteriorPoint для точки на левой границе\n";
        cerr << "Точка: 1, отрезок: [1, 2]\n";
        cerr << "Ожидаемый результат: 1\n";
        cerr << "Полученный результат: " << result << "\n";
    }
}

TEST_F(FragmentFixture, is_interior_point_on_end_border) {
    bool result = frag->IsInteriorPoint(2);
    EXPECT_TRUE(result);
    if (testing::Test::HasFailure()) {
        cerr << "\nТестируем метод IsInteriorPoint для точки на правой границе\n";
        cerr << "Точка: 2, отрезок: [1, 2]\n";
        cerr << "Ожидаемый результат: 1\n";
        cerr << "Полученный результат: " << result << "\n";
    }
}

TEST_F(FragmentFixture, is_interior_point_outside) {
    bool result = frag->IsInteriorPoint(10);
    EXPECT_TRUE(!result);
    if (testing::Test::HasFailure()) {
        cerr << "\nТестируем метод IsInteriorPoint для точки далеко за пределами отрезка\n";
        cerr << "Точка: 10, отрезок: [1, 2]\n";
        cerr << "Ожидаемый результат: 0\n";
        cerr << "Полученный результат: " << result << "\n";
    }
}

TEST_F(FragmentFixture, is_intersecting_true) {
    int arr2[2] = {5, 6};
    MutableArraySequence<int> otherCoeffs(arr2, 2);
    Fragment<int> other(1.5, 3, &otherCoeffs);
    bool result = frag->isIntersecting(other);
    EXPECT_TRUE(result);
    if (testing::Test::HasFailure()) {
        cerr << "\nТестируем метод isIntersecting для пересекающихся отрезков\n";
        cerr << "Первый отрезок: [1, 2], второй отрезок: [1.5, 3]\n";
        cerr << "Ожидаемый результат: 1\n";
        cerr << "Полученный результат: " << result << "\n";
    }
}

TEST_F(FragmentFixture, is_intersecting_false) {
    int arr2[2] = {5, 6};
    MutableArraySequence<int> otherCoeffs(arr2, 2);
    Fragment<int> other(3, 4, &otherCoeffs);
    bool result = frag->isIntersecting(other);
    EXPECT_TRUE(!result);
    if (testing::Test::HasFailure()) {
        cerr << "\nТестируем метод isIntersecting для непересекающихся отрезков\n";
        cerr << "Первый отрезок: [1, 2], второй отрезок: [3, 4]\n";
        cerr << "Ожидаемый результат: 0\n";
        cerr << "Полученный результат: " << result << "\n";
    }
}

TEST_F(FragmentFixture, is_intersecting_touching_border_false) {
    int arr2[2] = {5, 6};
    MutableArraySequence<int> otherCoeffs(arr2, 2);
    Fragment<int> other(2, 3, &otherCoeffs);
    bool result = frag->isIntersecting(other);
    EXPECT_TRUE(!result);
    if (testing::Test::HasFailure()) {
        cerr << "\nТестируем метод isIntersecting для отрезков, соприкасающихся границей\n";
        cerr << "Первый отрезок: [1, 2], второй отрезок: [2, 3]\n";
        cerr << "Ожидаемый результат: 0 (граница не считается пересечением)\n";
        cerr << "Полученный результат: " << result << "\n";
    }
}

TEST_F(FragmentFixture, is_intersecting_one_inside_another) {
    int arr2[2] = {5, 6};
    MutableArraySequence<int> otherCoeffs(arr2, 2);
    Fragment<int> other(0, 5, &otherCoeffs);
    bool result = frag->isIntersecting(other);
    EXPECT_TRUE(result);
    if (testing::Test::HasFailure()) {
        cerr << "\nТестируем метод isIntersecting когда один отрезок целиком внутри другого\n";
        cerr << "Первый отрезок: [1, 2], второй отрезок: [0, 5]\n";
        cerr << "Ожидаемый результат: 1\n";
        cerr << "Полученный результат: " << result << "\n";
    }
}

TEST_F(FragmentFixture, pruning_valid) {
    frag->Pruning(0, 5);
    EXPECT_TRUE(frag->GetBegin() == 0 && frag->GetEnd() == 5);
    if (testing::Test::HasFailure()) {
        cerr << "\nТестируем метод Pruning с корректными границами\n";
        cerr << "Ожидаемый отрезок: [0, 5]\n";
        cerr << "Полученный отрезок: [" 
            << frag->GetBegin() << ", " << frag->GetEnd() << "]\n";
    }
}

TEST_F(FragmentFixture, pruning_begin_greater_than_end_throws) {
    bool thrown = false;
    try {
        frag->Pruning(5, 0);
    } catch (const LogicErrorException&) {
        thrown = true;
    }
    EXPECT_TRUE(thrown);
    if (testing::Test::HasFailure()) {
        cerr << "\nТестируем метод Pruning при newBegin > newEnd\n";
        cerr << "Ожидаемое поведение: выброс LogicErrorException\n";
        cerr << "Полученное поведение: исключение " 
            << (thrown ? "выброшено" : "не выброшено") << "\n";
    }
}

TEST_F(FragmentFixture, pruning_begin_equal_end_throws) {
    bool thrown = false;
    try {
        frag->Pruning(5, 5);
    } catch (const LogicErrorException&) {
        thrown = true;
    }
    EXPECT_TRUE(thrown);
    if (testing::Test::HasFailure()) {
        cerr << "\nТестируем метод Pruning при newBegin == newEnd\n";
        cerr << "Ожидаемое поведение: выброс LogicErrorException\n";
        cerr << "Полученное поведение: исключение " 
            << (thrown ? "выброшено" : "не выброшено") << "\n";
    }
}

TEST_F(FragmentFixture, pruning_keeps_old_bounds_after_failed_call) {
    try {
        frag->Pruning(5, 5);
    } catch (const LogicErrorException&) {}
    EXPECT_TRUE(frag->GetBegin() == 1 && frag->GetEnd() == 2);
    if (testing::Test::HasFailure()) {
        cerr << "\nТестируем, что после неудачного вызова Pruning границы отрезка не меняются\n";
        cerr << "Ожидаемый отрезок: [1, 2]\n";
        cerr << "Полученный отрезок: [" << frag->GetBegin() 
            << ", " << frag->GetEnd() << "]\n";
    }
}

TEST_F(FragmentFixture, redefinition_valid) {
    int newArr[2] = {5, 6};
    MutableArraySequence<int> newCoeffs(newArr, 2);
    frag->Redefinition(&newCoeffs);
    double result = frag->Estimation(1);
    EXPECT_TRUE(result == 11);
    if (testing::Test::HasFailure()) {
        cerr << "\nТестируем метод Redefinition с новыми коэффициентами\n";
        cerr << "Новые коэффициенты: 5+6*x, точка: 1\n";
        cerr << "Ожидаемое значение: 11\n";
        cerr << "Полученное значение: " << result << "\n";
    }
}

TEST_F(FragmentFixture, redefinition_with_empty_sequence) {
    MutableArraySequence<int> emptyCoeffs;
    frag->Redefinition(&emptyCoeffs);
    double result = frag->Estimation(1);
    EXPECT_TRUE(result == 0);
    if (testing::Test::HasFailure()) {
        cerr 
            << "\nТестируем метод Redefinition с пустой (длины 0) "
            << "последовательностью коэффициентов\n";
        cerr << "Ожидаемое значение Estimation: 0\n";
        cerr << "Полученное значение: " << result << "\n";
    }
}

TEST_F(FragmentFixture, redefinition_nullptr_throws) {
    bool thrown = false;
    try {
        frag->Redefinition(nullptr);
    } catch (const LogicErrorException&) {
        thrown = true;
    }
    EXPECT_TRUE(thrown);
    if (testing::Test::HasFailure()) {
        cerr << "\nТестируем метод Redefinition с nullptr вместо коэффициентов\n";
        cerr << "Ожидаемое поведение: выброс LogicErrorException\n";
        cerr << "Полученное поведение: исключение " 
            << (thrown ? "выброшено" : "не выброшено") << "\n";
    }
}

TEST_F(FragmentFixture, estimation_int) {
    double result = frag->Estimation(1);
    EXPECT_TRUE(result == 10);
    if (testing::Test::HasFailure()) {
        cerr << "\nТестируем метод Estimation для Fragment<int>\n";
        cerr << "Многочлен: 1+2*x+3*x^2+4*x^3, точка: 1\n";
        cerr << "Ожидаемое значение: 10\n";
        cerr << "Полученное значение: " << result << "\n";
    }
}

TEST_F(FragmentFixture, derivative_at_int) {
    double result = frag->DerivativeAt(1);
    EXPECT_TRUE(result == 20);
    if (testing::Test::HasFailure()) {
        cerr << "\nТестируем метод DerivativeAt для Fragment<int>\n";
        cerr << "Производная многочлена: 2+6*x+12*x^2, точка: 1\n";
        cerr << "Ожидаемое значение: 20\n";
        cerr << "Полученное значение: " << result << "\n";
    }
}

TEST_F(FragmentFixture, estimation_int_on_empty_fragment) {
    Fragment<int> empty;
    double result = empty.Estimation(1);
    EXPECT_TRUE(result == 0);
    if (testing::Test::HasFailure()) {
        cerr << "\nТестируем метод Estimation для Fragment<int> с пустыми коэффициентами\n";
        cerr << "Ожидаемое значение: 0\n";
        cerr << "Полученное значение: " << result << "\n";
    }
}

TEST_F(FragmentFixture, derivative_at_int_on_empty_fragment) {
    Fragment<int> empty;
    double result = empty.DerivativeAt(1);
    EXPECT_TRUE(result == 0);
    if (testing::Test::HasFailure()) {
        cerr << "\nТестируем метод DerivativeAt для Fragment<int> с пустыми коэффициентами\n";
        cerr << "Ожидаемое значение: 0\n";
        cerr << "Полученное значение: " << result << "\n";
    }
}

TEST_F(FragmentFixture, estimation_double) {
    double result = fragDouble->Estimation(1);
    EXPECT_TRUE(result == 4.0);
    if (testing::Test::HasFailure()) {
        cerr << "\nТестируем метод Estimation для Fragment<double>\n";
        cerr << "Многочлен: 1.5+2.0*x+0.5*x^2, точка: 1\n";
        cerr << "Ожидаемое значение: 4\n";
        cerr << "Полученное значение: " << result << "\n";
    }
}

TEST_F(FragmentFixture, derivative_at_double) {
    double result = fragDouble->DerivativeAt(1);
    EXPECT_TRUE(result == 3.0);
    if (testing::Test::HasFailure()) {
        cerr << "\nТестируем метод DerivativeAt для Fragment<double>\n";
        cerr << "Производная многочлена: 2.0+1.0*x, точка: 1\n";
        cerr << "Ожидаемое значение: 3\n";
        cerr << "Полученное значение: " << result << "\n";
    }
}

TEST_F(FragmentFixture, estimation_double_on_empty_fragment) {
    Fragment<double> empty;
    double result = empty.Estimation(1);
    EXPECT_TRUE(result == 0);
    if (testing::Test::HasFailure()) {
        cerr << "\nТестируем метод Estimation для Fragment<double> с пустыми коэффициентами\n";
        cerr << "Ожидаемое значение: 0\n";
        cerr << "Полученное значение: " << result << "\n";
    }
}

TEST_F(FragmentFixture, derivative_at_double_on_empty_fragment) {
    Fragment<double> empty;
    double result = empty.DerivativeAt(1);
    EXPECT_TRUE(result == 0);
    if (testing::Test::HasFailure()) {
        cerr << "\nТестируем метод DerivativeAt для Fragment<double> с пустыми коэффициентами\n";
        cerr << "Ожидаемое значение: 0\n";
        cerr << "Полученное значение: " << result << "\n";
    }
}

TEST_F(FragmentFixture, estimation_complex) {
    Complex result = fragComplex->Estimation(1);
    EXPECT_TRUE(result == Complex(3, 1));
    if (testing::Test::HasFailure()) {
        cerr << "\nТестируем метод Estimation для Fragment<Complex>\n";
        cerr << "Многочлен: (1+1i)+(2+0i)*x, точка: 1\n";
        cerr << "Ожидаемое число: 3+1*i\n";
        cerr << "Полученное число: " << result.toString() << "\n";
    }
}

TEST_F(FragmentFixture, derivative_at_complex) {
    Complex result = fragComplex->DerivativeAt(1);
    EXPECT_TRUE(result == Complex(2, 0));
    if (testing::Test::HasFailure()) {
        cerr << "\nТестируем метод DerivativeAt для Fragment<Complex>\n";
        cerr << "Производная многочлена: (2+0i), точка: 1\n";
        cerr << "Ожидаемое число: 2+0*i\n";
        cerr << "Полученное число: " << result.toString() << "\n";
    }
}

TEST_F(FragmentFixture, estimation_complex_on_empty_fragment) {
    Fragment<Complex> empty;
    Complex result = empty.Estimation(1);
    EXPECT_TRUE(result == Complex(0, 0));
    if (testing::Test::HasFailure()) {
        cerr << "\nТестируем метод Estimation для Fragment<Complex> с пустыми коэффициентами\n";
        cerr << "Ожидаемое число: 0+0*i\n";
        cerr << "Полученное число: " << result.toString() << "\n";
    }
}

TEST_F(FragmentFixture, derivative_at_complex_on_empty_fragment) {
    Fragment<Complex> empty;
    Complex result = empty.DerivativeAt(1);
    EXPECT_TRUE(result == Complex(0, 0));
    if (testing::Test::HasFailure()) {
        cerr << "\nТестируем метод DerivativeAt для Fragment<Complex> с пустыми коэффициентами\n";
        cerr << "Ожидаемое число: 0+0*i\n";
        cerr << "Полученное число: " << result.toString() << "\n";
    }
}