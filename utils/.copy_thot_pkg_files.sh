#!/bin/bash
# Automatically copies source files that are necessary to build the
# package.

this_pkg_name="thot"
this_pkg_home=${THOT_HOME}

copy_pkg_files.sh ${this_pkg_name} ${this_pkg_home}
