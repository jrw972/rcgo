File Names
==========
File names are lowercase separated by underscores.

Include Guards
==============
* Header files should have an appropriate include guard.

        #ifndef rc_src_types_hpp
        #define rc_src_types_hpp
        ...
        #endif // rc_src_types_hpp

* The header guard is the path to the file using an underscore as a separator.

Include Order
=============
1. Corresponding header
2. C headers
3. C++ headers
4. Project headers

Namespaces
==========
1. Everything should be in a namespace.
2. Namespaces should be a single word, all lowercase.
3. Namespaces are not indented.
