---
layout: page
title: The Reactive Component Manifesto
rank: 1
---

1. **<a>There are two kinds of programs:  transformational programs and reactive programs.</a>**

    <a name="transformational">Transformational Programs</a>

    ![Transformational Program Paradigm](figures/transformational_program_paradigm.svg)

    The goal of a *transformational program* is to transform a finite input into a finite output in a finite amount of time.
    A program that sorts the lines of a file is a good example of a transformational program; the input is an unsorted list of lines, the output is the sorted list of lines, and the transformation applied is some kind of sorting algorithm.
    Formal models like the Turing Machine and Lambda Calculus are concerned with transformational programs.
    Termination in transformational programs is critical, i.e., the Halting Problem, hence the need or desire for a finite input, finite processing, and finite output.

    <a name="reactive">Reactive Programs</a>

    ![Reactive Program Paradigm](figures/reactive_program_paradigm.svg)

    A *reactive program* is one that has "ongoing interactions with an environment [cite Manna and Pneuli]."
    In the reactive progam paradigm, the activities of input, output, and processing are often overlapped and ongoing.
    Thus, reactive programs may operate on infinite streams of input and produce infinite streams of output.
    A web server is a good example of a reactive program as it receives requests, processes them, and sends responses.
    Formal models like Cooperating Sequential Processes, Communicating Sequential Processes, and Actors are concerned with reactive programs.
    The environment in the reactive program paradigm is often modeled as another reactive program as it may perform processing, receive input, and produce output.
    Reactive programs are inherently concurrent with respect to their environment since the program and its environment may perform activities at the same time.
    Reactive programs may be synchronous meaning that program and environment share a common clock or asynchronous.
    Most reactive programs encountered in practice are asynchronous due to the challenges of creating shared clocks.
    Thus, the remaining discussion applies to asynchronous reactive programs.

2. <a>Mainstream processors, programming languages, and operating systems were originally designed for transformational programs.</a>

    The very first computer programs performed ballistics calculations, processed census information, and processed payroll.
    The nature of computing at that time was to load the program into the computer, load the input into the computer, start the processor, wait until the program finished, and then record the output of the program.
    Thus, the transformational program paradigm was born.

    While the capabilities of mainstream processors have grown over the years, their basic mechanism has not.
    A processor consists of a set of registers, a control circuit, a clock, and memory.
    The memory may store both data and instructions (von Neumann architecture) or there may be separate memories for data and instructions (Harvard architecture).
    One important register is the program counter which stores the memory address of the next instruction to be executed.
    This address is presented to the memory to fetch the next instruction to be executed.
    The control circuit interprets the instruction and applies it to the registers to compute the next value for all registers.
    When the clock ticks, memory is written if necessary and the registers are updated with their new values including the program counter which is incremented to the next instruction address unless altered by a control instruction.
    The interplay between the clock and the program counter leads these kinds of processors to be called *synchronous sequential processors*.

    The preceding (simplified) description of a synchronous sequential processor immediately suggests how one might use it for transformational programs.
    The input and output of the program is stored in memory.
    The programming goal is to craft a sequence of instructions that transforms the contents of the memory into the desired output.
    Typically, the transformation cannot be expressed as a single machine instruction.
    Thus, a complex transformation can be decomposed into a sequence of simpler transformations.
    This process can be repeated until the program is expressed as a sequence of machine instructions and thus executable by a processor.

    The top-down approach to designing transformational programs just described is complemented by the bottom-up approach used to implement them.
    This is borne out the evolution of programming languages.
    The first programming language was assembly language where each instruction was given a textual mnemonic.
    For example, an ADD mnemonic represented the machine instruction for adding two numbers.
    A program specified in this way could be translated into machine language by looking up the instruction code that corresponded to each mnemonic.
    Initially, this process was performed by hand.
    However, it quickly became apparent that process of assembling a program was a tedious job best performed by a computer (a transformational program).

    The next major advancement in programming languages was the introduction of high-level languages like FORTRAN, LISP, Algol, C, etc.
    A high-level language abstracts away the low-level details of a processor leaving only the essential elements:  storage and control.
    Storage in an imperative programming language is realized through variables and values.
    An imperative program is written as a sequence of statements that manipulate variables, i.e., assignment statements, by assigning them values calculated from expressions.
    The sequence of statements can be manipulated using control statements/structures such a conditionals (if/else), loops, and calls to other statement sequences.
    The functional programming paradigm includes only values and expressions that manipulate those values.
    Notions of control are incorporated through the use of special forms and function calls.
    The job of the compiler or interpreter then is to translate the program into an equivalent sequence of machine instructions.
    A straightforward translation, i.e., no optimization, typically results in strong correspondence between the high-level program and the assembly/machine program due to the implicit sequential nature of both the high-level programming language and the underlying processor.
    Early high-level languages were strictly transformational while modern mainstream programming languages have started incorporating features for reactive programs, e.g., multi-threading in Java and C++.
    These features are typically thin wrappers around the facilities provided by the operating system.

    In the early days, computers were a scarce shared resource.
    A *user* would submit a *job* consisting of their (transformational) program and input, typically on punched cards, to the computing facility.
    An *operator* would then load the job into the computer, execute the program, and print the output so the user could pick it up.
    A running job was called a *process*.
    There was typically a backlog of jobs so operators had a choice as to what job would be the next to run.
    This led to the discovery of the Shortest Job First (SJF) scheduling algorithm that minimizes the total combined wait time for all jobs/users.

    Like human assemblers, it was soon realized that scheduling and loading duties of computer operators were best performed by the computer itself.
    The program that replaced human computer operators was called an *operating system*, a reactive program.
    The first responsibility of the operating system was to rapidly switch from one job to the next.
    That is, upon the completion of a job, the operating system was invoked to automatically load and execute the next job in the queue.

    Another source of inefficiency addressed by operating systems was time spent waiting for input and output.
    Initially, all I/O was performed by the process, that is, each job included code for accessing card readers, printers, etc. which were often slower than the processor.
    Programs, then, had to poll the status of these peripherals to ensure that the I/O was complete.
    For efficiency, programs were often written to perform useful work while waiting for peripherals leading to some highly optimized and unmaintainable code sequences.
    The capabilities of operating systems, then, were expanded into include facilities for I/O.
    That is, operating systems started to include *device drivers* which were libraries that allowed the peripherals of the machine to be accessed by programs.
    If a particular request for I/O would idle the processor for a significant amount of time, then the operating system would switch to another process that was ready to execute to improve the utilization of the processor (multiprogramming).
    The mode of I/O where a code sequence is blocked while waiting for the I/O to complete is called *blocking I/O*.
    Blocking I/O fits very well into the transformational program paradigm and is the default mode in all major operating systems.

    With the advent of multiprogramming, the processor, memory, and peripherals became shared resources.
    This led to the development of *virtual memory* which isolated the memory of each process and the memory of the operating system.
    This protected processes and the operating system from bugs in other processes.
    This allow led to the development of *privileged instructions* where certain capabilities, like peripheral I/O, were taken away from processes and claimed by the operating system.

    Another major advancement for performing I/O efficiently was the *interrupt*.
    When a peripheral had input available or had completed performing output, it could send the processor an interrupt request.
    The presumably busy processor, hence the name interrupt, would stop and process the interrupt request.
    The interrupt handling routine quickly became part of the operating system and operating systems used it to efficiently swap between processes as their I/O completed.
    Eventually, a timer was used to generate interrupts in addition to the conventional I/O peripherals.
    This gave the operating system a way to systematically switch between all processes that were ready to execute (multitasking).

    Multitasking opened the door for *safely* executing programs that do not terminate.
    A program that does not terminate can interact with other running processes and consume an infinite stream of input and produce an infinite stream of output.
    Hence, it became possible to write user-level reactive programs.
    Operating systems, then, started including facilities for *Inter-Process Communication (IPC)* like shared memory, pipes, sockets, etc.
    The process, an abstraction that emerged from the transformational program paradigm, was applied to the reactive program paradigm and reactive systems were conceived of as groups of interacting processes, e.g., Cooperating Sequential Processes, Cooperating Sequential Processes, etc.

3. <a>Popular techniques for reactive programs are based on maintaining compatibility with a transformational legacy.</a>

    The ability to perform I/O on an overlapped and ongoing basis is critical to the reactive program paradigm and allows reactive programs to be engaged in concurrent activities.
    For example, a web application may be processing multiple requests arriving on multiple connections and interacting with multiple databases and external systems to service the requests.
    Given the assumption that a program is a sequential flow of control, there are two ways of accomplishing this.

    First, one can expand the definition of a program to include multiple sequential flows of control, i.e., multi-threading.
    A thread is a sequential flow of control that may share memory with other threads in a group.
    Each thread or process is dedicated to a particular activity and performs blocking I/O to look and feel as transformational as possible.
    The advantage of this approach is that a reactive program can utilize all of the processors in a machine.
    The drawback of this approach is the overhead needed to switch among the threads, i.e., the overhead of context switches.

    Second, one can chop a multi-threaded program at the points where it performs input and output and rearrange it as an infinite loop that checks when I/O may be perform and then invokes a handler that performs that I/O and any subsequent processing, i.e., events.
    Events necessitate the use of non-blocking I/O to ensure responsiveness.
    Non-blocking I/O causes the operating system to return from the system call early instead of waiting for input or output to complete the request.
    Synchronous I/O multiplexing allows a process to wait, i.e., block, until a file descriptor is ready for input or output.
    This approach is often called an *event loop* as it is an (infinite) loop that checks for events on a set of file descriptors.
    The Reactor pattern formalizes this by defining abstract handlers that are invoked in response to the events.
    Events may be combined with multiple threads, that is, each threads runs an event loop on a set of file descriptors.
    The number of threads can be tuned to avoid context switches and achieve optimal performance.

    With respect to compatibility, events only require support from the operating system and the system libraries used to access the operating system.
    Taking advantage of multi-threading, on the other hand, requires changes to processors, the operating system, and programming languages.
    Multi-threading is realized physically through multi-core and multi-processor architectures.
    Modern operating systems use the virtual memory facilities of modern processor to share memory between the threads.
    At the operating system level, multi-threading requires support for the various synchronization and communication mechanisms used by the threads.
    Multi-threading does not necessarily require changes to a (transformational) programming language as exemplified by the C programming language.
    A programming language may choose to introduce features related to multi-threading for the convenience of the developer, e.g., the Java programming language,
    Multi-threading does typically require changes to the language run-time and other supporting libraries.
    For example, the compiler or interpreter may need a different approach to initialization given the existence of multiple threads.

    The influence of the assumption that a program is a sequential flow of control can be seen in the signals IPC mechanism.
    A signal is the software equivalent of a physical interrupt.
    When a process receives a signal, its normal flow of execution is interrupted and the appropriate signal handler is dispatched.
    The activities that can be safely perform in an event handler are quite limited given that the process may be in an arbitrary state, e.g., setting a flag.
    Signals are generally avoided since they interfere with a sequential flow of control.

4. <a>Transformational legacy leads to accidental complexity in reactive programs.</a>

    Adding threads to an inherently transformational language significantly alters the semantics of the language.
    For a good perspective on this read Edward Lee's "The Problem with Threads."
    When writing a multi-threaded program, one must always take into account the possibility of shared access and reason about all possible sequences caused by the threads being interleaved in different ways.
    Essentially, a developer must have a complete understanding of the call graph in order to not make mistakes with respect to concurrency, e.g., corrupted state due to non-synchronized access, deadlock, livelock, etc.
    Experience shows that developers are not able to cope with such an increase in detail and routinely introduce bugs related to concurrency.
    This problem is compounded when one considers software reuse as the modules being used have their own reactive behavior and requirements for correct use and synchronization.

    A single-threaded event loop resolves some of the difficulties of using multiple threads while presenting its own challenges.
    In a single-threaded event loop each event handle is implicitly atomic.
    This raises the level of abstraction from reasoning about interleaved statements and instructions to reasoning about interleaved atomic event handlers.
    This comes at a cost, namely the flow of control is *inverted* from the flow of control in a traditional transformational program since I/O handling code calls application logic.
    Consequently, event-based programs can be difficult to read and brittle.
    Event loops are tied to the features of operating systems and are thus limit in the set of events available and their semantics.
    In the context of reuse, each module tends to run its own event loop, have its own definition for events handler, etc.
    The presence of multiple event loops immediately introduces all of the problems associated with multiple threads.

    For software, accidental complexity is defined as the ``difficulties that today attend its production but that are not inherent [cite Brooks].''
    Thus, we seek to separate the inherent difficulties associated with reactive programs from the difficulties caused by the tools and techniques used to model and implement them.
    Reasoning about infinite and non-deterministic sequences of events *is* an inherent difficulty of asynchronous reactive programs.
    As demonstrated by single-threaded event loops, reasoning about and implementing fine-grained synchronization *is not* an inherent difficulty of asynchronous reactive programs.
    Furthermore, one can imagine implementing a reactive program without a persistent sequential flows of control, generalizing beyond file descriptor I/O, etc.

5. <a name="principled">A model for reactive programs should facilitate the principled composition and decomposition of reactive programs.</a>

    Many models in computing have a number of properties that cause things to "just work."
    Models that facilitate composition and decomposition are particularly attractive because they facilitate top-down design and bottom-up implementation.
    That is, one can start with a problem, decompose the problem into simpler sub-problems, and so on.
    Similarly, when implementing a solution, one can look for existing solutions and compose them together.
    Composition and decomposition are essential techniques in software engineering.

    Models facilitating principled composition and decomposition adhere to the following principles:

    1.  **Definitions** The model should define units of composition and a means of composition.
        Obviously, a model that does not define a unit of composition and a means of composition cannot support a design process based on composition or decomposition.

    2.  **Well-formed-ness** The result of composition should either be a well-formed entity in the model or be undefined.
        Thus, it is impossible to create an entity whose behavior and properties go beyond the scope of the model and therefore cannot be understood in terms of the model.
        When the result of composition is defined, it should often (if not always) be a unit of composition.
        This principle facilitates reuse and permits decomposition to an arbitrary *degree*.

    3.  **Recursive Encapsulation** A unit of composition should be able to encapsulate other units of composition.
        When this principle is combined with the previous principle, the result is *recursive encapsulation* which permits decomposition to an arbitrary *depth*.
        Recursive encapsulation allows the system being designed to take on a hierarchical organization.

    4. **Interfaces**  The behavior of a unit of composition should be encapsulated by its interface.
       Encapsulation allows one to hide implementation details and is necessary for abstraction.

    5. **Restriction** The model, unit of composition, and composition operator(s) should be defined so that composition can only reduce the set of possible behaviors that can be exhibited by a unit of composition.
       That is, given a unit of composition with behavior set *B* and an instance of composition resulting in a new behavior set *B'*, then it must be the case that *B' &sube; B*.
       Given that properties are assertions over behavior, this principle can be restated to say that composition can only restrict properties proved in isolation.
       This principle facilitates reuse since the possible behaviors of a unit of composition is fixed.

    6.  **Compositionality** Composition should be *compositional* meaning that the properties of a unit of composition can be stated in terms of the properties of its constituent units of composition.
        Thus, when attempting to understand an entity resulting from composition, one need only examine its constituent parts and their interactions.
        To illustrate, consider a system *X* that is a pipeline formed by composing a filter system *F* with reliable FIFO channel system *C*.
        This principle states that the properties of the composed Filter-Channel *X* can be expressed in terms of the properties of the Filter *F* and Channel *C*.
        Compositionality requires the ability to establish properties for units of composition that cannot be violated through subsequent composition.

    7. **Substitutional Equivalence** Units of composition should have some notion of substitutional equivalence.
       If a unit of composition *X* contains a unit of composition *Y*, then a unit of composition *X'* formed by substituting the definition of *Y* into *X* should be equivalent to *X*.
       Substitutional equivalence guarantees that we can compose and decompose at will and summarizes the preceding principles.
       We believe that substitution should be linear in the size of the units of composition.
       To illustrate, suppose that *X* and *Y* are mathematical functions in the description above. %% and substitution, therefore, is equivalent to substituting function definitions.
       If we take the size of a unit of composition to be the number of terms in the definition of a function then *|X'| &sim; |X| + |Y|*.

    As previously mentioned, a number of common models and their realizations in various programming languages support principled composition and decomposition including expressions, functions (methods), and types.
    However, most of the models and techniques used for reactive programs do not support principled composition and decomposition.
    To illustrate, threads do not support recursive encapsulation, lack interfaces, change behavior under composition, and have no notion of substitutional equivalence; even in languages that incorporate multi-threading primitives.
    Events lack precise definitions and notions of encapsulation and interfaces which are necessary for principled composition and decomposition.

    The [The Reactive Component Model](model) is a model for reactive programs that facilitates the principled composition and decomposition of reactive programs.

6. <a>A "clean slate" approach to reactive programs is necessary; starting with programming languages.</a>

     Many of the difficulties associated with designing and implementing reactive systems grow from the transformational legacy of modern processors, operating systems, and programming languages.
    It is reasonable to conclude that starting with different assumptions will yield different results.
    More specifically, a processor, operating system, or programming language designed specifically for reactive programs will be different from existing mainstream processors, operating systems, and programming languages.
    Programming languages are the logical starting point as it is much cheaper to experiment in that domain than in the domain of processors and operating systems.
