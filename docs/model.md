---
layout: page
title: The Reactive Component Model
rank: 2
---

## <a name="reactive_component">Reactive Components</a> ##

The core of a reactive component is a set of state variables and atomic transitions that operate on those state variables.
The state variables of a reactive component may be instances of other reactive components to facilitate recursive encapsulation.
Transitions are organized into *actions* and *reactions*.
An action has an associated Boolean expression called a *precondition*.
When the precondition is true, the action is eligible for execution by the *scheduler*.
The transition part of an action can *activate* zero or more *active push ports*.
An active push port is a named and typed interaction point.
An active push port in one component may be bound to a *passive push port* in another component.
A reaction is the combination of a passive push port and a transition.
A reaction can in turn activate other push ports.
A *getter* is the combination of a *passive pull port* and an expression and is so named due to its resemblance with getters in Object-Oriented Programming.
An *active pull port* represents an external data dependency that may be *called* to yield a value that is required in a transition or expression.

## <a name="binders">Binders</a> ##

A *binder* is a declarative specification that binds active push ports to reactions (passive push ports) and active pull ports to getters (passive pull ports).
Active push ports *may* be bound to zero or more passive push ports.
Active pull ports *must* be bound to exactly one passive pull port.
Ports, reactions, and getters are typed and have function-like signatures that are checked for compatibility.

## <a name="activations">Activations</a> ##

A transition is divided into two phases called the *immutable phase* and the *mutable phase*.
Conceptually, a transition can be thought of as a parallel assignment statement.
The left-hand side (LHS) lists the state variables that will be updated and is the target of the assignment.
The right-hand side (RHS) lists expressions that yield the next value of the corresponding state variable.
The state of a component does not change during the the evaluation of the LHS.
This is the immutable phase.
The state variables are updated during the evaluation of LHS which is the mutable phase.
The division of a transition into an immutable phase and mutable phase allows transitions to compose by aggregating the immutable phases and mutable phases.

Activations signal the end of the immutable phase and the beginning of the mutable phase.
Suppose that an action activates a single port that is bound to a single reaction.
The order is which the various phases are executed is (logically) 1) action immutable phase, 2) reaction immutable phase, 3) reaction mutable phase, and 4) action mutable phase.
The order of the mutable phases is irrelevant, however, there is a causality relationship among the immutable phases, that is, activation may be conditional.
Getters and active pull ports may only be called in the immutable phase.

## <a name="transactions">Transactions and Execution</a> ##

An action and the transitive closure of all potentially activated reactions is called a *transaction*.
For execution, the scheduler selects an enabled transaction from the set of transactions and executes the transition linked to the action.
This transition may cascade to reactions and those transitions may cascaded to other reactions and so on.
The scheduler has the responsibility of ensuring that each transaction is logically atomic.
That is, the scheduler must guarantee that all state involved in a transaction does not change during a transaction except for the state changed by the mutable phase of the transaction.
Transactions that do not mutate the same state may be executed in parallel.
