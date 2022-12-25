#include <algorithm>
#include <array>
#include <cassert>
#include <cstring>
#include <iostream>
#include <string>
#include <vector>

class BigInteger {
  private:
    int sign_;
    std::vector<int> digits_;
    static const int base = 1000'000'000;
    static const int number_of_digits = 9;
    void multiply_by_digit(long long x);
    void minus(const BigInteger& other);
    void move(int k);
    void fix();
    bool is_zero() const;
    void delete_zeros();
    BigInteger& div_or_mod(const BigInteger& other, bool return_div);
    friend bool less(const BigInteger& a, const BigInteger& b, bool by_abs);
    friend class Rational;

  public:
    BigInteger();
    BigInteger(const BigInteger& other);
    BigInteger(int x);
    explicit BigInteger(const char* arr);
    explicit BigInteger(const std::string& s);
    explicit operator bool() const;
    BigInteger& operator+=(const BigInteger& other);
    BigInteger& operator-=(const BigInteger& other);
    BigInteger& operator*=(const BigInteger& other);
    BigInteger& operator/=(const BigInteger& other);
    BigInteger& operator%=(const BigInteger& other);
    BigInteger operator-() const;
    BigInteger& operator++();
    BigInteger operator++(int);
    BigInteger& operator--();
    BigInteger operator--(int);
    std::string toString() const;
    BigInteger& operator=(const BigInteger& other) = default;
    void change_sign();
    friend std::istream& operator>>(std::istream& in, BigInteger& a);
    friend bool operator==(const BigInteger& a, const BigInteger& b);
};

BigInteger operator"" _bi(const char* arr, size_t sz);
BigInteger operator"" _bi(unsigned long long x);
std::ostream& operator<<(std::ostream& out, const BigInteger& a);
std::istream& operator>>(std::istream& in, BigInteger& a);
bool less(const BigInteger& a, const BigInteger& b, bool by_abs);
BigInteger operator+(const BigInteger& a, const BigInteger& b);
BigInteger operator-(const BigInteger& a, const BigInteger& b);
BigInteger operator*(const BigInteger& a, const BigInteger& b);
BigInteger operator/(const BigInteger& a, const BigInteger& b);
BigInteger operator%(const BigInteger& a, const BigInteger& b);
bool operator<(const BigInteger& a, const BigInteger& b);
bool operator==(const BigInteger& a, const BigInteger& b);
bool operator!=(const BigInteger& a, const BigInteger& b);
bool operator>(const BigInteger& a, const BigInteger& b);
bool operator<=(const BigInteger& a, const BigInteger& b);
bool operator>=(const BigInteger& a, const BigInteger& b);

class Rational {
  private:
    void fix_sign();
    void reduce();
    void fix();
    Rational& increase_or_decrease(const Rational& other, bool flag);

  public:
    BigInteger numerator_;
    BigInteger denominator_;
    int sign_;
    Rational(const BigInteger& val);
    Rational(int val);
    Rational();
    Rational(const Rational& other);
    Rational& operator+=(const Rational& other);
    Rational& operator-=(const Rational& other);
    Rational& operator*=(const Rational& other);
    Rational& operator/=(const Rational& other);
    Rational operator-() const;
    Rational& operator=(const Rational& other);
    std::string toString() const;
    std::string asDecimal(int precision) const;
    explicit operator double() const;
    friend bool operator==(const Rational& a, const Rational& b);
    friend bool operator<(const Rational& a, const Rational& b);
};

std::ostream& operator>>(std::ostream& out, Rational& a);
std::ostream& operator<<(std::ostream& out, const Rational& a);

bool operator!=(Rational& a, Rational& b);
bool operator<(const Rational& a, const Rational& b);
bool operator>(const Rational& a, const Rational& b);
bool operator<=(const Rational& a, const Rational& b);
bool operator>=(const Rational& a, const Rational& b);

Rational operator+(const Rational& a, const Rational& b);
Rational operator-(const Rational& a, const Rational& b);
Rational operator*(const Rational& a, const Rational& b);
Rational operator/(const Rational& a, const Rational& b);
