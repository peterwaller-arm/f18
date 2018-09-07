! Copyright (c) 2018, NVIDIA CORPORATION.  All rights reserved.
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

! Check correct modfile generation for type with private component.
module m
  integer :: i
  integer, private :: j
  type :: t
    integer :: i
    integer, private :: j
  end type
  type, private :: u
  end type
  type(t) :: x
end

!Expect: m.mod
!module m
!integer::i
!integer,private::j
!type::t
!integer::i
!integer,private::j
!end type
!type,private::u
!end type
!type(t)::x
!end
