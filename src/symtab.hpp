#ifndef symtab_hpp
#define symtab_hpp

/* Symbol table. */

#include "typed_value.hpp"
#include "type.hpp"
#include "SymbolVisitor.hpp"

// TODO:  Remove this class.
class symbol_holder
{
public:
    symbol_holder ()
        : symbol_ (NULL)
    { }

    void symbol (Symbol* s)
    {
        assert (symbol_ == NULL);
        symbol_ = s;
    }

    Symbol* symbol () const
    {
        return symbol_;
    }

private:
    Symbol* symbol_;
};

#endif /* symtab_hpp */
