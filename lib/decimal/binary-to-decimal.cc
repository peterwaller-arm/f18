// Copyright (c) 2018-2019, NVIDIA CORPORATION.  All rights reserved.
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//     http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.

#include "big-radix-floating-point.h"
#include "decimal.h"

namespace Fortran::decimal {

template<int PREC, int LOG10RADIX>
BigRadixFloatingPointNumber<PREC, LOG10RADIX>::BigRadixFloatingPointNumber(
    BinaryFloatingPointNumber<PREC> x, enum FortranRounding rounding)
  : rounding_{rounding} {
  if (x.IsNegative() < 0) {
    isNegative_ = true;
    x.Negate();
  }
  if (x.IsZero()) {
    return;
  }
  int twoPow{x.UnbiasedExponent()};
  twoPow -= x.bits - 1;
  if (!x.implicitMSB) {
    ++twoPow;
  }
  int lshift{x.exponentBits};
  if (twoPow <= -lshift) {
    twoPow += lshift;
    lshift = 0;
  } else if (twoPow < 0) {
    lshift += twoPow;
    twoPow = 0;
  }
  auto word{x.Fraction()};
  word <<= lshift;
  SetTo(word);

  // The significand is now encoded in *this as an integer (D) and
  // decimal exponent (E):  x = D * 10.**E * 2.**twoPow
  // twoPow can be positive or negative.
  // The goal now is to get twoPow up or down to zero, leaving us with
  // only decimal digits and decimal exponent.  This is done by
  // fast multiplications and divisions of D by 2 and 5.

  // (5*D) * 10.**E * 2.**twoPow -> D * 10.**(E+1) * 2.**(twoPow-1)
  for (; twoPow > 0 && IsDivisibleBy<5>(); --twoPow) {
    DivideBy<5>();
    ++exponent_;
  }

  for (; twoPow >= 9; twoPow -= 9) {
    // D * 10.**E * 2.**twoPow -> (D*(2**9)) * 10.**E * 2.**(twoPow-9)
    MultiplyByRounded<512>();
  }
  for (; twoPow >= 3; twoPow -= 3) {
    // D * 10.**E * 2.**twoPow -> (D*(2**3)) * 10.**E * 2.**(twoPow-3)
    MultiplyByRounded<8>();
  }
  for (; twoPow > 0; --twoPow) {
    // D * 10.**E * 2.**twoPow -> (2*D) * 10.**E * 2.**(twoPow-1)
    MultiplyByRounded<2>();
  }

  while (twoPow < 0) {
    int shift{common::TrailingZeroBitCount(digit_[0])};
    if (shift == 0) {
      break;
    }
    if (shift > log10Radix) {
      shift = log10Radix;
    }
    if (shift > -twoPow) {
      shift = -twoPow;
    }
    // (D*(2**S)) * 10.**E * 2.**twoPow -> D * 10.**E * 2.**(twoPow+S)
    DivideByPowerOfTwo(shift);
    twoPow += shift;
  }

  for (; twoPow <= -4; twoPow += 4) {
    // D * 10.**E * 2.**twoPow -> 625D * 10.**(E-4) * 2.**(twoPow+4)
    MultiplyByRounded<(5 * 5 * 5 * 5)>();
    exponent_ -= 4;
  }
  if (twoPow <= -2) {
    // D * 10.**E * 2.**twoPow -> 25D * 10.**(E-2) * 2.**(twoPow+2)
    MultiplyByRounded<25>();
    twoPow += 2;
    exponent_ -= 2;
  }
  for (; twoPow < 0; ++twoPow) {
    // D * 10.**E * 2.**twoPow -> 5D * 10.**(E-1) * 2.**(twoPow+1)
    MultiplyByRounded<5>();
    --exponent_;
  }

  // twoPow == 0, the decimal encoding is complete.
  Normalize();
}

template<int PREC, int LOG10RADIX>
ConversionToDecimalResult
BigRadixFloatingPointNumber<PREC, LOG10RADIX>::ConvertToDecimal(
    char *buffer, std::size_t n, int flags, int digits) const {
  if (n < (digits_ + 1) * LOG10RADIX) {  // pmk revisit
    return {nullptr, 0, 0};
  }
  char *start{buffer};
  if (isNegative_) {
    *start++ = '-';
  } else if (flags & AlwaysSign) {
    *start++ = '+';
  }
  char *p{start};
  if (IsZero()) {
    *p++ = '0';
    *p = '\0';
    return {buffer, static_cast<std::size_t>(p - buffer), 0};
  }
  static_assert((LOG10RADIX % 2) == 0, "radix not a power of 100");
  static const char lut[] = "0001020304050607080910111213141516171819"
                            "2021222324252627282930313233343536373839"
                            "4041424344454647484950515253545556575859"
                            "6061626364656667686970717273747576777879"
                            "8081828384858687888990919293949596979899";
  static constexpr Digit hundredth{radix / 100};
  // Treat the MSD specially: don't emit leading zeroes.
  Digit dig{digit_[digits_ - 1]};
  for (int k{0}; k < LOG10RADIX; k += 2) {
    Digit d{dig / hundredth};
    dig = 100 * (dig - d * hundredth);
    const char *q{lut + 2 * d};
    if (q[0] != '0' || p > start) {
      *p++ = q[0];
      *p++ = q[1];
    } else if (q[1] != '0') {
      *p++ = q[1];
    }
  }
  for (int j{digits_ - 1}; j-- > 0;) {
    Digit dig{digit_[j]};
    for (int k{0}; k < log10Radix; k += 2) {
      Digit d{dig / hundredth};
      dig = 100 * (dig - d * hundredth);
      const char *q = lut + 2 * d;
      *p++ = q[0];
      *p++ = q[1];
    }
  }
  int expo = exponent_ + p - start;
  while (p[-1] == '0') {
    --p;
  }
  *p = '\0';
  return {buffer, static_cast<std::size_t>(p - buffer), expo};
}

template<int PREC, int LOG10RADIX>
bool BigRadixFloatingPointNumber<PREC, LOG10RADIX>::Mean(
    const BigRadixFloatingPointNumber &that) {
  while (digits_ < that.digits_) {
    digit_[digits_++] = 0;
  }
  int carry{0};
  for (int j{0}; j < that.digits_; ++j) {
    Digit v{digit_[j] + that.digit_[j] + carry};
    if (v >= radix) {
      digit_[j] = v - radix;
      carry = 1;
    } else {
      digit_[j] = v;
      carry = 0;
    }
  }
  if (carry > 0) {
    AddCarry(that.digits_, carry);
  }
  return DivideBy<2>() != 0;
}

template<int PREC, int LOG10RADIX>
void BigRadixFloatingPointNumber<PREC, LOG10RADIX>::Minimize(
    BigRadixFloatingPointNumber &&less, BigRadixFloatingPointNumber &&more) {
  int leastExponent{exponent_};
  if (less.exponent_ < leastExponent) {
    leastExponent = less.exponent_;
  }
  if (more.exponent_ < leastExponent) {
    leastExponent = more.exponent_;
  }
  while (exponent_ > leastExponent) {
    --exponent_;
    MultiplyBy<10>();
  }
  while (less.exponent_ > leastExponent) {
    --less.exponent_;
    less.MultiplyBy<10>();
  }
  while (more.exponent_ > leastExponent) {
    --more.exponent_;
    more.MultiplyBy<10>();
  }
  if (less.Mean(*this)) {
    less.AddCarry();  // round up
  }
  more.Mean(*this);  // rounded down
  while (less.digits_ < more.digits_) {
    less.digit_[less.digits_++] = 0;
  }
  while (more.digits_ < less.digits_) {
    more.digit_[more.digits_++] = 0;
  }
  int digits{more.digits_};
  int same{0};
  while (same < digits &&
      less.digit_[digits - 1 - same] == more.digit_[digits - 1 - same]) {
    ++same;
  }
  if (same == digits) {
    return;
  }
  digits_ = same + 1;
  int offset{digits - digits_};
  exponent_ += offset * log10Radix;
  for (int j{0}; j < digits_; ++j) {
    digit_[j] = more.digit_[j + offset];
  }
  Digit least{less.digit_[offset]};
  Digit my{digit_[0]};
  while (true) {
    Digit q{my / 10};
    Digit r{my - 10 * q};
    Digit lq{least / 10};
    Digit lr{least - 10 * lq};
    if (r != 0 && lq == q) {
      Digit sub{(r - lr) >> 1};
      digit_[0] -= sub;
      break;
    } else {
      least = lq;
      my = q;
      DivideBy<10>();
      ++exponent_;
    }
  }
  Normalize();
}

template<int PREC>
ConversionToDecimalResult ConvertToDecimal(char *buffer, size_t size, int flags,
    int digits, enum FortranRounding rounding,
    BinaryFloatingPointNumber<PREC> x) {
  if (x.IsNaN()) {
    return {"NaN", 3, 0};
  } else if (x.IsInfinite()) {
    return {x.IsNegative() ? "-Inf" : "+Inf", 4, 0};
  } else {
    using Binary = BinaryFloatingPointNumber<PREC>;
    using Big = BigRadixFloatingPointNumber<PREC>;
    Big number{x, rounding};
    if ((flags & Minimize) && !x.IsZero()) {
      // To emit the fewest decimal digits necessary to represent the value
      // in such a way that decimal-to-binary conversion to the same format
      // with a fixed assumption about rounding will return the same binary
      // value, we also perform binary-to-decimal conversion on the two
      // binary values immediately adjacent to this one, use them to identify
      // the bounds of the range of decimal values that will map back to the
      // original binary value, and find a (not necessary unique) shortest
      // decimal sequence in that range.
      Binary less{x};
      --less.raw;
      Binary more{x};
      if (!x.IsMaximalFiniteMagnitude()) {
        ++more.raw;
      }
      number.Minimize(Big{less, rounding}, Big{more, rounding});
    }
    number.Clamp(digits);
    return number.ConvertToDecimal(buffer, size, flags, digits);
  }
}

template ConversionToDecimalResult ConvertToDecimal<8>(char *, size_t, int, int,
    enum FortranRounding, BinaryFloatingPointNumber<8>);
template ConversionToDecimalResult ConvertToDecimal<11>(char *, size_t, int,
    int, enum FortranRounding, BinaryFloatingPointNumber<11>);
template ConversionToDecimalResult ConvertToDecimal<24>(char *, size_t, int,
    int, enum FortranRounding, BinaryFloatingPointNumber<24>);
template ConversionToDecimalResult ConvertToDecimal<53>(char *, size_t, int,
    int, enum FortranRounding, BinaryFloatingPointNumber<53>);
template ConversionToDecimalResult ConvertToDecimal<64>(char *, size_t, int,
    int, enum FortranRounding, BinaryFloatingPointNumber<64>);
template ConversionToDecimalResult ConvertToDecimal<112>(char *, size_t, int,
    int, enum FortranRounding, BinaryFloatingPointNumber<112>);
}

extern "C" {
ConversionToDecimalResult ConvertFloatToDecimal(char *buffer, size_t size,
    int flags, int digits, enum FortranRounding rounding, float x) {
  return Fortran::decimal::ConvertToDecimal(buffer, size, flags, digits,
      rounding, Fortran::decimal::BinaryFloatingPointNumber<24>(x));
}

ConversionToDecimalResult ConvertDoubleToDecimal(char *buffer, size_t size,
    int flags, int digits, enum FortranRounding rounding, double x) {
  return Fortran::decimal::ConvertToDecimal(buffer, size, flags, digits,
      rounding, Fortran::decimal::BinaryFloatingPointNumber<53>(x));
}

#if __x86_64__
ConversionToDecimalResult ConvertLongDoubleToDecimal(char *buffer, size_t size,
    int flags, int digits, enum FortranRounding rounding, long double x) {
  return Fortran::decimal::ConvertToDecimal(buffer, size, flags, digits,
      rounding, Fortran::decimal::BinaryFloatingPointNumber<64>(x));
}
#endif
}
