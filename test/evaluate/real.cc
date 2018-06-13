// Copyright (c) 2018, NVIDIA CORPORATION.  All rights reserved.
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

#include "fp-testing.h"
#include "testing.h"
#include "../../lib/evaluate/type.h"
#include <cstdio>
#include <cstdlib>

using namespace Fortran::evaluate;

using Real2 = typename type::Real<2>::ValueType;
using Real4 = typename type::Real<4>::ValueType;
using Real8 = typename type::Real<8>::ValueType;
using Real10 = typename type::Real<10>::ValueType;
using Real16 = typename type::Real<16>::ValueType;
using Integer4 = typename type::Integer<4>::ValueType;
using Integer8 = typename type::Integer<8>::ValueType;

template<typename R> void basicTests(int rm, Rounding rounding) {
  char desc[64];
  using Word = typename R::Word;
  std::snprintf(
      desc, sizeof desc, "bits=%d, le=%d", R::bits, Word::littleEndian);
  R zero;
  TEST(!zero.IsNegative())(desc);
  TEST(!zero.IsNotANumber())(desc);
  TEST(!zero.IsInfinite())(desc);
  TEST(zero.IsZero())(desc);
  MATCH(0, zero.Exponent())(desc);
  TEST(zero.RawBits().IsZero())(desc);
  MATCH(0, zero.RawBits().ToUInt64())(desc);
  TEST(zero.ABS().RawBits().IsZero())(desc);
  TEST(zero.Negate().RawBits().IEOR(Word::MASKL(1)).IsZero())(desc);
  TEST(zero.Compare(zero) == Relation::Equal)(desc);
  R minusZero{Word{std::uint64_t{1}}.SHIFTL(R::bits - 1)};
  TEST(minusZero.IsNegative())(desc);
  TEST(!minusZero.IsNotANumber())(desc);
  TEST(!minusZero.IsInfinite())(desc);
  TEST(minusZero.IsZero())(desc);
  TEST(minusZero.ABS().RawBits().IsZero())(desc);
  TEST(minusZero.Negate().RawBits().IsZero())(desc);
  MATCH(0, minusZero.Exponent())(desc);
  MATCH(0, minusZero.RawBits().LEADZ())(desc);
  MATCH(1, minusZero.RawBits().POPCNT())(desc);
  TEST(minusZero.Compare(minusZero) == Relation::Equal)(desc);
  TEST(zero.Compare(minusZero) == Relation::Equal)(desc);
  ValueWithRealFlags<R> vr;
  MATCH(0, vr.value.RawBits().ToUInt64())(desc);
  TEST(vr.flags.empty())(desc);
  R nan{Word{std::uint64_t{1}}
            .SHIFTL(R::bits)
            .SubtractSigned(Word{std::uint64_t{1}})
            .value};
  MATCH(R::bits, nan.RawBits().POPCNT())(desc);
  TEST(!nan.IsNegative())(desc);
  TEST(nan.IsNotANumber())(desc);
  TEST(!nan.IsInfinite())(desc);
  TEST(!nan.IsZero())(desc);
  TEST(zero.Compare(nan) == Relation::Unordered)(desc);
  TEST(minusZero.Compare(nan) == Relation::Unordered)(desc);
  TEST(nan.Compare(zero) == Relation::Unordered)(desc);
  TEST(nan.Compare(minusZero) == Relation::Unordered)(desc);
  TEST(nan.Compare(nan) == Relation::Unordered)(desc);
  int significandBits{R::precision - R::implicitMSB};
  int exponentBits{R::bits - significandBits - 1};
  std::uint64_t maxExponent{(std::uint64_t{1} << exponentBits) - 1};
  MATCH(nan.Exponent(), maxExponent)(desc);
  R inf{Word{maxExponent}.SHIFTL(significandBits)};
  TEST(!inf.IsNegative())(desc);
  TEST(!inf.IsNotANumber())(desc);
  TEST(inf.IsInfinite())(desc);
  TEST(!inf.IsZero())(desc);
  TEST(inf.RawBits().CompareUnsigned(inf.ABS().RawBits()) == Ordering::Equal)
  (desc);
  TEST(zero.Compare(inf) == Relation::Less)(desc);
  TEST(minusZero.Compare(inf) == Relation::Less)(desc);
  TEST(nan.Compare(inf) == Relation::Unordered)(desc);
  TEST(inf.Compare(inf) == Relation::Equal)(desc);
  R negInf{Word{maxExponent}.SHIFTL(significandBits).IOR(Word::MASKL(1))};
  TEST(negInf.IsNegative())(desc);
  TEST(!negInf.IsNotANumber())(desc);
  TEST(negInf.IsInfinite())(desc);
  TEST(!negInf.IsZero())(desc);
  TEST(inf.RawBits().CompareUnsigned(negInf.ABS().RawBits()) == Ordering::Equal)
  (desc);
  TEST(inf.RawBits().CompareUnsigned(negInf.Negate().RawBits()) ==
      Ordering::Equal)
  (desc);
  TEST(inf.Negate().RawBits().CompareUnsigned(negInf.RawBits()) ==
      Ordering::Equal)
  (desc);
  TEST(zero.Compare(negInf) == Relation::Greater)(desc);
  TEST(minusZero.Compare(negInf) == Relation::Greater)(desc);
  TEST(nan.Compare(negInf) == Relation::Unordered)(desc);
  TEST(inf.Compare(negInf) == Relation::Greater)(desc);
  TEST(negInf.Compare(negInf) == Relation::Equal)(desc);
  for (std::uint64_t j{0}; j < 63; ++j) {
    char ldesc[128];
    std::uint64_t x{1};
    x <<= j;
    std::snprintf(ldesc, sizeof ldesc, "%s j=%d x=0x%llx rm=%d", desc,
        static_cast<int>(j), static_cast<unsigned long long>(x), rm);
    Integer8 ix{x};
    TEST(!ix.IsNegative())(ldesc);
    MATCH(x, ix.ToUInt64())(ldesc);
    vr = R::FromInteger(ix, rounding);
    TEST(!vr.value.IsNegative())(ldesc);
    TEST(!vr.value.IsNotANumber())(ldesc);
    TEST(!vr.value.IsZero())(ldesc);
    auto ivf = vr.value.template ToInteger<Integer8>();
    if (j > (maxExponent / 2)) {
      TEST(vr.flags.test(RealFlag::Overflow))(ldesc);
      TEST(vr.value.IsInfinite())(ldesc);
      TEST(ivf.flags.test(RealFlag::Overflow))(ldesc);
      MATCH(0x7fffffffffffffff, ivf.value.ToUInt64())(ldesc);
    } else {
      TEST(vr.flags.empty())(ldesc);
      TEST(!vr.value.IsInfinite())(ldesc);
      TEST(ivf.flags.empty())(ldesc);
      MATCH(x, ivf.value.ToUInt64())(ldesc);
    }
    ix = ix.Negate().value;
    TEST(ix.IsNegative())(ldesc);
    x = -x;
    std::int64_t nx = x;
    MATCH(x, ix.ToUInt64())(ldesc);
    MATCH(nx, ix.ToInt64())(ldesc);
    vr = R::FromInteger(ix);
    TEST(vr.value.IsNegative())(ldesc);
    TEST(!vr.value.IsNotANumber())(ldesc);
    TEST(!vr.value.IsZero())(ldesc);
    ivf = vr.value.template ToInteger<Integer8>();
    if (j > (maxExponent / 2)) {
      TEST(vr.flags.test(RealFlag::Overflow))(ldesc);
      TEST(vr.value.IsInfinite())(ldesc);
      TEST(ivf.flags.test(RealFlag::Overflow))(ldesc);
      MATCH(0x8000000000000000, ivf.value.ToUInt64())(ldesc);
    } else {
      TEST(vr.flags.empty())(ldesc);
      TEST(!vr.value.IsInfinite())(ldesc);
      TEST(ivf.flags.empty())(ldesc);
      MATCH(x, ivf.value.ToUInt64())(ldesc);
      MATCH(nx, ivf.value.ToInt64())(ldesc);
    }
  }
}

// Takes a 13-bit (or more) number and distributes its bits across a 32-bit
// single precision real.  All sign and exponent bit positions are tested, plus
// the upper two bits and lowest bit in the significand.  The middle bits
// of the significand are either all zeroes or all ones.
std::uint32_t MakeReal(std::uint32_t n) {
  std::uint32_t n0_8{n & 0x1ff};
  std::uint32_t n9{(n >> 9) & 1};
  std::uint32_t n10{(n >> 10) & 1};
  std::uint32_t n11{(n >> 11) & 1};
  std::uint32_t n12{(n >> 12) & 1};
  std::uint32_t n13{(n >> 13) & 1};
  std::uint32_t n14{(n >> 14) & 1};
  return ((n0_8 << 23) | (n9 << 22) | (n11 << 21) | (n14 << 16) |
          (n13 << 1) | n10) ^ -n12;
}

std::uint32_t NormalizeNaN(std::uint32_t x) {
  if ((x & 0x7f800000) == 0x7f800000 && (x & 0x007fffff) != 0) {
    x = 0x7fe00000;
  }
  return x;
}

std::uint32_t FlagsToBits(const RealFlags &flags) {
  std::uint32_t bits{0};
#ifndef __clang__
  // TODO: clang support for fenv.h is broken, so tests of flag settings
  // are disabled.
  if (flags.test(RealFlag::Overflow)) {
    bits |= 1;
  }
  if (flags.test(RealFlag::DivideByZero)) {
    bits |= 2;
  }
  if (flags.test(RealFlag::InvalidArgument)) {
    bits |= 4;
  }
  if (flags.test(RealFlag::Underflow)) {
    bits |= 8;
  }
  if (flags.test(RealFlag::Inexact)) {
    bits |= 0x10;
  }
#endif  // __clang__
  return bits;
}

void inttest(std::int64_t x, int pass, Rounding rounding) {
  union {
    std::uint32_t u32;
    float f;
  } u;
  ScopedHostFloatingPointEnvironment fpenv;
  Integer8 ix{x};
  ValueWithRealFlags<Real4> real;
  real = real.value.FromInteger(ix, rounding);
#ifndef __clang__  // broken and also slow
  fpenv.ClearFlags();
#endif
  float fcheck = x;  // TODO unsigned too
  auto actualFlags{FlagsToBits(fpenv.CurrentFlags())};
  u.f = fcheck;
  std::uint32_t rcheck{NormalizeNaN(u.u32)};
  std::uint32_t check = real.value.RawBits().ToUInt64();
  MATCH(rcheck, check)("%d 0x%llx", pass, x);
  MATCH(actualFlags, FlagsToBits(real.flags))("%d 0x%llx", pass, x);
}

void subset32bit(int pass, Rounding rounding, std::uint32_t opds) {
  for (int j{0}; j < 63; ++j) {
    std::int64_t x{1};
    x <<= j;
    inttest(x, pass, rounding);
    inttest(-x, pass, rounding);
  }
  inttest(0, pass, rounding);
  inttest(static_cast<std::int64_t>(0x8000000000000000), pass, rounding);

  union {
    std::uint32_t u32;
    float f;
  } u;
  ScopedHostFloatingPointEnvironment fpenv;

  for (std::uint32_t j{0}; j < opds; ++j) {
    std::uint32_t rj{MakeReal(j)};
    u.u32 = rj;
    float fj{u.f};
    Real4 x{Integer4{std::uint64_t{rj}}};
    for (std::uint32_t k{0}; k < opds; ++k) {
      std::uint32_t rk{MakeReal(k)};
      u.u32 = rk;
      float fk{u.f};
      Real4 y{Integer4{std::uint64_t{rk}}};
      {
        ValueWithRealFlags<Real4> sum{x.Add(y, rounding)};
#ifndef __clang__  // broken and also slow
        fpenv.ClearFlags();
#endif
        float fcheck{fj + fk};
        auto actualFlags{FlagsToBits(fpenv.CurrentFlags())};
        u.f = fcheck;
        std::uint32_t rcheck{NormalizeNaN(u.u32)};
        std::uint32_t check = sum.value.RawBits().ToUInt64();
        MATCH(rcheck, check)("%d 0x%x + 0x%x", pass, rj, rk);
        MATCH(actualFlags, FlagsToBits(sum.flags))
        ("%d 0x%x + 0x%x", pass, rj, rk);
      }
      {
        ValueWithRealFlags<Real4> diff{x.Subtract(y, rounding)};
#ifndef __clang__  // broken and also slow
        fpenv.ClearFlags();
#endif
        float fcheck{fj - fk};
        auto actualFlags{FlagsToBits(fpenv.CurrentFlags())};
        u.f = fcheck;
        std::uint32_t rcheck{NormalizeNaN(u.u32)};
        std::uint32_t check = diff.value.RawBits().ToUInt64();
        MATCH(rcheck, check)("%d 0x%x - 0x%x", pass, rj, rk);
        MATCH(actualFlags, FlagsToBits(diff.flags))
        ("%d 0x%x - 0x%x", pass, rj, rk);
      }
      {
        ValueWithRealFlags<Real4> prod{x.Multiply(y, rounding)};
#ifndef __clang__  // broken and also slow
        fpenv.ClearFlags();
#endif
        float fcheck{fj * fk};
        auto actualFlags{FlagsToBits(fpenv.CurrentFlags())};
        u.f = fcheck;
        std::uint32_t rcheck{NormalizeNaN(u.u32)};
        std::uint32_t check = prod.value.RawBits().ToUInt64();
        MATCH(rcheck, check)("%d 0x%x * 0x%x", pass, rj, rk);
        MATCH(actualFlags, FlagsToBits(prod.flags))
        ("%d 0x%x * 0x%x -> 0x%x", pass, rj, rk, rcheck);
      }
      {
        ValueWithRealFlags<Real4> quot{x.Divide(y, rounding)};
#ifndef __clang__  // broken and also slow
        fpenv.ClearFlags();
#endif
        float fcheck{fj / fk};
        auto actualFlags{FlagsToBits(fpenv.CurrentFlags())};
        u.f = fcheck;
        std::uint32_t rcheck{NormalizeNaN(u.u32)};
        std::uint32_t check = quot.value.RawBits().ToUInt64();
        MATCH(rcheck, check)("%d 0x%x / 0x%x", pass, rj, rk);
        MATCH(actualFlags, FlagsToBits(quot.flags))
        ("%d 0x%x / 0x%x", pass, rj, rk);
      }
    }
  }
}

void roundTest(int rm, Rounding rounding, std::uint32_t opds) {
  basicTests<Real2>(rm, rounding);
  basicTests<Real4>(rm, rounding);
  basicTests<Real8>(rm, rounding);
  basicTests<Real10>(rm, rounding);
  basicTests<Real16>(rm, rounding);
  ScopedHostFloatingPointEnvironment::SetRounding(rounding);
  subset32bit(rm, rounding, opds);
}

int main() {
  std::uint32_t opds{1024};
  if (const char *p{std::getenv("REAL_TEST_OPERANDS")}) {
    // Use 8192 for more exhaustive testing.
    opds = std::atol(p);
  }
  roundTest(0, Rounding::TiesToEven, opds);
  roundTest(1, Rounding::ToZero, opds);
  roundTest(2, Rounding::Up, opds);
  roundTest(3, Rounding::Down, opds);
  // TODO: how to test Rounding::TiesAwayFromZero on x86?
}
