dnl $Id$
dnl config.m4 for extension ebin

dnl Comments in this file start with the string 'dnl'.
dnl Remove where necessary. This file will not work
dnl without editing.

dnl If your extension references something external, use with:

dnl PHP_ARG_WITH(ebin, for ebin support,
dnl Make sure that the comment is aligned:
dnl [  --with-ebin             Include ebin support])

dnl Otherwise use enable:

PHP_ARG_ENABLE(ebin, whether to enable ebin support,
Make sure that the comment is aligned:
[  --enable-ebin           Enable ebin support])

if test "$PHP_EBIN" != "no"; then
  dnl Write more examples of tests here...

  dnl # --with-ebin -> check with-path
  dnl SEARCH_PATH="/usr/local /usr"     # you might want to change this
  dnl SEARCH_FOR="/include/ebin.h"  # you most likely want to change this
  dnl if test -r $PHP_EBIN/$SEARCH_FOR; then # path given as parameter
  dnl   EBIN_DIR=$PHP_EBIN
  dnl else # search default path list
  dnl   AC_MSG_CHECKING([for ebin files in default path])
  dnl   for i in $SEARCH_PATH ; do
  dnl     if test -r $i/$SEARCH_FOR; then
  dnl       EBIN_DIR=$i
  dnl       AC_MSG_RESULT(found in $i)
  dnl     fi
  dnl   done
  dnl fi
  dnl
  dnl if test -z "$EBIN_DIR"; then
  dnl   AC_MSG_RESULT([not found])
  dnl   AC_MSG_ERROR([Please reinstall the ebin distribution])
  dnl fi

  dnl # --with-ebin -> add include path
  dnl PHP_ADD_INCLUDE($EBIN_DIR/include)

  dnl # --with-ebin -> check for lib and symbol presence
  dnl LIBNAME=ebin # you may want to change this
  dnl LIBSYMBOL=ebin # you most likely want to change this 

  dnl PHP_CHECK_LIBRARY($LIBNAME,$LIBSYMBOL,
  dnl [
  dnl   PHP_ADD_LIBRARY_WITH_PATH($LIBNAME, $EBIN_DIR/$PHP_LIBDIR, EBIN_SHARED_LIBADD)
  dnl   AC_DEFINE(HAVE_EBINLIB,1,[ ])
  dnl ],[
  dnl   AC_MSG_ERROR([wrong ebin lib version or lib not found])
  dnl ],[
  dnl   -L$EBIN_DIR/$PHP_LIBDIR -lm
  dnl ])
  dnl
  dnl PHP_SUBST(EBIN_SHARED_LIBADD)
	PHP_ADD_INCLUDE(./erl_interface/include)
	PHP_ADD_LIBRARY_WITH_PATH(ei, ./erl_interface/lib/, EBIN_SHARED_LIBADD)
  PHP_SUBST(EBIN_SHARED_LIBADD)
  PHP_NEW_EXTENSION(ebin, ebin.c, $ext_shared)
fi
