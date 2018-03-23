Preparing New Releases of Falaise
=================================

The development/release process of Falaise largely follows the "flow" model
([A successful Git Branching model](http://nvie.com/posts/a-successful-git-branching-model/)).
This means that the `develop` branch is expected to always be in a ready-to-release
state. However, Falaise depends on several external packages with their own release
cycles, so releases should be coordinated with updates on the externals used for testing.
This document is very much a WIP as support for CI, Docker and package management can be
improved with automation and better tools! Contributions, comments and suggestions are therefore
welcome and expected, though this document is primarily for use and reference by Falaise's
release managers.

Initial experiences shows that updates to external packages should happen *as part of the
development process*. This allows API/ABI incompatibilities to be identified early, but requires
coordination across:

1) The Falaise repository via a special "update externals" Pull Request
2) The [homebrew-cadfael](https://github.com/SuperNEMO-DBD/homebrew-cadfael) repository
where externals are managed.
3) The [cadfael-deployment](https://github.com/SuperNEMO-DBD/cadfael-deployment) repository
where Docker containers for testing are managed.

Updating Linuxbrew
-------------------
This is the core package manager implementation, hosted in [supernemo-dbd/brew](https://github.com/supernemo-dbd/brew.git).
It's a fork of upstream [Linuxbrew](https://github.com/linuxbrew/brew.git), and whilst
the `master` branch of that can be followed, the recommended proceedure is:

1. Fetch upstream changes from https://github.com/linuxbrew/brew.git
2. Check for new tags
3. If there's a new tag, merge this onto `master`, e.g. `git checkout master; git merge <newtagname>`
4. Fix any conflicts, and commit
5. Push master back to origin (i.e. `supernemo-dbd/brew`)

This only needs doing when there's a new tag.

Updating homebrew-cadfael
-------------------------
[This](https://github.com/SuperNEMO-DBD/homebrew-cadfael) hosts the linuxbrew `Formulae`
for `falaise` and important dependencies like `bayeux`, `ROOT`, `Qt`, `boost` and `Geant4`. It needs updating in two main cases:

1. When upstream Formulae from [linuxbrew/homebrew-core](https://github.com/Linuxbrew/homebrew-core)
   that are used directly or indirectly by `falaise` are updated in an ABI incompatible way.
   - The main task here is to track the dependency, and issue revision bumps
     to each dependent formulae (and their dependees, and so on...)
   - Canonical case is `icu4c`/`boost`. If `icu4c` is upgraded, then the `boost` formula
     require a revision bump, as does `camp`, `bayeux` and `falaise`.
2. When Formulae in [homebrew-cadfael](https://github.com/SuperNEMO-DBD/homebrew-cadfael)
   undergo a version bump (e.g. we want to try a new version)
   - Simply employ the same proceedure as for 1.
3. TODO: Automate these, including building of binary bottles!!


Rebuilding Docker Containers
----------------------------
The [cadfael-deployment](https://github.com/SuperNEMO-DBD/cadfael-deployment) repository hosts
the `Dockerfile`s for building images of the Falaise dependencies on CentOS6/7 and Ubuntu 14.04LTS/16.04LTS.
The procedure below assumes that we have updated `homebrew-cadfael` as above
*with a version change in the main dependencies* so that we have a change to propagate to testing.

1. Ensure docker is running
2. Remove all local containers and images:
   ```
   #!/bin/bash
   # Delete all containers
   docker rm $(docker ps -a -q)
   # Delete all images
   docker rmi $(docker images -q)
   ```
   Use force options if necessary!
3. Rebuild each container by `cd`ing into the directory
   holding its Dockerfile and naming the built image as appropriate:

   ```
   $ docker build -t falaise-centos6-next .
   ... rebuilds ...
   ```

   Depending on any issues encountered (hence why we'd like to automate bottle building!)
   this may require some iterations.
4. Once the containers are built, tag them according to the scheme:

   ```
   $ docker images
   ... take note of the IMAGE ID entries ...
   ... say we find the IMAGE ID for the new Ubuntu 16.04 container:
   $ docker tag <UBUNTU16IMAGEID> supernemo/falaise-ubuntu1604-base:next
   ```

   Note the use of the organization name `supernemo`, the image name `falaise-<osname><osversion>-base`
   (`base` indicating base dependencies for falaise itself) and the "version" `:<version>`.
   In this case we use the `next` version as a symbolic name for testing (next step).
5. Push the newly tagged containers to Docker Hub (you will need to be a member
   of the `supernemo` or on Docker Hub to do this):

   ```
   ... e.g. for the example above, similar for others ...
   $ docker push supernemo/falaise-ubuntu1604-base:next
   ```

Testing Falaise with Rebuilt Containers
---------------------------------------
The idea of the `next` containers is to provide a testbench for building the current
`develop` branch of Falaise against updated external dependencies. It's likely that
a simpler system using Travis-CI environments or robots can acheieve the same thing,
but for now:

1. Open a new Pull Request that modifies the `.travis.yml` so that the docker image tags
   use the freshly pushed `:next` containers.
2. Submitting the PR will trigger a new Travis build using the updated container
3. If there are issues in the build due to the new dependencies
   - Fix and push changes to the Pull Request
   - Try to retain backward compatibility with older package versions if possible
   - If this is not possible, bump the minimum required version of the package in Falaise's
     calls to `find_package` in the CMake scripts
4. When resolved, return the docker image tag to `:latest` in the `.travis.yml` file
5. Merge the Pull Request
6. Retag the the `-base` docker images to `:latest` and push, e.g.
   ```
   $ docker tag <UBUNTU16IMAGEID> supernemo/falaise-ubuntu1604-base:latest
   $ docker push supernemo/falaise-ubuntu1604-base:latest
   ```
   It's fine to leave the `:next` tag in place.

After this step, a new release should be prepared

Releasing a New Version of Falaise
----------------------------------
This simply follows the `git-flow` model.

1. Create a release branch for Falaise from `develop`
   - Name it `release-<newversion>`
   - Bump version number and commit
2. Build/Test Falaise RC on each image (Travis-CI should eventually handle this!)
   - Resolve any issues as required, commit/push to release branch.
   - Iterate as required
3. Finish release process
   - Checkout master branch
   - Merge in release branch `git merge --no-ff release-<newversion>`
   - Tag it - tag names must be named "Falaise-<newversion>"
     `git tag -a Falaise-<newversion> -m "Falaise <newversion>"`
   - Checkout develop branch and merge in release branch as above
   - Delete release branch `git branch -d release-<newversion>`
   - Push develop, master and new tag to origin
   - Prepare release notes on GitHub (go to Releases, then tags, then click on "Add release notes")
     These should outline notable changes at minimum!
4. Update gh-pages branch
   - Checkout new tag `git checkout Falaise-<newversion>`
   - Create a build directory *outside of the git working copy*
   - Build the tag in that builddir, ensuring `FALAISE_BUILD_DOCS` is ON (which
     it should be by default)
   - Run the generated `release-tools/prepare_release_docs.bash` script to generate the docs
     to go into `gh-pages`. This will go into the `online_html` directory.
   - Go back to the working copy and prepare the new docs:
     ```
     $ git checkout gh-pages
     # gh-pages uses the "vendor branch" model, but git is smart enough
     # to detect changes in the following copy/add steps
     $ rm -rf *
     $ cp -R <pathtobuilddir>/online_html/ .
     $ git add .
     $ git commit -m "Documentation for <tagname>"
     $ git push
     ```
5. Update [homebrew-cadfael](https://github.com/SuperNEMO-DBD/homebrew-cadfael)
   tap formulae for Falaise
   - Point falaise stable to new release tag
6. Build new `supernemo/falaise` container
   - Go to [cadfael-deployment](https://github.com/SuperNEMO-DBD/cadfael-deployment) repo:
     ```
     $ cd docker/falaise
     $ docker build -t falaise-next .
     ```
   - Tag container as both `:latest` and `:<newversion>` then push e.g.
     ```
     $ docker tag <IMAGEID> supernemo/falaise:latest
     $ docker tag <IMAGEID> supernemo/falaise:3.1
     $ docker push supernemo/falaise:3.1
     $ docker push supernemo/falaise:latest
     ```

Notes
=====
As mentioned, this is a rough outline and to be improved. Major challenge
is syncing releases with external dependency versions. Package management with
[`spack`](https://github.com/llnl/spack) will help here, once enough experience with
it, and tested support on all platforms, is obtained. We could also explore use of
LCG via CVMFS or Docker, though may not help with macOS.

Need to also explore greater automation of building binary bottles (whether homebrew
or spack) and docker containers!


