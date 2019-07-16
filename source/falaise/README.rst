===================
Falaise C++ Library
===================

What's Here
===========

The Falaise C++ API, so main interfaces and resources needed for:

- Use in the main simulation/reconstruction applications
- Use in reconstruction pipeline modules

Code Organization
=================
The code is organised as follows, but note that this is still a work
in progress and is thus subject to aggressive refactoring (though
changes will be noted here).

::

    +- ./
       ... Top Level Code ... (e.g. API, versioning)
       +- common/
       |  ... Placeholder for utilities common to all Falaise tasks ...
       |  +- detail/
       |  |  ... Implementation details that appear in public API ...
       |  +- private/
       |     ... Completely private implementation details ...
       +- snemo/
          +- ... Any code specifically for SuperNEMO ...
          +- detail/
          +- private/


Header files should be organised into the above hierarchy, with
implementation files all in the main module directory, e.g.

::

    +- common/
       +- foo.h
       +- foo.cc
       +- bar.cc
       +- baz.cc
       +- detail/
       |  +- bar.h
       +- private/
          +- baz.h

Code should be namespaced according to this structure. For example, if
`foo.h`, `bar.h` and `baz.h` declare the classes `foo`, `bar` and `baz`,
then these will have the namespace structure:

.. code:: cpp

    namespace falaise {
    namespace common {
    class foo;

    namespace detail {
    class bar;
    class baz;
    }
    }
    }

In general, you

1. *must not* use more than these three levels of namespacing.

2. *must* use concise but descriptive names for headers and classes.

3. *should* use CamelCase to name headers and types.


Rationale
---------

1. Split code into coherent *modules* (`common` for example).

2. Split *module* code so that public and private interfaces are clear.

    - *public* interface means the classes, types and functions that a
      client of the *module* uses.
    - *private* interface means the classes, types and functions that
      are *only used internally to the module*.
    - See below for an example.

3. Keep *module* headers in same directory structure as they will be
   installed so paths are coherent between builds and installs.

    - A separate *private* directory is used to explicitely delimit
      headers that are not for installation.

4. Keep *all* *module* sources in same main directory. This simplifies the
   layout and build.


What are Public and Private Interfaces?
---------------------------------------
Let's revisit our example from above:

::

    +- common/
       +- foo.h
       +- foo.cc
       +- bar.cc
       +- baz.cc
       +- detail/
       |  +- bar.h
       +- private/
          +- baz.h

Here, `foo.h` is the only file of relevance to a client of the `common`
module. It could look something like

.. code:: cpp

    #include <falaise/common/detail/bar.h>

    namespace falaise {
    namespace common {
    class foo {
     public:
      foo();
      ~foo();

     private:
      detail::bar<int> barCol_;
      class detail::baz;
      detail::baz* bazPtr_;
    };
    }
    }

and the corresponding source file:

.. code:: cpp

    #include <falaise/common/foo.h>
    #include <falaise/private/baz.h>

    ...

Here, both `bar` and `baz` are implementation details of `foo` class, i.e.
they never appear in the public methods of `foo`. The difference between
them is that the declaration of `bar` is needed by `foo.h` whereas that
for `baz` isn't, due to the former being on the stack and the latter on the
heap. We therefore need inclusion of the `bar.h` header in `foo.h`, but
as clients of `foo` don't need to know, or use, `bar`, we simply install
the header into a special `detail` directory. This doesn't stop clients
using `bar`, but it's useful to use this `detail` marker to indicate that
it is an implementation detail. In the case of `baz` we completely hide it
using the "pImpl" idiom, so only a forward declaration is needed. The
`baz` header then only needs inclusion in the source file for `foo`. We use
a separate `private` directory to explicitely mark it as only relevant when
building the `common` module. We could put `baz.h` in the `detail`
directory, but a separate directory helps to identify the difference
between the two cases.

This is a slightly contrived example, so you should refer to documents
like "Effective C++" (Item 35) for more details.

