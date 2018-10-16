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

#ifndef FORTRAN_SEMANTICS_RESOLVE_NAMES_H_
#define FORTRAN_SEMANTICS_RESOLVE_NAMES_H_

#include <iosfwd>
#include <string>
#include <vector>

namespace Fortran::parser {
class Messages;
struct Program;
}  // namespace Fortran::parser

namespace Fortran::semantics {

class Scope;
class IntrinsicTypeDefaultKinds;

void ResolveNames(parser::Messages &, Scope &, const parser::Program &,
    const std::vector<std::string> &, const IntrinsicTypeDefaultKinds &);
void DumpSymbols(std::ostream &);

}  // namespace Fortran::semantics

#endif  // FORTRAN_SEMANTICS_RESOLVE_NAMES_H_
