! Copyright (c) 2019, ARM Ltd.  All rights reserved.
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
! C1107 -- COMMON, EQUIVALENCE, INTENT, NAMELIST, OPTIONAL, VALEU or
!          STATEMENT FUNCTIONS not allow in specification part

subroutine s1_c1107
  common /nl/x
  block
    !ERROR: COMMON statement is not allowed in a BLOCK construct
    common /nl/y
  end block
end

subroutine s2_c1107
  real x(100), i(5)
  integer y(100), j(5)
  equivalence (x, y)
  block
   !ERROR: EQUIVALENCE statement is not allowed in a BLOCK construct
   equivalence (i, j)
  end block
end

subroutine s3_c1107(x_in, x_out)
  integer x_in, x_out
  intent(in) x_in
  block
    !ERROR: INTENT statement is not allowed in a BLOCK construct
    intent(out) x_out
  end block
end

subroutine s4_c1107
  namelist /nl/x
  block
    !ERROR: NAMELIST statement is not allowed in a BLOCK construct
    namelist /nl/y
  end block
end

subroutine s5_c1107
  integer x, y
  value x
  block
    !ERROR: VALUE statement is not allowed in a BLOCK construct
    value y
  end block
end

subroutine s6_c1107(x, y)
  integer x, y
  optional x
  block
    !ERROR: OPTIONAL statement is not allowed in a BLOCK construct
    optional y
  end block
end

subroutine s7_c1107
 integer x
 inc(x) = x + 1
  block
    !ERROR: STATEMENT FUNCTION statement is not allowed in a BLOCK construct
    dec(x) = x - 1
  end block
end

