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

#ifndef FORTRAN_EVALUATE_TOOLS_H_
#define FORTRAN_EVALUATE_TOOLS_H_

#include "expression.h"
#include "../common/idioms.h"
#include "../parser/message.h"
#include <array>
#include <optional>
#include <utility>

namespace Fortran::evaluate {

// Convenience functions and operator overloadings for expression construction.

template<TypeCategory C, int K>
Expr<Type<C, K>> operator-(Expr<Type<C, K>> &&x) {
  return {Negate<Type<C, K>>{std::move(x)}};
}

template<TypeCategory C, int K>
Expr<Type<C, K>> operator+(Expr<Type<C, K>> &&x, Expr<Type<C, K>> &&y) {
  return {Add<Type<C, K>>{std::move(x), std::move(y)}};
}

template<TypeCategory C, int K>
Expr<Type<C, K>> operator-(Expr<Type<C, K>> &&x, Expr<Type<C, K>> &&y) {
  return {Subtract<Type<C, K>>{std::move(x), std::move(y)}};
}

template<TypeCategory C, int K>
Expr<Type<C, K>> operator*(Expr<Type<C, K>> &&x, Expr<Type<C, K>> &&y) {
  return {Multiply<Type<C, K>>{std::move(x), std::move(y)}};
}

template<TypeCategory C, int K>
Expr<Type<C, K>> operator/(Expr<Type<C, K>> &&x, Expr<Type<C, K>> &&y) {
  return {Divide<Type<C, K>>{std::move(x), std::move(y)}};
}

template<TypeCategory C> Expr<SomeKind<C>> operator-(Expr<SomeKind<C>> &&x) {
  return std::visit(
      [](auto &xk) { return Expr<SomeKind<C>>{-std::move(xk)}; }, x.u);
}

// TODO pmk revisit these below for type safety

template<TypeCategory C>
Expr<SomeKind<C>> operator+(Expr<SomeKind<C>> &&x, Expr<SomeKind<C>> &&y) {
  return std::visit(
      [](auto &xk, auto &yk) {
        return Expr<SomeKind<C>>{std::move(xk) + std::move(yk)};
      },
      x.u, y.u);
}

template<TypeCategory C>
Expr<SomeKind<C>> operator-(Expr<SomeKind<C>> &&x, Expr<SomeKind<C>> &&y) {
  return std::visit(
      [](auto &xk, auto &yk) {
        return Expr<SomeKind<C>>{std::move(xk) - std::move(yk)};
      },
      x.u, y.u);
}

template<TypeCategory C>
Expr<SomeKind<C>> operator*(Expr<SomeKind<C>> &&x, Expr<SomeKind<C>> &&y) {
  return std::visit(
      [](auto &xk, auto &yk) {
        return Expr<SomeKind<C>>{std::move(xk) * std::move(yk)};
      },
      x.u, y.u);
}

template<TypeCategory C>
Expr<SomeKind<C>> operator/(Expr<SomeKind<C>> &&x, Expr<SomeKind<C>> &&y) {
  return std::visit(
      [](auto &xk, auto &yk) {
        return Expr<SomeKind<C>>{std::move(xk) / std::move(yk)};
      },
      x.u, y.u);
}

// Generalizing packagers: these take operations and expressions of more
// specific types and wrap them in Expr<> containers of more abstract types.
// TODO: Would these be better as conversion constructors in the classes?
// TODO: Are the lvalue argument versions still needed?

template<typename A> Expr<ResultType<A>> AsExpr(const A &x) { return {x}; }
template<typename A> Expr<ResultType<A>> AsExpr(A &&x) {
  return {std::move(x)};
}

template<TypeCategory CAT, int KIND>
Expr<SomeKind<CAT>> AsCategoryExpr(const Expr<Type<CAT, KIND>> &x) {
  return {x};
}

template<TypeCategory CAT, int KIND>
Expr<SomeKind<CAT>> AsCategoryExpr(Expr<Type<CAT, KIND>> &&x) {
  return {std::move(x)};
}

template<TypeCategory CAT>
Expr<SomeKind<CAT>> AsCategoryExpr(SomeKindScalar<CAT> &&x) {
  return std::visit(
      [](auto &&scalar) {
        using Ty = TypeOf<std::decay_t<decltype(scalar)>>;
        return Expr<SomeKind<CAT>>{Expr<Ty>{Constant<Ty>{std::move(scalar)}}};
      },
      x.u);
}

template<typename A> Expr<SomeType> AsGenericExpr(const A &x) { return {x}; }

template<typename A> Expr<SomeType> AsGenericExpr(A &&x) {
  return {std::move(x)};
}

template<TypeCategory CAT, int KIND>
Expr<SomeType> AsGenericExpr(const Expr<Type<CAT, KIND>> &x) {
  return {AsCategoryExpr(x)};
}
template<TypeCategory CAT, int KIND>
Expr<SomeType> AsGenericExpr(Expr<Type<CAT, KIND>> &&x) {
  return {AsCategoryExpr(std::move(x))};
}

template<> inline Expr<SomeType> AsGenericExpr(Constant<SomeType> &&x) {
  return std::visit(
      [](auto &&scalar) {
        using Ty = TypeOf<std::decay_t<decltype(scalar)>>;
        return Expr<SomeType>{Expr<SomeKind<Ty::category>>{
            Expr<Ty>{Constant<Ty>{std::move(scalar)}}}};
      },
      x.value.u);
}

template<> inline Expr<SomeType> AsGenericExpr(GenericScalar &&x) {
  return std::visit(
      [](auto &&scalar) {
        using Ty = TypeOf<std::decay_t<decltype(scalar)>>;
        return Expr<SomeType>{Expr<SomeKind<Ty::category>>{
            Expr<Ty>{Constant<Ty>{std::move(scalar)}}}};
      },
      x.u);
}

// Creation of conversion expressions can be done to either a known
// specific intrinsic type with ConvertToType<T>(x) or by converting
// one arbitrary expression to the type of another with ConvertTo(to, from).

template<typename TO, TypeCategory FC>
Expr<TO> ConvertToType(Expr<SomeKind<FC>> &&x) {
  return {Convert<TO, FC>{std::move(x)}};
}

template<TypeCategory TC, int TK, TypeCategory FC>
Expr<Type<TC, TK>> ConvertTo(
    const Expr<Type<TC, TK>> &, Expr<SomeKind<FC>> &&x) {
  return ConvertToType<Type<TC, TK>>(std::move(x));
}

template<TypeCategory TC, int TK, TypeCategory FC, int FK>
Expr<Type<TC, TK>> ConvertTo(
    const Expr<Type<TC, TK>> &, Expr<Type<FC, FK>> &&x) {
  return ConvertToType<Type<TC, TK>>(AsCategoryExpr(std::move(x)));
}

template<TypeCategory TC, TypeCategory FC>
Expr<SomeKind<TC>> ConvertTo(
    const Expr<SomeKind<TC>> &to, Expr<SomeKind<FC>> &&from) {
  return std::visit(
      [&](const auto &toKindExpr) {
        using KindExpr = std::decay_t<decltype(toKindExpr)>;
        return AsCategoryExpr(
            ConvertToType<ResultType<KindExpr>>(std::move(from)));
      },
      to.u);
}

template<TypeCategory TC, TypeCategory FC, int FK>
Expr<SomeKind<TC>> ConvertTo(
    const Expr<SomeKind<TC>> &to, Expr<Type<FC, FK>> &&from) {
  return ConvertTo(to, AsCategoryExpr(std::move(from)));
}

template<typename FT>
Expr<SomeType> ConvertTo(const Expr<SomeType> &to, Expr<FT> &&from) {
  return std::visit(
      [&](const auto &toCatExpr) {
        return AsGenericExpr(ConvertTo(toCatExpr, std::move(from)));
      },
      to.u);
}

// Given references to two expressions of the same type category, convert
// either to the kind of the other in place if it has a smaller kind.
template<TypeCategory CAT>
void ConvertToSameKind(Expr<SomeKind<CAT>> &x, Expr<SomeKind<CAT>> &y) {
  std::visit(
      [&](auto &xk, auto &yk) {
        using xt = ResultType<decltype(xk)>;
        using yt = ResultType<decltype(yk)>;
        if constexpr (xt::kind < yt::kind) {
          x.u = Expr<yt>{Convert<yt, CAT>{x}};
        } else if constexpr (xt::kind > yt::kind) {
          y.u = Expr<xt>{Convert<xt, CAT>{y}};
        }
      },
      x.u, y.u);
}

// Ensure that both operands of an intrinsic REAL operation (or CMPLX()
// constructor) are INTEGER or REAL, then convert them as necessary to the
// same kind of REAL.
template<int N = 2> struct SameExprsHelper {
  template<typename A> using SameExprs = std::array<Expr<A>, N>;
};
template<typename A, int N = 2> using SameExprs = std::array<Expr<A>, N>;
template<TypeCategory CAT, int N = 2>
using SameKindExprs =
    common::MapTemplate<SameExprsHelper<N>::template SameExprs,
        CategoryTypes<CAT>>;
using ConvertRealOperandsResult =
    std::optional<SameKindExprs<TypeCategory::Real, 2>>;
ConvertRealOperandsResult ConvertRealOperands(
    parser::ContextualMessages &, Expr<SomeType> &&, Expr<SomeType> &&);
ConvertRealOperandsResult ConvertRealOperands(parser::ContextualMessages &,
    std::optional<Expr<SomeType>> &&, std::optional<Expr<SomeType>> &&);

template<typename A> Expr<TypeOf<A>> ScalarConstantToExpr(const A &x) {
  using Ty = TypeOf<A>;
  static_assert(
      std::is_same_v<Scalar<Ty>, std::decay_t<A>> || !"TypeOf<> is broken");
  return {Constant<Ty>{x}};
}

// Convert, if necessary, an expression to a specific kind in the same
// category.
template<typename TOTYPE>
Expr<TOTYPE> EnsureKind(Expr<SomeKind<TOTYPE::category>> &&x) {
  using ToType = TOTYPE;
  static_assert(ToType::isSpecificType);
  if (auto already{common::GetIf<Expr<ToType>>(x.u)}) {
    return std::move(*already);
  }
  if constexpr (ToType::category == TypeCategory::Complex) {
    return {std::visit(
        [](auto &z) -> ComplexConstructor<ToType::kind> {
          using FromType = ResultType<decltype(z)>;
          using FromPart = typename FromType::Part;
          using FromGeneric = SomeKind<TypeCategory::Real>;
          using ToPart = typename ToType::Part;
          Convert<ToPart, TypeCategory::Real> re{Expr<FromGeneric>{
              Expr<FromPart>{ComplexComponent<FromType::kind>{false, z}}}};
          Convert<ToPart, TypeCategory::Real> im{Expr<FromGeneric>{
              Expr<FromPart>{ComplexComponent<FromType::kind>{true, z}}}};
          return {std::move(re), std::move(im)};
        },
        x.u)};
  } else {
    return {Convert<ToType, ToType::category>{std::move(x)}};
  }
}

// Given two expressions of arbitrary kind in the same intrinsic type
// category, convert one of them if necessary to the larger kind of the
// other, then combine them with a operation and return a new expression
// in the same type category.
template<template<typename> class OPR, TypeCategory CAT>
Expr<SomeKind<CAT>> PromoteAndCombine(
    Expr<SomeKind<CAT>> &&x, Expr<SomeKind<CAT>> &&y) {
  return std::visit(
      [&](auto &&xk, auto &&yk) -> Expr<SomeKind<CAT>> {
        using xt = ResultType<decltype(xk)>;
        using yt = ResultType<decltype(yk)>;
        using ToType = Type<CAT, std::max(xt::kind, yt::kind)>;
        return AsCategoryExpr(
            AsExpr(OPR<ToType>{EnsureKind<ToType>(std::move(xk)),
                EnsureKind<ToType>(std::move(yk))}));
      },
      std::move(x.u), std::move(y.u));
}

// Given two expressions of arbitrary type, try to combine them with a
// binary numeric operation (e.g., Add), possibly with data type conversion of
// one of the operands to the type of the other.
template<template<typename> class OPR>
std::optional<Expr<SomeType>> NumericOperation(
    parser::ContextualMessages &, Expr<SomeType> &&, Expr<SomeType> &&);

extern template std::optional<Expr<SomeType>> NumericOperation<Add>(
    parser::ContextualMessages &, Expr<SomeType> &&, Expr<SomeType> &&);
// TODO pmk more

}  // namespace Fortran::evaluate
#endif  // FORTRAN_EVALUATE_TOOLS_H_
