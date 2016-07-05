#                                               -*- Autoconf -*-
# Process this file with autoconf to produce a configure script.

AC_PREREQ(2.57)
AC_INIT(thot, 3.1.0Beta, dortiz@prhlt.upv.es)
AC_CONFIG_MACRO_DIR([m4])
AM_INIT_AUTOMAKE([foreign subdir-objects])
AC_CONFIG_HEADER(config.h)
AX_PREFIX_CONFIG_H([./src/thot_config.h])
AM_PATH_PYTHON

dnl Initialize Libtool
LT_CONFIG_LTDL_DIR([libltdl])
LT_INIT([dlopen])
LTDL_INIT([recursive])

# Save previous compilation flags
CFLAGS_AUX=${CFLAGS}
CXXFLAGS_AUX=${CXXFLAGS}

# Checks for programs.
AC_PATH_PROGS(BASH, bash)
AC_PATH_PROGS(PERL, perl)
AC_PATH_PROGS(SPLIT, split)
AC_PATH_PROGS(SED, sed)
AC_PATH_PROGS(GREP, grep)
AC_PATH_PROGS(UNIQ, uniq)
AC_PATH_PROGS(HEAD, head)
AC_PATH_PROGS(TAIL, tail)
AC_PATH_PROGS(FIND, find)
AC_PATH_PROGS(LN, ln)
AC_PATH_PROGS(WC, wc)
AC_PATH_PROGS(BASENAME, basename)
AC_PATH_PROGS(DIRNAME, dirname)
AC_PATH_PROGS(MKTEMP, mktemp)
AC_PATH_PROGS(TOUCH, touch)
AC_PATH_PROGS(CUT, cut)
AC_PATH_PROGS(SSH, ssh)
AC_PATH_PROGS(QSUB, qsub)
AC_PATH_PROGS(QSTAT, qstat)
AC_PATH_PROGS(QRLS, qrls)

# check that PIPESTATUS is available
echo -n "checking whether PIPESTATUS is available... "
if test -z "${PIPESTATUS}"; then
  PIPE_FAIL="disabled_pipe_fail"
  echo "no"
else
  PIPE_FAIL="pipe_fail"
  echo "yes"
fi
AC_SUBST(PIPE_FAIL)

# check that qsub works
if test ! -z "$QSUB"; then
 echo -n "checking whether qsub works... "
 echo "" | ${QSUB} >/dev/null 2>&1 
 if test $? -eq 0; then
  QSUB_WORKS="yes"
  echo "yes"
 else
  QSUB_WORKS="no"
  echo "no"
 fi
else
 QSUB_WORKS="no"
fi
AC_SUBST(QSUB_WORKS)

# check qsub parameter (-terse)
if test "${QSUB_WORKS}" = "yes"; then
 echo -n "checking whether qsub accepts -terse... "
 echo "" | ${QSUB} -terse >/dev/null 2>&1
 if test $? -eq 0; then
  QSUB_TERSE_OPT="-terse"
  echo "yes"
 else
  QSUB_TERSE_OPT=""
  echo "no"
 fi 
else
 QSUB_TERSE_OPT=""
fi
AC_SUBST(QSUB_TERSE_OPT)

# check qstat parameter (-j)
if test "${QSUB_WORKS}" = "yes"; then
 echo -n "checking whether qstat accepts -j... "
 jid=0
 ${QSTAT} -j ${jid} 2>&1 | $GREP "invalid option" >/dev/null 2>&1
 if test $? -eq 0; then
  QSTAT_J_OPT=""
  echo "no"
 else
  QSTAT_J_OPT="-j"
  echo "yes"
 fi 
else
 QSTAT_J_OPT=""
fi
AC_SUBST(QSTAT_J_OPT)


# Search awk

## GAWK
AC_PATH_PROG(GAWK, gawk, no)
if test "$GAWK" = no; then

   ## NAWK
   AC_PATH_PROG(NAWK, nawk, no)
   if test "$NAWK" = no; then
        
        ## MAWK
	AC_PATH_PROG(MAWK, mawk, no)
        if test "$MAWK" = no; then
	      AWK=awk
        else
              # mawk found
              AWK=$MAWK
        fi
   else
        # nawk found
        AWK=$NAWK
   fi
else
        # gawk found
        AWK=$GAWK
fi

AC_SUBST(AWK)

AC_SUBST(USE_SOURCE_OPT)

# check awk delete array
AC_MSG_CHECKING(whether awk supports delete array )
USE_FAST_DELETE="no"
echo "" | $AWK '{ a[[1]]=1; delete a; if (a[[1]]!=0) exit (1) }' && USE_FAST_DELETE="yes"
if test ${USE_FAST_DELETE} = "yes"; then
   AC_MSG_RESULT(yes)   
else
   AC_MSG_RESULT(no) 
fi

AC_SUBST(USE_FAST_DELETE)

# Search sort and check parameters
AC_PATH_PROGS(SORT,sort)

echo -n "checking whether sort accepts -S <buffsize>... "
echo "" | ${SORT} -S 131072 >/dev/null 2>&1
if test $? -eq 0; then
   sortpars="-S 131072"
   echo "yes"
else
   sortpars=""
   echo "no"
fi 
AC_SUBST(sortpars)

echo -n "checking whether sort accepts -T <dir>... "
echo "" | ${SORT} -T /tmp >/dev/null 2>&1
if test $? -eq 0; then
   sortT="yes"
   echo "yes"
else
   sortT="no"
   echo "no"
fi 
AC_SUBST(sortT)

echo -n "checking whether sort accepts -m... "
echo "" | ${SORT} -m >/dev/null 2>&1
if test $? -eq 0; then
   sortm="yes"
   echo "yes"
else
   sortm="no"
   echo "no"
fi 
AC_SUBST(sortm)


# Check for compilers
AC_PROG_CXX
AC_PROG_CC
AM_PROG_CC_C_O

CFLAGS="${CFLAGS_AUX}"
CXXFLAGS="${CXXFLAGS_AUX}"

# Checks for basic flags
AC_LANG(C++)
for f in -Wno-deprecated
do                        
    AC_MSG_CHECKING(whether ${CXX} supports ${f})
    saved_cxxflags="${CXXFLAGS}"
    CXXFLAGS="${CXXFLAGS} ${f}"
    AC_COMPILE_IFELSE([AC_LANG_SOURCE([int main(void) { return 0; }],
                      AC_MSG_RESULT(yes),
                      AC_MSG_RESULT(no)
                      CXXFLAGS="${saved_cxxflags}")],[AC_MSG_RESULT(yes)],[AC_MSG_RESULT(no)])
done

dnl Check if we have to enable debug support.
AC_MSG_CHECKING(whether to enable debugging)
AC_ARG_ENABLE(debug,[  --enable-debug   turn on debugging], enable_debug=yes,
              enable_debug=no)

dnl Yes, debug is enabled
if test "$enable_debug" = "yes"; then

AC_MSG_RESULT(yes)

AC_DEFINE(DEBUG,,[Define if the debugging mode is enabled])

# Checks for flags
AC_LANG(C)
for f in -g -Wall  
do                        
    AC_MSG_CHECKING(whether ${CC} supports ${f})
    saved_cflags="${CFLAGS}"
    CFLAGS="${CFLAGS} ${f}"
    AC_COMPILE_IFELSE([AC_LANG_SOURCE([int main(void) { return 0; }],
                      AC_MSG_RESULT(yes),
                      AC_MSG_RESULT(no)
                      CFLAGS="${saved_cflags}")],[AC_MSG_RESULT(yes)],[AC_MSG_RESULT(no)])
done

AC_LANG(C++)
for f in -g -Wall
do                        
    AC_MSG_CHECKING(whether ${CXX} supports ${f})
    saved_cxxflags="${CXXFLAGS}"
    CXXFLAGS="${CXXFLAGS} ${f}"
    AC_COMPILE_IFELSE([AC_LANG_SOURCE([int main(void) { return 0; }],
                      AC_MSG_RESULT(yes),
                      AC_MSG_RESULT(no)
                      CXXFLAGS="${saved_cxxflags}")],[AC_MSG_RESULT(yes)],[AC_MSG_RESULT(no)])
done

else

AC_MSG_RESULT(no)

# Checks for flags
AC_LANG(C)
for f in -Wall -O2 
do                        
    AC_MSG_CHECKING(whether ${CC} supports ${f})
    saved_cflags="${CFLAGS}"
    CFLAGS="${CFLAGS} ${f}"
    AC_COMPILE_IFELSE([AC_LANG_SOURCE([int main(void) { return 0; }],
                      AC_MSG_RESULT(yes),
                      AC_MSG_RESULT(no)
                      CFLAGS="${saved_cflags}")],[AC_MSG_RESULT(yes)],[AC_MSG_RESULT(no)])
done

AC_LANG(C++)
for f in -Wall -O2
do                        
    AC_MSG_CHECKING(whether ${CXX} supports ${f})
    saved_cxxflags="${CXXFLAGS}"
    CXXFLAGS="${CXXFLAGS} ${f}"
    AC_COMPILE_IFELSE([AC_LANG_SOURCE([int main(void) { return 0; }],
                      AC_MSG_RESULT(yes),
                      AC_MSG_RESULT(no)
                      CXXFLAGS="${saved_cxxflags}")],[AC_MSG_RESULT(yes)],[AC_MSG_RESULT(no)])
done

fi

for top_builddir in . .. ../.. $ac_auxdir $ac_auxdir/..; do
  test -f $top_builddir/configure && break
done

# Checks for libraries.
# AC_PROG_RANLIB

# Adds an extra argument to configure (profiling)
AC_MSG_CHECKING(whether to enable profiling)
AC_ARG_ENABLE(developer,[  --enable-developer   enable developer features],enable_developer=yes,
              enable_developer=no)
dnl Yes, profiling is enabled
if test "$enable_developer" = "yes"; then
AC_MSG_RESULT(yes)

if test "$enable_debug" = "yes"; then
 flags="-pg"
else
 flags="-g -pg"
fi

AC_LANG(C)
for f in ${flags}
do                        
    AC_MSG_CHECKING(whether ${CC} supports ${f})
    saved_cflags="${CFLAGS}"
    CFLAGS="${CFLAGS} ${f}"
    AC_COMPILE_IFELSE([AC_LANG_SOURCE([int main(void) { return 0; }],
                      AC_MSG_RESULT(yes),
                      AC_MSG_RESULT(no)
                      CFLAGS="${saved_cflags}")],[AC_MSG_RESULT(yes)],[AC_MSG_RESULT(no)])
done

AC_LANG(C++)
for f in ${flags}
do                        
    AC_MSG_CHECKING(whether ${CXX} supports ${f})
    saved_cxxflags="${CXXFLAGS}"
    CXXFLAGS="${CXXFLAGS} ${f}"
    AC_COMPILE_IFELSE([AC_LANG_SOURCE([int main(void) { return 0; }],
                      AC_MSG_RESULT(yes),
                      AC_MSG_RESULT(no)
                      CXXFLAGS="${saved_cxxflags}")],[AC_MSG_RESULT(yes)],[AC_MSG_RESULT(no)])
done

else
AC_MSG_RESULT(no)

fi

# Adds an extra argument to configure (enable testing)
AC_MSG_CHECKING(whether to enable code testing)
AC_ARG_ENABLE(testing,[  --enable-testing   enable code testing],enable_testing=yes,enable_testing=no)
dnl Yes, code testing enabled
if test "$enable_testing" = "yes"; then
 AC_MSG_RESULT(yes)
else
 AC_MSG_RESULT(no)
fi
AM_CONDITIONAL([CODE_TESTING], [test x${enable_testing} = xyes])

# Checks if the compiler supports ISO C++ standard library
AC_CXX_HAVE_STD
if test "$ac_cv_cxx_have_std" = no; then
  AC_MSG_ERROR(C++ standard library not present!)
fi

# Checks if the compiler supports the Standard Template Library
AC_CXX_HAVE_STL
if test "$ac_cv_cxx_have_stl" = no; then
  AC_MSG_ERROR(C++ standard template library not present!)
fi

# Checks for lm library
AC_CHECK_LIB([m], [log])

# Checks for gmp library
LGMP=""
libaux=${LIBS}
AC_CHECK_LIB([gmp], [__gmpz_init])
if test $ac_cv_lib_gmp___gmpz_init = yes; then
   LGMP="-lgmp"
   AC_DEFINE(HAVE_GMP,,[Define if the gmp library is present])
fi
AC_SUBST(LGMP)
LIBS=${libaux}

# Checks for zlib library
ZLIB=""
libaux=${LIBS}
AC_CHECK_LIB([z], [inflate])
if test $ac_cv_lib_z_inflate = yes; then
   ZLIB="-lz"
   AC_DEFINE(HAVE_ZLIB,,[Define if the zlib library is present])
fi
AC_SUBST(ZLIB)
LIBS=${libaux}

# Checks for cppunit library
CPPUNIT=""
libaux=${LIBS}
AC_CHECK_LIB([cppunit], [strlen])
if test $ac_cv_lib_cppunit_strlen = yes; then
   CPPUNIT="-lcppunit"
   AC_DEFINE(HAVE_CPPUNIT,,[Define if the cppunit library is present])
fi
AC_SUBST(CPPUNIT)
LIBS=${libaux}

# Checks for header files.
AC_HEADER_STDC
AC_CHECK_HEADERS([float.h limits.h])
AC_HEADER_TIME

# Checks for typedefs, structures, and compiler characteristics.
AC_HEADER_STDBOOL
AC_C_CONST
AC_TYPE_SIZE_T
AC_STRUCT_TM
AC_CHECK_TYPE(ssize_t, int)
AC_TYPE_OFF_T

# Checks for large file support
AC_SYS_LARGEFILE
AC_FUNC_FSEEKO

# Checks for library functions.
AC_FUNC_REALLOC
AC_CHECK_FUNCS([gettimeofday pow getdelim])

 # Some systems do not supply getline()
AC_MSG_CHECKING([if getline() is supported])
AC_COMPILE_IFELSE(
  [AC_LANG_PROGRAM(
    [[
#ifndef _GNU_SOURCE
#define _GNU_SOURCE
#endif
#include <stdio.h>
    ]],[[
    char* buf = 0;
    size_t len = 0;
    FILE* f=NULL;
    getline(&buf,&len,f);
    ]]
  )],
  [AC_DEFINE(HAVE_GETLINE,[],[Define if getline() is supported])
   AC_MSG_RESULT(yes)],
  [AC_MSG_RESULT(no)])

# Checks for doxygen
AC_PATH_PROG(DOXYGEN,doxygen)

# Adds an extra argument to configure (enable use of casmacat library)
AC_MSG_CHECKING(whether to enable combination with casmacat library)
AC_ARG_WITH([casmacat],
 [AS_HELP_STRING([--with-casmacat],
   [installs thot with the necessary code to combine it with the casmacat library @<:@default=no@:>@])],
 [with_casmacat=$withval],
 [with_casmacat=no])
if test "$with_casmacat" = "no"; then
 enable_casmacat_lib="no"
 AC_MSG_RESULT(no)
else
 enable_casmacat_lib="yes"
 AC_MSG_RESULT(yes)
fi
AM_CONDITIONAL([CASMACAT_LIB_ENABLED], [test x${enable_casmacat_lib} = xyes])
AC_SUBST(CASMACAT_THOT_SERVER_LIB_HOME,$with_casmacat)

# Disable pre/post-processing code
AC_DEFINE(DISABLE_PREPROC_CODE,,[Define if the pre/post-processing code is disabled])

dnl Check if we have to enable casmacat home edition configuration.
AC_MSG_CHECKING(whether to enable casmacat home edition configuration)
AC_ARG_ENABLE(casmacat-he,[  --enable-casmacat-he   enable casmacat home edition configuration], enable_casmacat_he=yes,
              enable_casmacat_he=no)
dnl Yes, debug is enabled
if test "$enable_casmacat_he" = "yes"; then
 AC_MSG_RESULT(yes)

 AC_DEFINE(CASMACAT_HE,,[Define if casmacat home edition configuration is enabled])

 # Use IBM 2 alignment models
 AC_SUBST(ENABLE_IBM2_ALIG,"yes")

else
 AC_MSG_RESULT(no)
fi

dnl Check if fast language model downhill simplex algorithm is disabled.
AC_MSG_CHECKING(whether to disable fast lm downhill simplex algorithm)
AC_ARG_ENABLE(fast-dhslm,[  --disable-fast-dhslm   disable fast language model downhill simplex algorithm], disable_fast_dhslm=yes,
              disable_fast_dhslm=no)
dnl Yes, debug is disabled
if test "$disable_fast_dhslm" = "yes"; then
DISABLE_FAST_DHSLM="1"
AC_SUBST(DISABLE_FAST_DHSLM)
AC_MSG_RESULT(yes)
else
DISABLE_FAST_DHSLM="0"
AC_SUBST(DISABLE_FAST_DHSLM)
AC_MSG_RESULT(no)
fi

# Adds an extra argument to configure (enable updating of linear interpolation weights)
AC_MSG_CHECKING(whether to enable updating of phrase model linear interpolation weights)
AC_ARG_ENABLE(update-liweights,[  --enable-update-liweights   enable updating of phrase mode linear interpolation weights],enable_update_liweights=yes,enable_update_liweights=no)
dnl Yes, updating of log-linear weights enabled
if test "$enable_update_liweights" = "yes"; then
 AC_MSG_RESULT(yes)
else
 AC_MSG_RESULT(no)
fi
AM_CONDITIONAL([ENABLE_UPDATE_LIWEIGHTS], [test x${enable_update_liweights} = xyes])

if test "$enable_update_liweights" = "yes"; then
 ENABLE_UPDATE_LIWEIGHTS=1
 AC_SUBST(ENABLE_UPDATE_LIWEIGHTS)
else
 ENABLE_UPDATE_LIWEIGHTS=0
 AC_SUBST(ENABLE_UPDATE_LIWEIGHTS)
fi

# Adds an extra argument to configure (enable updating of log-linear weights)
AC_MSG_CHECKING(whether to enable online updating of log-linear weights)
AC_ARG_ENABLE(update-llweights,[  --enable-update-llweights   enable online updating of log-linear weights],enable_update_llweights=yes,enable_update_llweights=no)
dnl Yes, updating of log-linear weights enabled
if test "$enable_update_llweights" = "yes"; then
 AC_MSG_RESULT(yes)
else
 AC_MSG_RESULT(no)
fi
AM_CONDITIONAL([ENABLE_UPDATE_LLWEIGHTS], [test x${enable_update_llweights} = xyes])

if test "$enable_update_llweights" = "yes"; then
 AC_DEFINE(ENABLE_UPDATE_LLWEIGHTS,,[Define if updating of log-linear weights is enabled])
fi

# Adds an extra argument to configure (enable downhill simplex for ll weight updating)
AC_MSG_CHECKING(whether to enable downhill simplex algorithm for log-linear weight updating)
AC_ARG_ENABLE(enable-downhill-llw,[  --enable-downhill-llw   enable downhill simplex algorithm for log-linear weight updating],enable_downhill_llw=yes,enable_downhill_llw=no)
dnl Yes, downhill simplex for ll weight updating enabled
if test "$enable_downhill_llw" = "yes"; then
 AC_MSG_RESULT(yes)
else
 AC_MSG_RESULT(no)
fi

if test "$enable_downhill_llw" = "yes"; then
 ENABLE_DOWNHILL_LLW=1
 AC_SUBST(ENABLE_DOWNHILL_LLW)
else
 ENABLE_DOWNHILL_LLW=0
 AC_SUBST(ENABLE_DOWNHILL_LLW)
fi

dnl Check if we have to enable IBM 2 model as alignment model.
AC_MSG_CHECKING(whether to enable IBM 2 model as alignment model)
AC_ARG_ENABLE(ibm2-alig,[  --enable-ibm2-alig   enable IBM 2 model as alignment model], enable_ibm2_alig=yes,
              enable_ibm2_alig=no)

dnl Yes, debug is enabled
if test "$enable_ibm2_alig" = "yes"; then
 AC_MSG_RESULT(yes)
 AC_SUBST(ENABLE_IBM2_ALIG,"yes")
else
 AC_MSG_RESULT(no)
fi

dnl Check whether to enable viterbi training.
AC_MSG_CHECKING(whether to enable viterbi training for HMM-based alignment models)
AC_ARG_ENABLE(vit-train,[  --enable-vit-train   enable viterbi training for HMM-based alignment models], enable_vit_train=yes,
              enable_vit_train=no)

dnl Yes, space efficient lexical data structures are enabled
if test "$enable_vit_train" = "yes"; then
   AC_MSG_RESULT(yes)
else
   AC_MSG_RESULT(no)
fi

if test "$enable_vit_train" = "yes"; then
AC_DEFINE(ENABLE_VITERBI_TRAINING,,[Define if Viterbi training is enabled for HMM-based alignment models])
fi

# Process options given in shell variables

# Set different types based on previously defined shell variables

# LM_State type used
if test "$LM_STATE_H" = ""; then
   LM_STATE_H="LM_State.h"
fi
AC_DEFINE_UNQUOTED(LM_STATE_H,"${LM_STATE_H}",[File name with LM_State type definition])

# PpInfo type used
if test "$PPINFO_H" = ""; then
   PPINFO_H="PpInfo.h"
fi
AC_DEFINE_UNQUOTED(PPINFO_H,"${PPINFO_H}",[File name with PpInfo type definition])

# SmtModel type used
if test "$SMTMODEL_H" = ""; then
   SMTMODEL_H="SmtModel.h"
fi
AC_DEFINE_UNQUOTED(SMTMODEL_H,"${SMTMODEL_H}",[File name with SmtModel type definition])

#AC_CONFIG_FILES([Makefile])
AC_OUTPUT(Makefile doc/Makefile m4/Makefile src/Makefile src/nlp_common/Makefile src/incr_models/Makefile src/sw_models/Makefile src/phrase_models/Makefile src/smt_preproc/Makefile src/error_correction/Makefile src/downhill_simplex/Makefile src/stack_dec/Makefile src/exper/Makefile src/testing/Makefile utils/Makefile utils/nlp_common/Makefile utils/incr_models/Makefile utils/sw_models/Makefile utils/phrase_models/Makefile utils/smt_preproc/Makefile utils/error_correction/Makefile utils/downhill_simplex/Makefile utils/stack_dec/Makefile utils/exper/Makefile examples/Makefile examples/cfg_templates/Makefile examples/toy_corpus/Makefile examples/ini_files/Makefile)
