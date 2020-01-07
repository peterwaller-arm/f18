//===-- lib/evaluate/call.h -------------------------------------*- C++ -*-===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//----------------------------------------------------------------------------//

#ifndef FORTRAN_EVALUATE_CALL_H_
#define FORTRAN_EVALUATE_CALL_H_

#include "common.h"
#include "constant.h"
#include "formatting.h"
#include "type.h"
#include "../common/indirection.h"
#include "../common/reference.h"
#include "../parser/char-block.h"
#include "../semantics/attr.h"
#include <optional>
#include <ostream>
#include <vector>

namespace Fortran::semantics {
class Symbol;
}

// Mutually referential data structures are represented here with forward
// declarations of hitherto undefined class types and a level of indirection.
namespace Fortran::evaluate {
class Component;
class IntrinsicProcTable;
}
namespace Fortran::evaluate::characteristics {
struct DummyArgument;
struct Procedure;
}

extern template class Fortran::common::Indirection<Fortran::evaluate::Component,
    true>;
extern template class Fortran::common::Indirection<
    Fortran::evaluate::characteristics::Procedure, true>;

namespace Fortran::evaluate {

using semantics::Symbol;
using SymbolRef = common::Reference<const Symbol>;

class ActualArgument {
public:
  // Dummy arguments that are TYPE(*) can be forwarded as actual arguments.
  // Since that's the only thing one may do with them in Fortran, they're
  // represented in expressions as a special case of an actual argument.
  class AssumedType {
  public:
    explicit AssumedType(const Symbol &);
    DEFAULT_CONSTRUCTORS_AND_ASSIGNMENTS(AssumedType)
    const Symbol &symbol() const { return symbol_; }
    int Rank() const;
    bool operator==(const AssumedType &that) const {
      return &*symbol_ == &*that.symbol_;
    }
    std::ostream &AsFortran(std::ostream &) const;

  private:
    SymbolRef symbol_;
  };

  DECLARE_CONSTRUCTORS_AND_ASSIGNMENTS(ActualArgument)
  explicit ActualArgument(Expr<SomeType> &&);
  explicit ActualArgument(common::CopyableIndirection<Expr<SomeType>> &&);
  explicit ActualArgument(AssumedType);
  ~ActualArgument();
  ActualArgument &operator=(Expr<SomeType> &&);

  Expr<SomeType> *UnwrapExpr() {
    if (auto *p{
            std::get_if<common::CopyableIndirection<Expr<SomeType>>>(&u_)}) {
      return &p->value();
    } else {
      return nullptr;
    }
  }
  const Expr<SomeType> *UnwrapExpr() const {
    if (const auto *p{
            std::get_if<common::CopyableIndirection<Expr<SomeType>>>(&u_)}) {
      return &p->value();
    } else {
      return nullptr;
    }
  }

  const Symbol *GetAssumedTypeDummy() const {
    if (const AssumedType * aType{std::get_if<AssumedType>(&u_)}) {
      return &aType->symbol();
    } else {
      return nullptr;
    }
  }

  std::optional<DynamicType> GetType() const;
  int Rank() const;
  bool operator==(const ActualArgument &) const;
  std::ostream &AsFortran(std::ostream &) const;

  std::optional<parser::CharBlock> keyword() const { return keyword_; }
  void set_keyword(parser::CharBlock x) { keyword_ = x; }
  bool isAlternateReturn() const { return isAlternateReturn_; }
  void set_isAlternateReturn() { isAlternateReturn_ = true; }
  bool isPassedObject() const { return isPassedObject_; }
  void set_isPassedObject(bool yes = true) { isPassedObject_ = yes; }

  bool Matches(const characteristics::DummyArgument &) const;
  common::Intent dummyIntent() const { return dummyIntent_; }
  ActualArgument &set_dummyIntent(common::Intent intent) {
    dummyIntent_ = intent;
    return *this;
  }

  // Wrap this argument in parentheses
  void Parenthesize();

  // TODO: Mark legacy %VAL and %REF arguments

private:
  // Subtlety: There is a distinction that must be maintained here between an
  // actual argument expression that is a variable and one that is not,
  // e.g. between X and (X).  The parser attempts to parse each argument
  // first as a variable, then as an expression, and the distinction appears
  // in the parse tree.
  std::variant<common::CopyableIndirection<Expr<SomeType>>, AssumedType> u_;
  std::optional<parser::CharBlock> keyword_;
  bool isAlternateReturn_{false};  // whether expr is a "*label" number
  bool isPassedObject_{false};
  common::Intent dummyIntent_{common::Intent::Default};
};

using ActualArguments = std::vector<std::optional<ActualArgument>>;

// Intrinsics are identified by their names and the characteristics
// of their arguments, at least for now.
using IntrinsicProcedure = std::string;

struct SpecificIntrinsic {
  SpecificIntrinsic(IntrinsicProcedure, characteristics::Procedure &&);
  DECLARE_CONSTRUCTORS_AND_ASSIGNMENTS(SpecificIntrinsic)
  ~SpecificIntrinsic();
  bool operator==(const SpecificIntrinsic &) const;
  std::ostream &AsFortran(std::ostream &) const;

  IntrinsicProcedure name;
  bool isRestrictedSpecific{false};  // if true, can only call it, not pass it
  common::CopyableIndirection<characteristics::Procedure> characteristics;
};

struct ProcedureDesignator {
  EVALUATE_UNION_CLASS_BOILERPLATE(ProcedureDesignator)
  explicit ProcedureDesignator(SpecificIntrinsic &&i) : u{std::move(i)} {}
  explicit ProcedureDesignator(const Symbol &n) : u{n} {}
  explicit ProcedureDesignator(Component &&);

  // Exactly one of these will return a non-null pointer.
  const SpecificIntrinsic *GetSpecificIntrinsic() const;
  const Symbol *GetSymbol() const;  // symbol or component symbol

  // For references to NOPASS components and bindings only.
  // References to PASS components and bindings are represented
  // with the symbol below and the base object DataRef in the
  // passed-object ActualArgument.
  // Always null when the procedure is intrinsic.
  const Component *GetComponent() const;

  const Symbol *GetInterfaceSymbol() const;

  std::string GetName() const;
  std::optional<DynamicType> GetType() const;
  int Rank() const;
  bool IsElemental() const;
  std::optional<Expr<SubscriptInteger>> LEN() const;
  std::ostream &AsFortran(std::ostream &) const;

  std::variant<SpecificIntrinsic, SymbolRef,
      common::CopyableIndirection<Component>>
      u;
};

class ProcedureRef {
public:
  CLASS_BOILERPLATE(ProcedureRef)
  ProcedureRef(ProcedureDesignator &&p, ActualArguments &&a)
    : proc_{std::move(p)}, arguments_(std::move(a)) {}
  ~ProcedureRef();

  ProcedureDesignator &proc() { return proc_; }
  const ProcedureDesignator &proc() const { return proc_; }
  ActualArguments &arguments() { return arguments_; }
  const ActualArguments &arguments() const { return arguments_; }

  std::optional<Expr<SubscriptInteger>> LEN() const;
  int Rank() const;
  bool IsElemental() const { return proc_.IsElemental(); }
  bool operator==(const ProcedureRef &) const;
  std::ostream &AsFortran(std::ostream &) const;

protected:
  ProcedureDesignator proc_;
  ActualArguments arguments_;
};

template<typename A> class FunctionRef : public ProcedureRef {
public:
  using Result = A;
  CLASS_BOILERPLATE(FunctionRef)
  explicit FunctionRef(ProcedureRef &&pr) : ProcedureRef{std::move(pr)} {}
  FunctionRef(ProcedureDesignator &&p, ActualArguments &&a)
    : ProcedureRef{std::move(p), std::move(a)} {}

  std::optional<DynamicType> GetType() const { return proc_.GetType(); }
  std::optional<Constant<Result>> Fold(FoldingContext &);  // for intrinsics
};

FOR_EACH_SPECIFIC_TYPE(extern template class FunctionRef, )
}
#endif  // FORTRAN_EVALUATE_CALL_H_
