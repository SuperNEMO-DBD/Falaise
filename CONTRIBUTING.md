# How to Contribute to Falaise

Contributions to Falaise are essential to maintain and improve the code
to ensure that the SuperNEMO experiment reaches its physics goals.
To ensure stability, reliability and maintainability of the code, and hence physics results,
there are a few guidelines we need contributors to follow so that this
quality is maintained. All levels of contribution are welcome, from
additions/copy editing of documentation through bug fixes and new C++ features.

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
5. Check that you can [build, test and run Falaise locally](README.md#installing-falaise)
6. Install and configure additional software packages recommended for developers:
  - [`clang-format`](https://clang.llvm.org/docs/ClangFormat.html) for applying Falaise's code formatting and style style automatically (a code "spell checker")
    - If you use `brew`, then simply do `brew install clang-format`
    - On Ubuntu Linux, `apt-get install clang-format`
    - See the `clang-format` documentation for guides on integrating it with [Vim](https://clang.llvm.org/docs/ClangFormat.html#vim-integration) and
    [Emacs](https://clang.llvm.org/docs/ClangFormat.html#emacs-integration) so that it can be run easily or automatically
    - If you use the [VisualStudioCode](https://code.visualstudio.com) editor, it provides `clang-format` support via the [Microsoft C++ plugin](https://marketplace.visualstudio.com/items?itemName=ms-vscode.cpptools)
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
2. Create a branch off of `develop` on which to make your changes, keep the name short but descriptive:

   ```console
   $ git checkout -b expand-flsimulate-documentation develop
   Switched to a new branch 'expand-flsimulate-documentation'
   $ git status
   On branch expand-flsimulate-documentation
   nothing to commit, working tree clean
   ```

   In general, there should be one branch per logical task. For example, _"Fix Bug 1234"_ or
   _"Additional documentation for flsimulate"_, not _"Fix Issue X and Add new Module Y"_.
   This is done to help minimize clashes when integrating your changes and to keep a logical
   record of changes between stable releases.
3. [Build, test and run Falaise locally](README.md#installing-falaise). This provides an
   initial compilation and test before you make any changes.
4. Starting making your changes on the feature branch
   - Recompile and test regularly, ensuring that code compiles without warnings or errors
   - Follow the [style guide](documentation/development/CodingStandards.md) and use `clang-format` to apply/fix spacing and layout automatically
   - You must add [unit tests](documentation/development/UnitTestingWithCatch.md)for new classes and/or interfaces, or to exercise bugs
   - You must add [documentation](https://www.stack.nl/~dimitri/doxygen/manual/docblocks.html) for new classes and/or interfaces
   - Pure documentation updates are also welcome, including HOWTOs and user guides which can be written in [Markdown](http://www.stack.nl/~dimitri/doxygen/manual/markdown.html)
to make writing and online presentation clearer.
   - Commit regularly, but only when everything compiles without warning and all tests pass
   - Write [good commit messages](http://tbaggery.com/2008/04/19/a-note-about-git-commit-messages.html)
 5. Publish the feature branch on your fork

  ```console
  $ git push -u origin expand-flsimulate-documentation
  ```

  This can be done at any time to share changes between different clones (e.g. desktop, laptop)
  or as a simple backup.


## Submitting Changes
1. Publish your feature branch on your fork by pushing the branch to it:

   ```console
   $ git push -u origin my-feature-branch
   ```

   If you have already pushed the branch, you can omit the `-u` flag. The key thing is
   that the branch published on GitHub has the up to date set of commits you want to propose.

2. [Submit a Pull Request](https://help.github.com/articles/creating-a-pull-request/) with the branch to [SuperNEMO-DBD/Falaise](https://github.com/SuperNEMO-DBD/Falaise).
   A template is provided to help you describe the purpose of the PR (Bugfix,
   enhancement, documentation etc), and must be filled out. If your PR provides a
   fix for one or more already reported issues, ensure the `Fixes #<NUMBER>` lines
   are filled in so that the related issues will be automatically closed when the PR is merged.
   "Work in Progress" Pull Requests are welcome, and should have titles that begin with "WIP:".
   These are best used for work that you expect may take several iterations to complete,
   to mark the start of work on a fix/feature and avoid duplication of effort, or to mark
   work that you want help with. 
   
3. The PR will be reviewed by SuperNEMO users and developers as appropriate for the issue addressed.
   The core requirements for acceptance of a PR are:

   - The code compiles and passes tests on [Travis-CI](https://travis-ci.org/SuperNEMO-DBD/Falaise)
     for the main supported platforms.
   - If new code is added, it must have [unit tests](documentation/development/UnitTestingWithCatch.md) which pass on supported platforms.
   - Any needed API and usage documentation is supplied
   - The code follows the [Falaise style guide](documentation/development/CodingStandards.md)

   On the page for your Pull Request, GitHub will display the status of the automated checks
   performed. The main one to watch is the `continuous-integration/travis-ci/pr` check,
   which configure, builds and tests the Pull Request. Clicking on the `Details` link will
   take you to the page on Travis which will display progress and results of these steps.
   In the case of failures, the log can be reviewed to see what went wrong and where in
   the code it happened.

   Should changes be required to the Pull Request, simply make new commits on the feature
   branch in your local clone and push them to your fork. GitHub will automatically append
   them to the PR, and Travis-CI will run new builds.

4. Once approved, the PR will be merged on to the `develop` branch of [SuperNEMO-DBD/Falaise](https://github.com/SuperNEMO-DBD/Falaise). You can then delete your feature branch.


### Reviewing Pull Requests Locally
If you are a Pull Request reviewer, then changes can be reviewed and commented on
directly through the [GitHub review interface](https://help.github.com/articles/about-pull-request-reviews/).
This is useful to comment on higher level aspects like naming, style and design.

It is also possible to checkout the changes in a Pull Request onto a branch in
your working copy for detailed compile, test and run checks. The interface and
commands to do this are [documented in GitHub Help](https://help.github.com/articles/checking-out-pull-requests-locally/).
Note that this creates new branches in yout working copy, so after review and
merge/dismissal of the PR, you should delete the local branch using `git branch -d`
or `git branch -D` as needed.


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


# Release Management

The preparation of new releases is handled by Falaise's admins (@drbenmorgan and @goliviero). A workflow for
this process is outlined [in a draft document](documentation/development/PreparingReleases.md). Note in particular
that updates to external dependencies such as ROOT and Geant4 should be requested through a new Issue Report.

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

