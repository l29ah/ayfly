INCLUDES = -I$(top_srcdir)/gui -I$(top_srcdir)/src/libayfly -I$(top_srcdir)/src/libayfly/z80ex_new -I$(top_srcdir)/src/libayfly/z80ex_new/include $(GUI_CXXFLAGS) $(all_includes)

bin_PROGRAMS = ayfly
ayfly_SOURCES = AyflyApp.cpp AyflyFrame.cpp DnDFiles.cpp AyflyBindingsDlg.cpp

if IS_UNIX
EXTRAINCLUDES = $(SDL_CFLAGS)
EXTRALDADD = $(SDL_LIBS)
endif

if IS_WINDOWS
EXTRAINCLUDES = $(DX_CFLAGS)
EXTRALDADD = $(DX_LIBS)
endif

INCLUDES += $(EXTRAINCLUDES)

ayfly_LDADD = $(top_builddir)/src/libayfly/libayfly.a $(GUI_LIBS) $(SDL_LIBS) $(DX_LIBS)

datarootdir = @datarootdir@
