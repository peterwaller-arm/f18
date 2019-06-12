
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

! Test UTF-8 support in character literals
! TODO: test EUC-JP
! Note: Module files are encoded in UTF-8.

module m
character(kind=4,len=:), parameter :: c4 = 4_"Hi! 你好!"
! In CHARACTER(1) literals, codepoints > 0xff are serialized into UTF-8;
! each of those bytes then gets encoded into UTF-8 for the module file.
character(kind=1,len=:), parameter :: c1 = 1_"Hi! 你好!"
character(kind=4,len=:), parameter :: c4a(:) = [4_"一", 4_"二", 4_"三", 4_"四", 4_"五"]
integer, parameter :: lc4 = len(c4)
integer, parameter :: lc1 = len(c1)
end module m

!Expect: m.mod
!module m
!character(:,4),parameter::c4=4_"Hi! 你好!"
!character(:,1),parameter::c1=1_"Hi! ä½ å¥½!"
!character(:,4),parameter::c4a(1_8:)=[CHARACTER(KIND=4,LEN=1)::"一","二","三","四","五"]
!integer(4),parameter::lc4=7_4
!integer(4),parameter::lc1=11_4
!end
