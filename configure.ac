dnl configure script for the stormav project
dnl Copyright (C) 2007 The StormAV Project
dnl
dnl This library is free software; you can redistribute it and/or
dnl modify it under the terms of the GNU Lesser General Public
dnl License as published by the Free Software Foundation; either
dnl version 2.1 of the License, or (at your option) any later version.
dnl
dnl This library is distributed in the hope that it will be useful,
dnl but WITHOUT ANY WARRANTY; without even the implied warranty of
dnl MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
dnl Lesser General Public License for more details.
dnl 
dnl You should have received a copy of the GNU Lesser General Public
dnl License along with this library; if not, write to the Free Software
dnl Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301  USA

AC_INIT([check_ice2], [1.0.2])
AC_CONFIG_SRCDIR([src/main.c])
AM_INIT_AUTOMAKE

AC_CONFIG_HEADERS(src/include/config.h)

AC_PROG_CC
AM_PROG_CC_C_O

AC_C_INLINE

AC_DEFUN([AC_PROG_CXX], [])
AC_DEFUN([AC_PROG_F77], [])

dnl
dnl flags
dnl Add -Wall and friends if we are using GCC.
dnl

AC_SUBST(AM_CFLAGS)
AC_SUBST(CPPFLAGS)

AC_C_CONST

dnl
dnl types check
dnl

AC_CHECK_TYPE(u_char, unsigned char)

dnl
dnl headers check
dnl

AC_HEADER_STDC
AC_HEADER_TIME

AC_CHECK_HEADERS([arpa/inet.h])

dnl
dnl fonctions check
dnl

AC_FUNC_MALLOC

AC_CHECK_FUNCS([socket])

dnl
dnl paths
dnl

libexecdir='${exec_prefix}/libexec/iyell'

dnl strtok_r specific checks
AC_CHECK_FUNC(strtok_r, [
	AC_DEFINE(HAVE_STRTOK_R, [], ["Define to 1 if you have the `strtok_r' function."])
	ac_found_strtok_r=yes
	], [
	HAVE_STRTOK_R="# "
	])

AC_SUBST(HAVE_STRTOK_R)
if test "$ac_found_strtok_r" = "yes"; then
	AC_MSG_CHECKING(if strtok_r declaration requires -D__EXTENSIONS__)
	AC_EGREP_HEADER(strtok_r, string.h, [
		AC_MSG_RESULT(no)
		], [
		ac_save_cppflags="$CPPFLAGS"
		CPPFLAGS="$ac_save_cppflags -D__EXTENSIONS__"
		AC_EGREP_HEADER(strtok_r, string.h, [
			ac_save_cppflags=
			AC_MSG_RESULT(yes)
		], [
			CPPFLAGS="$ac_save_cppflags"
			AC_MSG_RESULT(still can't find it)
		])
	])
fi

dnl
dnl structures checks
dnl

AC_STRUCT_TM

dnl
dnl enable / disable debug
dnl

AC_ARG_ENABLE(debug,
[  --enable-debug          specify you are building with debugging on],
	AM_CFLAGS="-W -Wall -std=c99 -pedantic -g3" ,
	AM_CFLAGS="-W -Wall -std=c99 -pedantic -DNDEBUG"
)
AC_SUBST(AM_CFLAGS)

dnl solaris needs -lsocket -lnsl
AC_SEARCH_LIBS(socket,socket)
AC_SEARCH_LIBS(getnameinfo,nsl socket)
AC_SEARCH_LIBS(inet_ntoa,nsl socket)

dnl
dnl Create Makefiles
dnl

AC_OUTPUT([Makefile\
	src/Makefile]
)
