#!/bin/bash
# Remove thot package files using git

this_pkg_home=${THOT_GITHUB_HOME}

# Remove files in utils dir
git rm `find ${this_pkg_home}/utils -name "*.sh" | grep -v "utils/\."`
git rm `find ${this_pkg_home}/utils -name "*.pl"`

# Remove files in src dir
git rm `find ${this_pkg_home}/src -name "*.h" | grep -v "thot_config.h"`
git rm `find ${this_pkg_home}/src -name "*.c"`
git rm `find ${this_pkg_home}/src -name "*.cc"`
