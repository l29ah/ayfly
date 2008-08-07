AC_INIT([ayfly], [0.0.15])
AC_CANONICAL_SYSTEM

AM_INIT_AUTOMAKE(ayfly, 0.0.15)

AC_LANG_CPLUSPLUS
AC_PROG_CC
AC_PROG_CXX
AC_PROG_RANLIB
AC_EXEEXT

AC_DEFINE([UNICODE], [1])
AC_DEFINE([_UNICODE], [1])

case "$host_os" in
*mingw*) windows=yes
		AC_DEFINE([WINDOWS], [1])
		AC_DEFINE([WIN32], [1])
		AC_DEFINE([_WINDOWS], [1])
		AC_DEFINE([_WIN32], [1])
		AC_ARG_WITH([directx], [AS_HELP_STRING([--with-directx], [Specify directx SDK path])],
					[],
					[with_directx=/opt/directx])
		AC_SUBST(DX_CFLAGS, "-I$with_directx/include")
		AC_SUBST(DX_LIBS, "-L$with_directx/lib -ldsound -ldxguid -lole32 -luuid")
		;;
*) unix=yes
		AM_PATH_SDL
		if test "x$SDL_LIBS" = x0; then
			AC_ERROR([SDL libraries cannot be found...!])
		fi
		;;
esac


AM_CONDITIONAL(IS_WINDOWS, test x$windows = xyes)
AM_CONDITIONAL(IS_UNIX, test x$unix = xyes)

AC_SUBST(GUI_CXXFLAGS, `wx-config --cxxflags`)
AC_SUBST(GUI_LIBS, `wx-config --libs`)

if test "x$GUI_LIBS" = x0; then
	AC_ERROR([wxWidgets libraries cannot be found...!])
fi

AC_CONFIG_FILES([Makefile src/Makefile src/gui/Makefile src/libayfly/Makefile src/libayfly/z80ex/Makefile src/libayfly/unix/Makefile src/libayfly/windows/Makefile])
AC_OUTPUT

