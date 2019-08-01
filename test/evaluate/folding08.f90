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
! WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or
! implied.
! See the License for the specific language governing permissions and
! limitations under the License.

! Test folding of LBOUND and UBOUND

subroutine test(n1,a1,a2)
  integer, intent(in) :: n1
  real, intent(in) :: a1(0:n1), a2(0:*)
  type :: t
    real :: a
  end type
  type(t) :: ta(0:2)
  character(len=2) :: ca(-1:1)
  integer, parameter :: lba1(:) = lbound(a1)
  logical, parameter :: test_lba1 = all(lba1 == [0])
  integer, parameter :: lba2(:) = lbound(a2)
  logical, parameter :: test_lba2 = all(lba2 == [0])
  integer, parameter :: uba2(:) = ubound(a2)
  logical, parameter :: test_uba2 = all(uba2 == [-1])
  integer, parameter :: lbta1(:) = lbound(ta)
  logical, parameter :: test_lbta1 = all(lbta1 == [0])
  integer, parameter :: ubta1(:) = ubound(ta)
  logical, parameter :: test_ubta1 = all(ubta1 == [2])
  integer, parameter :: lbta2(:) = lbound(ta(:))
  logical, parameter :: test_lbta2 = all(lbta2 == [1])
  integer, parameter :: ubta2(:) = ubound(ta(:))
  logical, parameter :: test_ubta2 = all(ubta2 == [3])
  integer, parameter :: lbta3(:) = lbound(ta%a)
  logical, parameter :: test_lbta3 = all(lbta3 == [1])
  integer, parameter :: ubta3(:) = ubound(ta%a)
  logical, parameter :: test_ubta3 = all(ubta3 == [3])
  integer, parameter :: lbca1(:) = lbound(ca)
  logical, parameter :: test_lbca1 = all(lbca1 == [-1])
  integer, parameter :: ubca1(:) = ubound(ca)
  logical, parameter :: test_ubca1 = all(ubca1 == [1])
  integer, parameter :: lbca2(:) = lbound(ca(:)(1:1))
  logical, parameter :: test_lbca2 = all(lbca2 == [1])
  integer, parameter :: ubca2(:) = ubound(ca(:)(1:1))
  logical, parameter :: test_ubca2 = all(ubca2 == [3])
end
