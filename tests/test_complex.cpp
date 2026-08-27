#include "../headers/complex.hpp"
#include "fixtures.hpp"
#include <iostream>

using std::cerr;

TEST_F(ComplexFixture, defoult_constructor) {
    Complex defaultNum;
    EXPECT_TRUE(defaultNum.GetReal() == 0 && defaultNum.GetImage() == 0);
    if (testing::Test::HasFailure()) {
        cerr << "\nТестируем конструктор по умолчанию\n";
        cerr << "Ожидаемое число: 0\n";
        cerr << "Полученное число: " << defaultNum.toString() << "\n";
    }
}

TEST_F(ComplexFixture, definite_constructor) {
    Complex num(1, 5);
    EXPECT_TRUE(num.GetReal() == 1 && num.GetImage() == 5);
    if (testing::Test::HasFailure()) {
        cerr << "\nТестируем определенный конструктор\n";
        cerr << "Ожидаемое число 1+5*i:\n";
        cerr << "Полученное число: " << num.toString() << "\n";
    }
}

TEST_F(ComplexFixture, copy_constructor) {
    Complex copy(lhs);
    EXPECT_TRUE(copy == lhs);
    if (testing::Test::HasFailure()) {
        cerr << "\nТестируем конструктор копирования\n";
        cerr << "Ожидаемое число: " << lhs.toString() <<"\n";
        cerr << "Полученное число: " << copy.toString() << "\n";
    }
}

TEST_F(ComplexFixture, move_constructor) {
    Complex num(std::move(lhs));
    EXPECT_TRUE(num.GetReal() == 1 && num.GetImage() == 2);
    if (testing::Test::HasFailure()) {
        cerr << "\nТестируем конструктор перемещения\n";
        cerr << "Ожидаемое число: 1+2*i\n";
        cerr << "Полученное число: " << num.toString() << "\n";
    }
}

TEST_F(ComplexFixture, copy_operator) {
    Complex copy;
    copy = lhs;
    EXPECT_TRUE(copy.GetReal() == lhs.GetReal() 
        && copy.GetImage() == lhs.GetImage());
    if (testing::Test::HasFailure()) {
        cerr << "\nТестируем оператор присваивания копированием\n";
        cerr << "Ожидаемое число: " << lhs.toString() << "\n";
        cerr << "Полученное число: " << copy.toString() << "\n";
    }
}

TEST_F(ComplexFixture, move_operator) {
    Complex num;
    num = std::move(lhs);
    EXPECT_TRUE(num.GetReal() == 1 && num.GetImage() == 2);
    if (testing::Test::HasFailure()) {
        cerr << "\nТестируем оператор присваивания перемещением\n";
        cerr << "Ожидаемое число: 1+2*i\n";
        cerr << "Полученное число: " << num.toString() << "\n";
    }
}

TEST_F(ComplexFixture, plus_operator) {
    Complex sum = rhs + lhs;
    EXPECT_TRUE(sum.GetReal() == rhs.GetReal() + lhs.GetReal() 
        && sum.GetImage() == rhs.GetImage() + lhs.GetImage());
    if (testing::Test::HasFailure()) {
        cerr << "\nТестируем оператор +\n";
        cerr << "Первое слогаемое: 1+2*i\nВторое слогаемое: 3 + 4*i\n";
        cerr << "Ожидаемое число: 4+6*i\n";
        cerr << "Полученное число: " << sum.toString() << "\n";
    }
}

TEST_F(ComplexFixture, minus_operator) {
    Complex dif = lhs - rhs;
    EXPECT_TRUE(dif.GetReal() == lhs.GetReal() - rhs.GetReal()
        && dif.GetImage() == lhs.GetImage() - rhs.GetImage());
    if (testing::Test::HasFailure()) {
        cerr << "\nТестируем оператор -\n";
        cerr << "Уменьшаемое: 1+2*i\nВычитаемое: 3 + 4*i\n";
        cerr << "Ожидаемое число: -2-2*i\n";
        cerr << "Полученное число: " << dif.toString() << "\n";
    }
}

TEST_F(ComplexFixture, multiplication_operator) {
    Complex product = lhs*rhs;
    EXPECT_TRUE(product.GetReal() == -5
        && product.GetImage() == 10);
    if (testing::Test::HasFailure()) {
        cerr << "\nТестируем оператор *\n";
        cerr << "Первый множитель: 1+2*i\nВторой множитель: 3 + 4*i\n";
        cerr << "Ожидаемое число: -5+10*i\n";
        cerr << "Полученное число: " << product.toString() << "\n";
    }
}

TEST_F(ComplexFixture, addition_with_assignment_operator) {
    lhs += rhs;
    EXPECT_TRUE(lhs.GetReal() == 4 && lhs.GetImage() == 6);
    if (testing::Test::HasFailure()) {
        cerr << "\nТестируем оператор +=\n";
        cerr << "Первое слогаемое: 1+2*i\nВторое слогаемое: 3+4*i\n";
        cerr << "Ожидаемое число: 4+6*i\n";
        cerr << "Полученное число: " << lhs.toString() << "\n";
    }
}

TEST_F(ComplexFixture, subtraction_with_assignment_operator) {
    lhs -= rhs;
    EXPECT_TRUE(lhs.GetReal() == -2 && lhs.GetImage() == -2);
    if (testing::Test::HasFailure()) {
        cerr << "\nТестируем оператор -=\n";
        cerr << "Уменьшаемое: 1+2*i\nВычитаемое: 3+4*i\n";
        cerr << "Ожидаемое число: -2-2*i\n";
        cerr << "Полученное число: " << lhs.toString() << "\n";
    }
}

TEST_F(ComplexFixture, multiplication_with_assignment_operator) {
    lhs *= rhs;
    EXPECT_TRUE(lhs.GetReal() == -5 && lhs.GetImage() == 10);
    if (testing::Test::HasFailure()) {
        cerr << "\nТестируем оператор *\n";
        cerr << "Первый множитель: 1+2*i\nВторой множитель: 3+4*i\n";
        cerr << "Ожидаемое число: -5+10*i\n";
        cerr << "Полученное число: " << lhs.toString() << "\n";
    }
}

TEST_F(ComplexFixture, equal_operator) {
    Complex num(1,2);
    EXPECT_TRUE(num == lhs);
    if (testing::Test::HasFailure()) {
        cerr << "\nТестируем оператор ==\n";
        cerr << "Первый операнд: 1+2*i\nВторой операнд: " << lhs.toString() << "\n";
        cerr << "Ожидаемый результат: " << true << "\n";
        cerr << "Полученный результат: " << bool(num == lhs) << "\n";
    }
}