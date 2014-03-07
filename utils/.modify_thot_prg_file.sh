#!/bin/bash
# Automatically copies source files that are necessary to build the
# package.

if [ $# -ne 2 ]; then
    echo "Usage: modify_thot_prg_file.sh <curr_pkg> <prgfile>"
else

    # Read input parameters
    pkg=$1
    prgfile=$2

    # Set this_pkg_name variable
    this_pkg_name="thot"

    # Invoke utility
    modify_pkg_program_file.sh ${pkg} ${this_pkg_name} ${prgfile}

fi
