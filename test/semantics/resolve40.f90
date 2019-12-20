subroutine s1
  namelist /nl/x
  block
    !ERROR: NAMELIST statement is not allowed in a BLOCK construct
    namelist /nl/y
  end block
end

subroutine s2
  open(12, file='nl.out')
  !ERROR: Namelist group 'nl' not found
  write(12, nml=nl)
end

subroutine s3
  real :: x
  open(12, file='nl.out')
  !ERROR: 'x' is not the name of a namelist group
  write(12, nml=x)
end

module m4
  real :: x
  namelist /nl/x
end
subroutine s4a
  use m4
  namelist /nl2/x
  open(12, file='nl.out')
  write(12, nml=nl)
  write(12, nml=nl2)
end
subroutine s4b
  use m4
  real :: y
  !ERROR: 'nl' is already declared in this scoping unit
  namelist /nl/y
end

subroutine s5
  namelist /nl/x
  !ERROR: The type of 'x' has already been implicitly declared
  integer x
end

subroutine s6
  !ERROR: 's6' is not a variable
  namelist /nl/ s6
  !ERROR: 'f' is not a variable
  namelist /nl/ f
contains
  integer function f()
    f = 1
  end
end

subroutine s7
  real x
  namelist /nl/ x
  !ERROR: EXTERNAL attribute not allowed on 'x'
  external x
end

subroutine s8
  data x/1.0/
  !ERROR: The type of 'x' has already been implicitly declared
  integer x
end

subroutine s9
  real :: x(4)
  !ERROR: 'i' is already declared in this scoping unit
  data ((x(i,i),i=1,2),i=1,2)/4*0.0/
end
