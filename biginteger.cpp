#include "biginteger.h"

BigInteger::operator bool() const {
    return !is_zero();
}

BigInteger BigInteger::operator-() const {
    BigInteger tmp = *this;
    if ((tmp.digits.size() != 1) || (tmp.digits[0] != 0)) {
        tmp.sign *= (-1);
    }
    return tmp;
}

BigInteger& BigInteger::operator=(const BigInteger& other) {
    sign = other.sign;
    digits = other.digits;
    return *this;
}

BigInteger operator"" _bi(const char* arr, size_t sz) {
    assert(sz >= 0);
    return BigInteger(arr);
}

BigInteger operator"" _bi(unsigned long long x) {
    return BigInteger(std::to_string(x));
}

BigInteger::BigInteger(const BigInteger& other) = default;

BigInteger::BigInteger() : BigInteger(0) {}

BigInteger::BigInteger(int x) : BigInteger(std::to_string(x)) {}

BigInteger::BigInteger(const std::string& s) {
    int k = 0;
    if (s[0] == '-') {
        sign = -1;
        ++k;
    } else if (s[0] == '+') {
        sign = 1;
        ++k;
    } else {
        sign = 1;
    }
    int sz = static_cast<int>(s.size());
    std::vector<int> tmp;
    for (; k < sz; ++k) {
        tmp.push_back(s[k] - '0');
    }
    int tmp_size = static_cast<int>(tmp.size());
    for (int i = tmp_size - 1; i >= 0; i -= number_of_digits) {
        int sum = 0;
        for (int j = std::max(i - number_of_digits + 1, 0); j <= i; ++j) {
            sum *= 10;
            sum += tmp[j];
        }
        digits.push_back(sum);
    }
    fix();
}

BigInteger::BigInteger(const char* arr) : BigInteger(std::string(arr)) {}

void BigInteger::fix() {
    delete_zeros();
    if (is_zero()) {
        sign = 1;
    }
}

std::string BigInteger::toString() const {
    std::string ans;
    if (is_zero()) {
        ans = "0";
        return ans;
    }
    if (sign == -1) {
        ans.push_back('-');
    }
    int sz = static_cast<int>(digits.size());
    for (int i = sz - 1; i >= 0; --i) {
        std::string s = std::to_string(digits[i]);
        if (i != sz - 1) {
            while (s.size() < number_of_digits) {
                s = '0' + s;
            }
        }
        ans += s;
    }
    return ans;
}

std::ostream& operator<<(std::ostream& out, const BigInteger& a) {
    out << a.toString();
    return out;
}

std::istream& operator>>(std::istream& in, BigInteger& a) {
    std::string s;
    in >> s;
    a = BigInteger(s);
    a.fix();
    return in;
}

bool less(const BigInteger& a, const BigInteger& b, bool by_abs) {
    bool flag = true;
    if (!by_abs) {
        if (a.sign < b.sign) {
            return true;
        }
        if (a.sign > b.sign) {
            return false;
        }
        if (a.sign == -1) {
            flag = false;
        }
    }
    int sz1 = static_cast<int>(a.digits.size());
    int sz2 = static_cast<int>(b.digits.size());
    if (sz1 < sz2) {
        return flag;
    }
    if (sz2 < sz1) {
        return !flag;
    }
    for (int i = sz1 - 1; i >= 0; --i) {
        if (a.digits[i] < b.digits[i]) {
            return flag;
        } else if (a.digits[i] > b.digits[i]) {
            return !flag;
        }
    }
    return !flag;
}

void BigInteger::minus(const BigInteger& other) {
    int sz2 = static_cast<int>(other.digits.size());
    int carry = 0;
    for (int i = 0; (i < sz2) || (carry != 0); ++i) {
        int x = digits[i] - (i < sz2 ? other.digits[i] : 0) - carry;
        if (x >= 0) {
            digits[i] = x;
            carry = 0;
        } else {
            digits[i] = BigInteger::base + x;
            carry = 1;
        }
    }
    fix();
}

BigInteger& BigInteger::operator+=(const BigInteger& other) {
    if ((digits == other.digits) && (sign != other.sign)) {
        *this = BigInteger(0);
        return *this;
    }
    int sz1 = static_cast<int>(digits.size());
    int sz2 = static_cast<int>(other.digits.size());
    if (sign == other.sign) {
        int carry = 0;
        for (int i = 0; (i < sz2) || (carry != 0); ++i) {
            int x = carry + (i < sz1 ? digits[i] : 0) +
                    (i < sz2 ? other.digits[i] : 0);
            if (i < sz1) {
                digits[i] = x % base;
            } else {
                digits.push_back(x % base);
            }
            carry = x / base;
        }
    } else {
        if (less(*this, other, true)) {
            BigInteger tmp = other;
            tmp.minus(*this);
            *this = tmp;
        } else {
            this->minus(other);
        }
    }
    return *this;
}

BigInteger operator+(const BigInteger& a, const BigInteger& b) {
    BigInteger c = a;
    c += b;
    return c;
}

bool operator<(const BigInteger& a, const BigInteger& b) {
    return less(a, b, false);
}

bool operator==(const BigInteger& a, const BigInteger& b) {
    return (a.sign == b.sign) && (a.digits == b.digits);
}

bool operator!=(const BigInteger& a, const BigInteger& b) {
    return !(a == b);
}

bool operator>(const BigInteger& a, const BigInteger& b) {
    return b < a;
}

bool operator>=(const BigInteger& a, const BigInteger& b) {
    return !(a < b);
}

bool operator<=(const BigInteger& a, const BigInteger& b) {
    return !(a > b);
}

BigInteger& BigInteger::operator++() {
    *this += BigInteger(1);
    return *this;
}

BigInteger BigInteger::operator++(int) {
    BigInteger copy = *this;
    ++*this;
    return copy;
}

BigInteger& BigInteger::operator--() {
    *this += BigInteger(-1);
    return *this;
}

BigInteger BigInteger::operator--(int) {
    BigInteger copy = *this;
    --*this;
    return copy;
}

void BigInteger::move(int k) {
    std::vector<int> ans(digits.size() + k, 0);
    for (int i = 0; i < static_cast<int>(digits.size()); ++i) {
        ans[i + k] = digits[i];
    }
    digits = ans;
}

void BigInteger::multiply_by_digit(long long x) {
    int carry = 0;
    for (int& digit : digits) {
        long long product = digit * x + carry;
        digit = static_cast<int>(product % BigInteger::base);
        carry = static_cast<int>(product / BigInteger::base);
    }
    if (carry > 0) {
        digits.push_back(carry);
    }
    fix();
}

bool BigInteger::is_zero() const {
    return (digits.size() == 1) && (digits[0] == 0);
}

BigInteger& BigInteger::operator*=(const BigInteger& other) {
    if (is_zero() || other.is_zero()) {
        *this = BigInteger(0);
        return *this;
    }
    if (*this < other) {
        BigInteger tmp = other;
        tmp *= (*this);
        *this = tmp;
        return *this;
    }
    BigInteger ans = 0;
    for (int i = 0; i < static_cast<int>(other.digits.size()); ++i) {
        BigInteger tmp = *this;
        tmp.multiply_by_digit(other.digits[i]);
        tmp.move(i);
        ans += tmp;
    }
    ans.sign = sign * other.sign;
    *this = ans;
    fix();
    return *this;
}

BigInteger operator*(const BigInteger& a, const BigInteger& b) {
    BigInteger c = a;
    c *= b;
    return c;
}

BigInteger& BigInteger::operator-=(const BigInteger& other) {
    BigInteger tmp = other * (-1);
    *this += tmp;
    return *this;
}

BigInteger operator-(const BigInteger& a, const BigInteger& b) {
    BigInteger c = a;
    c -= b;
    return c;
}

BigInteger& BigInteger::operator/=(const BigInteger& other) {
    if (other == 1) {
        return *this;
    }
    if (other == -1) {
        sign *= -1;
        return *this;
    }
    if (this->is_zero()) {
        return *this;
    }
    this->sign *= other.sign;
    BigInteger b = other;
    b.sign = 1;
    BigInteger ans = 0;
    BigInteger curr = 0;
    int sz = static_cast<int>(digits.size());
    for (int i = sz - 1; (curr < b) && (i >= 0); --i) {
        curr *= base;
        curr += digits[i];
        int l = 0;
        int r = base;
        while (r - l > 1) {
            int m = (r + l) / 2;
            BigInteger tmp = curr - m * b;
            if (tmp < 0) {
                r = m;
            } else {
                l = m;
            }
        }
        if (ans.is_zero()) {
            ans.digits[0] = l;
        } else {
            ans.digits.push_back(l);
        }
        curr -= l * b;
    }
    std::reverse(ans.digits.begin(), ans.digits.end());
    this->digits = ans.digits;
    fix();
    return *this;
}

BigInteger operator/(const BigInteger& a, const BigInteger& b) {
    BigInteger c = a;
    c /= b;
    return c;
}

BigInteger& BigInteger::operator%=(const BigInteger& other) {
    int ans_sign = this->sign;
    this->sign = 1;
    BigInteger a = other;
    a.sign = 1;
    *this -= (*this / a) * a;
    this->sign = ans_sign;
    delete_zeros();
    if (is_zero()) {
        sign = 1;
    }
    fix();
    return *this;
}

BigInteger operator%(const BigInteger& a, const BigInteger& b) {
    BigInteger c = a;
    c %= b;
    return c;
}

void BigInteger::delete_zeros() {
    for (int i = static_cast<int>(digits.size()) - 1; i >= 1; --i) {
        if (digits[i] == 0) {
            digits.pop_back();
        } else {
            break;
        }
    }
}

BigInteger gcd(const BigInteger& a, const BigInteger& b) {
    if (b == 0) {
        return a;
    }
    return gcd(b, a % b);
}

Rational::Rational(const BigInteger& val) : x(val), y(1) {
    x.sign = 1;
    sign = val.sign;
    fix();
}
Rational::Rational(int val) : x(val), y(1) {
    x.sign = 1;
    if (val >= 0) {
        sign = 1;
    } else {
        sign = -1;
    }
    fix();
}
Rational::Rational() : Rational(0) {}
Rational::Rational(const Rational& other) = default;
Rational& Rational::operator+=(const Rational& other) {
    x = x * other.y + other.sign * y * other.x;
    y = y * other.y;
    fix();
    return *this;
}
Rational& Rational::operator-=(const Rational& other) {
    *this += other * (-1);
    fix();
    return *this;
}
Rational& Rational::operator*=(const Rational& other) {
    x *= other.x;
    y *= other.y;
    sign *= other.sign;
    fix();
    return *this;
}
Rational& Rational::operator/=(const Rational& other) {
    x *= other.y;
    y *= other.x;
    sign *= other.sign;
    fix();
    return *this;
}
Rational Rational::operator-() const {
    Rational ans = *this;
    ans.sign *= -1;
    ans.fix();
    return ans;
}
Rational& Rational::operator=(const Rational& other) = default;
std::string Rational::toString() const {
    std::string ans;
    if (sign == -1) {
        ans += '-';
    }
    ans += x.toString();
    if (y != 1) {
        ans += '/';
        ans += y.toString();
    }
    return ans;
}
std::string Rational::asDecimal(int precision = 0) const {
    BigInteger a = 1;
    for (int i = 0; i < precision; ++i) {
        a *= 10;
    }
    BigInteger ans = (x * a) / y;
    ans.sign = sign;
    std::string s = ans.toString();
    int pos = static_cast<int>(s.size()) - precision;
    if (pos <= 0) {
        std::string s2;
        if (sign == -1) {
            s2.push_back('-');
        }
        s2 += "0.";
        for (int i = 0; i < -pos + 1; ++i) {
            s2.push_back('0');
        }
        if (sign == -1) {
            s.erase(s.begin());
        }
        s2 += s;
        return s2;
    } else {
        s.insert(s.begin() + pos, '.');
        return s;
    }
}
Rational::operator double() const {
    return std::stod(asDecimal(500));
}
bool operator==(const Rational& a, const Rational& b) {
    return (a.sign == b.sign) && (a.x == b.x) && (a.y == b.y);
}
bool operator!=(const Rational& a, const Rational& b) {
    return !(a == b);
}
bool operator<(const Rational& a, const Rational& b) {
    if (a.sign < b.sign) {
        return true;
    }
    if (a.sign > b.sign) {
        return false;
    }
    if (a.sign == 1) {
        return (a.x * b.y < b.x * a.y);
    }
    return (a.x * b.y > b.x * a.y);
}
bool operator>(const Rational& a, const Rational& b) {
    return b < a;
}
bool operator<=(const Rational& a, const Rational& b) {
    return !(a > b);
}
bool operator>=(const Rational& a, const Rational& b) {
    return !(a < b);
}

Rational operator+(const Rational& a, const Rational& b) {
    Rational c = a;
    c += b;
    return c;
}
Rational operator-(const Rational& a, const Rational& b) {
    Rational c = a;
    c -= b;
    return c;
}
Rational operator*(const Rational& a, const Rational& b) {
    Rational c = a;
    c *= b;
    return c;
}
Rational operator/(const Rational& a, const Rational& b) {
    Rational c = a;
    c /= b;
    return c;
}

void Rational::fix_sign() {
    sign *= x.sign * y.sign;
    x.sign = 1;
    y.sign = 1;
}
void Rational::reduce() {
    BigInteger tmp = gcd(x, y);
    x /= tmp;
    y /= tmp;
}
void Rational::fix() {
    reduce();
    fix_sign();
    if (x.is_zero()) {
        sign = 1;
    }
}
