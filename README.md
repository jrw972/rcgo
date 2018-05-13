# rcgo

[![Build Status](https://travis-ci.org/jrw972/rcgo.svg?branch=master)](https://travis-ci.org/jrw972/rcgo)

The goal of this project is to produce a tool chain for a new
programming language called rcgo that is targeted at reactive
programs.  Reactive programs have "ongoing interactions with (their)
environment" [1] and include operating system kernels, networking
protocols, databases, clients, servers, smart phone apps, and embedded
firmware.  For comparison, the other broad class of programs are
transformational programs which read a finite input, perform some
processing that eventually terminates, and then write a finite output,
e.g., a compiler.  rcgo implements the reactive component model and is
based on the Go programming language.  The tool chain will include, at
a minimum, a compiler, a linker, and a run-time environment.
Additional tools, like libraries, debuggers, profilers, monitors,
analyzers, and visualizers will be added as the community requests and
produces them.

Today, reactive programs are implemented using a variety of languages,
models, and techniques.  The Cooperating Sequential Process model
(threads, shared memory, and locks) requires that every developer must
have a complete understanding of the call graph to avoid hazards like
livelock, deadlock, etc.  The Communicating Sequential Process model
(threads and channels) and the Actor model require that every problem
be solved in a distributed fashion.  Event-loops (reactor/proactor)
invert the flow of control which forces programmers to manually
preserve context across events.  Callbacks, futures, and promises are
often used to dynamically construct the handlers and preserve context
in an event-loop system.  However, the resulting code can still be
difficult to read and understand.

In general, reactive code is more difficult to test, and by extension
use, than transformational code.  Transformational code, on any level,
can be tested by executing it with known inputs and checking that the
outputs match the expected outputs.  Debugging a transformational
program uses the same paradigm, that is, one identifies a point in the
program where an expected condition has not been met and then
backtracks through the program to find the sequence that failed to
establish the condition.  Reactive code, on the other hand, tends to
be more difficult to test and debug because one cannot control and
observe all of the aspects of the environment needed for testing and
debugging.

rcgo is a programming language based on the
[reactive component model](https://openscholarship.wustl.edu/eng_etds/217/).
A reactive component is a set of state variables that are manipulated
by implicitly atomic transactions.  Reactive components can be
composed by 1) embedding one component in another and/or 2) linking
transactions to span multiple components.  Reactive components have a
number of features that make them an attractive option for reactive
programs.

1. The reactive component model defines a unit of composition, i.e., a
   reactive component, and a means to compose them.  The rcgo tool
   chain will enforce these semantics.
2. Composition results in a new reactive component.  This facilitates
   reuse and permits decomposition to an arbitrary degree.
3. Reactive components can encapsulate other reactive components.
   This also facilitates reuse and permits decomposition to an
   arbitrary depth.
4. The behavior of a reactive component is encapsulated by its
   interface.  The interface for a reactive component exposes both its
   input and output behavior.  This facilitates hiding implementation
   details, which is necessary for abstraction, and testing, by
   activating inputs and observing outputs and the resulting state.
5. Composition is *compositional* meaning that the properties of a
   composed component can be stated in terms of its constituent
   components.
6. The definition of a component can be substituted for its use.

rcgo as a general purpose programming language for reactive systems,
has a large potential audience.  rcgo is a candidate for projects that
would traditionally use C, C++, Java, Go, Python, Node, etc.  Our hope
is that rcgo will make the developers of reactive systems more
productive.  First, we hope that developers will spend more time
writing tests and less time debugging.  Second, when there is a bug,
we hope that the semantics of reactive components will allow a
developer to find it quickly.  Third, we hope that rcgo will allow
developers to create reactive components that are easy to reuse.  A
growing community, maturity in the tools, and a growing rcgo codebase
are indicators that rcgo is a successful project.

[1]: *The temporal logic of reactive and concurrent systems: Specification.* Z. Manna, and A. Pnueli. Spring, Volume 1, 1992.

## Relationship with Go

Every programming language for reactive programs needs support for
transformational computation.  This includes the syntax and semantics
of types, operands, functions, methods, etc.  There are a number of
programming languages that could be used as the transformational basis
of a language for reactive components.  We selected the
[Go programming language](https://golang.org) for three reasons.
First, Go is simple and modern.  The relatively simple semantics of Go
make it easier to implement compared to other languages.  Second, Go
is a statically checked language.  To support the semantics of
reactive components, additional static checks will be added.  Third,
Go has automatic memory management.  Go has pointers that facilitate
efficient communication but create the opportunity for components to
share state which is prohibited by the reactive component model.
Automatic memory management addresses some of these issues.

rcgo is not a superset of Go and therefore not compatible with it.
Specifically, rcgo has no support for go routines and channels as they
would interfere with the semantics of reactive components.  rcgo also
adds a number of keywords that could be used as identifiers in
standard Go code.  Thus, there is opportunity to use Go code in rcgo
so long as it does not use go routines and channels, avoids the
keywords of rcgo, and does not make any system calls.  The rcgo
codebase does not share any code with the Go codebase.

## Contributing

This projects follows the
[Collective Code Construction Contract (C4)](https://rfc.zeromq.org/spec:42/C4/)
with the following clarifications and exceptions:

* 2.3.7 - The problem and solution statements of a patch commit
  message need not begin with a literal "Problem:" and "Solution:",
  respectively.  The contributor SHOULD provide additional context
  describing the problem if it cannot be adequately described in 50
  characters.

This project follows the
[Google C++ Style Guide](https://google.github.io/styleguide/cppguide.html).

    astyle --style=google --indent=spaces=2 ast.h

## Development

### Build

    $ ./configure
    $ make
    $ make check

### Performing a coverage build

    $ ./configure --enable-coverage
    $ make
    $ make coverage
