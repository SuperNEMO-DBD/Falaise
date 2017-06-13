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
6. Install and configure additional software packages recommended for developers:
  - [`clang-format`](https://clang.llvm.org/docs/ClangFormat.html) for applying Falaise's code format style automatically (like a spell checker)
    - If you use `brew`, then simply do `brew install clang-format`
    - On Ubuntu Linux, `apt-get install clang-format`
    - See the `clang-format` documentation for guides on integrating it with [Vim](https://clang.llvm.org/docs/ClangFormat.html#vim-integration) and
    [Emacs](https://clang.llvm.org/docs/ClangFormat.html#emacs-integration) so that it can be run easily or automatically, just like a
    spell checker
    - If you use the [VisualStudioCode](https://code.visualstudio.com) editor, it provides `clang-format` support via its C++ plugin
7. If you encounter any issues with any of these steps, [raise an issue](https://github.com/SuperNEMO-DBD/Falaise/issues/new)

## Making Changes
The workflow adopted for Falaise is the basic "fork-and-branch" model. This
model allows easy contribution without requiring all contributors to have
push access to a single centralized repository, and allows work to continue
without breaking or interfering with that of others.

The `develop` branch of the [SuperNEMO-DBD/Falaise](https://github.com/SuperNEMO-DBD/Falaise)
repository is used as the designated development branch/repository for the SuperNEMO project.
To begin making changes, create a personal fork and local clone of this repository
as described above.

1. In your local clone, begin by ensuring you are syncronized with the latest upstream
   developments:

   ```console
   $ git checkout develop
   ... if not done already ...
   $ git remote add upstream https://github.com/SuperNEMO-DBD/Falaise.git
   $ git fetch upstream
   ...
   $ git merge upstream/develop
   ...
   $
   ```
2. Create a branch on which to make your changes, keep the name short but descriptive:

   ```console
   $ git checkout -b expand-flsimulate-documentation
   Switched to a new branch 'expand-flsimulate-documentation'
   $ git status
   On branch expand-flsimulate-documentation
   nothing to commit, working tree clean
   ```

   In general, there should be one branch per logical task. For example, _"Fix Bug 1234"_ or
   _"Additional documentation for flsimulate"_, not _"Fix Issue X and Add new Module Y"_.
   This is done to help minimize clashes when integrating your changes and to keep a logical
   record of changes between stable releases.
3. [Build, test and run Falaise locally](https://github.com/SuperNEMO-DBD/Falaise#installing-falaise). This provides an
   initial compilation and test before you make any changes.
4. Starting making your changes on the feature branch
   - Recompile and test regularly
   - Follow the style guide *(WIP: at present, just match existing case and indents)* and use `clang-format` to apply/fix spacing and layout
   - You must add [unit tests](https://github.com/philsquared/Catch/blob/master/docs/tutorial.md) for new classes and/or interfaces, or to exercise bugs
   - You must add [documentation](https://www.stack.nl/~dimitri/doxygen/manual/docblocks.html) for new classes and/or interfaces
   - Commit regularly, but only when everything compiles without warning and all tests pass
   - Write [good commit messages](http://tbaggery.com/2008/04/19/a-note-about-git-commit-messages.html)
 5. Publish the feature branch on your fork

  ```console
  $ git push -u origin expand-flsimulate-documentation
  ```

  This can be done at any time to share changes between different clones (e.g. desktop, laptop)
  or as a simple backup.


## Submitting Changes
1. When you have completed work on your feature branch, publish it on
   your fork by pushing the branch to it:

   ```console
   $ git push -u origin my-feature-branch
   ```

   If you have already pushed the branch, you can omit the `-u` flag. The key thing is
   that the branch on your fork has the full set of commits.

2. [Submit a Pull Request](https://help.github.com/articles/creating-a-pull-request/) with the branch to [SuperNEMO-DBD/Falaise](https://github.com/SuperNEMO-DBD/Falaise)

3. The PR will be reviewed by SuperNEMO users and developers as appropriate for the issue addressed.
   Other than style/usability, the core requirements for acceptance of a PR are:

   - If new/modified code, has unit tests which pass on supported platforms (_WIP: CI with Travis_)
   - Any needed documentation is supplied

   Should edits be required, simply add new commits on the feature branch in your local clone
   and push them to your fork. GitHub will automatically append them to the PR.

4. Once approved, the PR will be merged on to the `develop` branch of [SuperNEMO-DBD/Falaise](https://github.com/SuperNEMO-DBD/Falaise). You can then delete your feature branch.


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

