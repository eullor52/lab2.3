#pragma once
#include <gtest/gtest.h>
#include "../headers/complex.hpp"
#include "../headers/piecewisefunction.hpp"
#include "c:/projects/lab-2/headers/arraysequence.hpp"

class ComplexFixture : public testing::Test {
protected:
    Complex lhs = {1, 2};
    Complex rhs = {3, 4};
};

class FragmentFixture : public testing::Test {
protected:
    int arr[4] = {1, 2, 3, 4};
    MutableArraySequence<int>* coeffs;
    Fragment<int>* frag;
    double begin = 1;
    double end = 2;

    void SetUp() override {
        coeffs = new MutableArraySequence<int>(arr, 4);
        frag = new Fragment<int>(begin, end, coeffs);
    }

    void TearDown() override {
        delete frag;
        delete coeffs;
    }
};