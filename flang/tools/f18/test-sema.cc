#include "../../lib/parser/parsing.h"
#include "../../lib/semantics/attr.h"
#include "../../lib/semantics/type.h"
#include <cstdlib>
#include <iostream>
#include <list>
#include <optional>
#include <sstream>
#include <string>
#include <stddef.h>

using namespace Fortran;
using namespace parser;

extern void DoSemanticAnalysis(const CookedSource &, const Program &);

//static void visitProgramUnit(const ProgramUnit &unit);

int main(int argc, char *const argv[]) {
  if (argc != 2) {
    std::cerr << "Expected 1 source file, got " << (argc - 1) << "\n";
    return EXIT_FAILURE;
  }
  std::string path{argv[1]};
  Parsing parsing;
  if (!parsing.Prescan(path, Options{}) || !parsing.Parse()) {
    std::cerr << "parse FAILED\n";
    parsing.messages().Emit(std::cerr);
    return EXIT_FAILURE;
  }
  DoSemanticAnalysis(parsing.messages().cooked(), parsing.parseTree());
  return EXIT_SUCCESS;
}
