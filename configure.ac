#                                               -*- Autoconf -*-
# Process this file with autoconf to produce a configure script.

AC_PREREQ([2.63])
AC_INIT([ntcir], [1.4.0], [BUG-REPORT-ADDRESS])
AC_CONFIG_SRCDIR([src/query.cpp])
#AC_CONFIG_HEADERS([config.h])

AM_INIT_AUTOMAKE

# Checks for programs.
AC_PROG_CXX

# Checks for libraries.
AC_ARG_WITH(stpl,
            AC_HELP_STRING([--with-stpl=DIR],
                       [Define STPL directory for its include files]),
        [ if test -d "$withval" ; then
            CFLAGS="$CFLAGS -I$withval"
            CXXFLAGS="$CXXFLAGS -I$withval"
          else
            AC_MSG_ERROR([Cannot stat directory $withval])
          fi ] )
          
AC_ARG_WITH(uniseg,
            AC_HELP_STRING([--with-uniseg=DIR],
                       [Define uniseg directory for its include files]),
        [ if test -d "$withval" ; then
            LIB_UNISEG="$withval"
            CFLAGS="$CFLAGS -I$LIB_UNISEG"
            CXXFLAGS="$CXXFLAGS -I$LIB_UNISEG"
          else
            AC_MSG_ERROR([Cannot stat directory $withval])
          fi ], LIB_UNISEG="$withval")   
          
AC_ARG_WITH(ant,
            AC_HELP_STRING([--with-ant=DIR],
                       [Define ant directory for its include files]),
        [ if test -d "$withval" ; then
            LIB_ANT="$withval"
            CFLAGS="$CFLAGS -I$LIB_ANT"
            CXXFLAGS="$CXXFLAGS -I$LIB_ANT"
          else
            AC_MSG_ERROR([Cannot stat directory $withval])
          fi ], LIB_ANT="$withval")  
                    
AC_SUBST(LIB_UNISEG)
AC_SUBST(LIB_ANT)     

AC_CHECK_LIB([ant], [ant_easy_init])     
          
# Checks for header files.

# Checks for typedefs, structures, and compiler characteristics.

# Checks for library functions.

AC_CONFIG_FILES([Makefile])
AC_OUTPUT
