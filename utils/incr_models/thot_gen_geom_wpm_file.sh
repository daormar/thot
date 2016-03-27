# Author: Daniel Ortiz Mart\'inez
# *- bash -*

# Generates a geometric word penalty model file given the value of p.

if [ $# -ne 1 ]; then
    echo "Usage: thot_gen_geom_wpm_file <p>"
else
    echo "Geometric wp model file..."
    echo $1
fi
