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

class symtab_t
{
public:
    typedef std::vector<Symbol*> SymbolsType;

    symtab_t (symtab_t* p)
        : parent (p)
        , current_action (NULL)
        , current_function (NULL)
        , current_method (NULL)
        , current_initializer (NULL)
        , current_getter (NULL)
        , current_trigger (NULL)
        , current_receiver_type (NULL)
    { }

    void
    enter (Symbol * symbol)
    {
        symbols_.push_back (symbol);
    }

    Symbol *
    find (const std::string& identifier) const
    {
        for (std::vector<Symbol*>::const_iterator pos = symbols_.begin (),
                limit = symbols_.end ();
                pos != limit;
                ++pos)
            {
                Symbol *s = *pos;
                if (identifier == s->identifier)
                    {
                        return s;
                    }
            }

        /* Not found in this scope.  Try the parent. */
        if (parent)
            {
                return parent->find (identifier);
            }
        else
            {
                return NULL;
            }
    }

    Symbol *
    find_current (const std::string& identifier) const
    {
        for (std::vector<Symbol*>::const_iterator pos = symbols_.begin (),
                limit = symbols_.end ();
                pos != limit;
                ++pos)
            {
                Symbol *s = *pos;
                if (identifier == s->identifier)
                    {
                        return s;
                    }
            }

        return NULL;
    }

    Symbol *
    get_this () const
    {
        for (std::vector<Symbol*>::const_iterator pos = symbols_.begin (),
                limit = symbols_.end ();
                pos != limit;
                ++pos)
            {
                ParameterSymbol *s = SymbolCast<ParameterSymbol> (*pos);
                if (s != NULL
                        && s->kind == ParameterSymbol::Receiver)
                    {
                        return s;
                    }
            }

        if (parent)
            {
                return parent->get_this ();
            }
        else
            {
                return NULL;
            }
    }

    void trigger (symbol_holder& holder, ast_t* defining_node);

    // Enter all parameters and variables that are in scope and pointers as pointers to foreign.
    void change ();

    SymbolsType::const_iterator begin () const
    {
        return symbols_.begin ();
    }
    SymbolsType::const_iterator end () const
    {
        return symbols_.end ();
    }

    symtab_t *parent;

    std::ostream& print (std::ostream& o) const;

private:
    SymbolsType symbols_;

public:
    action_reaction_base_t* current_action;
    Function* current_function;
    Method* current_method;
    Initializer* current_initializer;
    Getter* current_getter;
    trigger_t* current_trigger;
    named_type_t* current_receiver_type;
};

inline std::ostream&
operator<< (std::ostream& o, const symtab_t& symtab)
{
    return symtab.print (o);
}

symtab_t *symtab_parent (symtab_t * symtab);

symtab_t *symtab_get_root (symtab_t * symtab);

const type_t *symtab_get_this_type (const symtab_t * symtab);

Function* symtab_get_current_function (const symtab_t * symtab);

Method* symtab_get_current_method (const symtab_t * symtab);

void symtab_set_current_initializer (symtab_t * symtab, Initializer* initializer);

#endif /* symtab_hpp */
