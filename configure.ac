AC_INIT([ayfly], [0.0.19])
AC_CANONICAL_SYSTEM

AM_INIT_AUTOMAKE(ayfly, 0.0.19)

AC_LANG_CPLUSPLUS
AC_PROG_CC
AC_PROG_CXX
AC_PROG_RANLIB
AC_EXEEXT

enable_gui=yes
enable_sdl=yes

AC_ARG_WITH(gui, [AS_HELP_STRING([--without-gui], [build only library and converter])],
[
enable_gui=no
]
)

AC_ARG_WITH(sdl, [AS_HELP_STRING([--without-sdl], [build without audio output support on unix])],
[
enable_sdl=no
]
)


if test "$enable_gui" = no; then
	AC_DEFINE(DISABLE_GUI, [1])
fi

if test "$enable_sdl" = no; then
	AC_DEFINE(DISABLE_SDL, [1])
fi

	

case "$host_os" in
*mingw*) windows=yes
		AC_DEFINE([WINDOWS], [1])
		AC_DEFINE([WIN32], [1])
		AC_DEFINE([_WINDOWS], [1])
		AC_DEFINE([_WIN32], [1])
		AC_DEFINE([AYFLY_STATIC], [1])
		AC_ARG_WITH([directx], [AS_HELP_STRING([--with-directx], [Specify directx SDK path])],
					[],
					[with_directx=/opt/directx])
		AC_SUBST(DX_CFLAGS, "-I$with_directx/include")
		AC_SUBST(DX_LIBS, "-L$with_directx/lib -ldsound -ldxguid -lole32 -luuid")
		;;
*) unix=yes

		if test "$enable_sdl" = "yes" ; then
			AM_PATH_SDL
			if test "x$SDL_LIBS" = x0; then
				AC_ERROR([SDL libraries cannot be found...!])
			fi
		fi
		;;
esac


AM_CONDITIONAL(IS_WINDOWS, test x$windows = xyes)
AM_CONDITIONAL(IS_UNIX, test x$unix = xyes)
AM_CONDITIONAL(ENABLE_GUI, test "$enable_gui" = "yes")
AM_CONDITIONAL(ENABLE_SDL, test "$enable_sdl" = "yes")

if test "$enable_gui" = "yes"; then
	AC_SUBST(GUI_CXXFLAGS, `wx-config --cxxflags`)
	AC_SUBST(GUI_LIBS, `wx-config --libs`)

	if test "x$GUI_LIBS" = x0; then
		AC_ERROR([wxWidgets libraries cannot be found...!])
	fi	
fi

CFLAGS="$CFLAGS -DZ80EX_OPSTEP_FAST_AND_ROUGH=1 -DUNICODE=1 -D_UNICODE=1"
CXXFLAGS="$CXXFLAGS -DZ80EX_OPSTEP_FAST_AND_ROUGH=1 -DUNICODE=1 -D_UNICODE=1"

AC_CONFIG_FILES([Makefile src/Makefile src/gui/Makefile src/converter/Makefile src/libayfly/Makefile src/libayfly/z80ex/Makefile src/libayfly/unix/Makefile src/libayfly/windows/Makefile])
AC_OUTPUT

