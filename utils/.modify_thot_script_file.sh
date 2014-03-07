#!/bin/bash
# Automatically copies source files that are necessary to build the
# package.

if [ $# -ne 2 ]; then
    echo "Usage: modify_thot_script_file.sh <curr_pkg> <scriptfile>"
else

    # Read input parameters
    pkg=$1
    scriptfile=$2

    # Set this_pkg_name variable
    this_pkg_name="thot"

    # Invoke utility
    modify_pkg_script_file.sh ${pkg} ${this_pkg_name} ${scriptfile}

fi
