File Names
==========
File names are lowercase separated by underscores.

Include Guards
==============
* Header files should have an appropriate include guard.

        #ifndef RC_SRC_TYPES_HPP
        #define RC_SRC_TYPES_HPP
        ...
        #endif // RC_SRC_TYPES_HPP

* The header guard is the all-caps path to the file using an underscore as a separator.

Include Order
=============
1. Corresponding header
2. C headers
3. C++ headers
4. Project headers

Macros
======
Macros are all-caps with underscores.

Namespaces
==========
1. Everything should be in a namespace.
2. Namespaces should be a single word, all lowercase.
3. Namespaces are not indented.
