#include "biginteger.h"

BigInteger::operator bool() const {
    return !is_zero();
}

BigInteger BigInteger::operator-() const {
    BigInteger tmp = *this;
    if (!is_zero()) {
        tmp.sign_ *= (-1);
    }
    return tmp;
}

BigInteger& BigInteger::operator=(const BigInteger& other) {
    sign_ = other.sign_;
    digits_ = other.digits_;
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
        sign_ = -1;
        ++k;
    } else if (s[0] == '+') {
        sign_ = 1;
        ++k;
    } else {
        sign_ = 1;
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
        digits_.push_back(sum);
    }
    fix();
}

BigInteger::BigInteger(const char* arr) : BigInteger(std::string(arr)) {}

void BigInteger::fix() {
    delete_zeros();
    if (is_zero()) {
        sign_ = 1;
    }
}

std::string BigInteger::toString() const {
    std::string ans;
    if (is_zero()) {
        ans = "0";
        return ans;
    }
    if (sign_ == -1) {
        ans.push_back('-');
    }
    int sz = static_cast<int>(digits_.size());
    for (int i = sz - 1; i >= 0; --i) {
        std::string s = std::to_string(digits_[i]);
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
    bool positive_signs = true;
    if (!by_abs) {
        if (a.sign_ < b.sign_) {
            return true;
        }
        if (a.sign_ > b.sign_) {
            return false;
        }
        if (a.sign_ == -1) {
            positive_signs = false;
        }
    }
    int sz1 = static_cast<int>(a.digits_.size());
    int sz2 = static_cast<int>(b.digits_.size());
    if (sz1 < sz2) {
        return positive_signs;
    }
    if (sz2 < sz1) {
        return !positive_signs;
    }
    for (int i = sz1 - 1; i >= 0; --i) {
        if (a.digits_[i] < b.digits_[i]) {
            return positive_signs;
        } else if (a.digits_[i] > b.digits_[i]) {
            return !positive_signs;
        }
    }
    return !positive_signs;
}

void BigInteger::minus(const BigInteger& other) {
    int sz2 = static_cast<int>(other.digits_.size());
    int carry = 0;
    for (int i = 0; (i < sz2) || (carry != 0); ++i) {
        int x = digits_[i] - (i < sz2 ? other.digits_[i] : 0) - carry;
        if (x >= 0) {
            digits_[i] = x;
            carry = 0;
        } else {
            digits_[i] = BigInteger::base + x;
            carry = 1;
        }
    }
    fix();
}

BigInteger& BigInteger::operator+=(const BigInteger& other) {
    int sz1 = static_cast<int>(digits_.size());
    int sz2 = static_cast<int>(other.digits_.size());
    if (sign_ == other.sign_) {
        int carry = 0;
        for (int i = 0; (i < sz2) || (carry != 0); ++i) {
            int x = carry + (i < sz1 ? digits_[i] : 0) +
                    (i < sz2 ? other.digits_[i] : 0);
            if (i < sz1) {
                digits_[i] = x % base;
            } else {
                digits_.push_back(x % base);
            }
            carry = x / base;
        }
    } else {
        if (less(*this, other, true)) {
            BigInteger tmp = other;
            tmp.minus(*this);
            *this = tmp;
        } else {
            minus(other);
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
    return (a.sign_ == b.sign_) && (a.digits_ == b.digits_);
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
    std::vector<int> ans(digits_.size() + k, 0);
    for (int i = 0; i < static_cast<int>(digits_.size()); ++i) {
        ans[i + k] = digits_[i];
    }
    digits_ = ans;
}

void BigInteger::multiply_by_digit(long long x) {
    int carry = 0;
    for (int& digit : digits_) {
        long long product = digit * x + carry;
        digit = static_cast<int>(product % BigInteger::base);
        carry = static_cast<int>(product / BigInteger::base);
    }
    if (carry > 0) {
        digits_.push_back(carry);
    }
    fix();
}

bool BigInteger::is_zero() const {
    return (digits_.size() == 1) && (digits_[0] == 0);
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
    for (int i = 0; i < static_cast<int>(other.digits_.size()); ++i) {
        BigInteger tmp = *this;
        tmp.multiply_by_digit(other.digits_[i]);
        tmp.move(i);
        ans += tmp;
    }
    ans.sign_ = sign_ * other.sign_;
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
    //BigInteger tmp = other * (-1);
    BigInteger tmp = other;
    tmp.change_sign();
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
        sign_ *= -1;
        return *this;
    }
    if (is_zero()) {
        return *this;
    }
    sign_ *= other.sign_;
    BigInteger b = other;
    b.sign_ = 1;
    BigInteger ans = 0;
    BigInteger curr = 0;
    int sz = static_cast<int>(digits_.size());
    for (int i = sz - 1; (curr < b) && (i >= 0); --i) {
        curr *= base;
        curr += digits_[i];
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
            ans.digits_[0] = l;
        } else {
            ans.digits_.push_back(l);
        }
        curr -= l * b;
    }
    std::reverse(ans.digits_.begin(), ans.digits_.end());
    digits_ = ans.digits_;
    fix();
    return *this;
}

BigInteger operator/(const BigInteger& a, const BigInteger& b) {
    BigInteger c = a;
    c /= b;
    return c;
}

BigInteger& BigInteger::operator%=(const BigInteger& other) {
    int ans_sign = sign_;
    sign_ = 1;
    BigInteger a = other;
    a.sign_ = 1;
    *this -= (*this / a) * a;
    sign_ = ans_sign;
    delete_zeros();
    if (is_zero()) {
        sign_ = 1;
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
    while ((digits_.back() == 0) && (digits_.size() >= 2)) {
        digits_.pop_back();
    }
}

BigInteger gcd(BigInteger a, BigInteger b) {
    BigInteger tmp;
    while (b != 0) {
        tmp = a % b;
        a = b;
        b = tmp;
    }
    return a;
}

void BigInteger::change_sign() {
    if (!is_zero()) {
        sign_ *= -1;
    }
}

Rational::Rational(const BigInteger& val) : numerator_(val), denominator_(1) {
    numerator_.sign_ = 1;
    sign = val.sign_;
    fix();
}
Rational::Rational(int val) : numerator_(val), denominator_(1) {
    numerator_.sign_ = 1;
    if (val >= 0) {
        sign = 1;
    } else {
        sign = -1;
    }
    fix();
}
Rational::Rational() : Rational(0) {}
Rational::Rational(const Rational& other) = default;
BigInteger plus_or_minus(const BigInteger& a, const BigInteger& b, bool flag) {
    if (flag) {
        return a + b;
    }
    return a - b;
}
Rational& Rational::increase_or_decrease(const Rational& other, bool flag) {
    numerator_ =
        plus_or_minus(numerator_ * other.denominator_,
                      other.sign * denominator_ * other.numerator_, flag);
    denominator_ = denominator_ * other.denominator_;
    fix();
    return *this;
}
Rational& Rational::operator+=(const Rational& other) {
    return increase_or_decrease(other, true);
}
Rational& Rational::operator-=(const Rational& other) {
    return increase_or_decrease(other, false);
}
Rational& Rational::operator*=(const Rational& other) {
    numerator_ *= other.numerator_;
    denominator_ *= other.denominator_;
    sign *= other.sign;
    fix();
    return *this;
}
Rational& Rational::operator/=(const Rational& other) {
    numerator_ *= other.denominator_;
    denominator_ *= other.numerator_;
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
    ans += numerator_.toString();
    if (denominator_ != 1) {
        ans += '/';
        ans += denominator_.toString();
    }
    return ans;
}
std::string Rational::asDecimal(int precision = 0) const {
    BigInteger a = 1;
    for (int i = 0; i < precision; ++i) {
        a *= 10;
    }
    BigInteger ans = (numerator_ * a) / denominator_;
    ans.sign_ = sign;
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
    return std::stod(asDecimal(20));
}
bool operator==(const Rational& a, const Rational& b) {
    return (a.sign == b.sign) && (a.numerator_ == b.numerator_) &&
           (a.denominator_ == b.denominator_);
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
        return (a.numerator_ * b.denominator_ < b.numerator_ * a.denominator_);
    }
    return (a.numerator_ * b.denominator_ > b.numerator_ * a.denominator_);
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
    sign *= numerator_.sign_ * denominator_.sign_;
    numerator_.sign_ = 1;
    denominator_.sign_ = 1;
}
void Rational::reduce() {
    BigInteger tmp = gcd(numerator_, denominator_);
    numerator_ /= tmp;
    denominator_ /= tmp;
}
void Rational::fix() {
    reduce();
    fix_sign();
    if (numerator_.is_zero()) {
        sign = 1;
    }
}
