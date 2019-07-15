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
2) The [homebrew](https://github.com/SuperNEMO-DBD/brew) repository
where externals are managed.

Updating brew
-------------
This is the core package manager implementation, hosted in [supernemo-dbd/brew](https://github.com/supernemo-dbd/brew.git).
It's a fork of upstream [Linuxbrew](https://github.com/linuxbrew/brew.git), and
is no largely locked down as SuperNEMO migrate to the [Spack](https://spack.io) system.

Updates are thus mostly to resolve missing/broken URLs in third-party packages
and bump versions of third-party or the SuperNEMO packages.

1. When formulae that are used directly or indirectly by `falaise` are updated in an ABI incompatible way.
   - The main task here is to track the dependency, and issue revision bumps
     to each dependent formulae (and their dependees, and so on...)
   - Canonical case is `icu4c`/`boost`. If `icu4c` is upgraded, then the `boost` formula
     requires a revision bump, as does `camp`, `bayeux` and `falaise`.
2. When Formulae undergo a version bump (e.g. we want to try a new version)
   - Simply employ the same proceedure as for 1.


Rebuilding Docker Containers
----------------------------
The [brew](https://github.com/SuperNEMO-DBD/brew) repository hosts
the `Dockerfile`s for building images of the Falaise dependencies on CentOS 7 and Ubuntu 16.04LTS/18.04LTS.
The procedure below assumes that we have updated `brew` as above
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
   $ docker build -t falaise-centos7-base .
   ... rebuilds ...
   ```

   Depending on any issues encountered (hence why we'd like to automate bottle building!)
   this may require some iterations.
4. Once the containers are built, tag them according to the scheme:

   ```
   $ docker images
   ... take note of the IMAGE ID entries ...
   ... say we find the IMAGE ID for the new Ubuntu 16.04 container:
   $ docker tag <UBUNTU16IMAGEID> supernemo/falaise-ubuntu1604-base:develop
   ```

   Note the use of the organization name `supernemo`, the image name `falaise-<osname><osversion>-base`
   (`base` indicating base dependencies for falaise itself) and the "version" `:<version>`.
   In this case we use the `develop` version as a symbolic name for testing the `develop` branch.
5. Push the newly tagged containers to Docker Hub (you will need to be a member
   of the `supernemo` or on Docker Hub to do this):

   ```
   ... e.g. for the example above, similar for others ...
   $ docker push supernemo/falaise-ubuntu1604-base:develop
   ```

Testing Falaise with Rebuilt Containers
---------------------------------------
Simply open a new Pull Request bumping the minimum require version(s) of the external
packages updated in the previous step. If there are issues in the build due to the new dependencies

- Fix and push changes to the Pull Request
- Try to retain backward compatibility with older package versions if possible
- Merge the Pull Request

The `-base` docker images can then be tagged again with the `latest` symbolic tag

   ```
   $ docker tag <UBUNTU16IMAGEID> supernemo/falaise-ubuntu1604-base:latest
   $ docker push supernemo/falaise-ubuntu1604-base:latest
   ```

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
5. Update the Falaise Formula in [brew](https://github.com/SuperNEMO-DBD/brew) to point
   the stable spec to the new tag
6. Build new `supernemo/falaise` container
   - Go to [brew](https://github.com/SuperNEMO-DBD/brew) repo:
     ```
     $ cd docker/falaise
     $ docker build -t falaise .
     ```
   - Tag container as both `:latest` and `:<newversion>` then push e.g.
     ```
     $ docker tag <IMAGEID> supernemo/falaise:latest
     $ docker tag <IMAGEID> supernemo/falaise:3.1
     $ docker push supernemo/falaise:3.1
     $ docker push supernemo/falaise:latest
     ```


