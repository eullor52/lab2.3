#include "../headers/complex.hpp"

Complex::Complex(double r, double i) 
    : Re(r), Im(i) {}

Complex::Complex(const Complex& other)
    : Re(other.Re), Im(other.Im) {}

Complex::Complex(Complex&& other)
    : Re(other.Re), Im(other.Im) {
        other.Re = 0;
        other.Im = 0;
}

Complex& Complex::operator=(const Complex& other) {
    Re = other.Re;
    Im = other.Im;
    return *this;
}

Complex& Complex::operator=(Complex&& other) {
    if (this != &other) {
        Re = other.Re;
        Im = other.Im;
        other.Re = 0;
        other.Im = 0;
    }
    return *this;
}

Complex Complex::operator+(const Complex& other) const{
    Complex result(Re + other.Re, Im + other.Im);
    return result;
}

Complex Complex::operator-(const Complex& other) const{
    Complex result(Re - other.Re, Im - other.Im);
    return result;
}

Complex Complex::operator*(const Complex& other) const{
    Complex result(Re*other.Re - Im*other.Im, Re*other.Im + Im*other.Re);
    return result;
}

Complex& Complex::operator+=(const Complex& other) {
    Re += other.Re;
    Im += other.Im;
    return *this;
}

Complex& Complex::operator-=(const Complex& other) {
    Re -= other.Re;
    Im -= other.Im;
    return *this;
}

Complex& Complex::operator*=(const Complex& other) {
    double newRe = Re*other.Re - Im*other.Im;
    double newIm = Re*other.Im + Im*other.Re;
    Re = newRe;
    Im = newIm;
    return *this;
}

bool Complex::operator==(const Complex& other) const {
    return Re == other.Re && Im == other.Im;
}

double Complex::GetReal() const {
    return Re;
}

double Complex::GetImage() const {
    return Im;
}

std::string Complex::toString() const {
    std::ostringstream oss;
    if (Im != 0) {
         if (Im > 0) oss << Re << "+" << Im << "*i";
         else oss << Re << Im << "*i";
    }
    else oss << Re;
    return oss.str();
}

double abs(const Complex& number) {
    return std::sqrt(number.GetReal()*number.GetReal() +
        number.GetImage() * number.GetImage());
}