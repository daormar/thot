# Author: Daniel Ortiz Mart\'inez
# *- bash -*

# In mathematical optimization, the Rosenbrock function is a non-convex
# function used as a test problem for optimization algorithms. It is
# also known as Rosenbrock's valley or Rosenbrock's banana function.
#
# This function is often used to test performance of optimization
# algorithms. The global minimum is inside a long, narrow, parabolic
# shaped flat valley. To find the valley is trivial, however to converge
# to the global minimum is difficult.
#
# It is defined by:
#
# f(x,y) = (1 - x)^2 + 100(y - x^2)^2.
#
# It has a global minimum at (x,y) = (1,1) where f(x,y) = 0. A different
# coefficient of the second term is sometimes given, but this does not
# affect the position of the global minimum.


if [ $# -ne 3 ]; then
    echo "Usage: thot_trgfunc_example <temp_dir> <x> <y>"
    echo ""
    echo "<temp_dir>     : Directory for temporaries (not used)."
    echo "<x>            : value of the x variable in the Rosenbrock function."
    echo "<y>            : value of the y variable in the Rosenbrock function."
    echo ""
    echo "NOTE: The Rosenbrock function, f(x,y) = (1 - x)^2 + 100(y - x^2)^2,"
    echo "      has a global minimum at (x,y) = (1,1), where f(x,y) = 0."
    echo " "
else
    echo "" | LC_ALL="C" ${AWK} -v x=$2 -v y=$3 '{printf"%g\n",(1-x)*(1-x)+100*((y-(x*x))*(y-(x*x)))}'
fi
