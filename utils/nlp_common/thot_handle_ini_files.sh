# Author: Daniel Ortiz Mart\'inez
# *- bash -*

if [ $# -lt 1 ]; then
    echo "Use: thot_handle_ini_files [-s|-w <string>|-r]"
    echo ""
    echo "-s          : Show content of master.ini file"
    echo "-w <string> : Overwrite master.ini with the file provided as argument."
    echo "              NOTE: this action may require superuser privileges depending"
    echo "              on where the package was installed"
    echo "-r          : Revert master.ini to its original content after installing"
    echo "              the package"
    echo ""
else
    
    # Read parameters
    s_given=0
    w_given=0
    r_given=0
    num_opts=0
    while [ $# -ne 0 ]; do
        case $1 in
            "-s")  s_given=1
                num_opts=`expr $num_opts + 1`
                ;;
            "-w") shift
                if [ $# -ne 0 ]; then
                    w_given=1
                    file=$1
                    num_opts=`expr $num_opts + 1`
                fi
                ;;
            "-r")  r_given=1
                num_opts=`expr $num_opts + 1`
                ;;
        esac
        shift
    done

    # Check parameters
    if [ $num_opts -gt 1 ]; then
        echo "Error: only one option can be given simultaneously" >&2
        exit 1
    fi

    if [ $w_given -eq 1 ]; then
        if [ ! -f $file ]; then
            echo "Error: file $file does not exist" >&2
            exit 1
        fi
    fi

    # Check availability of master.ini
    if [ ! -f ${datadir}/ini_files/master.ini ]; then
        echo "Fatal error: master.ini file does not exist, please verify package installation"
        exit 1
    fi

    # Process parameters

    if [ $s_given -eq 1 ]; then
        echo "* File location: ${datadir}/ini_files/master.ini"
        echo "* File content:"
        cat ${datadir}/ini_files/master.ini
    fi

    if [ $w_given -eq 1 ]; then
        echo "Overwriting current master.ini file with $file..." >&2
        cp $file ${datadir}/ini_files/master.ini || exit 1
        echo "Done" >&2
    fi

    if [ $r_given -eq 1 ]; then
        # Check if revert file exists
        if [ ! -f ${datadir}/ini_files/standard.ini ]; then
            echo "Fatal error: standard.ini file used to revert current content of master.ini does not exist, please verify package installation"
            exit 1
        fi

        # Revert file
        revertfile=${datadir}/ini_files/standard.ini
        echo "Overwriting current master.ini file with ${revertfile}..." >&2
        cp ${revertfile} ${datadir}/ini_files/master.ini || exit 1
        echo "Done" >&2
    fi

fi
