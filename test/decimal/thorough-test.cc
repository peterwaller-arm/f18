// Copyright (c) 2019, NVIDIA CORPORATION.  All rights reserved.
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

#include "../../lib/decimal/decimal.h"
#include <cinttypes>
#include <cstdio>
#include <cstring>
#include <iostream>

using namespace Fortran::decimal;

static std::uint64_t tests{0};
static std::uint64_t fails{0};

std::ostream &failed(float x) {
  ++fails;
  return std::cout << "FAIL: 0x" << std::hex
                   << *reinterpret_cast<std::uint32_t *>(&x) << std::dec;
}

void testReadback(float x, int flags) {
  char buffer[1024];
  if (!(tests & 0x3fffff)) {
    std::cerr << "\n0x" << std::hex << *reinterpret_cast<std::uint32_t *>(&x)
              << std::dec << ' ';
  } else if (!(tests & 0xffff)) {
    std::cerr << '.';
  }
  ++tests;
  auto result{ConvertFloatToDecimal(buffer, sizeof buffer,
      static_cast<enum DecimalConversionFlags>(flags), 1024, RoundNearest, x)};
  if (result.str == nullptr) {
    failed(x) << ' ' << flags << ": no result str\n";
  } else {
    float y{0};
    char *q{const_cast<char *>(result.str)};
    if ((*q >= '0' && *q <= '9') ||
        ((*q == '-' || *q == '+') && q[1] >= '0' && q[1] <= '9')) {
      int expo{result.decimalExponent};
      expo -= result.length;
      if (*q == '-' || *q == '+') {
        ++expo;
      }
      std::sprintf(q + result.length, "e%d", expo);
    }
    const char *p{q};
    auto rflags{ConvertDecimalToFloat(&p, &y, RoundNearest)};
    if (!(x == x)) {
      if (y == y || *p != '\0' || (rflags & Invalid)) {
        failed(x) << " (NaN) " << flags << ": -> '" << result.str << "' -> 0x"
                  << std::hex << *reinterpret_cast<std::uint32_t *>(&y)
                  << std::dec << " '" << p << "' " << rflags << '\n';
      }
    } else if (x != y || *p != '\0' || (rflags & Invalid)) {
      failed(x) << ' ' << flags << ": -> '" << result.str << "' -> 0x"
                << std::hex << *reinterpret_cast<std::uint32_t *>(&y)
                << std::dec << " '" << p << "' " << rflags << '\n';
    }
  }
}

int main() {
  float x;
  std::uint32_t *ix{reinterpret_cast<std::uint32_t *>(&x)};
  for (*ix = 0; *ix < 0x7f800010; ++*ix) {
    testReadback(x, 0);
    testReadback(-x, 0);
    testReadback(x, Minimize);
    testReadback(-x, Minimize);
  }
  std::cout << '\n' << tests << " tests run, " << fails << " tests failed\n";
  return fails > 0;
}
