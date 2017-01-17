---
layout: default
title: The RCGo Programming Language
---

RCGo is an experimental programming language for reactive programs.
It is an implementation of the [Reactive Component Model](model) using the syntax and semantics of [Go](https://golang.org).

## What is a reactive program? ##

![Reactive Program Paradigm](figures/reactive_program_paradigm.svg)

A reactive program is one that has "ongoing interactions with its environment[^1]."
Reactive programs include operating system kernels, network clients and servers, databases, smart phone apps, web apps, GUIs, and embedded software.
Reactive programs receive input, produce output, and perform processing on a ongoing basis.
Many reactive programs are designed to run forever or at least until they are manually terminated.
State is fundamental to reactive programs as one must reason about the evolution of the program with respect to its environment.
State can be modeled directly, e.g., variables and assignment, or indirectly, e.g., mail queues and monads.
Most of the reactive programs and systems encountered in practice are asynchronous meaning that the program and its environment do not share a common clock.
Reactive programs are inherently concurrent as the program and its environment may evolve at the same time.

## Yet another programming language? ##

The main goal of RCGo is to allow developers to produce reusable reactive software.
One indication that software is easy to reuse is that it is easy to test as test cases are a valid use of a piece of software.
Contra-positively, if software is difficult to test, then it is difficult to reuse.
Reactive software is notorious for being difficult to test and debug, e.g., heisenbugs, and by extension, difficult to reuse.

Most reactive software is divided into modules that interact via synchronous sequential call.
This requires a module to be extra defensive as it does not know how and when it will be called by other modules.
The synchronization logic itself is often the source of bugs and makes the code brittle as developers must have a complete understanding of the call graph to avoid concurrency hazards like concurrent updates, deadlock, livelock, etc.

RCGo's approach to delivering reusable reactive software is to focus on abstractions that composes in a [principled](manifesto#principled) way.
The test for principled composition and decomposition is the ability to substitute the definition of a unit for a particular instance, i.e., inlining, and the ability to extract the definition of a sub-unit from a compound unit.
To this end, the basic unit of reactive computing in RCGo is the [reactive component](model#reactive_component).

A reactive component consists of a set of private state variables, atomic transitions, and interface elements that allow a transition in one component to be linked to a transition in another component.
A transition is either an *action* which is a voluntary step for processing and/or output or a *reaction* which is a step to be taken in response to an external event.
A transition can *activate* a *push port* which may be bound to zero or more reactions.
The action and its transitively linked set of reactions is a *transaction*.
Transactions are logically atomic and checked for concurrent updates.
The interface of a reactive component consists of its push ports and reactions.
The behavior of a reactive component can be understood and tested via its interface.

A more detailed answer can be found in the [manifesto](manifesto) and [A Transactional Model and Platform for Designing and Implementing Reactive Systems](rcdissertation.pdf).

## Why Go? ##

The transitions in RCGo transform the state of a reactive component into its next state.
Rather that invent a new language, RCGo adopts Go as the basic language for encoding state transitions.
As RCGo is a prototype, we sought a language that was easy from both an implementation perspective and a user perspective.
Go fits this as it has a short and straight-forward specification and is approachable by a general software engineering audience.

## Status ##

RCGo is an experimental programming language.
Many features are missing and incomplete.
See the [roadmap](roadmap) for a list of planned development milestones.
If you would like to get involved, visit [GitHub](https://github.com/jrw972/rcgo).

[^1]: Z. Manna and A. Pnueli. *The temporal logic of reactive and concurrent systems: Specification*, volume 1. Springer, 1992.
