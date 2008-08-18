INCLUDES = -I$(top_srcdir)/gui -I$(top_srcdir)/src/libayfly $(GUI_CXXFLAGS) $(all_includes)

bin_PROGRAMS = ayfly
ayfly_SOURCES = AyflyApp.cpp AyflyFrame.cpp DnDFiles.cpp AyflyBindingsDlg.cpp

if IS_UNIX
EXTRAINCLUDES += $(SDL_CFLAGS)
endif

if IS_WINDOWS
EXTRAINCLUDES += $(DX_CFLAGS)
endif

INCLUDES += $(EXTRAINCLUDES)

ayfly_LDADD = $(top_builddir)/src/libayfly/libayfly.a $(GUI_LIBS)

datarootdir = @datarootdir@
