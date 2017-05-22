# How to Contribute

Contributions to Falaise are essential to maintain and improve the code
to ensure that the SuperNEMO experiment reaches its physics goals.
To ensure stability, reliability and maintainability of the code, and hence physics results,
there are a few guidelines we need contributors to follow so that this
quality is maintained.

In the following, familiarity with Falaise's core software development
tools:

- [C++](http://isocpp.org) as primary language
- [Catch](https://github.com/philsquared/Catch) for C++ unit testing
- [Doxygen](http://www.doxygen.org) plus [Markdown](https://www.stack.nl/~dimitri/doxygen/manual/markdown.html) for API and Reference Documentation
- [Git](https://git-scm.com) for version control, hosting on GitHub
- [CMake](https://cmake.org) for building, running tests and installing

is assumed. [Further documentation on these](#additional-resources) is
available if required.


## Getting Started
1. Get a [GitHub account](https://github.com/signup/free)
2. [Fork the Falaise repository](https://help.github.com/articles/fork-a-repo/) to your account.
3. [Create a local clone of your fork](https://help.github.com/articles/fork-a-repo/#step-2-create-a-local-clone-of-your-fork) to work on changes,
   e.g.
   ```console
   $ git clone https://github.com/<your-username>/Falaise.git
   ```
4. Set up your local clone to [sync with SuperNEMO-DBD/Falaise](https://help.github.com/articles/fork-a-repo/#keep-your-fork-synced),
   e.g.
   ```console
   $ cd Falaise
   $ git remote add upstream https://github.com/SuperNEMO-DBD/Falaise.git
   ```
5. Check that you can [build, test and run Falaise locally](https://github.com/SuperNEMO-DBD/Falaise#installing-falaise)
6. If you encounter any issues with any of these steps, [raise an issue](https://github.com/SuperNEMO-DBD/Falaise/issues/new)


## Making Changes
The workflow adopted for Falaise is the basic "fork-and-branch" model.


## Submitting Changes
1. When you have completed work on your feature branch, publish it on
   your fork by pushing the branch to it:

   ```console
   $ git push -u origin my-feature-branch
   ```
2. [Submit a Pull Request](https://help.github.com/articles/creating-a-pull-request/) with the branch to [SuperNEMO-DBD/Falaise](https://github.com/SuperNEMO-DBD/Falaise)



### Working with Others
In many cases, fixes and features may be worked on by more than one person at a time.
You can share changes with others in the forking workflow by simply adding additional
remotes to your local clone and using `git fetch` and `git merge` appropriately.

For example, let's say I've created a feature branch "fix-bug" in my local clone,
and published it on my public fork:

```console
$ git checkout -b fix-bug
... edit, test, edit...
$ git push -u origin fix-bug
```

If you want to contribute to this branch, you can add my fork to your local
clone, then fetch and checkout the `fix-bug` branch

```console
... in your local clone ...
$ git remote add myusername https://github.com/myusername/Falaise.git
$ git fetch myusername
... checkout their branch
$ git checkout myusername/fix-bug
```

You can now make changes on this branch and then publish these on your
fork:

```console
$ git push -u origin fix-bug
```

You can then either submit a Pull Request to my fork, or I can
add you fork as a remote and merge the changes:

```console
... in my local clone ...
$ git remote add yourusername https://github.com/yourusername/Falaise.git
$ git fetch yourusername
... ensure I'm on my version of the branch
$ git checkout fix-bug
$ git merge yourusername/fix-bug
```




# Additional Resources
## Git and GitHub
- [Software Carpentry Git Lessons](http://swcarpentry.github.io/git-novice/)
- [Pro Git Documentation](https://git-scm.com/book/en/v2)
- [General GitHub documentation](https://help.github.com/)
- [GitHub pull request documentation](https://help.github.com/articles/creating-a-pull-request/)

## C++
- [Official ISO C++ Site](https://isocpp.org)
- [cppreference](http://en.cppreference.com/w/)
- [Catch](https://github.com/philsquared/Catch) Unit Testing [Tutorial](https://github.com/philsquared/Catch/blob/master/docs/tutorial.md) and [Reference](https://github.com/philsquared/Catch/blob/master/docs/Readme.md) Guides

## CMake
- [Core CMake Documentation](https://cmake.org/documentation/)

## Falaise
- [Reference and API Guide](https://supernemo-dbd.github.io/Falaise)

