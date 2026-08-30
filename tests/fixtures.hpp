#pragma once
#include <gtest/gtest.h>
#include "../headers/complex.hpp"
#include "../headers/piecewisefunction.hpp"
#include "../headers/arraysequence.hpp"

class ComplexFixture : public testing::Test {
protected:
    Complex lhs = {1, 2};
    Complex rhs = {3, 4};
};

class FragmentFixture : public testing::Test {
protected:
    int arrInt[4] = {1, 2, 3, 4};
    double arrDouble[3] = {1.5, 2.0, 0.5};
    Complex arrComplex[2] = {Complex(1, 1), Complex(2, 0)};

    MutableArraySequence<int>* coeffsInt;
    MutableArraySequence<double>* coeffsDouble;
    MutableArraySequence<Complex>* coeffsComplex;

    Fragment<int>* frag;
    Fragment<double>* fragDouble;
    Fragment<Complex>* fragComplex;

    double begin = 1;
    double end = 2;

    void SetUp() override {
        coeffsInt = new MutableArraySequence<int>(arrInt, 4);
        coeffsDouble = new MutableArraySequence<double>(arrDouble, 3);
        coeffsComplex = new MutableArraySequence<Complex>(arrComplex, 2);

        frag = new Fragment<int>(begin, end, coeffsInt);
        fragDouble = new Fragment<double>(begin, end, coeffsDouble);
        fragComplex = new Fragment<Complex>(begin, end, coeffsComplex);
    }

    void TearDown() override {
        delete frag;
        delete fragDouble;
        delete fragComplex;
        delete coeffsInt;
        delete coeffsDouble;
        delete coeffsComplex;
    }
};

class PieceWiseFunctionFixture : public testing::Test {
protected:
    PieceWiseFunction<int>* func;
    PieceWiseFunction<double>* funcDouble;
    PieceWiseFunction<Complex>* funcComplex;

    void SetUp() override {
        func = new PieceWiseFunction<int>();
        funcDouble = new PieceWiseFunction<double>();
        funcComplex = new PieceWiseFunction<Complex>();
    }

    void TearDown() override {
        delete func;
        delete funcDouble;
        delete funcComplex;
    }
};