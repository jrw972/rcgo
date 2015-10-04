#ifndef action_hpp
#define action_hpp

#include "types.hpp"
#include "type.hpp"
#include "MemoryModel.hpp"
#include "typed_value.hpp"

class action_reaction_base_t
{
public:
    typedef std::vector<trigger_t*> TriggersType;

    action_reaction_base_t (named_type_t* type, ast_t* node_, ast_t* body_)
        : type_ (type)
        , node (node_)
        , body (body_)
        , has_dimension_ (false)
    { }

    action_reaction_base_t (named_type_t* type, ast_t* node_, ast_t* body_, const typed_value_t& dimension)
        : type_ (type)
        , node (node_)
        , body (body_)
        , has_dimension_ (true)
        , dimension_ (dimension)
    { }

    virtual ~action_reaction_base_t() { };

    named_type_t* type () const
    {
        return type_;
    }

    void
    add_trigger (trigger_t * trigger)
    {
        triggers_.push_back (trigger);
    }

    TriggersType::const_iterator begin () const
    {
        return triggers_.begin ();
    }
    TriggersType::const_iterator end () const
    {
        return triggers_.end ();
    }

    MemoryModel memory_model;

    bool has_dimension () const
    {
        return has_dimension_;
    }

    typed_value_t dimension () const
    {
        assert (has_dimension_);
        return dimension_;
    }

private:
    named_type_t* type_;
public:
    ast_t* const node;
    ast_t* const body;
private:
    TriggersType triggers_;
    bool has_dimension_;
    typed_value_t dimension_;
};

class action_t : public action_reaction_base_t
{
public:
    enum PreconditionKind
    {
        DYNAMIC,
        STATIC_TRUE,
        STATIC_FALSE,
    };

    action_t (named_type_t* type, ast_t* node, ast_t* body_)
        : action_reaction_base_t (type, node, body_)
        , precondition_kind (DYNAMIC)
        , precondition (NULL)
    { }

    action_t (named_type_t* type, ast_t* node, ast_t* body_, const typed_value_t& dimension)
        : action_reaction_base_t (type, node, body_, dimension)
        , precondition_kind (DYNAMIC)
        , precondition (NULL)
    { }

    PreconditionKind precondition_kind;
    ast_t* precondition;
};

class reaction_t : public action_reaction_base_t
{
public:
    reaction_t (named_type_t* type, ast_t* node, ast_t* body_, const std::string& name_, const signature_type_t* signature)
        : action_reaction_base_t (type, node, body_)
        , name (name_)
        , reaction_type (new reaction_type_t (signature))
    { }

    reaction_t (named_type_t* type, ast_t* node, ast_t* body_, const std::string& name_, const signature_type_t* signature, const typed_value_t& dimension)
        : action_reaction_base_t (type, node, body_, dimension)
        , name (name_)
        , reaction_type (new reaction_type_t (signature))
    { }

    std::string const name;
    reaction_type_t* const reaction_type;
};

#endif /* action_hpp */
