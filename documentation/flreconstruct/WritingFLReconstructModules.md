Writing FLReconstruct Modules
=============================
If you have just started using Falaise or the FLReconstruct application,
we strongly recommend that you familiarize yourself with the basic usage
of FLReconstruct covered in [The FLReconstruct Application](@ref md_FLReconstruct).

Start with an empty directory

~~~~~
$ cd MyWorkSpace
$ mkdir MyModule
$ cd MyModule
$ ls
$
~~~~~

FLReconstruct modules must inherit from Bayeux's @ref dpp::base_module
abstract base class. This declares three pure abstract methods that
any concrete module must implement:

- @ref dpp::base_module::initialize
- @ref dpp::base_module::process
- @ref dpp::base_module::reset

In addition, to make the module loadable by `flreconstruct`, we need
to use a couple of macros to declare and define the loading and
registration mechanisms for us.

We therefore begin implementing our module by writing the header file,
which we'll name `MyModule.h`:

\include flreconstruct/MyModule/MyModule.h

Now add the implementation file, which we'll name `MyModule.cpp`

\include flreconstruct/MyModule/MyModule.cpp

Finally, we need to build the binary module, so we add a CMake script
to do this.

\include flreconstruct/MyModule/CMakeLists.txt

