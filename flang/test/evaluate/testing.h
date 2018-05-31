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

#ifndef FORTRAN_EVALUATE_TESTING_H_
#define FORTRAN_EVALUATE_TESTING_H_

namespace testing {

// Returns EXIT_SUCCESS or EXIT_FAILURE, so a test's main() should end
// with "return testing::Complete()".
int Complete();

// Pass/fail testing.  These macros return a pointer to a printf-like
// function that can be optionally called to print more detail, e.g.
//   COMPARE(x, ==, y)("z is 0x%llx", z);
// will also print z after the usual failure message if x != y.
#define TEST(predicate) \
  testing::Test(__FILE__, __LINE__, #predicate, (predicate))
#define MATCH(want, got) \
  testing::Match(__FILE__, __LINE__, (want), #got, (got))
#define COMPARE(x, rel, y) \
  testing::Compare(__FILE__, __LINE__, #x, #rel, #y, (x), (y))

// Functions called by thesemacros; do not call directly.
using FailureDetailPrinter = void (*)(const char *, ...);
FailureDetailPrinter Test(
    const char *file, int line, const char *predicate, bool pass);
FailureDetailPrinter Match(const char *file, int line,
    unsigned long long want, const char *gots, unsigned long long got);
FailureDetailPrinter Compare(const char *file, int line, const char *xs,
    const char *rel, const char *ys, unsigned long long x,
    unsigned long long y);
}  // namespace testing
#endif  // FORTRAN_EVALUATE_TESTING_H_
