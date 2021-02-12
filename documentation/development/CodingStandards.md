Coding Style and Standards for Falaise
======================================

This document is a work in progress, and is expected to evolve
through both the lifetime of the Falaise software as well as
the now regular updates to the C++ language it is written in.

With language and design being an ever changing landscape, the
key guidelines when writing code for Falaise are

- **consistency** (mutual understanding)
- **readability** (clarity of intent)
- **C++ standard compliance** (reliability)

This guide is unapologetically "do as we say, not as we do", so fixes
to improve standardization are very welcome!

Consistency
-----------

Falaise code should follow the [Google C++ Style Guide](http://google-styleguide.googlecode.com/svn/trunk/cppguide.xml) for spacing, layout and organization of classes.
To help apply this formatting, use of the [clang-format tool](https://clang.llvm.org/docs/ClangFormat.html) is recommended.
A `.clang-format` file for Falaise is supplied at the root of the source tree.
Integration of `clang-format` with your favourite editor or IDE is easy, with guides
available in the [clang-format documentation](https://clang.llvm.org/docs/ClangFormat.html).
Falaise's build system can also run `clang-format` for you. Ensure that `clang-format`
is installed and available in your path, then

```
$ cmake <sourcedir>
$ make clang_format
```

will run the tool over all source code and apply the formatting automatically.


Readability
-----------
Keep names concise. Prefer `snake_case` for typenames and namespaces unless names are long (and then you should prefer to clarify the name!). Prefer `camelCase` for
variables and class member functions. Prefer `camelCase_` for
class data members to distinguish them from input variables or function calls.

Avoid

Use meaningful and concise names for variables and types. For example,
even though we're physicists, `speedOfLight` is a more meaningful variable name
than `c`. Conciseness helps readability, and is especially important
as `snake_case` is less readable to the human eye and `adds_one_underscore_extra_per_word`.


C++ Standard Compliance
-----------------------
Your code must be fully compliant with the C++11 standard. No C++14/17
constructs are permitted in core Falaise. The migration to C++17 is
expected in Falaise 5.

No compiler vendor extensions are permitted, e.g. "-std=gnu++11".

It is recommended to follow the [C++ Core Guidelines](http://isocpp.github.io/CppCoreGuidelines/CppCoreGuidelines) for practical and pragmatic use of the C++ language.
The [C++ Coding Standards](http://www.gotw.ca/publications/c++cs.htm) textbook also provides
useful guidance on writing stable and useful code.
