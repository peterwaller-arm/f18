INTEGER, PARAMETER :: maxvalue=1024

Type dt
  Integer :: l = 3
End Type
Type t
  Type(dt),Pointer :: p
End Type

Type(t),Allocatable :: x(:)
Type(t),Pointer :: y(:)
Type(t),Pointer :: z

Integer, Pointer :: pi
Procedure(Real), Pointer :: prp

Allocate(x(3))
Nullify(x(2)%p)

Nullify(y(2)%p)

Nullify(pi)
Nullify(prp)

Nullify(z%p)

End Program
