#!/bin/bash
# Add thot package files using git

this_pkg_home=${THOT_GITHUB_HOME}

# Add files in utils dir
git add `find ${this_pkg_home}/utils -name "*.sh"| grep -v "utils/\."`
git add `find ${this_pkg_home}/utils -name "*.pl"`

# Add files in src dir
git add `find ${this_pkg_home}/src -name "*.h" | grep -v "thot_config.h"`
git add `find ${this_pkg_home}/src -name "*.c"`
git add `find ${this_pkg_home}/src -name "*.cc"`
