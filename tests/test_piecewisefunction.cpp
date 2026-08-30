#include "../headers/piecewisefunction.hpp"
#include "../headers/fragment.hpp"
#include "../headers/arraysequence.hpp"
#include "../headers/exceptions.h"
#include "fixtures.hpp"
#include <iostream>

using std::cerr;

TEST_F(PieceWiseFunctionFixture, default_constructor_zero_fragments) {
    size_t count = func->CountFragments();
    EXPECT_TRUE(count == 0);
    if (testing::Test::HasFailure()) {
        cerr << "\nТестируем конструктор по умолчанию PieceWiseFunction\n";
        cerr << "Ожидаемое количество фрагментов: 0\n";
        cerr << "Полученное количество фрагментов: " << count << "\n";
    }
}

TEST_F(PieceWiseFunctionFixture, further_specification_into_empty) {
    int arr[1] = {5};
    MutableArraySequence<int> coeffs(arr, 1);
    Fragment<int> frag(0, 5, &coeffs);
    func->FurtherSpecification(frag);
    size_t count = func->CountFragments();
    EXPECT_TRUE(count == 1);
    if (testing::Test::HasFailure()) {
        cerr << "\nТестируем FurtherSpecification на пустой функции\n";
        cerr << "Ожидаемое количество фрагментов: 1\n";
        cerr << "Полученное количество фрагментов: " << count << "\n";
    }
}

TEST_F(PieceWiseFunctionFixture, further_specification_append_after_non_intersecting) {
    int arr1[1] = {1};
    int arr2[1] = {2};
    MutableArraySequence<int> coeffs1(arr1, 1);
    MutableArraySequence<int> coeffs2(arr2, 1);
    Fragment<int> item(0, 3, &coeffs1);
    Fragment<int> frag(5, 8, &coeffs2);

    func->FurtherSpecification(item);
    func->FurtherSpecification(frag);

    size_t count = func->CountFragments();
    Fragment<int> first = func->GetFragment(0);
    Fragment<int> second = func->GetFragment(1);
    bool ok = count == 2
        && first.GetBegin() == 0 && first.GetEnd() == 3
        && second.GetBegin() == 5 && second.GetEnd() == 8;
    EXPECT_TRUE(ok);
    if (testing::Test::HasFailure()) {
        cerr << "\nТестируем FurtherSpecification для отрезка, "
            << "добавляемого после существующего без пересечения\n";
        cerr << "Ожидаемые отрезки: [0, 3], [5, 8]\n";
        cerr << "Полученные отрезки: [" << first.GetBegin() 
            << ", " << first.GetEnd() << "], ["
            << second.GetBegin() << ", " << second.GetEnd() 
            << "], количество: " << count << "\n";
    }
}

TEST_F(PieceWiseFunctionFixture, further_specification_insert_before_non_intersecting) {
    int arr1[1] = {1};
    int arr2[1] = {2};
    MutableArraySequence<int> coeffs1(arr1, 1);
    MutableArraySequence<int> coeffs2(arr2, 1);
    Fragment<int> item(5, 8, &coeffs1);
    Fragment<int> frag(0, 3, &coeffs2);

    func->FurtherSpecification(item);
    func->FurtherSpecification(frag);

    size_t count = func->CountFragments();
    Fragment<int> first = func->GetFragment(0);
    Fragment<int> second = func->GetFragment(1);
    bool ok = count == 2
        && first.GetBegin() == 0 && first.GetEnd() == 3
        && second.GetBegin() == 5 && second.GetEnd() == 8;
    EXPECT_TRUE(ok);
    if (testing::Test::HasFailure()) {
        cerr << "\nТестируем FurtherSpecification для отрезка, "
            << "добавляемого перед существующим без пересечения\n";
        cerr << "Ожидаемые отрезки: [0, 3], [5, 8]\n";
        cerr << "Полученные отрезки: [" << first.GetBegin() 
            << ", " << first.GetEnd() << "], ["
            << second.GetBegin() << ", " << second.GetEnd() 
            << "], количество: " << count << "\n";
    }
}

TEST_F(PieceWiseFunctionFixture, further_specification_insert_in_middle) {
    int arr1[1] = {1};
    int arr2[1] = {1};
    int arr3[1] = {2};
    MutableArraySequence<int> coeffs1(arr1, 1);
    MutableArraySequence<int> coeffs2(arr2, 1);
    MutableArraySequence<int> coeffs3(arr3, 1);
    Fragment<int> item1(0, 2, &coeffs1);
    Fragment<int> item2(5, 7, &coeffs2);
    Fragment<int> frag(3, 4, &coeffs3);

    func->FurtherSpecification(item1);
    func->FurtherSpecification(item2);
    func->FurtherSpecification(frag);

    size_t count = func->CountFragments();
    Fragment<int> first = func->GetFragment(0);
    Fragment<int> second = func->GetFragment(1);
    Fragment<int> third = func->GetFragment(2);
    bool ok = count == 3
        && first.GetBegin() == 0 && first.GetEnd() == 2
        && second.GetBegin() == 3 && second.GetEnd() == 4
        && third.GetBegin() == 5 && third.GetEnd() == 7;
    EXPECT_TRUE(ok);
    if (testing::Test::HasFailure()) {
        cerr << "\nТестируем FurtherSpecification для отрезка, "
            << "вставляемого между двумя существующими\n";
        cerr << "Ожидаемые отрезки: [0, 2], [3, 4], [5, 7]\n";
        cerr << "Полученное количество отрезков: " << count << "\n";
    }
}

TEST_F(PieceWiseFunctionFixture, further_specification_item_fully_inside_frag_removed) {
    int arr1[1] = {1};
    int arr2[1] = {2};
    MutableArraySequence<int> coeffs1(arr1, 1);
    MutableArraySequence<int> coeffs2(arr2, 1);
    Fragment<int> item(2, 5, &coeffs1);
    Fragment<int> frag(0, 10, &coeffs2);

    func->FurtherSpecification(item);
    func->FurtherSpecification(frag);

    size_t count = func->CountFragments();
    Fragment<int> result = func->GetFragment(0);
    bool ok = count == 1 && result.GetBegin() == 0 && result.GetEnd() == 10;
    EXPECT_TRUE(ok);
    if (testing::Test::HasFailure()) {
        cerr << "\nТестируем FurtherSpecification когда "
            << "старый отрезок целиком поглощается новым\n";
        cerr << "Старый отрезок: [2, 5], новый отрезок: [0, 10]\n";
        cerr << "Ожидаемый результат: один отрезок [0, 10]\n";
        cerr << "Полученное количество отрезков: " << count << ", первый отрезок: ["
             << result.GetBegin() << ", " << result.GetEnd() << "]\n";
    }
}

TEST_F(PieceWiseFunctionFixture, further_specification_split_item_around_frag) {
    int arr1[1] = {1};
    int arr2[1] = {2};
    MutableArraySequence<int> coeffs1(arr1, 1);
    MutableArraySequence<int> coeffs2(arr2, 1);
    Fragment<int> item(0, 10, &coeffs1);
    Fragment<int> frag(3, 6, &coeffs2);

    func->FurtherSpecification(item);
    func->FurtherSpecification(frag);

    size_t count = func->CountFragments();
    Fragment<int> first = func->GetFragment(0);
    Fragment<int> second = func->GetFragment(1);
    Fragment<int> third = func->GetFragment(2);
    bool ok = count == 3
        && first.GetBegin() == 0 && first.GetEnd() == 3
        && second.GetBegin() == 3 && second.GetEnd() == 6
        && third.GetBegin() == 6 && third.GetEnd() == 10;
    EXPECT_TRUE(ok);
    if (testing::Test::HasFailure()) {
        cerr << "\nТестируем FurtherSpecification когда "
            << "новый отрезок целиком лежит внутри старого\n";
        cerr << "Старый отрезок: [0, 10], новый отрезок: [3, 6]\n";
        cerr << "Ожидаемые отрезки: [0, 3], [3, 6], [6, 10]\n";
        cerr << "Полученное количество отрезков: " << count << "\n";
    }
}

TEST_F(PieceWiseFunctionFixture, further_specification_item_extends_past_frag_end) {
    int arr1[1] = {1};
    int arr2[1] = {2};
    MutableArraySequence<int> coeffs1(arr1, 1);
    MutableArraySequence<int> coeffs2(arr2, 1);
    Fragment<int> item(2, 8, &coeffs1);
    Fragment<int> frag(0, 4, &coeffs2);

    func->FurtherSpecification(item);
    func->FurtherSpecification(frag);

    size_t count = func->CountFragments();
    Fragment<int> first = func->GetFragment(0);
    Fragment<int> second = func->GetFragment(1);
    bool ok = count == 2
        && first.GetBegin() == 0 && first.GetEnd() == 4
        && second.GetBegin() == 4 && second.GetEnd() == 8;
    EXPECT_TRUE(ok);
    if (testing::Test::HasFailure()) {
        cerr << "\nТестируем FurtherSpecification когда начало "
            << "старого отрезка внутри нового, а конец - нет\n";
        cerr << "Старый отрезок: [2, 8], новый отрезок: [0, 4]\n";
        cerr << "Ожидаемые отрезки: [0, 4], [4, 8]\n";
        cerr << "Полученное количество отрезков: " << count << "\n";
    }
}

TEST_F(PieceWiseFunctionFixture, further_specification_item_starts_before_frag_ends_inside) {
    int arr1[1] = {1};
    int arr2[1] = {2};
    MutableArraySequence<int> coeffs1(arr1, 1);
    MutableArraySequence<int> coeffs2(arr2, 1);
    Fragment<int> item(0, 4, &coeffs1);
    Fragment<int> frag(3, 8, &coeffs2);

    func->FurtherSpecification(item);
    func->FurtherSpecification(frag);

    size_t count = func->CountFragments();
    Fragment<int> first = func->GetFragment(0);
    Fragment<int> second = func->GetFragment(1);
    bool ok = count == 2
        && first.GetBegin() == 0 && first.GetEnd() == 3
        && second.GetBegin() == 3 && second.GetEnd() == 8;
    EXPECT_TRUE(ok);
    if (testing::Test::HasFailure()) {
        cerr << "\nТестируем FurtherSpecification когда конец "
            << "старого отрезка внутри нового, а начало - нет\n";
        cerr << "Старый отрезок: [0, 4], новый отрезок: [3, 8]\n";
        cerr << "Ожидаемые отрезки: [0, 3], [3, 8]\n";
        cerr << "Полученное количество отрезков: " << count << "\n";
    }
}

TEST_F(PieceWiseFunctionFixture, count_fragments_after_multiple_specifications) {
    int arr[1] = {1};
    MutableArraySequence<int> coeffs1(arr, 1);
    MutableArraySequence<int> coeffs2(arr, 1);
    MutableArraySequence<int> coeffs3(arr, 1);
    Fragment<int> f1(0, 1, &coeffs1);
    Fragment<int> f2(2, 3, &coeffs2);
    Fragment<int> f3(4, 5, &coeffs3);

    func->FurtherSpecification(f1);
    func->FurtherSpecification(f2);
    func->FurtherSpecification(f3);

    size_t count = func->CountFragments();
    EXPECT_TRUE(count == 3);
    if (testing::Test::HasFailure()) {
        cerr << "\nТестируем CountFragments после трёх последовательных FurtherSpecification\n";
        cerr << "Ожидаемое количество фрагментов: 3\n";
        cerr << "Полученное количество фрагментов: " << count << "\n";
    }
}

TEST_F(PieceWiseFunctionFixture, get_fragment_returns_correct_data) {
    int arr[2] = {1, 2};
    MutableArraySequence<int> coeffs(arr, 2);
    Fragment<int> frag(0, 5, &coeffs);
    func->FurtherSpecification(frag);

    Fragment<int> result = func->GetFragment(0);
    double est = result.Estimation(1);
    bool ok = result.GetBegin() == 0 && result.GetEnd() == 5 && est == 3;
    EXPECT_TRUE(ok);
    if (testing::Test::HasFailure()) {
        cerr << "\nТестируем метод GetFragment на корректность возвращаемых данных\n";
        cerr << "Многочлен: 1+2*x, точка: 1, ожидаемое значение: 3\n";
        cerr << "Ожидаемый отрезок: [0, 5]\n";
        cerr << "Полученный отрезок: [" << result.GetBegin() << ", " << result.GetEnd()
             << "], Estimation: " << est << "\n";
    }
}

TEST_F(PieceWiseFunctionFixture, get_fragment_returns_copy_independent_from_original) {
    int arr[1] = {1};
    MutableArraySequence<int> coeffs(arr, 1);
    Fragment<int> frag(0, 5, &coeffs);
    func->FurtherSpecification(frag);

    Fragment<int> copy = func->GetFragment(0);
    copy.Pruning(10, 20);
    Fragment<int> stillOriginal = func->GetFragment(0);

    bool ok = copy.GetBegin() == 10 && copy.GetEnd() == 20
        && stillOriginal.GetBegin() == 0 && stillOriginal.GetEnd() == 5;
    EXPECT_TRUE(ok);
    if (testing::Test::HasFailure()) {
        cerr << "\nТестируем, что GetFragment возвращает копию, "
            << "не затрагивающую внутреннее хранилище\n";
        cerr << "Ожидаемые начала отрезков: копия 10, оригинал внутри функции 0\n";
        cerr << "Полученные начала отрезков: копия " << copy.GetBegin()
             << ", оригинал внутри функции " << stillOriginal.GetBegin() << "\n";
    }
}

TEST_F(PieceWiseFunctionFixture, is_continuous_on_empty_function) {
    bool result = func->IsContinuous();
    EXPECT_TRUE(result);
    if (testing::Test::HasFailure()) {
        cerr << "\nТестируем метод IsContinuous на функции без фрагментов\n";
        cerr << "Ожидаемый результат: 1\n";
        cerr << "Полученный результат: " << result << "\n";
    }
}

TEST_F(PieceWiseFunctionFixture, is_continuous_single_fragment) {
    int arr[1] = {1};
    MutableArraySequence<int> coeffs(arr, 1);
    Fragment<int> frag(0, 5, &coeffs);
    func->FurtherSpecification(frag);

    bool result = func->IsContinuous();
    EXPECT_TRUE(result);
    if (testing::Test::HasFailure()) {
        cerr << "\nТестируем метод IsContinuous на функции из одного фрагмента\n";
        cerr << "Ожидаемый результат: 1\n";
        cerr << "Полученный результат: " << result << "\n";
    }
}

TEST_F(PieceWiseFunctionFixture, is_continuous_true_matching_values) {
    int arr1[2] = {0, 1};
    int arr2[2] = {0, 1};
    MutableArraySequence<int> coeffs1(arr1, 2);
    MutableArraySequence<int> coeffs2(arr2, 2);
    Fragment<int> f1(0, 2, &coeffs1);
    Fragment<int> f2(2, 4, &coeffs2);

    func->FurtherSpecification(f1);
    func->FurtherSpecification(f2);

    bool result = func->IsContinuous();
    EXPECT_TRUE(result);
    if (testing::Test::HasFailure()) {
        cerr << "\nТестируем метод IsContinuous для стыкующихся "
            << "и совпадающих по значению фрагментов\n";
        cerr << "f1(x)=x на [0,2], f2(x)=x на [2,4], в точке 2: f1=2, f2=2\n";
        cerr << "Ожидаемый результат: 1\n";
        cerr << "Полученный результат: " << result << "\n";
    }
}

TEST_F(PieceWiseFunctionFixture, is_continuous_false_value_mismatch) {
    int arr1[2] = {0, 1};
    int arr2[2] = {1, 1};
    MutableArraySequence<int> coeffs1(arr1, 2);
    MutableArraySequence<int> coeffs2(arr2, 2);
    Fragment<int> f1(0, 2, &coeffs1);
    Fragment<int> f2(2, 4, &coeffs2);

    func->FurtherSpecification(f1);
    func->FurtherSpecification(f2);

    bool result = func->IsContinuous();
    EXPECT_TRUE(!result);
    if (testing::Test::HasFailure()) {
        cerr << "\nТестируем метод IsContinuous для фрагментов со скачком значения на стыке\n";
        cerr << "f1(x)=x на [0,2] (f1(2)=2), f2(x)=1+x на [2,4] (f2(2)=3)\n";
        cerr << "Ожидаемый результат: 0\n";
        cerr << "Полученный результат: " << result << "\n";
    }
}

TEST_F(PieceWiseFunctionFixture, is_continuous_false_position_gap) {
    int arr1[2] = {0, 1};
    int arr2[2] = {0, 1};
    MutableArraySequence<int> coeffs1(arr1, 2);
    MutableArraySequence<int> coeffs2(arr2, 2);
    Fragment<int> f1(0, 2, &coeffs1);
    Fragment<int> f2(3, 5, &coeffs2);

    func->FurtherSpecification(f1);
    func->FurtherSpecification(f2);

    bool result = func->IsContinuous();
    EXPECT_TRUE(!result);
    if (testing::Test::HasFailure()) {
        cerr << "\nТестируем метод IsContinuous для фрагментов с разрывом по области определения\n";
        cerr << "f1 на [0,2], f2 на [3,5] - между ними промежуток (2, 3)\n";
        cerr << "Ожидаемый результат: 0\n";
        cerr << "Полученный результат: " << result << "\n";
    }
}

TEST_F(PieceWiseFunctionFixture, is_monotonic_on_empty_function) {
    bool result = func->IsMonotonic();
    EXPECT_TRUE(result);
    if (testing::Test::HasFailure()) {
        cerr << "\nТестируем метод IsMonotonic на функции без фрагментов\n";
        cerr << "Ожидаемый результат: 1\n";
        cerr << "Полученный результат: " << result << "\n";
    }
}

TEST_F(PieceWiseFunctionFixture, is_monotonic_increasing_single_fragment) {
    int arr[2] = {0, 1};
    MutableArraySequence<int> coeffs(arr, 2);
    Fragment<int> frag(0, 5, &coeffs);
    func->FurtherSpecification(frag);

    bool result = func->IsMonotonic();
    EXPECT_TRUE(result);
    if (testing::Test::HasFailure()) {
        cerr << "\nТестируем метод IsMonotonic для возрастающей функции f(x)=x на [0,5]\n";
        cerr << "Ожидаемый результат: 1\n";
        cerr << "Полученный результат: " << result << "\n";
    }
}

TEST_F(PieceWiseFunctionFixture, is_monotonic_decreasing_single_fragment) {
    int arr[2] = {0, -1};
    MutableArraySequence<int> coeffs(arr, 2);
    Fragment<int> frag(0, 5, &coeffs);
    func->FurtherSpecification(frag);

    bool result = func->IsMonotonic();
    EXPECT_TRUE(result);
    if (testing::Test::HasFailure()) {
        cerr << "\nТестируем метод IsMonotonic для убывающей функции f(x)=-x на [0,5]\n";
        cerr << "Ожидаемый результат: 1\n";
        cerr << "Полученный результат: " << result << "\n";
    }
}

TEST_F(PieceWiseFunctionFixture, is_monotonic_false_within_single_fragment) {
    int arr[3] = {0, 0, 1};
    MutableArraySequence<int> coeffs(arr, 3);
    Fragment<int> frag(-2, 2, &coeffs);
    func->FurtherSpecification(frag);

    bool result = func->IsMonotonic();
    EXPECT_TRUE(!result);
    if (testing::Test::HasFailure()) {
        cerr << "\nТестируем метод IsMonotonic для немонотонной функции f(x)=x^2 на [-2,2]\n";
        cerr << "Производная 2x меняет знак внутри отрезка\n";
        cerr << "Ожидаемый результат: 0\n";
        cerr << "Полученный результат: " << result << "\n";
    }
}

TEST_F(PieceWiseFunctionFixture, is_monotonic_true_across_increasing_fragments) {
    int arr1[2] = {0, 1};
    int arr2[2] = {0, 1};
    MutableArraySequence<int> coeffs1(arr1, 2);
    MutableArraySequence<int> coeffs2(arr2, 2);
    Fragment<int> f1(0, 2, &coeffs1);
    Fragment<int> f2(2, 4, &coeffs2);

    func->FurtherSpecification(f1);
    func->FurtherSpecification(f2);

    bool result = func->IsMonotonic();
    EXPECT_TRUE(result);
    if (testing::Test::HasFailure()) {
        cerr << "\nТестируем метод IsMonotonic для двух возрастающих "
            << "подряд идущих фрагментов f(x)=x\n";
        cerr << "Ожидаемый результат: 1\n";
        cerr << "Полученный результат: " << result << "\n";
    }
}

TEST_F(PieceWiseFunctionFixture, is_monotonic_false_conflicting_fragment_directions) {
    int arr1[2] = {0, 1};
    int arr2[2] = {10, -1};
    MutableArraySequence<int> coeffs1(arr1, 2);
    MutableArraySequence<int> coeffs2(arr2, 2);
    Fragment<int> f1(0, 2, &coeffs1);
    Fragment<int> f2(2, 4, &coeffs2);

    func->FurtherSpecification(f1);
    func->FurtherSpecification(f2);

    bool result = func->IsMonotonic();
    EXPECT_TRUE(!result);
    if (testing::Test::HasFailure()) {
        cerr << "\nТестируем метод IsMonotonic когда первый "
            << "фрагмент возрастает, а второй убывает\n";
        cerr << "f1(x)=x на [0,2] (производная 1), f2(x)=10-x на [2,4] (производная -1)\n";
        cerr << "Ожидаемый результат: 0\n";
        cerr << "Полученный результат: " << result << "\n";
    }
}

TEST_F(PieceWiseFunctionFixture, is_monotonic_false_boundary_drop_between_fragments) {
    int arr1[2] = {0, 1};
    int arr2[2] = {-10, 1};
    MutableArraySequence<int> coeffs1(arr1, 2);
    MutableArraySequence<int> coeffs2(arr2, 2);
    Fragment<int> f1(0, 2, &coeffs1);
    Fragment<int> f2(2, 4, &coeffs2);

    func->FurtherSpecification(f1);
    func->FurtherSpecification(f2);

    bool result = func->IsMonotonic();
    EXPECT_TRUE(!result);
    if (testing::Test::HasFailure()) {
        cerr << "\nТестируем метод IsMonotonic когда оба фрагмента "
            << "возрастают, но значение падает на стыке\n";
        cerr << "f1(x)=x на [0,2] (f1(2)=2), f2(x)=x-10 на [2,4] (f2(2)=-8)\n";
        cerr << "Ожидаемый результат: 0\n";
        cerr << "Полученный результат: " << result << "\n";
    }
}

TEST_F(PieceWiseFunctionFixture, is_monotonic_complex_throws) {
    bool thrown = false;
    try {
        funcComplex->IsMonotonic();
    } catch (const LogicErrorException&) {
        thrown = true;
    }
    EXPECT_TRUE(thrown);
    if (testing::Test::HasFailure()) {
        cerr << "\nТестируем метод IsMonotonic для PieceWiseFunction<Complex>\n";
        cerr << "Ожидаемое поведение: выброс LogicErrorException\n";
        cerr << "Полученное поведение: исключение "
             << (thrown ? "выброшено" : "не выброшено") << "\n";
    }
}