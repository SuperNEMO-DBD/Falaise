Developing Falaise {#developingfalaise}
==================

\tableofcontents

Introduction {#developer_intro}
============
Development of Falaise requires familiarity with four main topics
of modern software development

- [Version Control](@ref developingfalaise_subversion)
- [Build Tools](@ref developingfalaise_cmake)
- [Modern C++](@ref developingfalaise_cpparchapi)
- [Testing](@ref developingfalaise_testing)

These will be discussed below. This document is still a work in progress,
so feedback on what things you'd like to see covered are welcome.

Using Subversion for Version Control {#developingfalaise_subversion}
====================================
- [Subversion](http://subversion.apache.org/)
  - Checkout
  - Updating
  - Committing
  - Branching and Merging
  - Tagging
  - Use of Git

We recommend you consult the [main online Subversion Guide](http://svnbook.red-bean.com)
on these topics.

Commit Feature Based Changes  {#developingfalaise_logicalcommits}
----------------------------
Commits work best when they contain changes related to one *feature*, e.g.
everything needed to fix a bug, or provide a new feature. When commits
contain changes across several areas, it makes it difficult to patch
these into new releases of the code, consequently slowing down development
and hence productive use of the software.

**Never** use the repository as some kind of personal logbook or backup.
In particular, **never** commit any changes without confirming that
the code compiles and pass all tests. In line with the proceeding
discussion, another frequently observed bad practice is "end of day
commits" where you just commit whatever's in your working copy just
before you go home.

Writing Good Commit Messages {#developingfalaise_goodcommitmsgs}
----------------------------
The commit log of the repository acts just like the logbook of an
experiment. It describes exactly what, and more importantly, why, a change
was made. These records are important both for you and your colleagues!

Though Falaise uses Subversion for development, the use of "git style"
commit messages is recommended. Here’s a model commit message:

~~~~~
Capitalized, short (50 chars or less) summary

More detailed explanatory text, if necessary.  Wrap it to about 72
characters or so.  In some contexts, the first line is treated as the
subject of an email and the rest of the text as the body.  The blank
line separating the summary from the body is critical (unless you omit
the body entirely); tools like rebase can get confused if you run the
two together.

Write your commit message in the imperative: "Fix bug" and not "Fixed bug"
or "Fixes bug."  This convention matches up with commit messages generated
by commands like git merge and git revert.

Further paragraphs come after blank lines.

- Bullet points are okay, too

- Typically a hyphen or asterisk is used for the bullet, preceded by a
  single space, with blank lines in between, but conventions vary here

- Use a hanging indent
~~~~~


Building using CMake, Make and Xcode {#developingfalaise_cmake}
====================================
- [CMake](http://www.cmake.org)
  - Link to online guides
  - Handle specific gotchas

Setting Custom Compiler Flags
-----------------------------
HINT: Use the `-v` flag to GNU/Clang compilers to help trace include path problems.
This will print out a complete trace of how the compiler located headers, including
info on any reordering of search paths.

The C++ Architecture and API of Falaise {#developingfalaise_cpparchapi}
=======================================
- [C++](http://isocpp.org)
  - [Coding style and standards](@ref codingstandards)
  - Standards, C++11, C++14
  - Architecture of Falaise
  - Third Party Libraries
  - Multicore

Unit and Integration Testing {#developingfalaise_testing}
============================
Unit tests in Falaise are written using the [Catch framework](https://github.com/philsquared/Catch).
A [dedicated page on unit testing in Falaise is provided](@ref fldevel_unittest).

