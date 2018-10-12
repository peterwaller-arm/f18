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

#ifndef FORTRAN_COMMON_CONSTEXPR_BITSET_H_
#define FORTRAN_COMMON_CONSTEXPR_BITSET_H_

// Implements a replacement for std::bitset<> that is suitable for use
// in constexpr expressions.  Limited to elements in [0..63].

#include "bit-population-count.h"
#include <cstddef>
#include <cstdint>
#include <initializer_list>
#include <optional>
#include <type_traits>

namespace Fortran::common {

template<int BITS> class BitSet {
  static_assert(BITS > 0 && BITS <= 64);
  static constexpr bool partialWord{BITS != 32 && BITS != 64};
  using Word = std::conditional_t<(BITS > 32), std::uint64_t, std::uint32_t>;
  static constexpr Word allBits{
      partialWord ? (static_cast<Word>(1) << BITS) - 1 : ~static_cast<Word>(0)};

  constexpr BitSet(Word b) : bits_{b} {}

public:
  constexpr BitSet() {}
  constexpr BitSet(const std::initializer_list<int> &xs) {
    for (auto x : xs) {
      set(x);
    }
  }
  constexpr BitSet(const BitSet &) = default;
  constexpr BitSet(BitSet &&) = default;
  constexpr BitSet &operator=(const BitSet &) = default;
  constexpr BitSet &operator=(BitSet &&) = default;

  constexpr BitSet &operator&=(const BitSet &that) {
    bits_ &= that.bits_;
    return *this;
  }
  constexpr BitSet &operator&=(BitSet &&that) {
    bits_ &= that.bits_;
    return *this;
  }
  constexpr BitSet &operator^=(const BitSet &that) {
    bits_ ^= that.bits_;
    return *this;
  }
  constexpr BitSet &operator^=(BitSet &&that) {
    bits_ ^= that.bits_;
    return *this;
  }
  constexpr BitSet &operator|=(const BitSet &that) {
    bits_ |= that.bits_;
    return *this;
  }
  constexpr BitSet &operator|=(BitSet &&that) {
    bits_ |= that.bits_;
    return *this;
  }

  constexpr BitSet operator~() const { return ~bits_; }
  constexpr BitSet operator&(const BitSet &that) const {
    return bits_ & that.bits_;
  }
  constexpr BitSet operator&(BitSet &&that) const { return bits_ & that.bits_; }
  constexpr BitSet operator^(const BitSet &that) const {
    return bits_ ^ that.bits_;
  }
  constexpr BitSet operator^(BitSet &&that) const { return bits_ & that.bits_; }
  constexpr BitSet operator|(const BitSet &that) const {
    return bits_ | that.bits_;
  }
  constexpr BitSet operator|(BitSet &&that) const { return bits_ | that.bits_; }

  constexpr bool operator==(const BitSet &that) const {
    return bits_ == that.bits_;
  }
  constexpr bool operator==(BitSet &&that) const { return bits_ == that.bits_; }
  constexpr bool operator!=(const BitSet &that) const {
    return bits_ == that.bits_;
  }
  constexpr bool operator!=(BitSet &&that) const { return bits_ == that.bits_; }

  static constexpr std::size_t size() { return BITS; }
  constexpr bool test(std::size_t x) const {
    return x < BITS && ((bits_ >> x) & 1) != 0;
  }

  constexpr bool all() const { return bits_ == allBits; }
  constexpr bool any() const { return bits_ != 0; }
  constexpr bool none() const { return bits_ == 0; }

  constexpr std::size_t count() const { return BitPopulationCount(bits_); }

  constexpr BitSet &set() {
    bits_ = allBits;
    return *this;
  }
  constexpr BitSet set(std::size_t x, bool value = true) {
    if (!value) {
      return reset(x);
    } else {
      bits_ |= static_cast<Word>(1) << x;
      return *this;
    }
  }
  constexpr BitSet &reset() {
    bits_ = 0;
    return *this;
  }
  constexpr BitSet &reset(std::size_t x) {
    bits_ &= ~(static_cast<Word>(1) << x);
    return *this;
  }
  constexpr BitSet &flip() {
    bits_ ^= allBits;
    return *this;
  }
  constexpr BitSet &flip(std::size_t x) {
    bits_ ^= static_cast<Word>(1) << x;
    return *this;
  }

  constexpr std::optional<std::size_t> LeastElement() const {
    if (bits_ == 0) {
      return std::nullopt;
    } else {
      return {TrailingZeroCount(bits_)};
    }
  }

  Word bits() const { return bits_; }

private:
  Word bits_{0};
};
}  // namespace Fortran::common
#endif  // FORTRAN_COMMON_CONSTEXPR_BITSET_H_
