#pragma once

#include <string>
#include <sstream>
#include <cmath>

class Complex {
private:
    double Re = 0;
    double Im = 0;
public:
    Complex() = default;
    Complex(double r, double i);
    Complex(const Complex& other);
    Complex(Complex&& other);
    ~Complex() = default;

    Complex& operator=(const Complex& other);
    Complex& operator=(Complex&& other);
    Complex operator+(const Complex& other) const;
    Complex operator-(const Complex& other) const;
    Complex operator*(const Complex& other) const;
    Complex& operator+=(const Complex& other);
    Complex& operator-=(const Complex& other);
    Complex& operator*=(const Complex& other);
    bool operator==(const Complex& other) const;

    double GetReal() const;
    double GetImage() const;

    std::string toString() const;
};

double abs(const Complex& number);