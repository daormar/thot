#!/bin/bash
# Add thot package files using git

this_pkg_home=${THOT_HOME}

# Add files in utils dir

# .sh
git add `find ${this_pkg_home}/utils -name "*.sh"| grep -v "utils/\."`

# .pl
for file in `find ${this_pkg_home}/utils -name "*.pl"`; do
    git add $file
done

# .py
for file in `find ${this_pkg_home}/utils -name "*.py"`; do
    git add $file
done

# Add files in src dir
git add `find ${this_pkg_home}/src -name "*.h" | grep -v "thot_config.h"`
git add `find ${this_pkg_home}/src -name "*.c"`
git add `find ${this_pkg_home}/src -name "*.cc"`
