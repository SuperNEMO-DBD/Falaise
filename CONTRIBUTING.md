# How to Contribute to Falaise

The authoritative Falaise Git repository is hosted on GitHub at
https://github.com/supernemo-dbd/falaise.git. New code developments
whether bug fixes, documentation, or new features, are essential to maintain
and improve the software to ensure that the SuperNEMO experiment reaches its physics goals.
To ensure stability, reliability and maintainability of the code, and hence physics results,
a standardized workflow is enforced for submission of new code via
Git and GitHub Pull Requests.

If you have any issues with or questions about this workflow, don't
hesitate to raise these via a new topic on the Issues Board at https://github.com/supernemp-dbd/falaise/issues.

# Basic Tools
## Git
[Git](https://git-scm.com) is the version control system used by SuperNEMO. If it
is not installed on your system already, it is usually available through
package managers for your system (apt, yum, spack, Homebrew, vcpkg) or comes
bundled with IDEs such as XCode or Visual Studio. Failing this, the Git homepage provides
a range of [binary bundles for Linux, MacOS, and Windows](https://git-scm.com/downloads).

If you are new to git or need additional information, a wide range of documentation
and tutorials are available, including:

- The [Atlassian Tutorials](https://www.atlassian.com/git/tutorials)
  are a good starting point, and also cover more advanced topics.
- The official [Pro Git book](https://git-scm.com/book/en/v2) offers greater
  depth and breadth, Chapters 2 and 3 covering the basics.

Many other resources are available, with a selected few being:

- [Version Control with Git](http://swcarpentry.github.io/git-novice) tutorial by the
  [Software Carpentry](https://software-carpentry.org) project
- [Help by GitHub](https://help.github.com)
- [Learning Resources by GitHub](https://try.github.io). See in particular the
  [Learn Git Branching](https://learngitbranching.js.org) tutorial on the very
  important concept of branches

Whilst the remainder of this guide will focus on the use of `git` on the
command line, [several GUIs are available](https://git-scm.com/downloads/guis/)
allowing viewing and/or interaction with local and remote repositories.

## GitHub
SuperNEMO uses [GitHub](https://github.com) to host the authoritative repository
for Falaise and developer Forks. Here, "authoritative" means the Collaboration's main
repository containing the current development line of Falaise and from which approved and validated releases are made.

GitHub's [help pages](https://help.github.com/) provide a good guide and overall reference to GitHub. [Getting Started With GitHub](https://help.github.com/en/categories/getting-started-with-github)
covers most of the major topics in more detail. In particular,
the following three guides will help you get started with:

- [Creation of your Individual GitHub Account](https://help.github.com/en/articles/signing-up-for-a-new-github-account)
- [Basic Git Setup](https://help.github.com/en/articles/set-up-git#setting-up-git)
- [Setting up SSH Authentication with Keys](https://help.github.com/en/articles/set-up-git#next-steps-authenticating-with-github-from-git)

We'll refer to pages under this later in the guide of specific areas. [Full
documentation](https://help.github.com/en#dotcom) on GitHub is available, including
additional topics beyond the basics listed above.

GitHub has quite an intuitive web browser interface, so just going to https://github.com/supernemo-dbd/falaise
and clicking around to explore and view files, branches or commits is quite instructive.

## C++ Coding Tools and Guidelines
System and software requirements, plus instructions on how to compile and test
Falaise are described in the main [README](README.md)

- [C++](http://isocpp.org) as primary language
- [CMake](https://cmake.org) for building, running tests and installing
- [Catch](https://github.com/philsquared/Catch) for C++ unit testing
- [Doxygen](http://www.doxygen.org) plus [Markdown](https://www.stack.nl/~dimitri/doxygen/manual/markdown.html) for API and Reference Documentation


# Falaise Git Workflow
## Basic Git Configuration
If you are using git **for the first time**, it is important to set the name
and email address which git will use to mark authorship of your commits. Failure
to do this properly may cause problems when you try to push to GitHub
You can check that your name and email address are properly set by running

``` console
$ git config --list
```

and looking for the `user.name` and `user.email` entries. If these are not
set, they can be set using the following commands

``` console
$ git config --global user.name "Your Name"
$ git config --global user.email "your.email@domain.xyz"
```

So that GitHub can track your contributions, `user.email` should be an address
that you have [registered in your GitHub profile](https://help.github.com/en/articles/managing-email-preferences).

We strongly suggest that you [set up SSH keys on GitHub](https://help.github.com/en/articles/connecting-to-github-with-ssh)
to authenticate when using command line `git`.

We also strongly recommend that you also add the following setting (provided
that you have Git version 1.7.11 or newer):

``` console
$ git config --global push.default simple
```

There are many other configuration settings for git, and the
[Atlassian Git Config Tutorial](https://www.atlassian.com/git/tutorials/setting-up-a-repository/git-config)
gives a good overview of the most useful ones.

Note that the above are "global" settings, applying to all Git repositories
on your system. If you need different settings for other projects, then
after cloning your Fork of the Falaise repository (see [Clone the Forked Repository Locally](#Clone the Forked Repository Locally)),
change directory into it and run:

``` console
$ git config --local user.name "Your Name"
$ git config --local user.email "your.name@domain.xyz"
```

These settings will then only apply to this clone of Falaise.

The above settings are the minimum required for using Falaise's Git/GitHub
Workflow.

## Fork the Main Repository
The main Falaise git repository is https://github.com/supernemo-dbd/falaise. Follow that link
and then create your own copy of the repository by pressing the `Fork` button on the
project’s front page (usually in the top right hand corner). If you are presented with a number
of *group options* when you start to fork, pick your personal account.

You now have your own personal copy of the Falaise code on GitHub that you can use to clone
from, and publish to, for your own developments. You will also be unperturbed by other developers
working on their forked copies, and likewise your work will not affect theirs.

To keep informed about ongoing developments and discussions you should set up
GitHub notifications by visiting the main repository at https://github.com/supernemo-dbd/falaise
and clicking on the "Watch" icon in the top right hand corner next to the "Star" and
"Fork" buttons. It's recommended that you set the notification level to
at least "Releases Only" so you will receive emails for all Issues/Pull Requests
you submit or comment on, when others mention you on Issues/Pull Requests, and for
new Releases.


## Clone the Forked Repository Locally
You now have a GitHub fork of your own, but this is used primarily for sharing your changes
with others. To make a change you need a local working copy on your computer(s) that you can
edit yourself. In git this is done by *cloning* your fork.

The `git clone` command requires a URL pointing to the repository you want to clone, in
this case your fork. If you have set up [SSH keys for your GitHub account](https://help.github.com/en/articles/connecting-to-github-with-ssh),
then the following will take a copy of the repository and check out a working copy for you.

``` console
$ git clone git@github.com:[YOUR_GITHUB_ID]/falaise.git
  ... output ...
```

This will give you a Local Clone of your fork under a `falaise` subdirectory of the directory
where you run this command. GitHub also offers an HTTPS URL for cloning, but we recommend SSH keys
for your own repositories for ease and security. We'll see the HTTPS URL in use in the next section when we come to connect the authoritative repo as a remote, where we only require read access.

## Add the Authoritative Repository as a Remote

The tip (or `HEAD`) of the `develop` branch of the **authoritative** Falaise repository, https://github.com/supernemo-dbd/falaise,
always has the latest tested and approved code. Your local developments therefore need to be
able to start from, and track the changes on, this (See also `Create and Manage a Topic Branch` below). We enable this by adding the authoritative repository to our Local Clone as a *remote*:

``` console
$ cd falaise
$ git remote add upstream https://github.com/supernemo-dbd/falaise.git
```

Your Local Clone now has two remotes, `origin` (your fork) and `upstream` (the authoritative repository):

``` console
$ git remote -v
origin	git@github.com:[YOUR_GITHUB_ID]/falaise.git (fetch)
origin	git@github.com:[YOUR_GITHUB_ID]/falaise.git (push)
upstream	https://github.com/supernemo-dbd/falaise.git (fetch)
upstream	https://github.com/supernemo-dbd/falaise.git (push)
```

We use the name `upstream` for the authoritative repository purely by convention. You
are free use another name if that makes the workflow easier, simply use that name
in place of `upstream` when adding the remote and running the tasks documented below.


## Develop Code
### Create and Manage a Topic Branch
In Git, different lines of development are separated by *branches*. The first step of development
is thus to create a new branch to hold your changes:

``` console
$ git checkout develop
...
$ git fetch upstream
... information on status and updates ...
$ git rebase upstream/develop
... information on rebasing develop ...
$ git push origin develop
$ git checkout -b topic-name develop
```

Why the `checkout`, `fetch`, `rebase`, and `push` operations first? As noted above, your fork
and local clone are independent copies of the main Falaise git repository.
GitHub keeps a record of this to help in book-keeping and making Pull Requests, but
it *does not* automatically update your fork or clone with any changes made to the main
repository such as updates to branches or new tags. These commands ensure that
you are starting from the correct source for your Topic Branch, that your local
repository has the newest commits from the main repository, and that the `develop`
branches of your Local Clone and remote Fork are correctly synchronised with the
main repository before creating the branch.

The `checkout` command creates and switches your working copy to a branch called
`topic-name`. Branches can have any name, but we recommend that you name your
branch to reflect the topic being addressed, e.g. `fix-issue-42`.

You should have one branch per topic you are working on (hence the term
"Topic Branch") to keep the changes focussed and ease the later Pull Request
process. Git branches are cheap in CPU/Storage, so use them to organise your
development work into focussed, orthogonal tasks.

The last element in the `checkout` command is the *start point* for your work.
We use `upstream/develop` as this points to the current stable development line
in Falaise that we've just fetched. That is, all code that has passed testing, and has
been approved and merged to the authoritative repository's `develop` branch. You should always start new
Topic Branches from `upstream/develop`. Because the `develop` branch is used for integrating
code for release, you *should never* make commits directly to it, only ever committing to
Topic Branches. This helps both your work and the upstream integration avoid conflicts and
ease testing.

You can see what branches you are currently on plus others available in your working copy
with the `git branch` command:

```console
$ git branch
...
  develop
* my-topic
...
```

You can switch between branches at any point using the `git checkout` command

```console
$ git checkout develop
Switched to branch 'develop'
$ git branch
...
* develop
  my-topic
...
```

You can also list branches present on the remote repositories using the `-a` flag
to `git branch`:

``` console
$ git branch -a
...
* develop
  my-topic
...
remotes/origin/develop
...
remotes/upstream/develop
```

When created, your Topic Branch only exists in your Local Clone. To publish it
on your fork on GitHub use `git push`:

```console
$ git push -u origin my-topic-branch
```

the `-u` flag is only required the first time you `push` your branch and
associates the local Topic Branch with one of the same name on GitHub (or sets up your
local branch to "track" that on your fork).

If you develop on several machines (e.g. Desktop and Laptop), simply repeat the above process
to create Local Clones on them. If you have created and published a
Topic Branch on your fork, it can be checked out later in any of these
clones using:

```console
$ git fetch origin
... updates
$ git branch -a
...
* whatever-branch-you-are-currently-on
...
remotes/origin/my-topic-branch
...
$ git checkout -t origin/my-topic-branch
```

Git will checkout and "track" your Topic Branch, so you can work on and
share changes between as many clones as you require.


### Local Development Cycle
Once you are on your Topic Branch, you can start making changes to the code
with your preferred editor/IDE. Whilst working on your Topic Branch, you should
regularly synchronize it with the `develop` branch of the authoritative Geant4 repository (i.e.
`upstream/develop` as we've set the remotes up) to ensure your work builds on the latest
developments. This is done in your local repository using the `fetch` and `rebase` commands:

``` console
$ git checkout my-topic-branch
... ensures we are on the correct branch
$ git status
On branch my-topic-branch
$ git fetch upstream
...
$ git rebase upstream/develop
```

You should run this regularly, and also immediately **before** submitting your Topic
Branch as a Pull Request. This minimizes any fixes you may have to do as a result
of conflicts between your changes and those in other Pull Requests.
Be careful with `rebase` after you have pushed your Topic Branch to your Fork
as `rebase` changes the commit history. We'll discuss how to handle this later.

You should also frequently check that your changes compile and pass needed tests.
Please consult the main [README](README.md) for complete details on configuration, build, and testing options.

Do remember that you have the power of a local git repository at your fingertips
during the development process so:

- You can commit code locally at any time, to make it easy to rollback to a known working point
- You can even do more speculative development on a branch from your main Topic Branch - merge it in if it works, throw it away otherwise

We strongly recommend that you *commit early, commit often* for every coherent,
compilable piece of the overall topic you are working on. Git commits code in two phases:

1. You add changed files to a staging area with `git add`
2. You commit the staging area with `git commit`

```console
# ... assuming you are in the git repository
# ... always good to check where we are, what's been done
$ git status
... content...

# ... more detail if we need
$ git diff

# ... add files
$ git add <files we want to add to the staging area>
$ git status
... show status output
```

`git add` can take wildcards and it’s possible to add all changed files and
new files automatically with `git add -A`, **but be extremely careful not to add unwanted
files to the repository such as editor or build temporaries**. Always use `git status` to
check what will be/had been staged **before you commit**. Note that `git status` also helpfully tells
you what to do if you want to rollback changes to a file or unstage files:

- `git reset` will unstage changes for you if you did an add of something in error.
- `git checkout FILE` will revert `FILE` to the original version.
- `git reset --hard` will unstage and discard all local changes, use with caution

Once you're happy with the changes that have been staged, we can commit:

```console
$ git commit
```

At this point, `git` will open up an editor (determined by `git config` or
the `EDITOR` environment variable) to allow you to write a message describing the commit.
It is **vital** that you write good commit messages, both for your own records and
to help your colleagues now and in the future. Think of the commit log as the "experiment
shift logbook" for Falaise!

Commit messages should be informative yet concise. It is suggested that you write
a short one line explanation, followed by a blank line, then some additional longer
paragraphs explaining the changes. Use less than 72 characters per line - commit
logs get read at the terminal.

Further guidelines are in `How to Write a Git Commit Message <https://chris.beams.io/posts/git-commit/>`_
and `5 Useful Tips For A Better Commit Message <https://robots.thoughtbot.com/5-useful-tips-for-a-better-commit-message>`_.

An example of a good commit message would be:

```console
Fixed uninitialised value and leak in alignment tool

It was possible to have the alignmentOffset variable
uninitialised in certain situations outlined in Issue #1001.
This patch corrects for that and changes the behaviour
to return StatusCode::FAILURE immediately if the
setup is inconsistent at initialise.

The memory leak from failing to delete the pointer returned
from the foo tool was fixed.
```

The commit message should say *why* something was changed, not *what* was changed
(so it’s not necessary to list changes by filename). You can also cross-reference
GitHub Issues and Pull Requests using [GitHub Markdown References](),
which can help to keep track of developments and progress.

Git is a distributed version control system, so all of the commits made so far
are *only present in your local working copy*. If you haven't published your Topic Branch
already on your fork on GitHub, as documented earlier, use `git push`:

```console
$ git push -u origin my-topic-branch
```

the `-u` flag is only required the first time you `push` your branch and
associates the local Topic Branch with that on your GitHub fork (or sets up your local branch to
"track" that on your fork). Subsequent pushes can be done with a simple `git push`.

If you have pushed to your fork and then rebased your Topic Branch, the next
push will need to be forced as rebasing changes the Git history. In this case,
you should push using the ``--force`` argument:

```console
$ git push --force origin my-topic-branch
```

If you work on your Topic Branch across several Local Clones (e.g. Desktop/Laptop), you
should synchronize these using the same procedure of `fetch` followed by `rebase`. For example,
using a Local Clone of your Fork on your Desktop, you have made some commits to your Topic Branch
and pushed them to your Fork. Later on, you move to your Laptop and continue to work on the
Topic Branch, beginning work by doing:

```console
$ git branch
...
* my-topic-branch
...
$ git fetch origin
... notifies you about new commits on my-topic-branch
$ git rebase origin/my-topic-branch my-topic-branch
```

Consistent use of fetch/rebase will help to maintain a consistent linear history on
your Topic Branch. It will not prevent true conflicts however. These should be dealt with
as described in the [GitHub tutorial](https://help.github.com/articles/resolving-merge-conflicts-after-a-git-rebase).


## Making a Pull Request
Once you have developed the Topic Branch to your satisfaction, it needs
to be submitted as a GitHub [Pull Request](https://help.github.com/en/github/collaborating-with-issues-and-pull-requests/about-pull-requests).
to the authoritative Falaise repository for full testing and integration.

### Submitting your Topic Branch
Ensure your Topic Branch is pushed to your Fork on GitHub with all commits you want to submit.
To submit the Topic Branch as a Pull Request, go to the authoritative repository,
and follow the GitHub Help guide on [Creating a pull request from a fork](https://help.github.com/en/articles/creating-a-pull-request-from-a-fork),
ensuring that:

- At Step 4 you select the **base fork** as `SuperNEMO-DBD/Falaise``, and `base` is `develop`
- At Step 5 you select the **head fork** as yours (i.e. `YOUR_GITHUB_ID/Falaise`) and the `compare`
  as your Topic Branch.
- At Step 6 you fill out the details:
  - `Title` must  include a short description, e.g. `Fix Issue #1234`
  - `Description` should be filled in with an overview of, and motivation for, the proposed changes
    - If the Pull Request provides an implementation/fix for an Issue, you can cross-reference
      to the Issue using a [GitHub Autolink](https://help.github.com/en/articles/autolinked-references-and-urls#issues-and-pull-requests)
    - You can also use [mark the Issue for closure](https://help.github.com/en/articles/closing-issues-using-keywords) if the Pull Request will resolve it


Finally, click on the `Create pull request` button, which will set it up take you to the review and
integration page for your new Pull Request. This page is used to manage and track progress
of the Topic Branch through the review and test process.


### Merge Request Review and Testing

After submitting your Topic Branch as a Pull Request it is tested and reviewed, this
process being managed through [the Pull Requests page for Falaise](https://github.com/supernemo-dbd/falaise/pull_requests).
This page lists all in progress Pull Requests, so simply find yours by the `Title`
you entered for it. The page for your Pull Request is basically a thread of comments to which
collaborators and you can post on to discuss its status and progress through review
and testing. GitHub should notify you via email of all new posts and updates, so you can
monitor and respond to testing problems or reviewer requests. Please keep all discussions
online in the Pull Request comments to keep everyone in the loop on changes.

The stages of review and testing are listed below:

1. Automatic Travis-CI Build and Test against latest `develop`
2. Code Fixes and Review
3. Acceptance and merge to `develop`

It's important to note these steps are iterative and that you can update your Topic Branch
to address any issues identified. You should not close the Pull Request or delete the
associated Topic Branch in response to failures or requests for changes. The workflow aims
to gradually tune and improve the code responsively without requiring a start from scratch.
Generally, Pull Requests are *only* closed without a merge to the `develop` branch if
the work reaches a dead-end or a better solution to the topic is identified.

### Finishing Up
After your Pull Request is accepted and merged to the authoritative repository's `develop` branch,
you should delete the Topic Branch from your Fork and Local Clone, and update them with
the new `develop`.

Whilst deleting the Topic Branch is not required, it avoids clutter and clarifies your active work.
No information is lost, as the commit history is part of `develop` after merging.
Go to the GitHub page for your fork and click on ``Repository -> Branches``
from the left hand pane. This will bring up a list of branches present in your fork,
and you can delete any you no longer require using the red trashbin icon on the right
hand side.

To delete the Topic Branch in your Local Clone, first fetch and rebase the latest
changes from the authoritative repoistory:

```console
$ git checkout develop
$ git fetch upstream
...
$ git rebase upstream/develop

To synchronize your Local Clone and Fork, run a fetch followed by push:

```console
$ git fetch origin --prune
...
$ git push origin master
...
$ git push origin --tags
```

Here, the fetch is used with `--prune` to remove any local references to
branches deleted on your Fork. We do two push operations, one for the `develop`
branch, and one for any new tags (new releases).

After synchronizing the `develop` branch, the local Topic Branch can be deleted:

```console
$ git branch --delete <NAME_OF_TOPIC_BRANCH>
```

If you need to delete a Topic Branch that has not been merged to `develop`,
you may need to force deletion:

```console
$ git branch --delete --force <NAME_OF_TOPIC_BRANCH>
```

Make absolutely sure you want the branch deleted before running either
delete operation!

### "WIP" Merge Requests

The workflow documented above covers the most common use case of submitting
a Topic Branch as a Pull Request at the point development is felt to be complete.
In certain cases, it may be more useful to create a so-called "WIP" Pull Request,
where an empty or partially complete Topic Branch is submitted. These can be useful for
work such as testing features on platforms you may not have access to via Travis-CI,
to identify a regression and then fix it using the testing results, or to start implementing
a feature and request input/feedback from others.

Other than the point in the development work at which the Topic Branch is submitted
as a Pull Request, the workflow is identical to that described above. However, to ensure
that others are aware of the intent of the Pull Request, you **must** prefix its `Title`
with `WIP:`, e.g. `WIP: Resolve regression reported in Issue XYZ`.

Once you feel development is complete, report this via a comment in the Pull
Request thread, and edit the `Title` to remove the `WIP:` prefix. Any final
testing and review will be performed before mergin


# Additional Topics
## Working with Others
In many cases, fixes and features may be worked on by more than one person at a time.
Here, one developer takes responsibility for the topic and creates the Topic Branch
and publishes it on their Fork. Others may contribute changes to the Topic Branch before
and during its submission as a Pull Request by the main developer.

For example, let's say I've created a Topic Branch `super-new-feature` and
published it on my Fork:

```
me> git checkout -b super-new-feature
... edit, test, edit ...
me> git push -u origin super-new-feature
```

If you want to contribute to this branch, you can add my Fork
as a new remote of your Local Clone:

```console
... in your local clone ...
you> git remote add me https://github.com/<MY_GITHUB_ID>/Falaise.git
```

then fetch and checkout the `super-new-feature` branch

```
you> git fetch me
... checkout my branch
you> git checkout me/super-new-feature
```

You could now make changes on this branch and then publish these on your
fork:

```console
you> git push -u origin super-new-feature
```

To share these changes with me, you can either submit them as a Pull
Request to my Fork, or I can add your Fork as a remote, fetch your
changes, and merge them into my branch:

```console
... in my local clone ...
me> git remote add you https://github.com/<YOUR_GITHUB_ID>/Falaise.git
$ git fetch you
... ensure I'm on my version of the branch
$ git checkout super-new-feature
$ git merge you/super-new-feature
```


## Reviewing Pull Requests Locally
If you are a Pull Request reviewer, then changes can be reviewed and commented on
directly through the [GitHub review interface](https://help.github.com/articles/about-pull-request-reviews/).
This is useful to comment on higher level aspects like naming, style and design.

It is also possible to checkout the changes in a Pull Request onto a branch in
your working copy for detailed compile, test and run checks. The interface and
commands to do this are [documented in GitHub Help](https://help.github.com/articles/checking-out-pull-requests-locally/).
Note that this creates new branches in yout working copy, so after review and
merge/dismissal of the PR, you should delete the local branch using `git branch -d`
or `git branch -D` as needed.


# Quick Reference
## Git Workflow
### Creating a Fork of Falaise
1. Sign in to https://github.com
2. Go to https://github.com/supernemo-dbd/falaise
3. Click on `Fork`
4. Select your personal GitHub account to fork into, if required
5. Your fork will now be created at https://github.com/[YOUR_GITHUB_ID]/falaise

### Creating a Local Clone
```
$ git clone git@github.com:[YOUR_GITHUB_ID]/falaise.git
...
$ cd falaise
$ git remote add upstream https://github.com/supernemo-dbd/falaise.git
$ git fetch upstream
$ git rebase upstream/master
```

### Starting a Topic Branch
```console
$ cd falaise
$ git checkout develop
$ git fetch upstream
$ git rebase upstream/develop
$ git push origin develop
$ git checkout -b name-of-topic
```

### Developing a Topic Branch Locally
```console
$ git checkout name-of-topic
$ git fetch upstream
$ git rebase upstream/develop
...
$ ... edit code ...
$ ... configure, build, test ...
$ ... fix issues ...
...
$ git add <list of files changed>
$ git commit
... write a good commit message ...
$
... repeat edit/test/fix cycle
... repeat add/commit
...
```

### Publishing a Topic Branch on Your Fork
```console
$ git checkout name-of-topic
... publishing for first time
$ git push -u origin name-of-topic
...
... after initial developments/rebases
$ git fetch upstream
$ git rebase upstream/develop
$ git push --force origin name-of-topic

### Submitting a Merge Request to SuperNEMO-DBD/Falaise
1. Ensure your Topic Branch is published and up to date on your fork
2. **WIP**

### Updating a Merge Request
Just follow the same steps as in Developing a Topic Branch Locally. and
Publishing a Topic Branch on Your Fork. GitHub watches the Topic Branch
on your Fork for changes and will update the Pull Request accordingly.

### Cleaning up a Topic Branch
```console
$ git checkout master
...
$ git fetch upstream
...
$ git rebase upstream/develop
...
$ git fetch origin --prune
...
$ git push origin develop
...
$ git push origin --tags
...
$ git branch --delete name-of-topic
...
... if deleteing an un-merged Topic Branch
$ git branch --delete --force name-of-topic
```


## Tools and Tutorials
### Git and GitHub
- [Software Carpentry Git Lessons](http://swcarpentry.github.io/git-novice/)
- [Pro Git Documentation](https://git-scm.com/book/en/v2)
- [General GitHub documentation](https://help.github.com/)
- [GitHub pull request documentation](https://help.github.com/articles/creating-a-pull-request/)

### C++
- [Official ISO C++ Site](https://isocpp.org)
- [cppreference](http://en.cppreference.com/w/)
- [CPP Core Guidelines](https://isocpp.github.io/CppCoreGuidelines/CppCoreGuidelines)
- [Catch](https://github.com/catchorg/Catch2) Unit Testing [Tutorial](https://github.com/catchorg/Catch2/blob/master/docs/tutorial.md) and [Reference](https://github.com/catchorg/Catch/blob/master/docs/Readme.md) Guides

### CMake
- [Core CMake Documentation](https://cmake.org/documentation/)

### Falaise
- [Reference and API Guide](https://supernemo-dbd.github.io/Falaise)

