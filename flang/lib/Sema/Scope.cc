

#include "flang/Sema/Scope.h"
#include "flang/Sema/Identifier.h"

#include <cassert>


namespace Fortran::semantics {

Scope::Scope(Kind k, Scope *p, Symbol *s) :
 kind_(k), 
 id_or_counter_(0),
 self_(s), 
 parent_scope_(p), 
 host_scope_(p)
{
  switch(k) {

  case SK_SYSTEM:
    assert( parent_scope_ == NULL) ;
    id_or_counter_ = -1 ;
    break ;

  case SK_GLOBAL:
    assert( parent_scope_->kind_ == SK_SYSTEM) ; 
    id_or_counter_ = 2 ;
    break ;

  case SK_PROGRAM:
    assert( parent_scope_->kind_ == SK_GLOBAL) ;
    if( self_ ) { 
      assert( self_->owner() == parent_scope_) ; 
    }
    break;

  case SK_MODULE:  
    assert( parent_scope_->kind_ == SK_GLOBAL) ;
    assert( self_ ) ; 
    assert( self_->owner() == parent_scope_) ; 
    break;

  case SK_SUBMODULE:         
    assert( parent_scope_->kind_ == SK_GLOBAL) ;
    assert( self_ ) ; 
    assert( self_->owner() == parent_scope_) ; 
    break;

  case SK_FUNCTION: 
    assert( parent_scope_->kind_ == SK_GLOBAL ||
            parent_scope_->kind_ == SK_PROGRAM ||  
            parent_scope_->kind_ == SK_MODULE ||  
            parent_scope_->kind_ == SK_FUNCTION ||  
            parent_scope_->kind_ == SK_SUBROUTINE ||  
            parent_scope_->kind_ == SK_INTERFACE ||  
            parent_scope_->kind_ == SK_USE_MODULE 
            ) ;
    assert( self_ ) ; 
    assert( self_->owner() == parent_scope_) ; 
    break;

  case SK_SUBROUTINE:  
    assert( parent_scope_->kind_ == SK_GLOBAL ||
            parent_scope_->kind_ == SK_PROGRAM ||  
            parent_scope_->kind_ == SK_MODULE ||  
            parent_scope_->kind_ == SK_FUNCTION ||  
            parent_scope_->kind_ == SK_SUBROUTINE ||  
            parent_scope_->kind_ == SK_INTERFACE ||  
            parent_scope_->kind_ == SK_USE_MODULE 
            ) ;
    assert( self_ ) ; 
    assert( self_->owner() == parent_scope_) ; 
    break;

  case SK_BLOCKDATA:   
    assert( parent_scope_->kind_ == SK_GLOBAL) ;
    assert( self_ ) ; 
    assert( self_->owner() == parent_scope_) ; 
    break;

  case SK_USE_MODULE:  
    assert( parent_scope_->kind_ == SK_GLOBAL) ;
    assert( self_ ) ; 
    assert( self_->owner() == parent_scope_ ) ; 
    break;

  case SK_BLOCK:     
    assert( parent_scope_->kind_ == SK_PROGRAM ||
            parent_scope_->kind_ == SK_BLOCK ||
            parent_scope_->kind_ == SK_FUNCTION ||
            parent_scope_->kind_ == SK_SUBROUTINE 
            ) ;    
    if ( self_ ) {
      assert( self_->owner() == parent_scope_ ) ; 
      assert( self_->toConstructSymbol() ) ; 
    }
    break;

  case SK_DERIVED: 
    assert( parent_scope_->kind_ == SK_PROGRAM ||  
            parent_scope_->kind_ == SK_MODULE ||  
            parent_scope_->kind_ == SK_FUNCTION ||  
            parent_scope_->kind_ == SK_SUBROUTINE ||  
            parent_scope_->kind_ == SK_INTERFACE ||  
            parent_scope_->kind_ == SK_USE_MODULE 
            ) ;   
    break;

  case SK_INTERFACE:
    assert( parent_scope_->kind_ == SK_PROGRAM ||  
            parent_scope_->kind_ == SK_MODULE ||  
            parent_scope_->kind_ == SK_FUNCTION ||  
            parent_scope_->kind_ == SK_SUBROUTINE ||  
            parent_scope_->kind_ == SK_USE_MODULE 
            ) ; 

    // Within an interface, the symbols must be explicitly imported 
    // from the parent scope so there is no default host scope.
    host_scope_ = NULL ;      
    break;

  default:
    fail("unknown scope kind") ; 
    break;

  } // of switch

  // Figure out the ID
  if (id_or_counter_ == 0) {
    if ( Scope *global_scope = getGlobalScope() ) {
      // Within the global scope, the IDs are growing 
      id_or_counter_ = global_scope->id_or_counter_++ ;
    } else {
      // Within the system scope, the IDs are shrinking (in negative) 
      Scope *system_scope = const_cast<Scope *>( getSystemScope() ) ;
      id_or_counter_ = system_scope->id_or_counter_-- ;
    }
  }

}

Scope::~Scope() {
}


void 
Scope::fail(const std::string &msg) const
{ 
  std::cerr << "FATAL Scope: " << msg << "\n" ; 
  exit(1) ; 
} 

const Scope * 
Scope::getSystemScope() const
{
  for ( const Scope * scope = this ; scope ; scope = scope->parent_scope_) {
    if ( scope->kind_ == SK_SYSTEM ) 
      return scope;
  }
  fail("System scope not found") ;
  return nullptr;
}


Scope * 
Scope::getGlobalScope() 
{  
  for ( auto scope = this ; scope ; scope = scope->parent_scope_) {
    if ( scope->kind_ == SK_GLOBAL ) 
      return scope;
  }
  return nullptr;
}

const Scope * 
Scope::getGlobalScope() const 
{
  return const_cast<Scope*>(this)->getGlobalScope() ; 
} 

Symbol *
Scope::Lookup(const Identifier *name) 
{
  if ( kind_ == SK_GLOBAL ) 
    return parent_scope_->Lookup(name) ;

  // TODO
  
  return nullptr ;
}

Symbol *
Scope::LookupLocal(const Identifier *name) 
{
  if ( kind_ == SK_GLOBAL ) 
    return nullptr ;

  auto & entries{this->entries_}  ;
  for (auto it = entries.rbegin() ; it != entries.rend() ; ++it) {
    Symbol *s = *it ;
    if ( s->Match(name) )
      return s ;              
  }
  
  return nullptr ;
}



Symbol *
Scope::LookupProgramUnit(const Identifier *name)  
{
  Scope * scope = this->getGlobalScope() ;
  auto & entries{scope->entries_}  ;

  for (auto it = entries.rbegin() ; it != entries.rend() ; ++it) {
    Symbol *s = *it ;
    std::cerr << "found " << s << " " << s->name() << "\n" ;
    if ( s->toModuleSymbol() || 
         s->toProgramSymbol() || 
         s->toSubroutineSymbol() ||
         s->toFunctionSymbol() ||
         s->toBlockDataSymbol() 
         )
      {
        if ( s->Match(name) )
          return s ;              
      }
  }

  return nullptr ;
}

const Symbol *
Scope::LookupProgramUnit(const Identifier *name) const 
{
  return const_cast<Scope*>(this)->LookupProgramUnit(name) ; 
}

Symbol *
Scope::LookupModule(const Identifier *name)
{
  return nullptr ;
}
 
const Symbol *
Scope::LookupModule(const Identifier *name) const 
{
  return const_cast<Scope*>(this)->LookupModule(name) ; 
} 

int Scope::getId(void) const 
{
  if (this->kind_ == SK_SYSTEM)
    return 0;
  else if (this->kind_ == SK_GLOBAL)
    return 1;
  else 
    return id_or_counter_; 
}


void Scope::add(Symbol *s) 
{
  assert(s) ;
  assert(s->owner()==this) ;
  entries_.push_back(s); 
}


std::string Scope::toString(void) 
{
  const char * info;
  switch(kind_) {
#define SEMA_DEFINE_SCOPE(KIND,INFO) case KIND: info=INFO ; break;
#include "flang/Sema/Scope.def"
#undef SEMA_DEFINE_SCOPE
  default: 
    info = "???????" ;
  }

  if ( self_ ) {
    return std::string("#") + std::to_string(getId()) + " " + info + " (" + self_->toString() + ")" ;
  } else {
    return std::string("#") +  std::to_string(getId()) + " " + info + " (***UNNAMED***)" ;
  }

}


} // of namespace 
