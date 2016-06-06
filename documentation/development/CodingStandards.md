Coding Style and Standards for Falaise {#codingstandards}
======================================

\tableofcontents

Introduction {#coding_intro}
============
The key guidelines for Falaise code are **consistency**, **readability**
and **C++ standard compliance**. The aim is not to fight wars
over minutiae, but provide an easy to use and common guide for how
to layout, name and implement code.

This guide is still a work in progress. If in doubt, use what's in the
file you're working on already. If you're starting from scratch,
read the sections below!

Basic Style Guide
-----------------
Falaise code should follow the [Google C++ Style Guide](http://google-styleguide.googlecode.com/svn/trunk/cppguide.xml). The [C++ Coding Standards](http://www.gotw.ca/publications/c++cs.htm) textbook is also used to
provide guidance on writing production quality code.


Consistency
-----------
Whatever coding style you use, it **must be consistent** throughout the
code. Layout code blocks using the same style throughout your code.
Don't swap styles between files, or worse, halfway through a file.
If a style is already present in a file you are working on, use that
style.

Readability
-----------
Prefer `PascalCase` for typenames and namespaces. Prefer `camelCase` for
variables, class member functions and class data members. The reason
for this is that `snake_case` names can reduce readability, especially
if used for everything.

Use meaningful and concise names for variables and types. For example,
even though we're physicists, `speedOfLight` is a better variable name
than `c`.


C++ Standard Compliance
-----------------------
Your code must be fully compliant with the C++03 standard. No C++11/14
constructs are permitted in core Falaise. The migration to C++11/14 is
expected in Falaise 3. FLreconstruct modules outside of core Falaise
are permitted to use C++11/14.

Use modern compilers, such as GCC or Clang at their latest stable
revisions. Compilers are continually improving, and there is no excuse
not to be using recent versions.

