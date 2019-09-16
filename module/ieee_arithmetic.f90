! Copyright (c) 2019, NVIDIA CORPORATION.  All rights reserved.
!
! Licensed under the Apache License, Version 2.0 (the "License");
! you may not use this file except in compliance with the License.
! You may obtain a copy of the License at
!
!     http://www.apache.org/licenses/LICENSE-2.0
!
! Unless required by applicable law or agreed to in writing, software
! distributed under the License is distributed on an "AS IS" BASIS,
! WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
! See the License for the specific language governing permissions and
! limitations under the License.

! See Fortran 2018, clause 17.2
module ieee_arithmetic

  type :: ieee_class_type
    private
    integer(kind=1) :: which = 0
  end type ieee_class_type

  type(ieee_class_type), parameter :: &
    ieee_signaling_nan = ieee_class_type(1), &
    ieee_quiet_nan = ieee_class_type(2), &
    ieee_negative_inf = ieee_class_type(3), &
    ieee_negative_normal = ieee_class_type(4), &
    ieee_negative_denormal = ieee_class_type(5), &
    ieee_negative_zero = ieee_class_type(6), &
    ieee_positive_zero = ieee_class_type(7), &
    ieee_positive_subnormal = ieee_class_type(8), &
    ieee_positive_normal = ieee_class_type(9), &
    ieee_positive_inf = ieee_class_type(10), &
    ieee_other_value = ieee_class_type(11)

  type(ieee_class_type), parameter :: &
    ieee_negative_subnormal = ieee_negative_denormal, &
    ieee_positive_denormal = ieee_negative_subnormal

  type :: ieee_round_type
    private
    integer(kind=1) :: mode = 0
  end type ieee_round_type

  type(ieee_round_type), parameter :: &
    ieee_nearest = ieee_round_type(1), &
    ieee_to_zero = ieee_round_type(2), &
    ieee_up = ieee_round_type(3), &
    ieee_down = ieee_round_type(4), &
    ieee_away = ieee_round_type(5), &
    ieee_other = ieee_round_type(6)

  interface operator(==)
    module procedure class_eq
    module procedure round_eq
  end interface operator(==)
  interface operator(/=)
    module procedure class_ne
    module procedure round_ne
  end interface operator(/=)

  ! See Fortran 2018, 17.10 & 17.11
  interface ieee_class
    module procedure ieee_class_a2
    module procedure ieee_class_a3
    module procedure ieee_class_a4
    module procedure ieee_class_a8
    module procedure ieee_class_a10
    module procedure ieee_class_a16
  end interface ieee_class

  interface ieee_copy_sign
    module procedure ieee_copy_sign_a2
    module procedure ieee_copy_sign_a3
    module procedure ieee_copy_sign_a4
    module procedure ieee_copy_sign_a8
    module procedure ieee_copy_sign_a10
    module procedure ieee_copy_sign_a16
  end interface ieee_copy_sign

  ! TODO: more interfaces (_fma, &c.)

  private :: classify

 contains

  elemental logical function class_eq(x,y)
    type(ieee_class_type), intent(in) :: x, y
    class_eq = x%which == y%which
  end function class_eq

  elemental logical function class_ne(x,y)
    type(ieee_class_type), intent(in) :: x, y
    class_ne = x%which /= y%which
  end function class_ne

  elemental logical function round_eq(x,y)
    type(ieee_round_type), intent(in) :: x, y
    round_eq = x%mode == y%mode
  end function round_eq

  elemental logical function round_ne(x,y)
    type(ieee_round_type), intent(in) :: x, y
    round_ne = x%mode /= y%mode
  end function round_ne

  elemental type(ieee_class_type) function classify( &
      expo,maxExpo,negative,significandNZ,quietBit)
    integer, intent(in) :: expo, maxExpo
    logical, intent(in) :: negative, significandNZ, quietBit
    if (expo == 0) then
      if (significandNZ) then
        if (negative) then
          classify = ieee_negative_denormal
        else
          classify = ieee_positive_denormal
        end if
      else
        if (negative) then
          classify = ieee_negative_zero
        else
          classify = ieee_positive_zero
        end if
      end if
    else if (expo == maxExpo) then
      if (significandNZ) then
        if (quietBit) then
          classify = ieee_quiet_nan
        else
          classify = ieee_signaling_nan
        end if
      else
        if (negative) then
          classify = ieee_negative_inf
        else
          classify = ieee_positive_inf
        end if
      end if
    else
      if (negative) then
        classify = ieee_negative_normal
      else
        classify = ieee_positive_normal
      end if
    end if
  end function classify

#define _CLASSIFY(RKIND,IKIND,TOTALBITS,PREC,IMPLICIT) \
  type(ieee_class_type) elemental function ieee_class_a##RKIND(x); \
    real(kind=RKIND), intent(in) :: x; \
    integer(kind=IKIND) :: raw; \
    integer, parameter :: significand = PREC - IMPLICIT; \
    integer, parameter :: exponentBits = TOTALBITS - 1 - significand; \
    integer, parameter :: maxExpo = shiftl(1, exponentBits) - 1; \
    integer :: exponent, sign; \
    logical :: negative, nzSignificand, quiet; \
    raw = transfer(x, raw); \
    exponent = ibits(raw, significand, exponentBits); \
    negative = btest(raw, TOTALBITS - 1); \
    nzSignificand = ibits(raw, 0, significand) /= 0; \
    quiet = btest(raw, significand - 1); \
    ieee_class_a##RKIND = classify(exponent, maxExpo, negative, nzSignificand, quiet); \
  end function ieee_class_a##RKIND
  _CLASSIFY(2,2,16,11,1)
  _CLASSIFY(3,2,16,8,1)
  _CLASSIFY(4,4,32,24,1)
  _CLASSIFY(8,8,64,53,1)
  _CLASSIFY(10,16,80,64,0)
  _CLASSIFY(16,16,128,112,1)
#undef _CLASSIFY

  ! TODO: This might need to be an actual Operation instead
#define _COPYSIGN(RKIND,IKIND,BITS) \
  real(kind=RKIND) elemental function ieee_copy_sign_a##RKIND(x,y); \
    real(kind=RKIND), intent(in) :: x, y; \
    integer(kind=IKIND) :: xbits, ybits; \
    xbits = transfer(x, xbits); \
    ybits = transfer(y, ybits); \
    xbits = ior(ibclr(xbits, BITS-1), iand(ybits, shiftl(1_##IKIND, BITS-1))); \
    ieee_copy_sign_a##RKIND = transfer(xbits, x); \
  end function ieee_copy_sign_a##RKIND
  _COPYSIGN(2,2,16)
  _COPYSIGN(3,2,16)
  _COPYSIGN(4,4,32)
  _COPYSIGN(8,8,64)
  _COPYSIGN(10,16,80)
  _COPYSIGN(16,16,128)
#undef _COPYSIGN

end module ieee_arithmetic
