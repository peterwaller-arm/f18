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

!DEF: /s1 Subprogram
subroutine s1
 implicit none
 !DEF: /s1/x ObjectEntity REAL(8)
 real(kind=8) :: x = 2.0
 !DEF: /s1/a ObjectEntity INTEGER(4)
 integer a
 !DEF: /s1/t DerivedType
 type :: t
 end type
 !REF: /s1/t
 !DEF: /s1/z ALLOCATABLE ObjectEntity CLASS(t)
 class(t), allocatable :: z
 !DEF: /s1/Block1/a AssocEntity REAL(8)
 !REF: /s1/x
 !DEF: /s1/Block1/b AssocEntity REAL(8)
 !DEF: /s1/Block1/c AssocEntity CLASS(t)
 !REF: /s1/z
 associate (a => x, b => x+1, c => z)
  !REF: /s1/x
  !REF: /s1/Block1/a
  x = a
 end associate
end subroutine

!DEF: /s2 Subprogram
subroutine s2
 !DEF: /s2/x ObjectEntity CHARACTER(4_4,1)
 !DEF: /s2/y ObjectEntity CHARACTER(4_4,1)
 character(len=4) x, y
 !DEF: /s2/Block1/z AssocEntity CHARACTER(4_4,1)
 !REF: /s2/x
 associate (z => x)
  !REF: /s2/Block1/z
  print *, "z:", z
 end associate
 !TODO: need correct length for z
 !DEF: /s2/Block2/z AssocEntity CHARACTER(0_8,1)
 !REF: /s2/x
 !REF: /s2/y
 associate (z => x//y)
  !REF: /s2/Block2/z
  print *, "z:", z
 end associate
end subroutine

!DEF: /s3 Subprogram
subroutine s3
 !DEF: /s3/t1 DerivedType
 type :: t1
  !DEF: /s3/t1/a1 ObjectEntity INTEGER(4)
  integer :: a1
 end type
 !REF: /s3/t1
 !DEF: /s3/t2 DerivedType
 type, extends(t1) :: t2
  !DEF: /s3/t2/a2 ObjectEntity INTEGER(4)
  integer :: a2
 end type
 !DEF: /s3/i ObjectEntity INTEGER(4)
 integer i
 !REF: /s3/t1
 !DEF: /s3/x POINTER ObjectEntity CLASS(t1)
 class(t1), pointer :: x
 !REF: /s3/x
 select type (y => x)
  !REF: /s3/t2
  class is (t2)
   !REF: /s3/i
   !DEF: /s3/Block1/y TARGET AssocEntity TYPE(t2)
   !REF: /s3/t2/a2
   i = y%a2
   type is (integer(kind=8))
    !REF: /s3/i
    !DEF: /s3/Block2/y TARGET AssocEntity INTEGER(8)
    i = y
   class default
    !DEF: /s3/Block3/y TARGET AssocEntity CLASS(t1)
    print *, y
 end select
end subroutine
